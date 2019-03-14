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
class Nullable;

// Microsoft GSL-like NotNull class that restricts a pointer or smart pointer to
// only hold non-null values.
template <class T>
class NotNull {
 public:
  NotNull() = delete;

  NotNull(T ptr) : ptr_(ptr) { RST_DCHECK(ptr_ != nullptr); }

  template <class U>
  NotNull(const NotNull<U>& other) : NotNull(other.ptr_) {}

  template <class U>
  NotNull(NotNull<U>&& other) : NotNull(other) {}

  template <class U>
  NotNull(const Nullable<U>& nullable) : NotNull(nullable.ptr_) {
    RST_DCHECK(nullable.was_checked_);
  }

  template <class U>
  NotNull(Nullable<U>&& nullable) : NotNull(nullable) {}

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
    ptr_ = rhs.ptr_;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(NotNull<U>&& rhs) {
    return *this = rhs;
  }

  template <class U>
  NotNull& operator=(const Nullable<U>& nullable) {
    ptr_ = nullable.ptr_;
    RST_DCHECK(nullable.was_checked_);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(Nullable<U>&& nullable) {
    return *this = nullable;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_;
  }

  T operator->() const { return get(); }
  auto& operator*() const { return *get(); }
  auto& operator[](const size_t i) const {
    const auto ptr = get() + i;
    RST_DCHECK(ptr != nullptr);
    return *ptr;
  }

 private:
  template <class U>
  friend class NotNull;

  template <class U>
  friend class Nullable;

  T ptr_;
};

template <class T>
class Nullable {
 public:
  Nullable() = default;

  Nullable(T ptr) : ptr_(ptr) {}

  template <class U>
  Nullable(const Nullable<U>& other) : Nullable(other.ptr_) {}

  template <class U>
  Nullable(Nullable<U>&& other) : Nullable(other) {}

  template <class U>
  Nullable(const NotNull<U>& not_null) : Nullable(not_null.ptr_) {
    set_was_checked(true);
  }

  template <class U>
  Nullable(NotNull<U>&& not_null) : Nullable(not_null) {}

  Nullable(std::nullptr_t) {}

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(U ptr) {
    ptr_ = ptr;
    set_was_checked(false);
    return *this;
  }

  template <class U>
  Nullable& operator=(const Nullable<U>& rhs) {
    ptr_ = rhs.ptr_;
    set_was_checked(false);
    return *this;
  }

  template <class U>
  Nullable& operator=(Nullable<U>&& rhs) {
    return *this = rhs;
  }

  template <class U>
  Nullable& operator=(const NotNull<U>& not_null) {
    ptr_ = not_null.ptr_;
    set_was_checked(true);
    return *this;
  }

  template <class U>
  Nullable& operator=(NotNull<U>&& not_null) {
    return *this = not_null;
  }

  Nullable& operator=(std::nullptr_t) {
    ptr_ = nullptr;
    set_was_checked(false);
    return *this;
  }

  T get() const { return ptr_; }

  T operator->() const {
    const auto p = get();
    RST_DCHECK(was_checked_);
    RST_DCHECK(p != nullptr);
    return p;
  }
  auto& operator*() const {
    const auto p = get();
    RST_DCHECK(was_checked_);
    RST_DCHECK(p != nullptr);
    return *p;
  }
  auto& operator[](const size_t i) const {
    const auto p = get();
    RST_DCHECK(was_checked_);
    RST_DCHECK(p != nullptr);
    const auto ptr = p + i;
    return *ptr;
  }

 private:
  template <class U>
  friend class Nullable;

  template <class U>
  friend class NotNull;

  template <class U>
  friend bool operator==(const Nullable<U>& lhs, std::nullptr_t);

#ifndef NDEBUG
  void set_was_checked(bool was_checked) { was_checked_ = was_checked; }
#else   // NDEBUG
  void set_was_checked(bool) {}
#endif  // NDEBUG

  T ptr_ = nullptr;
#ifndef NDEBUG
  mutable bool was_checked_ = false;
#endif  // NDEBUG
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
  NotNull(NotNull<U>&& other) : NotNull(other.Take()) {}

  template <class U>
  NotNull(Nullable<U>&& nullable) : NotNull(nullable.Take()) {
    RST_DCHECK(nullable.was_checked_);
  }

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

