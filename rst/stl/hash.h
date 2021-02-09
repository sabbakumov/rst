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
#include <cstdint>

namespace rst {
namespace internal {

inline uint32_t Rotl32(const uint32_t x, const uint32_t r) {
  return (x << r) | (x >> (32 - r));
}

inline uint32_t HashCombineUint32(uint32_t h, uint32_t k) {
  static constexpr uint32_t kC1 = 0xcc9e2d51;
  static constexpr uint32_t kC2 = 0x1b873593;

  k *= kC1;
  k = Rotl32(k, 15);
  k *= kC2;

  h ^= k;
  h = Rotl32(h, 13);
  h = h * 5 + 0xe6546b64;

  return h;
}

inline uint64_t HashCombineUint64(uint64_t h, uint64_t k) {
  static constexpr uint64_t kM = 0xc6a4a7935bd1e995ULL;
  static constexpr int kR = 47;

  k *= kM;
  k ^= k >> kR;
  k *= kM;

  h ^= k;
  h *= kM;

  h += 0xe6546b64;

  return h;
}

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
//       std::hash<int> hasher;
//       const auto hash_value = hasher(point.x);
//       return rst::HashCombine(hash_value, hasher(point.y));
//     }
//   };
//
inline size_t HashCombine(const size_t h, const size_t k) {
  if constexpr (sizeof h == 8)
    return internal::HashCombineUint64(h, k);
  else if constexpr (sizeof h == 4)
#pragma warning(push)
#pragma warning(disable : 4267)
    return internal::HashCombineUint32(h, k);
#pragma warning(pop)
}

}  // namespace rst

#endif  // RST_STL_HASH_H_
