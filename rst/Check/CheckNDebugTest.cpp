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

#undef NDEBUG
#define NDEBUG
#include "rst/Check/Check.h"

#include <gtest/gtest.h>

namespace rst {
namespace {

auto g_int = 0;

bool IncrementIntAndReturnTrue() {
  g_int++;
  return true;
}

constexpr double Divide(const double a, const double b) {
  RST_DCHECK(b != 0.0);
  RST_CHECK(b != 0.0);
  return a / b;
}
}  // namespace

TEST(NDebugCheck, Check) {
  EXPECT_NO_FATAL_FAILURE(RST_CHECK(true));
  EXPECT_DEATH(RST_CHECK(false), "");
}

TEST(NDebugCheck, DCheck) {
  EXPECT_NO_FATAL_FAILURE(RST_DCHECK(true));
  EXPECT_NO_FATAL_FAILURE(RST_DCHECK(false));
}

TEST(NDebugCheck, Notreached) { EXPECT_NO_FATAL_FAILURE(RST_NOTREACHED()); }

TEST(NDebugCheck, DCheckInConstexpr) {
  static constexpr auto result = Divide(1.0, 1.0);
  EXPECT_EQ(result, 1.0);
}

TEST(NDebugCheck, NoUnused) {
  static constexpr auto result = Divide(1.0, 1.0);
  RST_DCHECK(result == 1.0);
}

TEST(NDebugCheck, ConditionEvaluation) {
  EXPECT_EQ(g_int, 0);
  RST_DCHECK(IncrementIntAndReturnTrue());
  EXPECT_EQ(g_int, 0);
}

}  // namespace rst
