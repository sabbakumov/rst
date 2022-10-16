// Copyright (c) 2021, Sergey Abbakumov
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

#ifndef RST_STL_HASH_H_
#define RST_STL_HASH_H_

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <type_traits>

namespace rst {
namespace internal {

class Hash {
 public:
  template <class T>
  Hash(T&& value)
      : hash_(
            std::hash<std::remove_cv_t<std::remove_reference_t<T>>>()(value)) {}

  size_t hash() const { return hash_; }

 private:
  const size_t hash_ = 0;
};

}  // namespace internal

// Boost-like functions to create a hash value from several variables.
//
// Example:
//
//   struct Point {
//     Point(const int x, const int y) : x(x), y(y) {}
//
//     int x = 0;
//     int y = 0;
//   };
//
//   bool operator==(const Point lhs, const Point rhs) {
//     return (lhs.x == rhs.x) && (lhs.y == rhs.y);
//   }
//
//   namespace std {
//
//   template <>
//   struct hash<Point> {
//     size_t operator()(const Point point) const {
//       return rst::HashCombine({point.x, point.y});
//     }
//   };
//
//   }  // namespace std
//
size_t HashCombine(std::initializer_list<internal::Hash> hashes);

}  // namespace rst

#endif  // RST_STL_HASH_H_
