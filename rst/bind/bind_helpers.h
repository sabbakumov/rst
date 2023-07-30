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

#ifndef RST_BIND_BIND_HELPERS_H_
#define RST_BIND_BIND_HELPERS_H_

#include <functional>

namespace rst {

// Creates a null function object that will implicitly convert into any
// `std::function` type.
//
// Example:
//
//   #include "rst/bind/bind_helpers.h"
//
//   using MyCallback = std::function<void(bool arg)>;
//   void MyFunction(const MyCallback& callback) {
//     if (callback != nullptr)
//       callback(true);
//   }
//
//   MyFunction(rst::NullFunction());
//
class NullFunction {
 public:
  template <class R, class... Args>
  operator std::function<R(Args...)>() const {
    return std::function<R(Args...)>();
  }
};

// Creates a placeholder function object that will implicitly convert into any
// `std::function` type, and does nothing when called.
//
// Example:
//
//   #include "rst/bind/bind_helpers.h"
//
//   using MyCallback = std::function<void(bool arg)>;
//   void MyFunction(const MyCallback& callback) {
//     callback(true);  // Uh oh...
//   }
//
//   MyFunction(MyCallback());  // ... this will crash!
//
//   // Instead, use rst::DoNothing():
//   MyFunction(rst::DoNothing());  // Can be run, will no-op.
//
class DoNothing {
 public:
  template <class... Args>
  operator std::function<void(Args...)>() const {
    return Function<Args...>();
  }

  // Explicit way of setting a specific callback type when the compiler can't
  // deduce it.
  template <class... Args>
  static std::function<void(Args...)> Function() {
    return std::function<void(Args...)>([](Args...) {});
  }
};

}  // namespace rst

#endif  // RST_BIND_BIND_HELPERS_H_
