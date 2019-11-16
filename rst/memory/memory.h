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

#ifndef RST_MEMORY_MEMORY_H_
#define RST_MEMORY_MEMORY_H_

#include <memory>
#include <type_traits>

#include "rst/not_null/not_null.h"

namespace rst {

// Chromium-like WrapUnique.
//
// Helper to transfer ownership of a raw pointer to a std::unique_ptr<T>. It is
// usually used inside factory methods.
//
// Example:
//   class Foo {
//    public:
//     std::unique_ptr<Foo> Create() {
//       return WrapUnique(NotNull(new Foo()));
//     }
//
//    private:
//     Foo() = default;
//   };
//
template <class T>
NotNull<std::unique_ptr<T>> WrapUnique(const NotNull<T*> ptr) {
  static_assert(!std::is_array<T>::value);
  return std::unique_ptr<T>(ptr.get());
}

}  // namespace rst

#endif  // RST_MEMORY_MEMORY_H_
