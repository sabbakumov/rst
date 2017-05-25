// Copyright (c) 2017, Sergey Abbakumov
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

#include <string>

#include "Defer.h"

#include <gtest/gtest.h>

using std::string;

namespace {

auto g_int = 0;

}  // namespace

TEST(Defer, Lambda) {
  auto i = 0;
  {
    RST_DEFER([&i]() { i = 1; });
  }

  EXPECT_EQ(1, i);
}

void Foo() { g_int = 1; }

TEST(Defer, Function) {
  { RST_DEFER(Foo); }

  EXPECT_EQ(1, g_int);
}

TEST(Defer, MultipleTimesDeclaration) {
  string result;
  {
    RST_DEFER([&result]() { result += '1'; });
    RST_DEFER([&result]() { result += '2'; });
  }

  EXPECT_EQ("21", result);
}

TEST(Defer, NoExceptionPropagation) {
  auto i = 0;
  {
    RST_DEFER([]() { throw 0; });
  }

  EXPECT_EQ(0, i);
}
