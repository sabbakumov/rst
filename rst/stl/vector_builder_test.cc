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

#include "rst/stl/vector_builder.h"

#include <memory>
#include <vector>

#include <gtest/gtest.h>

namespace rst {

TEST(VectorBuilder, Copyable) {
  {
    const std::vector<int> vec = VectorBuilder<int>().Build();
    EXPECT_TRUE(vec.empty());
  }

  {
    const std::vector<int> vec =
        VectorBuilder<int>().emplace_back(1).emplace_back(-1).Build();
    EXPECT_EQ(vec, (std::vector<int>{1, -1}));
  }
}

TEST(VectorBuilder, Movable) {
  {
    const std::vector<std::unique_ptr<int>> vec =
        VectorBuilder<std::unique_ptr<int>>().Build();
    EXPECT_TRUE(vec.empty());
  }

  {
    const std::vector<std::unique_ptr<int>> vec =
        VectorBuilder<std::unique_ptr<int>>()
            .emplace_back(std::make_unique<int>(1))
            .emplace_back(std::make_unique<int>(-1))
            .Build();
    std::vector<std::unique_ptr<int>> result;
    result.emplace_back(std::make_unique<int>(1));
    result.emplace_back(std::make_unique<int>(-1));

    ASSERT_EQ(vec.size(), 2U);
    EXPECT_EQ(vec.size(), result.size());
    EXPECT_EQ(*vec[0], *result[0]);
    EXPECT_EQ(*vec[1], *result[1]);
  }
}

}  // namespace rst
