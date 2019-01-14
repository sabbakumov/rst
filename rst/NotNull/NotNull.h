// Copyright (c) 2019, Sergey Abbakumov
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

#ifndef RST_NOTNULL_NOTNULL_H_
#define RST_NOTNULL_NOTNULL_H_

#include <cstddef>
#include <memory>
#include <utility>

#include "rst/Check/Check.h"

namespace rst {

template <class T>
using Nullable = T;

// Microsoft GSL-like NotNull class that restricts a pointer or smart pointer to
// only hold non-null values.
template <class T>
class NotNull {
 public:
  NotNull() = delete;

  NotNull(T ptr) : ptr_(ptr) {
    RST_DCHECK(ptr_ != nullptr);
  }

  template <class U>
  NotNull(const NotNull<U>& rhs) : NotNull(rhs.get()) {}

  template <class U>
  NotNull(NotNull<U>&& rhs) : NotNull(rhs) {}

  NotNull(std::nullptr_t) = delete;

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(U ptr) {
    ptr_ = ptr;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(const NotNull<U>& rhs) {
    ptr_ = rhs.get();
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(NotNull<U>&& rhs) {
    return *this = rhs;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_;
  }

  T operator->() const { return get(); }
  auto& operator*() const { return *ptr_; }

 private:
  T ptr_;
};

template <class T>
class NotNull<std::unique_ptr<T>> {
 public:
  NotNull() = delete;

  template <class U>
  NotNull(std::unique_ptr<U> ptr) : ptr_(std::move(ptr)) {
    RST_DCHECK(ptr_ != nullptr);
  }

  template <class U>
  NotNull(NotNull<U>&& rhs) : NotNull(rhs.Take()) {}

  NotNull(std::nullptr_t) = delete;

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(std::unique_ptr<U> ptr) {
    ptr_ = std::move(ptr);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(NotNull<U>&& rhs) {
    ptr_ = rhs.Take();
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T* get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_.get();
  }

  T* operator->() const { return get(); }
  auto& operator*() const { return *ptr_; }

  std::unique_ptr<T> Take() {
    RST_DCHECK(ptr_ != nullptr);
    return std::move(ptr_);
  }

 private:
  std::unique_ptr<T> ptr_;
};

template <class T>
class NotNull<std::shared_ptr<T>> {
 public:
  NotNull() = delete;

  template <class U>
  NotNull(std::shared_ptr<U> ptr) : ptr_(std::move(ptr)) {
    RST_DCHECK(ptr_ != nullptr);
  }

  template <class U>
  NotNull(const NotNull<U>& rhs) : ptr_(rhs.ptr_) {
    RST_DCHECK(ptr_ != nullptr);
  }

  template <class U>
  NotNull(NotNull<U>&& rhs) : NotNull(rhs.Take()) {}

  NotNull(std::nullptr_t) = delete;

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(std::shared_ptr<U> ptr) {
    ptr_ = ptr;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(const NotNull<U>& rhs) {
    ptr_ = rhs.ptr_;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(NotNull<U>&& rhs) {
    ptr_ = rhs.Take();
    return *this;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T* get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_.get();
  }

  T* operator->() const { return get(); }
  auto& operator*() const { return *ptr_; }

  std::shared_ptr<T> Take() {
    RST_DCHECK(ptr_ != nullptr);
    return std::move(ptr_);
  }

 private:
  template <class U>
  friend class NotNull;

  std::shared_ptr<T> ptr_;
};

template <class T, class U>
bool operator==(const NotNull<T>& lhs, const NotNull<U>& rhs) {
  return lhs.get() == rhs.get();
}

template <class T, class U>
bool operator!=(const NotNull<T>& lhs, const NotNull<U>& rhs) {
  return !(lhs == rhs);
}

}  // namespace rst

#endif  // RST_NOTNULL_NOTNULL_H_