  template <class U>
  NotNull& operator=(Nullable<U>&& nullable) {
    ptr_ = nullable.Take();
    RST_DCHECK(nullable.was_checked_);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T* get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_.get();
  }

  T* operator->() const { return get(); }
  auto& operator*() const { return *get(); }

  std::unique_ptr<T> Take() {
    RST_DCHECK(ptr_ != nullptr);
    return std::move(ptr_);
  }

 private:
  std::unique_ptr<T> ptr_;
};

template <class T>
class Nullable<std::unique_ptr<T>> {
 public:
  Nullable() = default;

  template <class U>
  Nullable(std::unique_ptr<U> ptr) : ptr_(std::move(ptr)) {}

  template <class U>
  Nullable(Nullable<U>&& other) : Nullable(other.Take()) {}

  template <class U>
  Nullable(NotNull<U>&& not_null) : Nullable(not_null.Take()) {
    set_was_checked(true);
  }

  Nullable(std::nullptr_t) {}

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(std::unique_ptr<U> ptr) {
    ptr_ = std::move(ptr);
    set_was_checked(false);
    return *this;
  }

  template <class U>
  Nullable& operator=(Nullable<U>&& rhs) {
    ptr_ = rhs.Take();
    set_was_checked(false);
    return *this;
  }

  template <class U>
  Nullable& operator=(NotNull<U>&& not_null) {
    ptr_ = not_null.Take();
    set_was_checked(true);
    return *this;
  }

  Nullable& operator=(std::nullptr_t) {
    ptr_ = nullptr;
    set_was_checked(false);
    return *this;
  }

  T* get() const { return ptr_.get(); }

  T* operator->() const {
    const auto p = get();
    RST_DCHECK(was_checked_);
    RST_DCHECK(p != nullptr);
    return p;
  }
  auto& operator*() const {
    const auto p = get();
    RST_DCHECK(was_checked_);
    RST_DCHECK(p != nullptr);
    return *p;
  }

  std::unique_ptr<T> Take() { return std::move(ptr_); }

 private:
  template <class U>
  friend class NotNull;

  template <class U>
  friend bool operator==(const Nullable<U>& lhs, std::nullptr_t);

#ifndef NDEBUG
  void set_was_checked(bool was_checked) { was_checked_ = was_checked; }
#else   // NDEBUG
  void set_was_checked(bool) {}
#endif  // NDEBUG

  std::unique_ptr<T> ptr_;
#ifndef NDEBUG
  mutable bool was_checked_ = false;
#endif  // NDEBUG
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
  NotNull(const NotNull<U>& other) : NotNull(other.ptr_) {}

  template <class U>
  NotNull(NotNull<U>&& other) : NotNull(other.Take()) {}

  template <class U>
  NotNull(const Nullable<U>& nullable) : NotNull(nullable.ptr_) {
    RST_DCHECK(nullable.was_checked_);
  }

  template <class U>
  NotNull(Nullable<U>&& nullable) : NotNull(nullable.Take()) {
    RST_DCHECK(nullable.was_checked_);
  }

