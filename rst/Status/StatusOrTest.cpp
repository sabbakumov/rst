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

#include "rst/Status/StatusOr.h"

#include <complex>
#include <string>
#include <utility>

#include <gtest/gtest.h>

using std::complex;
using std::string;

namespace rst {

namespace {

constexpr auto kStringValue = "String value for testing StatusOr";

constexpr auto kDomain = "Domain";

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
    ASSERT_TRUE(status_or.ok());
    EXPECT_EQ(0, *status_or);

    StatusOr<complex<double>> status_or_cmplx = complex<double>(0.0, 0.0);
    ASSERT_TRUE(status_or_cmplx.ok());
    EXPECT_EQ(complex<double>(0.0, 0.0), *status_or_cmplx);

    string s = "Test string";
    StatusOr<string> status_or_str = s;
    ASSERT_TRUE(status_or_str.ok());
    EXPECT_EQ("Test string", *status_or_str);

    StatusOr<string> status_or_str2 = std::move(s);
    ASSERT_TRUE(status_or_str2.ok());
    EXPECT_EQ("Test string", *status_or_str2);
  }

  {
    StatusOr<int> status_or = Status(kDomain, -1, string());
    EXPECT_FALSE(status_or.ok());

    auto status = Status::OK();
    EXPECT_DEATH(StatusOr<int>(std::move(status)), "");
    status.Ignore();
  }
}

TEST(StatusOr, MoveCtor) {
  {
    StatusOr<int> status_or = 0;
    StatusOr<int> status_or2 = std::move(status_or);
    ASSERT_TRUE(status_or2.ok());
    EXPECT_EQ(0, *status_or2);

    StatusOr<complex<double>> status_or_cmplx = complex<double>(0.0, 0.0);
    StatusOr<complex<double>> status_or_cmplx2 = std::move(status_or_cmplx);
    ASSERT_TRUE(status_or_cmplx2.ok());
    EXPECT_EQ(complex<double>(0.0, 0.0), *status_or_cmplx2);
  }

  {
    StatusOr<int> status_or = Status(kDomain, -1, string());
    StatusOr<int> status_or2 = std::move(status_or);
    EXPECT_FALSE(status_or2.ok());
  }
}

TEST(StatusOr, Dtor) {
  EXPECT_EQ(0, DtorHelper::counter());

  {
    StatusOr<DtorHelper> status_or = DtorHelper();
    status_or.Ignore();
    EXPECT_EQ(1, DtorHelper::counter());

    StatusOr<DtorHelper> status_or2 = DtorHelper();
    status_or2.Ignore();
    EXPECT_EQ(2, DtorHelper::counter());
  }

  EXPECT_EQ(0, DtorHelper::counter());

  { EXPECT_DEATH((StatusOr<int>(0)), ""); }
}

TEST(StatusOr, OperatorEquals) {
  EXPECT_EQ(0, DtorHelper::counter());

  {
    StatusOr<DtorHelper> status_or = DtorHelper();
    status_or.Ignore();
    status_or = DtorHelper();
    status_or.Ignore();
    EXPECT_EQ(1, DtorHelper::counter());

    StatusOr<DtorHelper> status_or2 = DtorHelper();
    status_or2.Ignore();
    status_or2 = DtorHelper();
    status_or2.Ignore();
    EXPECT_EQ(2, DtorHelper::counter());

    string temp = kStringValue;
    StatusOr<string> os = string();
    os.Ignore();
    os = temp;
    os.Ignore();
    EXPECT_EQ(kStringValue, *os);

    StatusOr<string> os2 = string();
    os2.Ignore();
    os2 = std::move(temp);
    os2.Ignore();
    EXPECT_EQ(kStringValue, *os2);
  }

  EXPECT_EQ(0, DtorHelper::counter());

  {
    StatusOr<int> status_or = Status("", -10, string());
    status_or.Ignore();
    status_or = Status(kDomain, -1, "Message");
    ASSERT_FALSE(status_or.ok());
    EXPECT_EQ(kDomain, status_or.status().error_domain());
    EXPECT_EQ(-1, status_or.status().error_code());
    EXPECT_EQ("Message", status_or.status().error_message());

    StatusOr<DtorHelper> status_or2 = Status(kDomain, -1, string());
    status_or2.Ignore();
    status_or2 = DtorHelper();
    status_or2.Ignore();
    EXPECT_EQ(1, DtorHelper::counter());

    StatusOr<int> status_or3 = 0;
    auto status = Status::OK();
    EXPECT_DEATH(status_or3 = std::move(status), "");
    status_or3.Ignore();
    status.Ignore();

    EXPECT_DEATH(status_or3 = std::move(status), "");
  }

  EXPECT_EQ(0, DtorHelper::counter());

  {
    StatusOr<int> status_or = 0;
    StatusOr<int> status_or2 = 0;
    EXPECT_DEATH(status_or2 = std::move(status_or), "");
    status_or.Ignore();
    status_or2.Ignore();

    StatusOr<complex<double>> status_or_cmplx = complex<double>(0.0, 0.0);
    StatusOr<complex<double>> status_or_cmplx2 = complex<double>(0.0, 0.0);
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
    ASSERT_TRUE(status_or.ok());
    EXPECT_EQ(8, *status_or);

    StatusOr<int> status_or2 = 168;
    ASSERT_TRUE(status_or2.ok());
    EXPECT_EQ(168, *status_or2);

    status_or = std::move(status_or2);
    ASSERT_TRUE(status_or.ok());
    EXPECT_EQ(168, *status_or);

    string test = "Test string! Test!";
    StatusOr<string> os = test;
    ASSERT_TRUE(os.ok());
    EXPECT_EQ("Test string! Test!", *os);

    StatusOr<string> os2 = std::move(os);
    ASSERT_TRUE(os2.ok());
    EXPECT_EQ("Test string! Test!", *os2);
  }
}

TEST(StatusOr, Ok) {
  {
    StatusOr<int> status_or = 0;
    EXPECT_TRUE(status_or.ok());

    StatusOr<complex<double>> status_or_cmplx = complex<double>(0.0, 0.0);
    EXPECT_TRUE(status_or_cmplx.ok());
  }
}

TEST(StatusOr, OperatorStar) {
  {
    StatusOr<int> status_or = 0;
    ASSERT_TRUE(status_or.ok());
    EXPECT_EQ(0, *status_or);
    *status_or = 1;
    ASSERT_TRUE(status_or.ok());
    EXPECT_EQ(1, *status_or);

    StatusOr<complex<double>> status_or_cmplx = complex<double>(0.0, 0.0);
    ASSERT_TRUE(status_or_cmplx.ok());
    EXPECT_EQ(complex<double>(0.0, 0.0), *status_or_cmplx);
    *status_or_cmplx = complex<double>(1.0, 1.0);
    ASSERT_TRUE(status_or_cmplx.ok());
    EXPECT_EQ(complex<double>(1.0, 1.0), *status_or_cmplx);
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
    ASSERT_TRUE(r.ok());
    r->foo();
  }

  {
    StatusOr<ArrowHelper> r = ArrowHelper();
    EXPECT_DEATH(r->foo(), "");
    r.Ignore();
  }

  {
    StatusOr<ArrowHelper> r = Status(kDomain, -1, string());
    ASSERT_FALSE(r.ok());
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
    StatusOr<int> status_or = Status(kDomain, -1, "Message");
    status_or.Ignore();
    const auto& status = status_or.status();
    EXPECT_EQ(kDomain, status.error_domain());
    EXPECT_EQ(-1, status.error_code());
    EXPECT_EQ("Message", status.error_message());
  }
}

}  // namespace rst
