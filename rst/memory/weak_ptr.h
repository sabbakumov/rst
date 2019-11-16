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

#ifndef RST_MEMORY_WEAK_PTR_H_
#define RST_MEMORY_WEAK_PTR_H_

#include <cstddef>
#include <memory>
#include <utility>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"

// Chromium-based WeakPtr.
//
// Weak pointers are pointers to an object that do not affect its lifetime, and
// which may be invalidated (i.e. reset to nullptr) by the object, or its
// owner, at any time, most commonly when the object is about to be deleted.
//
// Weak pointers are useful when an object needs to be accessed safely by one
// or more objects other than its owner, and those callers can cope with the
// object vanishing and e.g. tasks posted to it being silently dropped.
// Reference-counting such an object would complicate the ownership graph and
// make it harder to reason about the object's lifetime.
//
// Example:
//
//   class Controller {
//    public:
//     void SpawnWorker() { Worker::StartNew(weak_factory_.GetWeakPtr()); }
//     void WorkComplete(const Result& result) { ... }
//
//    private:
//     // Member variables should appear before the WeakPtrFactory, to ensure
//     // that any WeakPtrs to Controller are invalidated before its members
//     // variable's destructors are executed, rendering them invalid.
//     WeakPtrFactory<Controller> weak_factory_{this};
//   };
//
//   class Worker {
//    public:
//     static void StartNew(WeakPtr<Controller>&& controller) {
//       auto worker = new Worker(std::move(controller));
//       // Asynchronous processing...
//     }
//
//    private:
//     Worker(WeakPtr<Controller>&& controller)
//         : controller_(std::move(controller)) {}
//
//     void DidCompleteAsynchronousProcessing(const Result& result) {
//       Nullable<Controller*> controller = controller_.get();
//       if (controller != nullptr)
//         controller->WorkComplete(result);
//     }
//
//     WeakPtr<Controller> controller_;
//   };
//
// With this implementation a caller may use SpawnWorker() to dispatch multiple
// Workers and subsequently delete the Controller, without waiting for all
// Workers to have completed.
namespace rst {
namespace internal {

struct Flag {};

}  // namespace internal

// Holds a weak reference to T*.
template <class T>
class WeakPtr {
 public:
  WeakPtr() = default;
  WeakPtr(std::nullptr_t) {}

  WeakPtr(std::weak_ptr<internal::Flag>&& flag, const NotNull<T*> ptr)
      : flag_(std::move(flag)), ptr_(ptr.get()) {}

  WeakPtr(const WeakPtr&) = default;

  template <class U>
  WeakPtr(const WeakPtr<U>& other) : flag_(other.flag_), ptr_(other.ptr_) {}

  WeakPtr(WeakPtr&&) noexcept = default;

  template <class U>
  WeakPtr(WeakPtr<U>&& other) noexcept
      : flag_(std::move(other.flag_)), ptr_(other.ptr_) {}

  WeakPtr& operator=(const WeakPtr&) = default;

  template <class U>
  WeakPtr& operator=(const WeakPtr<U>& rhs) {
    flag_ = rhs.flag_;
    ptr_ = rhs.ptr_;
    return *this;
  }

  WeakPtr& operator=(WeakPtr&&) noexcept = default;

  template <class U>
  WeakPtr& operator=(WeakPtr<U>&& rhs) noexcept {
    flag_ = std::move(rhs.flag_);
    ptr_ = rhs.ptr_;
    return *this;
  }

  WeakPtr& operator=(std::nullptr_t) {
    flag_.reset();
    ptr_ = nullptr;
    return *this;
  }

  Nullable<T*> get() const { return IsValid() ? ptr_ : nullptr; }

 private:
  template <class U>
  friend class WeakPtr;

  bool IsValid() const { return !flag_.expired(); }

  std::weak_ptr<internal::Flag> flag_;
  Nullable<T*> ptr_;
};

// A class may be composed of a WeakPtrFactory and thereby control how it
// exposes weak pointers to itself. This is helpful if you only need weak
// pointers within the implementation of a class.
template <class T>
class WeakPtrFactory {
 public:
  explicit WeakPtrFactory(const NotNull<T*> ptr) : ptr_(ptr) {}

  WeakPtr<T> GetWeakPtr() const { return WeakPtr<T>(flag_, ptr_); }

 private:
  const std::shared_ptr<internal::Flag> flag_ =
      std::make_shared<internal::Flag>();
  const NotNull<T*> ptr_;

  RST_DISALLOW_COPY_AND_ASSIGN(WeakPtrFactory);
};

}  // namespace rst

#endif  // RST_MEMORY_WEAK_PTR_H_