  NotNull(std::nullptr_t) = delete;

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(std::shared_ptr<U> ptr) {
    ptr_ = std::move(ptr);
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
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(const Nullable<U>& nullable) {
    ptr_ = nullable.ptr_;
    RST_DCHECK(nullable.was_checked_);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(Nullable<U>&& nullable) {
    ptr_ = nullable.Take();
    RST_DCHECK(nullable.was_checked_);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T* get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_.get();
  }

  T* operator->() const { return get(); }
  auto& operator*() const { return *get(); }

  std::shared_ptr<T> Take() {
    RST_DCHECK(ptr_ != nullptr);
    return std::move(ptr_);
  }

 private:
  template <class U>
  friend class NotNull;

  template <class U>
  friend class Nullable;

  std::shared_ptr<T> ptr_;
};

template <class T>
class Nullable<std::shared_ptr<T>> {
 public:
  Nullable() = default;

  template <class U>
  Nullable(std::shared_ptr<U> ptr) : ptr_(std::move(ptr)) {}

  template <class U>
  Nullable(const Nullable<U>& other) : ptr_(other.ptr_) {}

  template <class U>
  Nullable(Nullable<U>&& other) : Nullable(other.Take()) {}

  template <class U>
  Nullable(const NotNull<U>& not_null) : ptr_(not_null.ptr_) {
    set_was_checked(true);
  }

  template <class U>
  Nullable(NotNull<U>&& not_null) : Nullable(not_null.Take()) {
    set_was_checked(true);
  }

  Nullable(std::nullptr_t) {}

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(std::shared_ptr<U> ptr) {
    ptr_ = std::move(ptr);
    set_was_checked(false);
    return *this;
  }

  template <class U>
  Nullable& operator=(const Nullable<U>& rhs) {
    ptr_ = rhs.ptr_;
    set_was_checked(false);
    return *this;
  }

  template <class U>
  Nullable& operator=(Nullable<U>&& rhs) {
    ptr_ = rhs.Take();
    set_was_checked(false);
    return *this;
  }

  Nullable& operator=(std::nullptr_t) {
    ptr_ = nullptr;
    set_was_checked(false);
    return *this;
  }

  T* get() const { return ptr_.get(); }

  T* operator->() const {
    const auto p = get();
    RST_DCHECK(was_checked_);
    RST_DCHECK(p != nullptr);
    return p;
  }
  auto& operator*() const {
    const auto p = get();
    RST_DCHECK(was_checked_);
    RST_DCHECK(p != nullptr);
    return *p;
  }

  std::shared_ptr<T> Take() { return std::move(ptr_); }

 private:
  template <class U>
  friend class Nullable;

  template <class U>
  friend class NotNull;

  template <class U>
  friend bool operator==(const Nullable<U>& lhs, std::nullptr_t);

#ifndef NDEBUG
  void set_was_checked(bool was_checked) { was_checked_ = was_checked; }
#else   // NDEBUG
  void set_was_checked(bool) {}
#endif  // NDEBUG

  std::shared_ptr<T> ptr_;
#ifndef NDEBUG
  mutable bool was_checked_ = false;
#endif  // NDEBUG
};

template <class T, class U>
bool operator==(const NotNull<T>& lhs, const NotNull<U>& rhs) {
  return lhs.get() == rhs.get();
}

template <class T, class U>
bool operator!=(const NotNull<T>& lhs, const NotNull<U>& rhs) {
  return !(lhs == rhs);
}

template <class T, class U>
bool operator<(const NotNull<T>& lhs, const NotNull<U>& rhs) {
  return lhs.get() < rhs.get();
}

template <class T>
bool operator==(const Nullable<T>& lhs, std::nullptr_t) {
#ifndef NDEBUG
  lhs.was_checked_ = true;
#endif  // NDEBUG
  return lhs.ptr_ == nullptr;
}

template <class T>
bool operator==(std::nullptr_t, const Nullable<T>& rhs) {
  return rhs == nullptr;
}

template <class T>
bool operator!=(const Nullable<T>& lhs, std::nullptr_t) {
  return !(lhs == nullptr);
}

template <class T>
bool operator!=(std::nullptr_t, const Nullable<T>& rhs) {
  return rhs != nullptr;
}

template <class T, class U>
bool operator==(const Nullable<T>& lhs, const Nullable<U>& rhs) {
  return lhs.get() == rhs.get();
}

template <class T, class U>
bool operator!=(const Nullable<T>& lhs, const Nullable<U>& rhs) {
  return !(lhs == rhs);
}

template <class T, class U>
bool operator<(const Nullable<T>& lhs, const Nullable<U>& rhs) {
  return lhs.get() < rhs.get();
}

template <class T, class U>
bool operator==(const Nullable<T>& lhs, U* rhs) {
  return lhs.get() == rhs;
}

template <class T, class U>
bool operator==(U* lhs, const Nullable<T>& rhs) {
  return rhs == lhs;
}

template <class T, class U>
bool operator!=(const Nullable<T>& lhs, U* rhs) {
  return !(lhs == rhs);
}

template <class T, class U>
bool operator!=(U* lhs, const Nullable<T>& rhs) {
  return !(rhs == lhs);
}

}  // namespace rst

#endif  // RST_NOTNULL_NOTNULL_H_
