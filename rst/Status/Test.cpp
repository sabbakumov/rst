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

#include "rst/Status.h"

#include "gtest/gtest.h"

using rst::Code;
using rst::Status;

TEST(Status, EmptyConstructor) {
  Status status;
  ASSERT_TRUE(status.ok());

  ASSERT_EQ(Code::kOk, status.error_code());
  ASSERT_EQ("", status.error_message());
}

TEST(Status, Constructor) {
  Status status(Code::kCanceled, "Message");
  ASSERT_FALSE(status.ok());

  ASSERT_EQ(Code::kCanceled, status.error_code());
  ASSERT_EQ("Message", status.error_message());
}

TEST(Status, MoveConstructor) {
  Status status(Code::kCanceled, "Message");
  Status status2(std::move(status));
  ASSERT_FALSE(status2.ok());
  
  ASSERT_EQ(Code::kCanceled, status2.error_code());
  ASSERT_EQ("Message", status2.error_message());
}

TEST(Status, MoveAssignment) {
  Status status(Code::kCanceled, "Message");
  Status status2;
  status2.Ignore();
  Status& ref = status2 = std::move(status);
  ASSERT_FALSE(status2.ok());
  ASSERT_EQ(&ref, &status2);
  
  ASSERT_EQ(Code::kCanceled, status2.error_code());
  ASSERT_EQ("Message", status2.error_message());
}

TEST(Status, ToString) {
  Status status(Code::kCanceled, "Message");
  status.Ignore();

  ASSERT_EQ("kCanceled: Message", status.ToString());
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

