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
#include <type_traits>
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Macros/Macros.h"

namespace rst {

template <class T>
class Nullable;

// Microsoft GSL-like NotNull class that restricts a pointer or smart pointer to
// only hold non-null values.
template <class T>
class NotNull {
 public:
  static_assert(std::is_pointer<T>::value);

  NotNull() = delete;

  NotNull(T ptr) : ptr_(ptr) { RST_DCHECK(ptr_ != nullptr); }

  NotNull(const NotNull& other) : NotNull(other.ptr_) {}

  template <class U>
  NotNull(const NotNull<U>& other) : NotNull(other.ptr_) {
    static_assert(std::is_pointer<U>::value);
  }

  template <class U>
  NotNull(const Nullable<U>& nullable) : NotNull(nullable.ptr_) {
    static_assert(std::is_pointer<U>::value);
  }

  NotNull(std::nullptr_t) = delete;

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(U ptr) {
    static_assert(std::is_pointer<U>::value);
    ptr_ = ptr;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  NotNull& operator=(const NotNull& rhs) {
    if (this == &rhs)
      return *this;

    ptr_ = rhs.ptr_;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(const NotNull<U>& rhs) {
    static_assert(std::is_pointer<U>::value);
    if (static_cast<void*>(this) == static_cast<const void*>(&rhs))
      return *this;

    ptr_ = rhs.ptr_;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(const Nullable<U>& nullable) {
    static_assert(std::is_pointer<U>::value);
    ptr_ = nullable.ptr_;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
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
  static_assert(std::is_pointer<T>::value);

  Nullable() = default;

  Nullable(T ptr) : ptr_(ptr) {}

  Nullable(const Nullable& other) : Nullable(other.ptr_) {}

  template <class U>
  Nullable(const Nullable<U>& other) : Nullable(other.ptr_) {
    static_assert(std::is_pointer<U>::value);
  }

  template <class U>
  Nullable(const NotNull<U>& not_null) : Nullable(not_null.ptr_) {
    static_assert(std::is_pointer<U>::value);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  Nullable(std::nullptr_t) {}

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(U ptr) {
    static_assert(std::is_pointer<U>::value);
    ptr_ = ptr;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  Nullable& operator=(const Nullable& rhs) {
    if (this == &rhs)
      return *this;

    ptr_ = rhs.ptr_;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  template <class U>
  Nullable& operator=(const Nullable<U>& rhs) {
    static_assert(std::is_pointer<U>::value);
    if (static_cast<void*>(this) == static_cast<const void*>(&rhs))
      return *this;

    ptr_ = rhs.ptr_;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  template <class U>
  Nullable& operator=(const NotNull<U>& not_null) {
    static_assert(std::is_pointer<U>::value);
    ptr_ = not_null.ptr_;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  Nullable& operator=(std::nullptr_t) {
    ptr_ = nullptr;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
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

  T ptr_ = nullptr;
#if RST_BUILDFLAG(DCHECK_IS_ON)
  mutable bool was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
};

template <class T>
class NotNull<std::unique_ptr<T>> {
 public:
  NotNull() = delete;

  template <class U>
  NotNull(std::unique_ptr<U> ptr) : ptr_(std::move(ptr)) {
    RST_DCHECK(ptr_ != nullptr);
  }

  NotNull(NotNull&& other) : NotNull(other.Take()) {}

  template <class U>
  NotNull(NotNull<U>&& other) : NotNull(other.Take()) {}

  template <class U>
  NotNull(Nullable<U>&& nullable) : NotNull(nullable.Take()) {}

  NotNull(std::nullptr_t) = delete;

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(std::unique_ptr<U> ptr) {
    ptr_ = std::move(ptr);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  NotNull& operator=(NotNull&& rhs) {
    if (this == &rhs)
      return *this;

    ptr_ = rhs.Take();
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(NotNull<U>&& rhs) {
    if (static_cast<void*>(this) == static_cast<const void*>(&rhs))
      return *this;

    ptr_ = rhs.Take();
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(Nullable<U>&& nullable) {
    ptr_ = nullable.Take();
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

  Nullable(Nullable&& other) : Nullable(other.Take()) {}

  template <class U>
  Nullable(Nullable<U>&& other) : Nullable(other.Take()) {}

  template <class U>
  Nullable(NotNull<U>&& not_null) : Nullable(not_null.Take()) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  Nullable(std::nullptr_t) {}

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(std::unique_ptr<U> ptr) {
    ptr_ = std::move(ptr);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  Nullable& operator=(Nullable&& rhs) {
    if (this == &rhs)
      return *this;

    ptr_ = rhs.Take();
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  template <class U>
  Nullable& operator=(Nullable<U>&& rhs) {
    if (static_cast<void*>(this) == static_cast<const void*>(&rhs))
      return *this;

    ptr_ = rhs.Take();
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  template <class U>
  Nullable& operator=(NotNull<U>&& not_null) {
    ptr_ = not_null.Take();
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  Nullable& operator=(std::nullptr_t) {
    ptr_ = nullptr;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
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

  std::unique_ptr<T> ptr_;
#if RST_BUILDFLAG(DCHECK_IS_ON)
  mutable bool was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
};

template <class T>
class NotNull<std::shared_ptr<T>> {
 public:
  NotNull() = delete;

  template <class U>
  NotNull(std::shared_ptr<U>&& ptr) : ptr_(std::move(ptr)) {
    RST_DCHECK(ptr_ != nullptr);
  }

  template <class U>
  NotNull(const NotNull<U>&) = delete;

  NotNull(NotNull&& other) : NotNull(other.Take()) {}

  template <class U>
  NotNull(NotNull<U>&& other) : NotNull(other.Take()) {}

  template <class U>
  NotNull(const Nullable<U>&) = delete;

  template <class U>
  NotNull(Nullable<U>&& nullable) : NotNull(nullable.Take()) {}

  NotNull(std::nullptr_t) = delete;

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(std::shared_ptr<U>&& ptr) {
    ptr_ = std::move(ptr);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(const NotNull<U>&) = delete;

  NotNull& operator=(NotNull&& rhs) {
    if (this == &rhs)
      return *this;

    ptr_ = rhs.Take();
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(NotNull<U>&& rhs) {
    if (static_cast<void*>(this) == static_cast<const void*>(&rhs))
      return *this;

    ptr_ = rhs.Take();
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  template <class U>
  NotNull& operator=(const Nullable<U>&) = delete;

  template <class U>
  NotNull& operator=(Nullable<U>&& nullable) {
    ptr_ = nullable.Take();
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  NotNull Clone() const { return NotNull(std::shared_ptr(ptr_)); }

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

  RST_DISALLOW_COPY_AND_ASSIGN(NotNull);
};

template <class T>
class Nullable<std::shared_ptr<T>> {
 public:
  Nullable() = default;

  template <class U>
  Nullable(std::shared_ptr<U>&& ptr) : ptr_(std::move(ptr)) {}

  template <class U>
  Nullable(const Nullable<U>&) = delete;

  Nullable(Nullable&& other) : Nullable(other.Take()) {}

  template <class U>
  Nullable(Nullable<U>&& other) : Nullable(other.Take()) {}

  template <class U>
  Nullable(const NotNull<U>&) = delete;

  template <class U>
  Nullable(NotNull<U>&& not_null) : Nullable(not_null.Take()) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  Nullable(std::nullptr_t) {}

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(std::shared_ptr<U>&& ptr) {
    ptr_ = std::move(ptr);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  template <class U>
  Nullable& operator=(const Nullable<U>&) = delete;

  Nullable& operator=(Nullable&& rhs) {
    if (this == &rhs)
      return *this;

    ptr_ = rhs.Take();
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  template <class U>
  Nullable& operator=(Nullable<U>&& rhs) {
    if (static_cast<void*>(this) == static_cast<const void*>(&rhs))
      return *this;

    ptr_ = rhs.Take();
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  template <class U>
  Nullable& operator=(const NotNull<U>&) = delete;

  template <class U>
  Nullable& operator=(NotNull<U>&& not_null) {
    ptr_ = not_null.Take();
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  Nullable& operator=(std::nullptr_t) {
    ptr_ = nullptr;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

  Nullable Clone() const { return Nullable(std::shared_ptr(ptr_)); }

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

  std::shared_ptr<T> ptr_;
#if RST_BUILDFLAG(DCHECK_IS_ON)
  mutable bool was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  RST_DISALLOW_COPY_AND_ASSIGN(Nullable);
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
#if RST_BUILDFLAG(DCHECK_IS_ON)
  lhs.was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
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
