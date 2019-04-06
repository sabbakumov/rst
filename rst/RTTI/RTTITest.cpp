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

#include <string>

#include <gtest/gtest.h>

#include "rst/Check/Check.h"
#include "rst/Macros/Macros.h"

namespace rst {
namespace {

constexpr auto kError1 = "Error 1";
constexpr auto kError2 = "Error 2";
constexpr auto kError3 = "Error 3";

class ErrorInfoBase {
 public:
  ErrorInfoBase() = default;
  virtual ~ErrorInfoBase() = default;

  static const void* GetClassID() { return &id_; }

  virtual const std::string& AsString() const = 0;
  virtual const void* GetDynamicClassID() const = 0;

  virtual bool IsA(const void* class_id) const {
    RST_DCHECK(class_id != nullptr);
    return class_id == GetClassID();
  }

  template <class ErrorInfoT>
  bool IsA() const {
    return IsA(ErrorInfoT::GetClassID());
  }

 private:
  static char id_;

  RST_DISALLOW_COPY_AND_ASSIGN(ErrorInfoBase);
};

char ErrorInfoBase::id_ = 0;

template <class T>
class ErrorInfo : public ErrorInfoBase {
 public:
  using ErrorInfoBase::ErrorInfoBase;

  static const void* GetClassID() { return &T::id_; }

  const void* GetDynamicClassID() const override { return &T::id_; }

  bool IsA(const void* class_id) const override {
    RST_DCHECK(class_id != nullptr);
    return class_id == GetClassID() || ErrorInfoBase::IsA(class_id);
  }

 private:
  RST_DISALLOW_COPY_AND_ASSIGN(ErrorInfo);
};

class Error1 : public ErrorInfo<Error1> {
 public:
  Error1() = default;

  const std::string& AsString() const override { return message_; }

  static char id_;

 private:
  std::string message_ = kError1;

  RST_DISALLOW_COPY_AND_ASSIGN(Error1);
};

char Error1::id_ = 0;

class Error2 : public ErrorInfo<Error2> {
 public:
  Error2() = default;

  const std::string& AsString() const override { return message_; }

  static char id_;

 private:
  std::string message_ = kError2;

  RST_DISALLOW_COPY_AND_ASSIGN(Error2);
};

char Error2::id_ = 0;

class Error3 : public Error2 {
 public:
  Error3() = default;

  const std::string& AsString() const override { return message_; }

  static char id_;

 private:
  std::string message_ = kError3;

  RST_DISALLOW_COPY_AND_ASSIGN(Error3);
};

char Error3::id_ = 0;

}  // namespace

TEST(RTTI, Check) {
  Error1 error1;
  ErrorInfoBase* base1 = &error1;

  Error2 error2;
  ErrorInfoBase* base2 = &error2;

  Nullable<Error1*> cast1 = dyn_cast<Error1>(NotNull(base1));
  ASSERT_NE(cast1, nullptr);
  ASSERT_EQ(dyn_cast<Error2>(NotNull(base1)), nullptr);
  EXPECT_EQ(cast1->AsString(), kError1);

  Nullable<Error2*> cast2 = dyn_cast<Error2>(NotNull(base2));
  ASSERT_NE(cast2, nullptr);
  ASSERT_EQ(dyn_cast<Error1>(NotNull(base2)), nullptr);
  EXPECT_EQ(cast2->AsString(), kError2);
}

TEST(RTTI, ConstCheck) {
  const Error1 error1;
  const ErrorInfoBase* base1 = &error1;

  const Error2 error2;
  const ErrorInfoBase* base2 = &error2;

  Nullable<const Error1*> cast1 = dyn_cast<Error1>(NotNull(base1));
  ASSERT_NE(cast1, nullptr);
  ASSERT_EQ(dyn_cast<Error2>(NotNull(base1)), nullptr);
  EXPECT_EQ(cast1->AsString(), kError1);

  Nullable<const Error2*> cast2 = dyn_cast<Error2>(NotNull(base2));
  ASSERT_NE(cast2, nullptr);
  ASSERT_EQ(dyn_cast<Error1>(NotNull(base2)), nullptr);
  EXPECT_EQ(cast2->AsString(), kError2);
}

TEST(RTTI, Derived) {
  Error3 error;
  ErrorInfoBase* base = &error;

  Nullable<Error3*> cast3 = dyn_cast<Error3>(NotNull(base));
  ASSERT_NE(cast3, nullptr);
  EXPECT_EQ(cast3->AsString(), kError3);

  ASSERT_EQ(dyn_cast<Error1>(NotNull(base)), nullptr);

  Nullable<Error2*> cast2 = dyn_cast<Error2>(NotNull(base));
  ASSERT_NE(cast2, nullptr);
  EXPECT_EQ(cast3->AsString(), kError3);
}

}  // namespace rst
