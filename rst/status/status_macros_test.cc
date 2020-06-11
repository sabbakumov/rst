// Copyright (c) 2019, Sergey Abbakumov
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

#include "rst/status/status_macros.h"

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "rst/macros/macros.h"
#include "rst/status/status.h"
#include "rst/status/status_or.h"

namespace rst {
namespace {

class Error : public ErrorInfo<Error> {
 public:
  Error() = default;

  const std::string& AsString() const override { return message_; }

  static char id_;

 private:
  const std::string message_;

  RST_DISALLOW_COPY_AND_ASSIGN(Error);
};

char Error::id_ = '\0';

Status StatusOK() { return Status::OK(); }

Status CheckStatusOK() {
  RST_TRY(StatusOK());
  return Status::OK();
}

StatusOr<int> StatusOrIntOK() { return 0; }

Status CheckStatusOrIntOK() {
  RST_TRY_CREATE(auto, i, StatusOrIntOK());
  RST_TRY_ASSIGN(i, StatusOrIntOK());
  auto int_i = 0;
  RST_TRY_ASSIGN_UNWRAP(int_i, StatusOrIntOK());
  return Status::OK();
}

StatusOr<std::string> StatusOrStringOK() { return std::string(); }

Status CheckStatusOrStringOK() {
  RST_TRY_CREATE(StatusOr<std::string>, s, StatusOrStringOK());
  RST_TRY_ASSIGN(s, StatusOrStringOK());
  std::string string_s;
  RST_TRY_ASSIGN_UNWRAP(string_s, StatusOrStringOK());
  return StatusOK();
}

StatusOr<std::unique_ptr<int>> StatusOrUniquePtrOK() {
  return std::make_unique<int>(0);
}

Status CheckStatusOrUniquePtrOK() {
  RST_TRY_CREATE(StatusOr<std::unique_ptr<int>>, p, StatusOrUniquePtrOK());
  RST_TRY_ASSIGN(p, StatusOrUniquePtrOK());
  std::unique_ptr<int> unique_ptr_p;
  RST_TRY_ASSIGN_UNWRAP(unique_ptr_p, StatusOrUniquePtrOK());
  return Status::OK();
}

Status StatusError() { return MakeStatus<Error>(); }

Status CheckStatusError() {
  RST_TRY(StatusError());
  return Status::OK();
}

StatusOr<int> StatusOrIntError() { return MakeStatus<Error>(); }

Status CheckStatusOrIntErrorCreate() {
  RST_TRY_CREATE(auto, i, StatusOrIntError());
  return Status::OK();
}

Status CheckStatusOrIntErrorAssign() {
  RST_TRY_CREATE(auto, i, StatusOrIntOK());
  RST_TRY_ASSIGN(i, StatusOrIntError());
  auto int_i = 0;
  RST_TRY_ASSIGN_UNWRAP(int_i, StatusOrIntError());
  return Status::OK();
}

StatusOr<std::string> StatusOrStringError() { return MakeStatus<Error>(); }

Status CheckStatusOrStringErrorCreate() {
  RST_TRY_CREATE(StatusOr<std::string>, s, StatusOrStringError());
  return Status::OK();
}

Status CheckStatusOrStringErrorAssign() {
  RST_TRY_CREATE(StatusOr<std::string>, s, StatusOrStringOK());
  RST_TRY_ASSIGN(s, StatusOrStringError());
  std::string string_s;
  RST_TRY_ASSIGN_UNWRAP(string_s, StatusOrStringError());
  return Status::OK();
}

StatusOr<std::unique_ptr<int>> StatusOrUniquePtrError() {
  return MakeStatus<Error>();
}

Status CheckStatusOrUniquePtrErrorCreate() {
  RST_TRY_CREATE(StatusOr<std::unique_ptr<int>>, p, StatusOrUniquePtrError());
  return Status::OK();
}

Status CheckStatusOrUniquePtrErrorAssign() {
  RST_TRY_CREATE(StatusOr<std::unique_ptr<int>>, p, StatusOrUniquePtrOK());
  RST_TRY_ASSIGN(p, StatusOrUniquePtrError());
  std::unique_ptr<int> unique_ptr_p;
  RST_TRY_ASSIGN_UNWRAP(unique_ptr_p, StatusOrUniquePtrError());
  return Status::OK();
}

}  // namespace

TEST(StatusMacros, StatusOK) {
  auto status = CheckStatusOK();
  EXPECT_FALSE(status.err());
}

TEST(StatusOrMacros, StatusOrIntOK) {
  auto status = CheckStatusOrIntOK();
  EXPECT_FALSE(status.err());
}

TEST(StatusOrMacros, StatusOrStringOK) {
  auto status = CheckStatusOrStringOK();
  EXPECT_FALSE(status.err());
}

TEST(StatusOrMacros, StatusOrUniquePtrOK) {
  auto status = CheckStatusOrUniquePtrOK();
  EXPECT_FALSE(status.err());
}

TEST(StatusMacros, StatusError) {
  auto status = CheckStatusError();
  EXPECT_TRUE(status.err());
}

TEST(StatusMacros, StatusOrIntErrorCreate) {
  auto status = CheckStatusOrIntErrorCreate();
  EXPECT_TRUE(status.err());
}

TEST(StatusMacros, StatusOrIntErrorAssign) {
  auto status = CheckStatusOrIntErrorAssign();
  EXPECT_TRUE(status.err());
}

TEST(StatusMacros, StatusOrStringErrorCreate) {
  auto status = CheckStatusOrStringErrorCreate();
  EXPECT_TRUE(status.err());
}

TEST(StatusMacros, StatusOrStringErrorAssign) {
  auto status = CheckStatusOrStringErrorAssign();
  EXPECT_TRUE(status.err());
}

TEST(StatusMacros, StatusOrUniquePtrErrorCreate) {
  auto status = CheckStatusOrUniquePtrErrorCreate();
  EXPECT_TRUE(status.err());
}

TEST(StatusMacros, StatusOrUniquePtrErrorAssign) {
  auto status = CheckStatusOrUniquePtrErrorAssign();
  EXPECT_TRUE(status.err());
}

}  // namespace rst
