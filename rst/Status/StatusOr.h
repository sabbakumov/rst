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

#ifndef RST_STATUS_STATUSOR_H_
#define RST_STATUS_STATUSOR_H_

#include <new>
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Noncopyable/Noncopyable.h"
#include "rst/Status/Status.h"

namespace rst {

template <class T>
class [[nodiscard]] StatusOr : public NonCopyable {
 public:
  StatusOr() = delete;

  StatusOr(StatusOr&& rhs) : status_(std::move(rhs.status_)) {
    if (status_.error_info_ == nullptr)
      Construct(std::move(rhs.value_));
    rhs.set_was_checked(true);
  }

  StatusOr(const T& value) { Construct(value); }

  StatusOr(T&& value) { Construct(std::move(value)); }

  StatusOr(Status status) : status_(std::move(status)) {
    RST_DCHECK(status_.error_info_ != nullptr);
  }

  ~StatusOr() {
    RST_DCHECK(was_checked_);

    if (status_.error_info_ == nullptr)
      Destruct();
  }

  StatusOr& operator=(StatusOr&& rhs) {
    RST_DCHECK(was_checked_);

    if (this == &rhs)
      return *this;

    if (status_.error_info_ == nullptr)
      Destruct();

    status_ = std::move(rhs.status_);

    if (status_.error_info_ == nullptr)
      Construct(std::move(rhs.value_));

    set_was_checked(false);
    rhs.set_was_checked(true);

    return *this;
  }

  StatusOr& operator=(const T& value) {
    RST_DCHECK(was_checked_);

    if (status_.error_info_ == nullptr)
      Destruct();

    status_ = Status::OK();

    Construct(value);

    set_was_checked(false);

    return *this;
  }

  StatusOr& operator=(T&& value) {
    RST_DCHECK(was_checked_);

    if (status_.error_info_ == nullptr)
      Destruct();

    status_ = Status::OK();

    Construct(std::move(value));

    set_was_checked(false);

    return *this;
  }

  StatusOr& operator=(Status status) {
    RST_DCHECK(was_checked_);
    RST_DCHECK(status.error_info_ != nullptr);

    if (status.error_info_ == nullptr)
      Destruct();

    status_ = std::move(status);
    set_was_checked(false);

    return *this;
  }

  bool ok() const {
    set_was_checked(true);
    return status_.ok();
  }

  T& operator*() {
    RST_DCHECK(was_checked_);
    RST_DCHECK(status_.error_info_ == nullptr);

    return value_;
  }

  T* operator->() {
    RST_DCHECK(was_checked_);
    RST_DCHECK(status_.error_info_ == nullptr);

    return &value_;
  }

  Status& status() {
    RST_DCHECK(was_checked_);
    RST_DCHECK(status_.error_info_ != nullptr);

    return status_;
  }

  const Status& status() const {
    RST_DCHECK(was_checked_);
    RST_DCHECK(status_.error_info_ != nullptr);

    return status_;
  }

  void Ignore() const {
    set_was_checked(true);
    status_.set_was_checked(true);
  }

 private:
  void Construct(const T& value) { new (&value_) T(value); }

  void Construct(T&& value) { new (&value_) T(std::move(value)); }

  void Destruct() { value_.~T(); }

#ifndef NDEBUG
  void set_was_checked(bool was_checked) const { was_checked_ = was_checked; }
#else   // NDEBUG
  void set_was_checked(bool) const {}
#endif  // NDEBUG

  Status status_;
  union {
    T value_;
  };

#ifndef NDEBUG
  mutable bool was_checked_ = false;
#endif  // NDEBUG
};

}  // namespace rst

#endif  // RST_STATUS_STATUSOR_H_
