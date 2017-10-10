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
using std::unique_ptr;

namespace {

constexpr const char* kDomain = "Domain";

}  // namespace

TEST(Status, OK) {
  auto status = Status::OK();
  EXPECT_TRUE(status.ok());

  EXPECT_DEATH(Status::OK(), "");
}

TEST(Status, Constructor) {
  Status status(kDomain, -1, "Message");
  ASSERT_FALSE(status.ok());
  EXPECT_EQ(kDomain, status.error_domain());
  EXPECT_EQ(-1, status.error_code());
  EXPECT_EQ("Message", status.error_message());

  EXPECT_DEATH((Status(nullptr, -1, "Message")), "");
  EXPECT_DEATH((Status(kDomain, 0, "Message")), "");
}

TEST(Status, MoveConstructor) {
  Status status(kDomain, -1, "Message");
  Status status2(std::move(status));
  ASSERT_FALSE(status2.ok());
  EXPECT_EQ(kDomain, status2.error_domain());
  EXPECT_EQ(-1, status2.error_code());
  EXPECT_EQ("Message", status2.error_message());
}

TEST(Status, MoveAssignment) {
  Status status(kDomain, -1, "Message");
  auto status2 = Status::OK();
  status2.Ignore();
  status2 = std::move(status);
  ASSERT_FALSE(status2.ok());
  EXPECT_EQ(kDomain, status2.error_domain());
  EXPECT_EQ(-1, status2.error_code());
  EXPECT_EQ("Message", status2.error_message());

  status2 = Status(kDomain, -1, "Message");
  EXPECT_DEATH(status2 = std::move(status), "");
  status2.Ignore();
}

TEST(Status, Dtor) { EXPECT_DEATH((Status(kDomain, -1, "Message")), ""); }

TEST(Status, ErrorInfo) {
  auto status = Status::OK();
  EXPECT_DEATH(status.error_domain(), "");
  EXPECT_DEATH(status.error_code(), "");
  EXPECT_DEATH(status.error_message(), "");
  status.Ignore();

  auto status2 = Status::OK();
  EXPECT_TRUE(status2.ok());
  EXPECT_DEATH(status.error_domain(), "");
  EXPECT_DEATH(status.error_code(), "");
  EXPECT_DEATH(status.error_message(), "");
}
