// Copyright (c) 2020, Sergey Abbakumov
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

#ifndef RST_STL_FUNCTION_H_
#define RST_STL_FUNCTION_H_

#include <functional>
#include <utility>

namespace rst {

// Like `std::move()` for `std::function` except that it also assigns `nullptr`
// to a moved argument.
//
// Example:
//
//   #include "rst/stl/function.h"
//
//   std::function<void()> f = ...;
//   auto moved_f = std::move(f);  // f is in a valid but unspecified state
//                                 // after the call.
//   std::function<void()> f = ...;
//   auto moved_f = rst::TakeFunction(std::move(f));  // f is nullptr.
//
template <class R, class... Args>
std::function<R(Args...)> TakeFunction(std::function<R(Args...)>&& f) {
  auto moved_f = std::move(f);
  f = nullptr;
  return moved_f;
}

}  // namespace rst

#endif  // RST_STL_FUNCTION_H_
