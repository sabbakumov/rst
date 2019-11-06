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

#include "rst/status/status.h"

#include <memory>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "rst/macros/macros.h"
#include "rst/rtti/rtti.h"

namespace rst {
namespace {

constexpr auto kError = "Error";
constexpr auto kError2 = "Error2";
constexpr auto kError3 = "Error2";

class Error : public ErrorInfo<Error> {
 public:
  Error() = default;

  const std::string& AsString() const override { return message_; }

  static char id_;

 private:
  const std::string message_ = kError;

  RST_DISALLOW_COPY_AND_ASSIGN(Error);
};

char Error::id_ = 0;

class Error2 : public ErrorInfo<Error2> {
 public:
  Error2() = default;

  const std::string& AsString() const override { return message_; }

  static char id_;

 private:
  const std::string message_ = kError2;

  RST_DISALLOW_COPY_AND_ASSIGN(Error2);
};

char Error2::id_ = 0;

class Error3 : public ErrorInfo<Error3, Error2> {
 public:
  Error3() = default;

  const std::string& AsString() const override { return message_; }

  static char id_;

 private:
  const std::string message_ = kError3;

  RST_DISALLOW_COPY_AND_ASSIGN(Error3);
};

char Error3::id_ = 0;

}  // namespace

TEST(Status, OK) {
  auto status = Status::OK();
  EXPECT_FALSE(status.err());

  EXPECT_DEATH({ auto status = Status::OK(); }, "");
}

TEST(Status, Err) {
  auto status = Status::OK();
  EXPECT_FALSE(status.err());
}

TEST(Status, Ctor) {
  Status status = MakeStatus<Error>();
  ASSERT_TRUE(status.err());
  EXPECT_EQ(status.GetError()->AsString(), kError);
}

TEST(Status, MoveCtor) {
  Status status = MakeStatus<Error>();
  Status status2(std::move(status));
  ASSERT_TRUE(status2.err());
  EXPECT_EQ(status2.GetError()->AsString(), kError);
}

TEST(Status, MoveAssignment) {
  Status status = MakeStatus<Error>();
  auto status2 = Status::OK();
  status2.Ignore();
  status2 = std::move(status);
  ASSERT_TRUE(status2.err());
  EXPECT_EQ(status2.GetError()->AsString(), kError);

  status2 = MakeStatus<Error>();
  EXPECT_DEATH(status2 = std::move(status), "");
  status2.Ignore();
}

TEST(Status, Dtor) {
  EXPECT_DEATH({ const auto status = MakeStatus<Error>(); }, "");
}

TEST(Status, ErrorInfo) {
  auto status = Status::OK();
  EXPECT_DEATH(status.GetError(), "");
  status.Ignore();

  auto status2 = Status::OK();
  EXPECT_FALSE(status2.err());
  EXPECT_DEATH(status.GetError(), "");
}

TEST(Status, MakeStatus) {
  auto status = MakeStatus<Error>();

  ASSERT_TRUE(status.err());
  EXPECT_EQ(status.GetError()->AsString(), kError);
}

TEST(Status, Errors) {
  {
    auto status = MakeStatus<Error3>();
    ASSERT_TRUE(status.err());

    Nullable<const Error3*> cast3 = dyn_cast<Error3>(status.GetError());
    ASSERT_NE(cast3, nullptr);
    EXPECT_EQ(cast3->AsString(), kError3);

    EXPECT_EQ(dyn_cast<Error>(status.GetError()), nullptr);

    Nullable<const Error2*> cast2 = dyn_cast<Error2>(status.GetError());
    ASSERT_NE(cast2, nullptr);
    EXPECT_EQ(cast2->AsString(), kError3);
  }

  {
    Error2 error;
    auto status = MakeStatus<Error2>();
    ASSERT_TRUE(status.err());

    EXPECT_EQ(dyn_cast<Error3>(status.GetError()), nullptr);

    EXPECT_EQ(dyn_cast<Error>(status.GetError()), nullptr);

    Nullable<const Error2*> cast2 = dyn_cast<Error2>(status.GetError());
    ASSERT_NE(cast2, nullptr);
    EXPECT_EQ(cast2->AsString(), kError2);
  }
}

}  // namespace rst
