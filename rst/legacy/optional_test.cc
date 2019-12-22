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

#include "rst/legacy/optional.h"

#include <complex>
#include <string>
#include <utility>

#include "gtest/gtest.h"

namespace rst {
namespace {

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
  void Foo() const {}
};

template <class T>
optional<T> ReturnOptional(const T& val) {
  return optional<T>(val);
}

}  // namespace

TEST(Optional, ValueCtor) {
  optional<int> oi = 0;
  EXPECT_TRUE(oi.has_value());

  optional<std::complex<double>> ocmplx = std::complex(0.0, 0.0);
  EXPECT_TRUE(ocmplx.has_value());

  std::string s = "Test string";
  optional<std::string> os = s;
  EXPECT_TRUE(os.has_value());

  optional<std::string> os2 = std::move(s);
  EXPECT_TRUE(os2.has_value());

  optional<int> ni(nullopt);
  EXPECT_FALSE(ni.has_value());
}

TEST(Optional, DefaultCtor) {
  optional<int> oi;
  EXPECT_FALSE(oi.has_value());

  optional<std::complex<double>> ocmplx;
  EXPECT_FALSE(ocmplx.has_value());
}

TEST(Optional, CopyCtor) {
  {
    optional<int> oi = 0;
    optional<int> oi2(oi);
    ASSERT_TRUE(oi2.has_value());
    EXPECT_EQ(*oi2, 0);

    optional<std::complex<double>> ocmplx = std::complex(0.0, 0.0);
    optional<std::complex<double>> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2.has_value());
    EXPECT_EQ(*ocmplx2, std::complex(0.0, 0.0));
  }

  {
    optional<int> oi;
    optional<int> oi2(oi);
    EXPECT_FALSE(oi2.has_value());

    optional<std::complex<double>> ocmplx;
    optional<std::complex<double>> ocmplx2(ocmplx);
    EXPECT_FALSE(ocmplx2.has_value());
  }
}

TEST(Optional, Dtor) {
  EXPECT_EQ(DtorHelper::counter(), 0);

  {
    optional<DtorHelper> o = DtorHelper();
    EXPECT_EQ(DtorHelper::counter(), 1);

    optional<DtorHelper> o2 = DtorHelper();
    EXPECT_EQ(DtorHelper::counter(), 2);
  }

  EXPECT_EQ(DtorHelper::counter(), 0);
}

TEST(Optional, OperatorEquals) {
  EXPECT_EQ(DtorHelper::counter(), 0);

  {
    optional<DtorHelper> o;
    o = DtorHelper();
    EXPECT_EQ(DtorHelper::counter(), 1);

    optional<DtorHelper> o2;
    o2 = DtorHelper();
    EXPECT_EQ(DtorHelper::counter(), 2);
  }

  EXPECT_EQ(DtorHelper::counter(), 0);

  optional<int> oi;
  oi = nullopt;
  EXPECT_FALSE(oi.has_value());
}

TEST(Optional, CopyOperatorEquals) {
  {
    optional<int> oi = 0;
    const auto& oi2 = oi;
    oi = oi2;
    ASSERT_TRUE(oi.has_value());
    EXPECT_EQ(*oi, 0);
  }

  {
    optional<int> oi = 0;
    optional<int> oi2;
    oi2 = oi;
    ASSERT_TRUE(oi2.has_value());
    EXPECT_EQ(*oi2, 0);

    optional<std::complex<double>> ocmplx = std::complex(0.0, 0.0);
    optional<std::complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2.has_value());
    EXPECT_EQ(*ocmplx2, std::complex(0.0, 0.0));
  }

  {
    optional<int> oi;
    optional<int> oi2;
    oi2 = oi;
    EXPECT_FALSE(oi2.has_value());

    optional<std::complex<double>> ocmplx;
    optional<std::complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    EXPECT_FALSE(ocmplx2.has_value());
  }
}

TEST(Optional, OperatorBool) {
  optional<int> o;
  EXPECT_FALSE(o.has_value());
  {
    optional<int> oi = 0;
    EXPECT_TRUE(oi.has_value());

    optional<std::complex<double>> ocmplx = std::complex(0.0, 0.0);
    EXPECT_TRUE(ocmplx.has_value());
  }
}

TEST(Optional, HasValue) {
  optional<int> o;
  EXPECT_FALSE(o.has_value());
  {
    optional<int> oi = 0;
    EXPECT_TRUE(oi.has_value());

    optional<std::complex<double>> ocmplx = std::complex(0.0, 0.0);
    EXPECT_TRUE(ocmplx.has_value());
  }
}

TEST(Optional, OperatorStar) {
  {
    optional<int> oi = 0;
    ASSERT_TRUE(oi.has_value());
    EXPECT_EQ(*oi, 0);
    *oi = 1;
    ASSERT_TRUE(oi.has_value());
    EXPECT_EQ(*oi, 1);

    optional<std::complex<double>> ocmplx = std::complex(0.0, 0.0);
    ASSERT_TRUE(ocmplx.has_value());
    EXPECT_EQ(*ocmplx, std::complex(0.0, 0.0));
    *ocmplx = std::complex(1.0, 1.0);
    ASSERT_TRUE(ocmplx.has_value());
    EXPECT_EQ(*ocmplx, std::complex(1.0, 1.0));

    std::string s = "Test string";
    optional<std::string> os = s;
    ASSERT_TRUE(os.has_value());
    EXPECT_EQ(*os, "Test string");

    optional<std::string> os2 = std::move(s);
    ASSERT_TRUE(os2.has_value());
    EXPECT_EQ(*os2, "Test string");
  }
  {
    optional<int> oi;
    EXPECT_DEATH(*oi, "");

    optional<std::complex<double>> ocmplx;
    EXPECT_DEATH(ocmplx->real(), "");
  }
  {
    optional<int> oi;
    EXPECT_DEATH(*oi, "");

    optional<int> ni(nullopt);
    EXPECT_FALSE(ni.has_value());
    EXPECT_DEATH(*ni, "");
  }
}

TEST(Optional, ReturnByValue) {
  optional<int> oi = ReturnOptional(10);
  ASSERT_TRUE(oi.has_value());
  EXPECT_EQ(*oi, 10);

  optional<std::complex<double>> ocmplx =
      ReturnOptional(std::complex(1.0, 1.0));
  ASSERT_TRUE(ocmplx.has_value());
  EXPECT_EQ(*ocmplx, std::complex(1.0, 1.0));
}

TEST(Optional, OperatorArrow) {
  {
    optional<ArrowHelper> o = ArrowHelper();

    EXPECT_TRUE(o.has_value());
    o->Foo();
  }
  {
    optional<ArrowHelper> o;
    EXPECT_DEATH(o->Foo(), "");

    optional<ArrowHelper> ni(nullopt);
    EXPECT_FALSE(ni.has_value());
    EXPECT_DEATH(ni->Foo(), "");
  }
}

TEST(Optional, ValueOr) {
  optional<std::string> o;
  EXPECT_EQ(o.value_or("test"), "test");

  optional<std::string> o2("data");
  EXPECT_EQ(o2.value_or("test"), "data");
}

TEST(Optional, Emplace) {
  optional<std::string> o;
  EXPECT_FALSE(o.has_value());
  o.emplace("test");
  EXPECT_TRUE(o.has_value());
  EXPECT_EQ(*o, "test");
}

TEST(Optional, Comparison) {
  optional<std::string> o;
  optional<std::string> o2("data");

  EXPECT_NE(o, "data");
  EXPECT_NE(o, "data");

  EXPECT_EQ(o2, "data");
  EXPECT_EQ("data", o2);
}

}  // namespace rst
