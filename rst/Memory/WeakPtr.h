// Copyright (c) 2018, Sergey Abbakumov
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

#ifndef RST_MEMORY_WEAKPTR_H_
#define RST_MEMORY_WEAKPTR_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Macros/Macros.h"

// Chromium-based WeakPtr.
namespace rst {
namespace internal {

struct Flag {};

}  // namespace internal

template <class T>
class WeakPtr {
 public:
  WeakPtr() = default;
  explicit WeakPtr(std::nullptr_t) {}

  WeakPtr(std::weak_ptr<internal::Flag> flag, T* ptr)
      : flag_(std::move(flag)), ptr_(ptr) {
    RST_DCHECK(ptr != nullptr);
  }

  template <typename U>
  WeakPtr(const WeakPtr<U>& rhs) : flag_(rhs.flag_) {
    T* t = static_cast<U*>(rhs.ptr_);
    ptr_ = t;
  }

  template <typename U>
  WeakPtr(WeakPtr<U>&& rhs) : flag_(std::move(rhs.flag_)) {
    T* t = static_cast<U*>(rhs.ptr_);
    ptr_ = t;
  }

  T* get() const { return IsValid() ? ptr_ : nullptr; }

  T& operator*() const {
    RST_DCHECK(get() != nullptr);
    return *get();
  }

  T* operator->() const {
    RST_DCHECK(get() != nullptr);
    return get();
  }

 private:
  template <typename U>
  friend class WeakPtr;

  bool IsValid() const { return !flag_.expired(); }

  std::weak_ptr<internal::Flag> flag_;
  T* ptr_ = nullptr;
};

template <class T>
bool operator==(const WeakPtr<T>& weak_ptr, std::nullptr_t) {
  return weak_ptr.get() == nullptr;
}

template <class T>
bool operator==(std::nullptr_t, const WeakPtr<T>& weak_ptr) {
  return weak_ptr == nullptr;
}

template <class T>
bool operator!=(const WeakPtr<T>& weak_ptr, std::nullptr_t) {
  return !(weak_ptr == nullptr);
}

template <class T>
bool operator!=(std::nullptr_t, const WeakPtr<T>& weak_ptr) {
  return weak_ptr != nullptr;
}

template <class T>
class WeakPtrFactory {
 public:
  explicit WeakPtrFactory(T* ptr) : ptr_(ptr) { RST_DCHECK(ptr != nullptr); }

  WeakPtrFactory(WeakPtrFactory&&) = delete;
  WeakPtrFactory& operator=(WeakPtrFactory&&) = delete;

  WeakPtr<T> GetWeakPtr() const { return WeakPtr<T>(flag_, ptr_); }

 private:
  std::shared_ptr<internal::Flag> flag_ = std::make_shared<internal::Flag>();
  T* ptr_ = nullptr;

  RST_DISALLOW_COPY_AND_ASSIGN(WeakPtrFactory);
};

}  // namespace rst

#endif  // RST_MEMORY_WEAKPTR_H_
