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

#include "rst/macros/optimization.h"

#include <gtest/gtest.h>

namespace rst {

TEST(Optimization, Likely) {
  if (RST_LIKELY(true))
    SUCCEED();
  else
    FAIL();

  if (RST_LIKELY(false))
    FAIL();
  else
    SUCCEED();
}

TEST(Optimization, Unlikely) {
  if (RST_LIKELY(true))
    SUCCEED();
  else
    FAIL();

  if (RST_LIKELY(false))
    FAIL();
  else
    SUCCEED();
}

TEST(Optimization, LikelyEq) {
  switch (RST_LIKELY_EQ(5, 5)) {
    case 5:
      SUCCEED();
      break;
    default:
      FAIL();
  }

  switch (RST_LIKELY_EQ(5, 6)) {
    case 5:
      SUCCEED();
      break;
    default:
      FAIL();
  }
}

}  // namespace rst
