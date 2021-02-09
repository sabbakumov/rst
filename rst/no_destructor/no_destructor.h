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

#ifndef RST_NO_DESTRUCTOR_NO_DESTRUCTOR_H_
#define RST_NO_DESTRUCTOR_NO_DESTRUCTOR_H_

#include <new>
#include <utility>

#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"

namespace rst {

// Chromium-like NoDestructor class.
//
// A wrapper that makes it easy to create an object of type T with static
// storage duration that:
// * is only constructed on first access
// * never invokes the destructor
//
// Runtime constant example:
// const std::string& GetLineSeparator() {
//   static const rst::NoDestructor<std::string> s(5, '-');
//   return *s;
// }
//
// More complex initialization with a lambda:
// const std::string& GetSession() {
//   static const rst::NoDestructor<std::string> session([] {
//     std::string s(16);
//     ...
//     return s;
//   }());
//   return *session;
// }
//
// NoDestructor<T> stores the object inline, so it also avoids a pointer
// indirection and memory allocation.
//
// Note that since the destructor is never run, this will leak memory if used
// as a stack or member variable. Furthermore, a NoDestructor<T> should never
// have global scope as that may require a static initializer.
template <class T>
class NoDestructor {
 public:
  template <class... Args>
  explicit NoDestructor(Args&&... args) {
    new (storage_) T(std::forward<Args>(args)...);
  }

  explicit NoDestructor(const T& x) { new (storage_) T(x); }
  explicit NoDestructor(T&& x) { new (storage_) T(std::move(x)); }

  ~NoDestructor() = default;

  const T& operator*() const { return *get(); }
  T& operator*() { return *get(); }

  NotNull<const T*> operator->() const { return get(); }
  NotNull<T*> operator->() { return get(); }

  NotNull<const T*> get() const {
    return std::launder(reinterpret_cast<const T*>(storage_));
  }
  NotNull<T*> get() { return const_cast<T*>(std::as_const(*this).get().get()); }

 private:
  alignas(T) char storage_[sizeof(T)];
  // This is a hack to work around the fact that LSan doesn't seem to treat
  // NoDestructor as a root for reachability analysis. This means that code:
  //   static rst::NoDestructor<std::vector<int>> v({1, 2, 3});
  // is considered a leak. Using the standard leak sanitizer annotations to
  // suppress leaks doesn't work: std::vector is implicitly constructed before
  // calling the rst::NoDestructor constructor.
  //
  // Hold an explicit pointer to the placement-new'd object in leak sanitizer
  // mode to help it realize that objects allocated by the contained type are
  // still reachable.
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
  const NotNull<const T*> storage_ptr_ = get();
#endif  // __has_feature(address_sanitizer)
#elif defined(__SANITIZE_ADDRESS__)
  const NotNull<const T*> storage_ptr_ = get();
#endif  // defined(__has_feature)

  RST_DISALLOW_COPY_AND_ASSIGN(NoDestructor);
};

}  // namespace rst

#endif  // RST_NO_DESTRUCTOR_NO_DESTRUCTOR_H_
