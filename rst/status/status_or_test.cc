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

#include "rst/status/status_or.h"

#include <complex>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "rst/macros/macros.h"

namespace rst {
namespace {

constexpr auto kStringValue = "String value for testing StatusOr";

constexpr auto kError = "Error";

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

class DtorHelper {
 public:
  DtorHelper() { counter_++; }
  DtorHelper(const DtorHelper&) { counter_++; }
  ~DtorHelper() { counter_--; }

  static int counter() { return counter_; }

 private:
  static int counter_;
};

int DtorHelper::counter_ = 0;

class ArrowHelper {
 public:
  void foo() const {}
};

}  // namespace

TEST(StatusOr, ValueCtor) {
  {
    StatusOr<int> status_or = 0;
    ASSERT_FALSE(status_or.err());
    EXPECT_EQ(*status_or, 0);

    StatusOr<std::complex<double>> status_or_cmplx = std::complex(0.0, 0.0);
    ASSERT_FALSE(status_or_cmplx.err());
    EXPECT_EQ(*status_or_cmplx, std::complex(0.0, 0.0));

    std::string s = "Test string";
    StatusOr<std::string> status_or_str = s;
    ASSERT_FALSE(status_or_str.err());
    EXPECT_EQ(*status_or_str, "Test string");

    StatusOr<std::string> status_or_str2 = std::move(s);
    ASSERT_FALSE(status_or_str2.err());
    EXPECT_EQ(*status_or_str2, "Test string");
  }

  {
    StatusOr<int> status_or = MakeStatus<Error>();
    EXPECT_TRUE(status_or.err());

    auto status = Status::OK();
    EXPECT_DEATH(StatusOr<int>(std::move(status)), "");
    status.Ignore();
  }
}

TEST(StatusOr, MoveCtor) {
  {
    StatusOr<int> status_or = 0;
    StatusOr<int> status_or2 = std::move(status_or);
    ASSERT_FALSE(status_or2.err());
    EXPECT_EQ(*status_or2, 0);

    StatusOr<std::complex<double>> status_or_cmplx = std::complex(0.0, 0.0);
    StatusOr<std::complex<double>> status_or_cmplx2 =
        std::move(status_or_cmplx);
    ASSERT_FALSE(status_or_cmplx2.err());
    EXPECT_EQ(*status_or_cmplx2, std::complex(0.0, 0.0));
  }

  {
    StatusOr<int> status_or = MakeStatus<Error>();
    StatusOr<int> status_or2 = std::move(status_or);
    EXPECT_TRUE(status_or2.err());
  }
}

TEST(StatusOr, Dtor) {
  EXPECT_EQ(DtorHelper::counter(), 0);

  {
    StatusOr<DtorHelper> status_or = DtorHelper();
    status_or.Ignore();
    EXPECT_EQ(DtorHelper::counter(), 1);

    StatusOr<DtorHelper> status_or2 = DtorHelper();
    status_or2.Ignore();
    EXPECT_EQ(DtorHelper::counter(), 2);
  }

  EXPECT_EQ(DtorHelper::counter(), 0);

  { EXPECT_DEATH((StatusOr<int>(0)), ""); }
}

TEST(StatusOr, OperatorEquals) {
  EXPECT_EQ(DtorHelper::counter(), 0);

  {
    StatusOr<DtorHelper> status_or = DtorHelper();
    status_or.Ignore();
    status_or = DtorHelper();
    status_or.Ignore();
    EXPECT_EQ(DtorHelper::counter(), 1);

    StatusOr<DtorHelper> status_or2 = DtorHelper();
    status_or2.Ignore();
    status_or2 = DtorHelper();
    status_or2.Ignore();
    EXPECT_EQ(DtorHelper::counter(), 2);

    std::string temp = kStringValue;
    StatusOr<std::string> os = std::string();
    os.Ignore();
    os = temp;
    os.Ignore();
    EXPECT_EQ(*os, kStringValue);

    StatusOr<std::string> os2 = std::string();
    os2.Ignore();
    os2 = std::move(temp);
    os2.Ignore();
    EXPECT_EQ(*os2, kStringValue);
  }

  EXPECT_EQ(DtorHelper::counter(), 0);

  {
    StatusOr<int> status_or = MakeStatus<Error>();
    status_or.Ignore();
    status_or = MakeStatus<Error>();
    ASSERT_TRUE(status_or.err());
    EXPECT_EQ(status_or.status().GetError()->AsString(), kError);

    StatusOr<DtorHelper> status_or2 = MakeStatus<Error>();
    status_or2.Ignore();
    status_or2 = DtorHelper();
    status_or2.Ignore();
    EXPECT_EQ(DtorHelper::counter(), 1);

    StatusOr<int> status_or3 = 0;
    auto status = Status::OK();
    EXPECT_DEATH(status_or3 = std::move(status), "");
    status_or3.Ignore();
    status.Ignore();

    EXPECT_DEATH(status_or3 = std::move(status), "");
  }

  EXPECT_EQ(DtorHelper::counter(), 0);

  {
    StatusOr<int> status_or = 0;
    StatusOr<int> status_or2 = 0;
    EXPECT_DEATH(status_or2 = std::move(status_or), "");
    status_or.Ignore();
    status_or2.Ignore();

    StatusOr<std::complex<double>> status_or_cmplx = std::complex(0.0, 0.0);
    StatusOr<std::complex<double>> status_or_cmplx2 = std::complex(0.0, 0.0);
    EXPECT_DEATH(status_or_cmplx2 = std::move(status_or_cmplx), "");
    status_or_cmplx.Ignore();
    status_or_cmplx2.Ignore();
  }

  {
    StatusOr<int> status_or = 0;
    EXPECT_DEATH(status_or = 0, "");
    status_or.Ignore();
  }
}

TEST(StatusOr, MoveOperatorEquals) {
  {
    StatusOr<int> status_or = 8;
    ASSERT_FALSE(status_or.err());
    EXPECT_EQ(*status_or, 8);

    StatusOr<int> status_or2 = 168;
    ASSERT_FALSE(status_or2.err());
    EXPECT_EQ(*status_or2, 168);

    status_or = std::move(status_or2);
    ASSERT_FALSE(status_or.err());
    EXPECT_EQ(*status_or, 168);

    std::string test = "Test string! Test!";
    StatusOr<std::string> os = test;
    ASSERT_FALSE(os.err());
    EXPECT_EQ(*os, "Test string! Test!");

    StatusOr<std::string> os2 = std::move(os);
    ASSERT_FALSE(os2.err());
    EXPECT_EQ(*os2, "Test string! Test!");
  }
}

TEST(StatusOr, Ok) {
  {
    StatusOr<int> status_or = 0;
    EXPECT_FALSE(status_or.err());

    StatusOr<std::complex<double>> status_or_cmplx = std::complex(0.0, 0.0);
    EXPECT_FALSE(status_or_cmplx.err());
  }
}

TEST(StatusOr, OperatorStar) {
  {
    StatusOr<int> status_or = 0;
    ASSERT_FALSE(status_or.err());
    EXPECT_EQ(*status_or, 0);
    *status_or = 1;
    ASSERT_FALSE(status_or.err());
    EXPECT_EQ(*status_or, 1);

    StatusOr<std::complex<double>> status_or_cmplx = std::complex(0.0, 0.0);
    ASSERT_FALSE(status_or_cmplx.err());
    EXPECT_EQ(*status_or_cmplx, std::complex(0.0, 0.0));
    *status_or_cmplx = std::complex(1.0, 1.0);
    ASSERT_FALSE(status_or_cmplx.err());
    EXPECT_EQ(*status_or_cmplx, std::complex(1.0, 1.0));
  }

  {
    StatusOr<int> status_or = 0;
    EXPECT_DEATH(*status_or, "");
    status_or.Ignore();
  }
}

TEST(StatusOr, OperatorArrow) {
  {
    StatusOr<ArrowHelper> r = ArrowHelper();
    ASSERT_FALSE(r.err());
    r->foo();
  }

  {
    StatusOr<ArrowHelper> r = ArrowHelper();
    EXPECT_DEATH(r->foo(), "");
    r.Ignore();
  }

  {
    StatusOr<ArrowHelper> r = MakeStatus<Error>();
    ASSERT_TRUE(r.err());
    r.Ignore();
    EXPECT_DEATH(r->foo(), "");
  }
}

TEST(StatusOr, Status) {
  {
    StatusOr<int> status_or = 0;
    EXPECT_DEATH(status_or.status(), "");
    status_or.Ignore();
  }

  {
    StatusOr<int> status_or = 0;
    status_or.Ignore();
    EXPECT_DEATH(status_or.status(), "");
  }

  {
    StatusOr<int> status_or = MakeStatus<Error>();
    status_or.Ignore();
    const auto& status = status_or.status();
    EXPECT_EQ(status.GetError()->AsString(), kError);
  }
}

TEST(StatusOr, StatusOrFromAnother) {
  StatusOr<std::string> status_or = MakeStatus<Error>();
  EXPECT_TRUE(status_or.err());
  StatusOr<std::string> status_or2 = std::move(status_or).TakeStatus();
  EXPECT_TRUE(status_or2.err());
}

TEST(StatusOr, StatusOrFromAnotherMove) {
  StatusOr<std::string> status_or = MakeStatus<Error>();
  EXPECT_TRUE(status_or.err());
  StatusOr<std::string> status_or2(std::move(status_or));
  EXPECT_TRUE(status_or2.err());
}

}  // namespace rst
