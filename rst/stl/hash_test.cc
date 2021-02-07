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

#include "rst/stl/hash.h"

#include <cstdint>
#include <functional>
#include <unordered_set>

#include <gtest/gtest.h>

namespace {

struct Point {
  Point(const int x, const int y) : x(x), y(y) {}

  int x = 0;
  int y = 0;
};

bool operator==(const Point lhs, const Point rhs) {
  return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

}  // namespace

namespace std {

template <>
struct hash<Point> {
  size_t operator()(const Point point) const {
    std::hash<int> hasher;
    const auto hash_value = hasher(point.x);
    return rst::HashCombine(hash_value, hasher(point.y));
  }
};

}  // namespace std

namespace rst {

TEST(Hash, size_t) {
  static constexpr size_t kH = 1, kK = 1;
  const size_t result = HashCombine(kH, kK);
  (void)result;
}

TEST(Hash, Point) {
  std::unordered_set<Point> points;
  points.emplace(0, 0);
}

}  // namespace rst
