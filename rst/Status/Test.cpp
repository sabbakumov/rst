// Copyright (c) 2015, Sergey Abbakumov
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

#include "rst/Status/Status.h"

#include "gtest/gtest.h"

using rst::Status;
using rst::StatusErr;
using rst::StatusErrWithCode;
using rst::StatusOk;

TEST(Status, EmptyConstructor) {
  Status status;
  ASSERT_TRUE(status.ok());
}

TEST(Status, Constructor) {
  Status status(-1, "Message");
  ASSERT_EQ(-1, status.code());
  ASSERT_FALSE(status.ok());

  Status ok = StatusOk();
  ASSERT_EQ(0, ok.code());
  ASSERT_TRUE(ok.ok());

  Status err = StatusErr("Message");
  ASSERT_EQ(-1, err.code());
  ASSERT_FALSE(err.ok());

  Status err2 = StatusErrWithCode(-2, "Message");
  ASSERT_EQ(-2, err2.code());
  ASSERT_FALSE(err2.ok());
}

TEST(Status, MoveConstructor) {
  Status status(-1, "Message");
  Status status2(std::move(status));
  ASSERT_EQ(-1, status2.code());
  ASSERT_FALSE(status2.ok());
}

TEST(Status, MoveAssignment) {
  Status status(-1, "Message");
  Status status2;
  Status& ref = status2 = std::move(status);
  ASSERT_EQ(-1, status2.code());
  ASSERT_FALSE(status2.ok());
  ASSERT_EQ(&ref, &status2);
}

TEST(Status, ToString) {
  Status status(-1, "Message");
  status.Ignore();

  ASSERT_EQ("Message", status.ToString());
}

TEST(Status, OperatorEquals) {
  Status status;
  Status status2;

  ASSERT_TRUE(status == status2);

  status = StatusErrWithCode(-1, "Status 1");
  status.Ignore();

  status2 = StatusErrWithCode(-1, "Status 2");
  status2.Ignore();

  ASSERT_TRUE(status == status2);

  status = StatusErrWithCode(-2, "Status 1");
  status.Ignore();

  status2 = StatusErrWithCode(-1, "Status 1");
  status2.Ignore();

  ASSERT_FALSE(status == status2);
}

TEST(Status, Nothing) {
  Status status;
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

