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

#include "rst/STL/Algorithm.h"

#include <algorithm>
#include <vector>

#include <gtest/gtest.h>

namespace rst {

TEST(Sort, Vector) {
  std::vector<int> vec = {-1, 400, 10, 0, 3, -5};
  Sort(NotNull(&vec));
  EXPECT_TRUE(std::is_sorted(std::cbegin(vec), std::cend(vec)));
}

TEST(Sort, Array) {
  int arr[] = {-1, 4, 10, 400, 3, -5};
  Sort(NotNull(&arr));
  EXPECT_TRUE(std::is_sorted(std::cbegin(arr), std::cend(arr)));
}

TEST(StableSort, Vector) {
  std::vector<int> vec = {-1, 400, 10, 0, 3, -5};
  StableSort(NotNull(&vec));
  EXPECT_TRUE(std::is_sorted(std::cbegin(vec), std::cend(vec)));
}

TEST(StableSort, Array) {
  int arr[] = {-1, 4, 10, 400, 3, -5};
  StableSort(NotNull(&arr));
  EXPECT_TRUE(std::is_sorted(std::cbegin(arr), std::cend(arr)));
}

TEST(FindIf, Vector) {
  std::vector<int> vec = {-1, 400, 10, 0, 3, -5};
  const auto it =
      FindIf(NotNull(&vec), [](const int val) -> bool { return val == -1; });
  ASSERT_NE(it, std::cend(vec));
  EXPECT_EQ(*it, -1);

  const std::vector<int> const_vec = {-1, 400, 10, 0, 3, -5};
  const auto const_it = FindIf(NotNull(&const_vec),
                               [](const int val) -> bool { return val == -1; });
  ASSERT_NE(const_it, std::cend(const_vec));
  EXPECT_EQ(*const_it, -1);
}

TEST(FindIf, Array) {
  int arr[] = {-1, 400, 10, 0, 3, -5};
  const auto it =
      FindIf(NotNull(&arr), [](const int val) -> bool { return val == -1; });
  ASSERT_NE(it, std::cend(arr));
  EXPECT_EQ(*it, -1);

  const int const_arr[] = {-1, 400, 10, 0, 3, -5};
  const auto const_it = FindIf(NotNull(&const_arr),
                               [](const int val) -> bool { return val == -1; });
  ASSERT_NE(const_it, std::cend(const_arr));
  EXPECT_EQ(*const_it, -1);
}

}  // namespace rst
