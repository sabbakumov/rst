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

#include "rst/hidden_string/hidden_string.h"

#include <gtest/gtest.h>

namespace rst {

TEST(HiddenString, Normal) {
  RST_HIDDEN_STRING(kFirst, "abcDEF");
  EXPECT_EQ(kFirst.Decrypt(), "abcDEF");

  RST_HIDDEN_STRING(kSecond, "ABC");
  EXPECT_EQ(kSecond.Decrypt(), "ABC");

  RST_HIDDEN_STRING(kThird, "0123456789");
  EXPECT_EQ(kThird.Decrypt(), "0123456789");

  RST_HIDDEN_STRING(kLong, "abcDEFabcDEFabcDEFabcDEFabcDEFabcDEFabcDEFabcDE");
  EXPECT_EQ(kLong.Decrypt(), "abcDEFabcDEFabcDEFabcDEFabcDEFabcDEFabcDEFabcDE");
}

}  // namespace rst
