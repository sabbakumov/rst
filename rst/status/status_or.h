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

#include <cstdint>
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
//   StatusOr<std::string> foo = Foo();
//   if (foo.err())
//     return std::move(foo).TakeStatus();
//
//   // Or:
//   RST_TRY_CREATE(auto, foo, Foo());
//   RST_TRY_CREATE(StatusOr<std::string>, foo, Foo());
//   ...
//   RST_TRY_ASSIGN(foo, Foo());
//
//   std::cout << *foo << ", " << foo->size() << std::endl;
//
template <class T>
class [[nodiscard]] StatusOr {
 public:
  StatusOr() = delete;

  StatusOr(StatusOr && other) noexcept {
    MoveConstructFromStatusOr(std::move(other));
  }

  // Stores success value.
  template <class U,
            class =
                typename std::enable_if<std::is_trivially_copyable<U>{}>::type>
  StatusOr(const U& value) {  // NOLINT(runtime/explicit)
    static_assert(std::is_same<T, U>::value);
    CopyConstructFromT(value);
  }
  StatusOr(T && value) {  // NOLINT(runtime/explicit)
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

    if (RST_LIKELY(type_ == rhs.type_)) {
      MoveAssignFromStatusOr(std::move(rhs));
    } else {
      Cleanup();
      MoveConstructFromStatusOr(std::move(rhs));
    }

    return *this;
  }

  // Asserts that it was checked before and sets as not checked.
  template <class U,
            class =
                typename std::enable_if<std::is_trivially_copyable<U>{}>::type>
  StatusOr& operator=(const U& value) {
    static_assert(std::is_same<T, U>::value);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    switch (static_cast<Type>(RST_LIKELY_EQ(
        static_cast<typename std::underlying_type<Type>::type>(type_),
        static_cast<typename std::underlying_type<Type>::type>(Type::kOk)))) {
      case Type::kOk:
        CopyAssignFromT(value);
        break;
      case Type::kError:
        Cleanup();
        CopyConstructFromT(value);
        break;
    }

    return *this;
  }

  StatusOr& operator=(T&& value) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    switch (static_cast<Type>(RST_LIKELY_EQ(
        static_cast<typename std::underlying_type<Type>::type>(type_),
        static_cast<typename std::underlying_type<Type>::type>(Type::kOk)))) {
      case Type::kOk:
        MoveAssignFromT(std::move(value));
        break;
      case Type::kError:
        Cleanup();
        MoveConstructFromT(std::move(value));
        break;
    }

    return *this;
  }

  // Asserts that it was checked before and sets as not checked.
  StatusOr& operator=(Status status) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(status.error_ != nullptr);

    switch (static_cast<Type>(RST_LIKELY_EQ(
        static_cast<typename std::underlying_type<Type>::type>(type_),
        static_cast<typename std::underlying_type<Type>::type>(Type::kOk)))) {
      case Type::kOk:
        Cleanup();
        MoveConstructFromStatus(std::move(status));
        break;
      case Type::kError:
        MoveAssignFromStatus(std::move(status));
        break;
    }

    return *this;
  }

  // Sets the object to be checked and returns whether the status is error
  // object.
  bool err() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
    if (RST_UNLIKELY(type_ == Type::kError))
      (void)status_.err();
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return type_ == Type::kError;
  }

  // Asserts that it was checked.
  T& operator*() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(type_ == Type::kOk);

    return value_;
  }

  // Asserts that it was checked.
  NotNull<T*> operator->() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(type_ == Type::kOk);

    return &value_;
  }

  // Asserts that it was checked.
  Status TakeStatus()&& {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(type_ == Type::kError);

    return std::move(status_);
  }

  // Asserts that it was checked.
  const Status& status() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(type_ == Type::kError);

    return status_;
  }

  // Sets the object to be checked.
  void Ignore() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
    if (RST_UNLIKELY(type_ == Type::kError))
      status_.was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

 private:
  enum class Type : int8_t {
    kOk,
    kError,
  };

  void MoveConstructFromStatusOr(StatusOr && other) {
    type_ = other.type_;

    switch (static_cast<Type>(RST_LIKELY_EQ(
        static_cast<typename std::underlying_type<Type>::type>(type_),
        static_cast<typename std::underlying_type<Type>::type>(Type::kOk)))) {
      case Type::kOk:
        new (&value_) T(std::move(other.value_));
        break;
      case Type::kError:
        new (&status_) Status(std::move(other.status_));
        break;
    }

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
    other.was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveAssignFromStatusOr(StatusOr && other) {
    RST_DCHECK(type_ == other.type_);

    switch (static_cast<Type>(RST_LIKELY_EQ(
        static_cast<typename std::underlying_type<Type>::type>(type_),
        static_cast<typename std::underlying_type<Type>::type>(Type::kOk)))) {
      case Type::kOk:
        value_ = std::move(other.value_);
        break;
      case Type::kError:
        status_ = std::move(other.status_);
        break;
    }

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
    other.was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void CopyConstructFromT(const T& value) {
    type_ = Type::kOk;
    new (&value_) T(value);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveConstructFromT(T && value) {
    type_ = Type::kOk;
    new (&value_) T(std::move(value));

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void CopyAssignFromT(const T& value) {
    RST_DCHECK(type_ == Type::kOk);
    value_ = value;

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveAssignFromT(T && value) {
    RST_DCHECK(type_ == Type::kOk);
    value_ = std::move(value);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveConstructFromStatus(Status status) {
    type_ = Type::kError;
    new (&status_) Status(std::move(status));
    RST_DCHECK(status_.error_ != nullptr);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void MoveAssignFromStatus(Status status) {
    RST_DCHECK(type_ == Type::kError);
    status_ = std::move(status);
    RST_DCHECK(status_.error_ != nullptr);

#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  void Cleanup() {
    switch (static_cast<Type>(RST_LIKELY_EQ(
        static_cast<typename std::underlying_type<Type>::type>(type_),
        static_cast<typename std::underlying_type<Type>::type>(Type::kOk)))) {
      case Type::kOk:
        value_.~T();
        break;
      case Type::kError:
        status_.~Status();
        break;
    }
  }

  Type type_;
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
