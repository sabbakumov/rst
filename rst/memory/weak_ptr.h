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
#include <type_traits>
#include <utility>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"

// Chromium-based `WeakPtr`.
//
// Weak pointers are pointers to an object that do not affect its lifetime, and
// which may be invalidated (i.e. reset to `nullptr`) by the object, or its
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
//   #include "rst/memory/weak_ptr.h"
//
//   class Controller : public rst::SupportsWeakPtr<Controller> {
//    public:
//     void SpawnWorker() { Worker::StartNew(AsWeakPtr()); }
//     void WorkComplete(const Result& result) {}
//   };
//
//   class Worker {
//    public:
//     static void StartNew(rst::WeakPtr<Controller>&& controller) {
//       new Worker(std::move(controller));
//       // Asynchronous processing...
//     }
//
//    private:
//     Worker(rst::WeakPtr<Controller>&& controller)
//         : controller_(std::move(controller)) {}
//
//     void DidCompleteAsynchronousProcessing(const Result& result) {
//       rst::Nullable<Controller*> controller = controller_.GetNullable();
//       if (controller != nullptr)
//         controller->WorkComplete(result);
//     }
//
//     const rst::WeakPtr<Controller> controller_;
//   };
//
// With this implementation a caller may use `SpawnWorker()` to dispatch
// multiple Workers and subsequently delete the Controller, without waiting for
// all Workers to have completed.
namespace rst {

template <class T>
class WeakPtr;

template <class T>
class SupportsWeakPtr;

namespace internal {

struct Flag {};

// This class provides a common implementation of common functions that would
// otherwise get instantiated separately for each distinct instantiation of
// SupportsWeakPtr.
class SupportsWeakPtrBase {
 public:
  // A safe static downcast of a WeakPtr<Base> to WeakPtr<Derived>. This
  // conversion will only compile if there is exists a Base which inherits from
  // SupportsWeakPtr<Base>.
  template <class Derived>
  static WeakPtr<Derived> StaticAsWeakPtr(const NotNull<Derived*> p) {
    static_assert(std::is_base_of_v<SupportsWeakPtrBase, Derived>,
                  "AsWeakPtr argument must inherit from SupportsWeakPtr");
    return AsWeakPtrImpl<Derived>(p.get());
  }

 private:
  // Uses type inference to find a Base of Derived which is an instance of
  // SupportsWeakPtr<Base>. We can then safely static_cast the Base* to a
  // Derived*.
  template <class Derived, class Base>
  static WeakPtr<Derived> AsWeakPtrImpl(SupportsWeakPtr<Base>* p) {
    WeakPtr<Base> ptr = p->AsWeakPtr();
    return WeakPtr<Derived>(std::move(ptr.flag_),
                            static_cast<Derived*>(ptr.ptr_.get()));
  }

  template <class Derived, class Base>
  static WeakPtr<const Derived> AsWeakPtrImpl(const SupportsWeakPtr<Base>* p) {
    WeakPtr<const Base> ptr = p->AsWeakPtr();
    return WeakPtr<const Derived>(std::move(ptr.flag_),
                                  static_cast<const Derived*>(ptr.ptr_.get()));
  }
};

}  // namespace internal

// Holds a weak reference to T*.
template <class T>
class WeakPtr {
 public:
  WeakPtr() = default;
  WeakPtr(std::nullptr_t) {}  // NOLINT(runtime/explicit)

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

  Nullable<T*> GetNullable() const { return IsValid() ? ptr_ : nullptr; }

 private:
  template <class U>
  friend class WeakPtr;

  template <class U>
  friend class SupportsWeakPtr;

  friend class internal::SupportsWeakPtrBase;

  WeakPtr(std::weak_ptr<internal::Flag>&& flag, const NotNull<T*> ptr)
      : flag_(std::move(flag)), ptr_(ptr.get()) {}

  bool IsValid() const { return !flag_.expired(); }

  std::weak_ptr<internal::Flag> flag_;
  Nullable<T*> ptr_;
};

// To use WeakPtr a class should extend from SupportsWeakPtr.
template <class T>
class SupportsWeakPtr : public internal::SupportsWeakPtrBase {
 public:
  WeakPtr<T> AsWeakPtr() { return WeakPtr<T>(flag_, static_cast<T*>(this)); }
  WeakPtr<const T> AsWeakPtr() const {
    return WeakPtr<const T>(flag_, static_cast<const T*>(this));
  }

 protected:
  SupportsWeakPtr() = default;
  ~SupportsWeakPtr() = default;

 private:
  const std::shared_ptr<internal::Flag> flag_ =
      std::make_shared<internal::Flag>();

  RST_DISALLOW_COPY_AND_ASSIGN(SupportsWeakPtr);
};

// Uses type deduction to safely return a WeakPtr<Derived> when Derived doesn't
// directly extend SupportsWeakPtr<Derived>, instead it extends a Base that
// extends SupportsWeakPtr<Base>.
//
// Example:
//
//   #include "rst/memory/weak_ptr.h"
//
//   class Base : public rst::SupportsWeakPtr<Base> {};
//   class Derived : public Base {};
//
//   Derived derived;
//   rst::WeakPtr<Derived> ptr = rst::AsWeakPtr(&derived);
//
// Note that the following doesn't work (invalid type conversion) since
// Derived::AsWeakPtr() is WeakPtr<Base> SupportsWeakPtr<Base>::AsWeakPtr(),
// and there's no way to safely cast WeakPtr<Base> to WeakPtr<Derived> at the
// caller.
//
//   rst::WeakPtr<Derived> ptr = derived.AsWeakPtr();  // Fails.
//
template <class Derived>
WeakPtr<Derived> AsWeakPtr(Derived* p) {
  return AsWeakPtr(NotNull(p));
}

template <class Derived>
WeakPtr<Derived> AsWeakPtr(const NotNull<Derived*> p) {
  return internal::SupportsWeakPtrBase::StaticAsWeakPtr<Derived>(p);
}

}  // namespace rst

#endif  // RST_MEMORY_WEAK_PTR_H_
