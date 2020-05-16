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

#ifndef RST_NOT_NULL_NOT_NULL_H_
#define RST_NOT_NULL_NOT_NULL_H_

#include <cstddef>
#include <memory>
#include <type_traits>
#include <utility>

#include "rst/check/check.h"
#include "rst/macros/macros.h"

// NotNull is a Microsoft GSL-like class that restricts a pointer or a smart
// pointer to only hold non-null values. It doesn't support constructing and
// assignment from nullptr. Also it asserts that the passed pointer is not
// nullptr.
//
// Example:
//
//   void Foo(NotNul<int*>) {}
//
//   int i = 0;
//   Foo(&i);  // OK.
//   Foo(nullptr);  // Compilation error.
//   int* ptr = nullptr;
//   Foo(ptr);  // Asserts.
//
// There are specializations for std::unique_ptr and std::shared_ptr. In order
// to take the inner smart pointer use Take() method:
//   NotNull<std::unique_ptr<T>> p = ...;
//   std::unique_ptr<T> inner = std::move(p).Take();
//
// Nullable is a class that explicitly state that a pointer or a smart pointer
// can hold non-null values. It asserts that the object is checked for nullptr
// after construction.
//
// Example:
//
//   void Foo(Nullable<int*> ptr) {
//     if (ptr != nullptr)
//       *ptr = 0;  // OK.
//   }
//
//   void Bar(Nullable<int*> ptr) {
//     *ptr = 0;  // Assert.
//   }
//
// There are specializations for std::unique_ptr and std::shared_ptr. In order
// to take the inner smart pointer use Take() method:
//   Nullable<std::unique_ptr<T>> p = ...;
//   std::unique_ptr<T> inner = std::move(p).Take();
//
// Note std::move(p) is used to call Take(). It is a sign that |p| is in valid
// but unspecified state. No method other than destructor can be called.
namespace rst {

template <class T>
class Nullable;

template <class T>
class NotNull {
 public:
  static_assert(std::is_pointer<T>::value);

  NotNull() = delete;

  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(T ptr) : ptr_(ptr) { RST_DCHECK(ptr_ != nullptr); }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  NotNull(const NotNull& other) : NotNull(other.ptr_) {}
  NotNull(NotNull&& other) noexcept : NotNull(other) {}
#else
  NotNull(const NotNull&) = default;
  NotNull(NotNull&&) noexcept = default;
#endif

