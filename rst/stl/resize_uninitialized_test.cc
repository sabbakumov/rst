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

#include "rst/stl/resize_uninitialized.h"

#include <cstddef>

#include <gtest/gtest.h>

#include "rst/not_null/not_null.h"

namespace rst {
namespace {

int g_resize_call_count = 0;
int g_resize_default_init_call_count = 0;

struct ResizableString {
  void resize(size_t) { g_resize_call_count++; }
};

struct ResizeDefaultInitString {
  void resize(size_t) { g_resize_call_count++; }
  void __resize_default_init(size_t) { g_resize_default_init_call_count++; }
};

class StringResizeUninitializedTest : public testing::Test {
 public:
  StringResizeUninitializedTest() {
    g_resize_call_count = 0;
    g_resize_default_init_call_count = 0;
  }
};

}  // namespace

TEST_F(StringResizeUninitializedTest, ResizableString) {
  ResizableString rs;

  EXPECT_EQ(g_resize_call_count, 0);
  EXPECT_EQ(g_resize_default_init_call_count, 0);
  StringResizeUninitialized(NotNull(&rs), 237);
  EXPECT_EQ(g_resize_call_count, 1);
  EXPECT_EQ(g_resize_default_init_call_count, 0);
}

TEST_F(StringResizeUninitializedTest, ResizeDefaultInitString) {
  ResizeDefaultInitString rus;

  EXPECT_EQ(g_resize_call_count, 0);
  EXPECT_EQ(g_resize_default_init_call_count, 0);
  StringResizeUninitialized(NotNull(&rus), 237);
  EXPECT_EQ(g_resize_call_count, 0);
  EXPECT_EQ(g_resize_default_init_call_count, 1);
}

TEST_F(StringResizeUninitializedTest, ResizableStringRawPointer) {
  ResizableString rs;

  EXPECT_EQ(g_resize_call_count, 0);
  EXPECT_EQ(g_resize_default_init_call_count, 0);
  StringResizeUninitialized(&rs, 237);
  EXPECT_EQ(g_resize_call_count, 1);
  EXPECT_EQ(g_resize_default_init_call_count, 0);
}

TEST_F(StringResizeUninitializedTest, ResizeDefaultInitStringRawPointer) {
  ResizeDefaultInitString rus;

  EXPECT_EQ(g_resize_call_count, 0);
  EXPECT_EQ(g_resize_default_init_call_count, 0);
  StringResizeUninitialized(&rus, 237);
  EXPECT_EQ(g_resize_call_count, 0);
  EXPECT_EQ(g_resize_default_init_call_count, 1);
}

}  // namespace rst
