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

#include "rst/RTTI/RTTI.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::Return;

namespace rst {
namespace {

class Mock {
 public:
  template <class T>
  bool IsA() {
    return DoIsA();
  }
  MOCK_METHOD0(DoIsA, bool());
};

class ConstMock {
 public:
  template <class T>
  bool IsA() const {
    return DoIsA();
  }
  MOCK_CONST_METHOD0(DoIsA, bool());
};

}  // namespace

TEST(RTTI, Check) {
  Mock mock;

  EXPECT_CALL(mock, DoIsA()).WillOnce(Return(false));
  EXPECT_EQ(dyn_cast<void>(NotNull(&mock)), nullptr);
  testing::Mock::VerifyAndClearExpectations(&mock);

  EXPECT_CALL(mock, DoIsA()).WillOnce(Return(true));
  EXPECT_NE(dyn_cast<void>(NotNull(&mock)), nullptr);
  testing::Mock::VerifyAndClearExpectations(&mock);
}

TEST(RTTI, ConstCheck) {
  ConstMock mock;

  EXPECT_CALL(mock, DoIsA()).WillOnce(Return(false));
  EXPECT_EQ(dyn_cast<void>(NotNull(const_cast<const ConstMock*>(&mock))),
            nullptr);
  testing::Mock::VerifyAndClearExpectations(&mock);

  EXPECT_CALL(mock, DoIsA()).WillOnce(Return(true));
  EXPECT_NE(dyn_cast<void>(NotNull(const_cast<const ConstMock*>(&mock))),
            nullptr);
  testing::Mock::VerifyAndClearExpectations(&mock);
}

}  // namespace rst
