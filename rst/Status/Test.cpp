// Copyright (c) 2016, Sergey Abbakumov
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

#include <memory>

#include <gtest/gtest.h>

using rst::Status;
using rst::StatusErr;
using rst::StatusErrWithCode;
using rst::StatusOk;
using std::unique_ptr;

TEST(Status, EmptyConstructor) {
  Status status;
  EXPECT_TRUE(status.ok());
}

TEST(Status, Constructor) {
  Status status(-1, "Message");
  EXPECT_EQ(-1, status.error_code());
  EXPECT_FALSE(status.ok());

  unique_ptr<Status> status2;
  EXPECT_DEATH((status2.reset(new Status(0, "Message"))), "");

  Status ok = StatusOk();
  EXPECT_DEATH(ok.error_code(), "");
  EXPECT_DEATH(ok.error_message(), "");
  EXPECT_TRUE(ok.ok());

  Status err = StatusErr("Message");
  EXPECT_EQ(-1, err.error_code());
  EXPECT_FALSE(err.ok());

  Status err2 = StatusErrWithCode(-2, "Message");
  EXPECT_EQ(-2, err2.error_code());
  EXPECT_FALSE(err2.ok());
}

TEST(Status, MoveConstructor) {
  Status status(-1, "Message");
  Status status2(std::move(status));
  EXPECT_EQ(-1, status2.error_code());
  EXPECT_FALSE(status2.ok());
}

TEST(Status, MoveAssignment) {
  Status status(-1, "Message");
  Status status2;
  Status& ref = status2 = std::move(status);
  EXPECT_EQ(-1, status2.error_code());
  EXPECT_FALSE(status2.ok());
  EXPECT_EQ(&ref, &status2);

  status2 = Status(-1, "Message");
  EXPECT_DEATH(status2 = std::move(status), "");
  status2.Ignore();
}

TEST(Status, ErrorMessage) {
  Status status(-1, "Message");
  status.Ignore();

  EXPECT_EQ("Message", status.error_message());
}

TEST(Status, OperatorEquals) {
  Status status;
  Status status2;

  EXPECT_TRUE(status == status2);

  status = StatusErrWithCode(-1, "Status 1");
  status.Ignore();

  status2 = StatusErrWithCode(-1, "Status 2");
  status2.Ignore();

  EXPECT_TRUE(status == status2);

  status = StatusErrWithCode(-2, "Status 1");
  status.Ignore();

  status2 = StatusErrWithCode(-1, "Status 1");
  status2.Ignore();

  EXPECT_FALSE(status == status2);
}

TEST(Status, Nothing) { Status status; }

TEST(Status, Dtor) {
  EXPECT_DEATH((Status(-1, "Message")), "");
}
