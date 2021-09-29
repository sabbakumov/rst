// Copyright (c) 2017, Sergey Abbakumov
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef RST_STATUS_STATUS_OR_H_
#define RST_STATUS_STATUS_OR_H_

#include <new>
#include <type_traits>
#include <utility>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/macros/optimization.h"
#include "rst/not_null/not_null.h"
#include "rst/status/status.h"

namespace rst {

// A Google-like StatusOr class for error handling.
//
// Example:
//
//   rst::StatusOr<std::string> foo = Foo();
//   if (foo.err())
//     return std::move(foo).TakeStatus();
//
//   // Or:
//   RST_TRY_CREATE(auto, foo, Foo());
//   RST_TRY_CREATE(rst::StatusOr<std::string>, foo, Foo());
//   ...
//   RST_TRY_ASSIGN(foo, Foo());
//
//   std::cout << *foo << ", " << foo->size() << std::endl;
//
template <class T>
class [[nodiscard]] StatusOr {
 public:
  StatusOr() = delete;

  StatusOr(StatusOr&& other) noexcept {
    MoveConstructFromStatusOr(std::move(other));
  }

  // Stores success value.
  template <class U,
            class = typename std::enable_if_t<std::is_trivially_copyable_v<U>>>
  StatusOr(const U& value) {  // NOLINT(runtime/explicit)
    static_assert(std::is_same_v<T, U>);
    CopyConstructFromT(value);
  }
  StatusOr(T&& value) noexcept {  // NOLINT(runtime/explicit)
    MoveConstructFromT(std::move(value));
  }

  // Stores error value.
  StatusOr(Status status) {  // NOLINT(runtime/explicit)
    MoveConstructFromStatus(std::move(status));
  }

  // Asserts that it was checked.
  ~StatusOr() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    Cleanup();
  }

  // Asserts that it was checked before and sets as not checked.
  StatusOr& operator=(StatusOr&& rhs) noexcept {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    if (RST_LIKELY(has_error_ == rhs.has_error_)) {
      MoveAssignFromStatusOr(std::move(rhs));
    } else {
      Cleanup();
      MoveConstructFromStatusOr(std::move(rhs));
    }

    return *this;
  }

  // Asserts that it was checked before and sets as not checked.
  template <class U,
            class = typename std::enable_if_t<std::is_trivially_copyable_v<U>>>
  StatusOr& operator=(const U& value) {
    static_assert(std::is_same_v<T, U>);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    if (RST_UNLIKELY(has_error_)) {
      Cleanup();
      CopyConstructFromT(value);
    } else {
      CopyAssignFromT(value);
    }

    return *this;
  }

  StatusOr& operator=(T&& value) noexcept {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    if (RST_UNLIKELY(has_error_)) {
      Cleanup();
      MoveConstructFromT(std::move(value));
    } else {
      MoveAssignFromT(std::move(value));
    }

    return *this;
  }

  // Asserts that it was checked before and sets as not checked.
  StatusOr& operator=(Status status) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(status.error_ != nullptr);

    if (RST_UNLIKELY(has_error_)) {
      MoveAssignFromStatus(std::move(status));
    } else {
      Cleanup();
      MoveConstructFromStatus(std::move(status));
    }

    return *this;
  }

  // Sets the object to be checked and returns whether the status is error
  // object.
  [[nodiscard]] bool err() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
    if (has_error_)
      (void)get_status().err();
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return has_error_;
  }

  // Asserts that it was checked.
  T& operator*() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(!has_error_);

    return get_value();
  }

  // Asserts that it was checked.
  NotNull<T*> operator->() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(!has_error_);

    return &get_value();
  }

  // Asserts that it was checked.
  Status TakeStatus() && {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(has_error_);

    return std::move(get_status());
  }

  // Asserts that it was checked.
  const Status& status() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(has_error_);

    return get_status();
  }

  // Sets the object to be checked.
  void Ignore() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
    if (has_error_)
      get_status().was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

 private:
  void MoveConstructFromStatusOr(StatusOr&& other) {
    has_error_ = other.has_error_;

    if (RST_UNLIKELY(has_error_))
      new (&status_) Status(std::move(other.get_status()));
    else
      new (&value_) T(std::move(other.get_value()));

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
    other.was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveAssignFromStatusOr(StatusOr&& other) {
    RST_DCHECK(has_error_ == other.has_error_);

    if (RST_UNLIKELY(has_error_))
      get_status() = std::move(other.get_status());
    else
      get_value() = std::move(other.get_value());

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
    other.was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void CopyConstructFromT(const T& value) {
    has_error_ = false;
    new (&value_) T(value);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveConstructFromT(T&& value) {
    has_error_ = false;
    new (&value_) T(std::move(value));

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void CopyAssignFromT(const T& value) {
    RST_DCHECK(!has_error_);
    get_value() = value;

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveAssignFromT(T&& value) {
    RST_DCHECK(!has_error_);
    get_value() = std::move(value);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveConstructFromStatus(Status status) {
    has_error_ = true;
    new (&status_) Status(std::move(status));
    RST_DCHECK(get_status().error_ != nullptr);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveAssignFromStatus(Status status) {
    RST_DCHECK(has_error_);
    get_status() = std::move(status);
    RST_DCHECK(get_status().error_ != nullptr);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void Cleanup() {
    if (RST_UNLIKELY(has_error_))
      get_status().~Status();
    else
      get_value().~T();
  }

  const Status& get_status() const { return *std::launder(&status_); }
  Status& get_status() {
    return const_cast<Status&>(std::as_const(*this).get_status());
  }

  T& get_value() { return *std::launder(&value_); }

  bool has_error_;
  union {
    Status status_;
    T value_;
  };

#if RST_BUILDFLAG(DCHECK_IS_ON)
  bool was_checked_;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  RST_DISALLOW_COPY_AND_ASSIGN(StatusOr);
};

}  // namespace rst

#endif  // RST_STATUS_STATUS_OR_H_