  template <class U>
  NotNull(const NotNull<U>& other) : NotNull(other.ptr_) {
    static_assert(std::is_pointer<U>::value);
  }

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(const Nullable<U>& nullable) : NotNull(nullable.ptr_) {
    static_assert(std::is_pointer<U>::value);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  NotNull(std::nullptr_t) = delete;  // NOLINT(runtime/explicit)

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(U ptr) {
    static_assert(std::is_pointer<U>::value);
    ptr_ = ptr;
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  NotNull& operator=(const NotNull& other) { return *this = other.ptr_; }
  NotNull& operator=(NotNull&& other) noexcept { return *this = other; }
#else
  NotNull& operator=(const NotNull&) = default;
  NotNull& operator=(NotNull&&) noexcept = default;
#endif

  template <class U>
  NotNull& operator=(const NotNull<U>& rhs) {
    static_assert(std::is_pointer<U>::value);
    return *this = rhs.ptr_;
  }

  template <class U>
  NotNull& operator=(const Nullable<U>& nullable) {
    static_assert(std::is_pointer<U>::value);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this = nullable.ptr_;
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_;
  }

  T operator->() const { return get(); }
  auto& operator*() const { return *get(); }
  auto& operator[](const size_t i) const {
    const auto p = get();
    RST_DCHECK(p != nullptr);
    const auto ptr = p + i;
    return *ptr;
  }

 private:
  template <class U>
  friend class NotNull;

  template <class U>
  friend class Nullable;

  T ptr_ = nullptr;
};

template <class T>
class Nullable {
 public:
  static_assert(std::is_pointer<T>::value);

  Nullable() = default;

  Nullable(T ptr) : ptr_(ptr) {}  // NOLINT(runtime/explicit)

  Nullable(const Nullable&) = default;
  Nullable(Nullable&&) noexcept = default;

  template <class U>
  Nullable(const Nullable<U>& other) : Nullable(other.ptr_) {
    static_assert(std::is_pointer<U>::value);
  }

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(const NotNull<U>& not_null) : Nullable(not_null.ptr_) {
    static_assert(std::is_pointer<U>::value);
  }

  Nullable(std::nullptr_t) {}  // NOLINT(runtime/explicit)

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

#if RST_BUILDFLAG(DCHECK_IS_ON)
  Nullable& operator=(const Nullable& rhs) { return *this = rhs.ptr_; }
  Nullable& operator=(Nullable&& rhs) noexcept { return *this = rhs; }
#else
  Nullable& operator=(const Nullable&) = default;
  Nullable& operator=(Nullable&&) noexcept = default;
#endif

  template <class U>
  Nullable& operator=(const Nullable<U>& rhs) {
    static_assert(std::is_pointer<U>::value);
    return *this = rhs.ptr_;
  }

  template <class U>
  Nullable& operator=(const NotNull<U>& not_null) {
    static_assert(std::is_pointer<U>::value);
    return *this = not_null.ptr_;
  }

  Nullable& operator=(std::nullptr_t) { return *this = T{nullptr}; }

  T get() const { return ptr_; }

  T operator->() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    const auto p = get();
    RST_DCHECK(p != nullptr);
    return p;
  }
  auto& operator*() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    const auto p = get();
    RST_DCHECK(p != nullptr);
    return *p;
  }
  auto& operator[](const size_t i) const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    const auto p = get();
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
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(std::unique_ptr<U> ptr) : ptr_(std::move(ptr)) {
    RST_DCHECK(ptr_ != nullptr);
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  NotNull(NotNull&& other) noexcept : NotNull(std::move(other).Take()) {}
#else
  NotNull(NotNull&&) noexcept = default;
#endif

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(NotNull<std::unique_ptr<U>>&& other) noexcept
      : NotNull(std::move(other).Take()) {}

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(Nullable<std::unique_ptr<U>>&& nullable) noexcept
      : NotNull(std::move(nullable).Take()) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  NotNull(std::nullptr_t) = delete;  // NOLINT(runtime/explicit)

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(std::unique_ptr<U> ptr) {
    ptr_ = std::move(ptr);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  NotNull& operator=(NotNull&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }
#else
  NotNull& operator=(NotNull&&) noexcept = default;
#endif

  template <class U>
  NotNull& operator=(NotNull<std::unique_ptr<U>>&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }

  template <class U>
  NotNull& operator=(Nullable<std::unique_ptr<U>>&& nullable) noexcept {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this = std::move(nullable).Take();
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T* get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_.get();
  }

  T* operator->() const { return get(); }
  auto& operator*() const { return *get(); }

  std::unique_ptr<T> Take() && {
    RST_DCHECK(ptr_ != nullptr);
    return std::move(ptr_);
  }

 private:
  std::unique_ptr<T> ptr_;

  RST_DISALLOW_COPY_AND_ASSIGN(NotNull);
};

template <class T>
class Nullable<std::unique_ptr<T>> {
 public:
  Nullable() = default;

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(std::unique_ptr<U> ptr) : ptr_(std::move(ptr)) {}

  Nullable(Nullable&&) noexcept = default;

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(Nullable<std::unique_ptr<U>>&& other) noexcept
      : Nullable(std::move(other).Take()) {}

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(NotNull<std::unique_ptr<U>>&& not_null) noexcept
      : Nullable(std::move(not_null).Take()) {}

  Nullable(std::nullptr_t) {}  // NOLINT(runtime/explicit)

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(std::unique_ptr<U> ptr) {
    ptr_ = std::move(ptr);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  Nullable& operator=(Nullable&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }
#else
  Nullable& operator=(Nullable&&) noexcept = default;
#endif

  template <class U>
  Nullable& operator=(Nullable<std::unique_ptr<U>>&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }

  template <class U>
  Nullable& operator=(NotNull<std::unique_ptr<U>>&& not_null) noexcept {
    return *this = std::move(not_null).Take();
  }

  Nullable& operator=(std::nullptr_t) { return *this = std::unique_ptr<T>(); }

  T* get() const { return ptr_.get(); }

  T* operator->() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    const auto p = get();
    RST_DCHECK(p != nullptr);
    return p;
  }
  auto& operator*() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    const auto p = get();
    RST_DCHECK(p != nullptr);
    return *p;
  }

  std::unique_ptr<T> Take() && { return std::move(ptr_); }

 private:
  template <class U>
  friend class NotNull;

  template <class U>
  friend bool operator==(const Nullable<U>& lhs, std::nullptr_t);

  std::unique_ptr<T> ptr_;
#if RST_BUILDFLAG(DCHECK_IS_ON)
  mutable bool was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  RST_DISALLOW_COPY_AND_ASSIGN(Nullable);
};

template <class T>
class NotNull<std::shared_ptr<T>> {
 public:
  NotNull() = delete;

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(std::shared_ptr<U>&& ptr) : ptr_(std::move(ptr)) {
    RST_DCHECK(ptr_ != nullptr);
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(const NotNull& other) : NotNull(std::shared_ptr(other.ptr_)) {}
#else
  NotNull(const NotNull&) = default;
#endif

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(const NotNull<std::shared_ptr<U>>& other)
      : NotNull(std::shared_ptr(other.ptr_)) {}

#if RST_BUILDFLAG(DCHECK_IS_ON)
  NotNull(NotNull&& other) noexcept : NotNull(std::move(other).Take()) {}
#else
  NotNull(NotNull&&) noexcept = default;
#endif

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(NotNull<std::shared_ptr<U>>&& other) noexcept
      : NotNull(std::move(other).Take()) {}

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(const Nullable<std::shared_ptr<U>>& nullable)
      : NotNull(std::shared_ptr(nullable.ptr_)) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  NotNull(Nullable<std::shared_ptr<U>>&& nullable) noexcept
      : NotNull(std::move(nullable).Take()) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

  NotNull(std::nullptr_t) = delete;  // NOLINT(runtime/explicit)

  ~NotNull() = default;

  template <class U>
  NotNull& operator=(std::shared_ptr<U>&& ptr) {
    ptr_ = std::move(ptr);
    RST_DCHECK(ptr_ != nullptr);
    return *this;
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  NotNull& operator=(const NotNull& rhs) {
    return *this = std::shared_ptr(rhs.ptr_);
  }
#else
  NotNull& operator=(const NotNull&) = default;
#endif

  template <class U>
  NotNull& operator=(const NotNull<std::shared_ptr<U>>& rhs) {
    return *this = std::shared_ptr(rhs.ptr_);
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  NotNull& operator=(NotNull&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }
#else
  NotNull& operator=(NotNull&&) noexcept = default;
#endif

  template <class U>
  NotNull& operator=(NotNull<std::shared_ptr<U>>&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }

  template <class U>
  NotNull& operator=(const Nullable<std::shared_ptr<U>>& nullable) {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this = std::shared_ptr(nullable.ptr_);
  }

  template <class U>
  NotNull& operator=(Nullable<std::shared_ptr<U>>&& nullable) noexcept {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(nullable.was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this = std::move(nullable).Take();
  }

  NotNull& operator=(std::nullptr_t) = delete;

  T* get() const {
    RST_DCHECK(ptr_ != nullptr);
    return ptr_.get();
  }

  T* operator->() const { return get(); }
  auto& operator*() const { return *get(); }

  std::shared_ptr<T> Take() && {
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
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(std::shared_ptr<U>&& ptr) : ptr_(std::move(ptr)) {}

  Nullable(const Nullable&) = default;

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(const Nullable<std::shared_ptr<U>>& other)
      : Nullable(std::shared_ptr(other.ptr_)) {}

  Nullable(Nullable&&) noexcept = default;

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(Nullable<std::shared_ptr<U>>&& other) noexcept
      : Nullable(std::move(other).Take()) {}

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(const NotNull<std::shared_ptr<U>>& not_null)
      : Nullable(std::shared_ptr(not_null.ptr_)) {}

  template <class U>
  // NOLINTNEXTLINE(runtime/explicit)
  Nullable(NotNull<std::shared_ptr<U>>&& not_null) noexcept
      : Nullable(std::move(not_null).Take()) {}

  Nullable(std::nullptr_t) {}  // NOLINT(runtime/explicit)

  ~Nullable() = default;

  template <class U>
  Nullable& operator=(std::shared_ptr<U>&& ptr) {
    ptr_ = std::move(ptr);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return *this;
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  Nullable& operator=(const Nullable& rhs) {
    return *this = std::shared_ptr(rhs.ptr_);
  }
#else
  Nullable& operator=(const Nullable&) = default;
#endif

  template <class U>
  Nullable& operator=(const Nullable<std::shared_ptr<U>>& rhs) {
    return *this = std::shared_ptr(rhs.ptr_);
  }

#if RST_BUILDFLAG(DCHECK_IS_ON)
  Nullable& operator=(Nullable&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }
#else
  Nullable& operator=(Nullable&&) noexcept = default;
#endif

  template <class U>
  Nullable& operator=(Nullable<std::shared_ptr<U>>&& rhs) noexcept {
    return *this = std::move(rhs).Take();
  }

  template <class U>
  Nullable& operator=(const NotNull<std::shared_ptr<U>>& not_null) {
    return *this = std::shared_ptr(not_null.ptr_);
  }

  template <class U>
  Nullable& operator=(NotNull<std::shared_ptr<U>>&& not_null) noexcept {
    return *this = std::move(not_null).Take();
  }

  Nullable& operator=(std::nullptr_t) { return *this = std::shared_ptr<T>(); }

  T* get() const { return ptr_.get(); }

  T* operator->() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    const auto p = get();
    RST_DCHECK(p != nullptr);
    return p;
  }
  auto& operator*() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    const auto p = get();
    RST_DCHECK(p != nullptr);
    return *p;
  }

  std::shared_ptr<T> Take() && { return std::move(ptr_); }

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

#endif  // RST_NOT_NULL_NOT_NULL_H_
