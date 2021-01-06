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

#include "rst/stl/reversed.h"

#include <string>
#include <vector>

#include <gtest/gtest.h>

namespace rst {

TEST(Reversed, Vector) {
  const std::vector<int> vec = {1, 2, 3, 4};
  std::string str;
  for (const auto i : Reversed(vec))
    str += std::to_string(i);
  EXPECT_EQ(str, "4321");

  std::vector<int> vec2 = {1, 2, 3, 4};
  str.clear();
  for (const auto i : Reversed(vec2))
    str += std::to_string(i);
  EXPECT_EQ(str, "4321");
}

TEST(Reversed, Array) {
  const int arr[] = {1, 2, 3, 4};
  std::string str;
  for (const auto i : Reversed(arr))
    str += std::to_string(i);
  EXPECT_EQ(str, "4321");

  int arr2[] = {1, 2, 3, 4};
  str.clear();
  for (const auto i : Reversed(arr2))
    str += std::to_string(i);
  EXPECT_EQ(str, "4321");
}

}  // namespace rst
