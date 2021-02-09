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

#ifndef RST_STL_REVERSED_H_
#define RST_STL_REVERSED_H_

#include <iterator>
#include <utility>

#include "rst/macros/macros.h"

namespace rst {
namespace internal {

template <class T>
class ReversedAdapter {
 public:
  using Iterator = decltype(std::rbegin(std::declval<T&>()));

  explicit ReversedAdapter(T& t) : t_(t) {}
  ReversedAdapter(const ReversedAdapter&) = default;

  Iterator begin() const { return std::rbegin(t_); }
  Iterator end() const { return std::rend(t_); }

 private:
  T& t_;

  RST_DISALLOW_ASSIGN(ReversedAdapter);
};

}  // namespace internal

// Returns a Chromium-like container adapter usable in a range-based for
// statement for iterating a reversible container in reverse order.
//
// Example:
//
//   std::vector<int> v = ...;
//   for (auto i : rst::Reversed(v)) {
//     // Iterates through v from back to front.
//   }
template <class T>
internal::ReversedAdapter<T> Reversed(T& t) {  // NOLINT(runtime/references)
  return internal::ReversedAdapter(t);
}

}  // namespace rst

#endif  // RST_STL_REVERSED_H_
