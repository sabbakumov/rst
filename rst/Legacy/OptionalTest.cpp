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

#include "rst/Legacy/Optional.h"

#include <complex>
#include <string>
#include <utility>

#include "gtest/gtest.h"

using std::complex;
using std::string;

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
Optional<T> ReturnOptional(const T& val) {
  return Optional<T>(val);
}

}  // namespace

TEST(Optional, ValueCtor) {
  Optional<int> oi = 0;
  EXPECT_TRUE(oi == true);

  Optional<complex<double>> ocmplx = complex<double>(0.0, 0.0);
  EXPECT_TRUE(ocmplx == true);

  string s = "Test string";
  Optional<string> os = s;
  EXPECT_TRUE(os == true);

  Optional<string> os2 = std::move(s);
  EXPECT_TRUE(os2 == true);

  Optional<int> ni(None);
  EXPECT_TRUE(ni == false);
}

TEST(Optional, DefaultCtor) {
  Optional<int> oi;
  EXPECT_TRUE(oi == false);

  Optional<complex<double>> ocmplx;
  EXPECT_TRUE(ocmplx == false);
}

TEST(Optional, CopyCtor) {
  {
    Optional<int> oi = 0;
    Optional<int> oi2(oi);
    ASSERT_TRUE(oi2 == true);
    EXPECT_EQ(0, *oi2);

    Optional<complex<double>> ocmplx = complex<double>(0.0, 0.0);
    Optional<complex<double>> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2 == true);
    EXPECT_EQ(complex<double>(0.0, 0.0), *ocmplx2);
  }

  {
    Optional<int> oi;
    Optional<int> oi2(oi);
    EXPECT_TRUE(oi2 == false);

    Optional<complex<double>> ocmplx;
    Optional<complex<double>> ocmplx2(ocmplx);
    EXPECT_TRUE(ocmplx2 == false);
  }
}

TEST(Optional, Dtor) {
  EXPECT_EQ(0, DtorHelper::counter());

  {
    Optional<DtorHelper> o = DtorHelper();
    EXPECT_EQ(1, DtorHelper::counter());

    Optional<DtorHelper> o2 = DtorHelper();
    EXPECT_EQ(2, DtorHelper::counter());
  }

  EXPECT_EQ(0, DtorHelper::counter());
}

TEST(Optional, OperatorEquals) {
  EXPECT_EQ(0, DtorHelper::counter());

  {
    Optional<DtorHelper> o;
    o = DtorHelper();
    EXPECT_EQ(1, DtorHelper::counter());

    Optional<DtorHelper> o2;
    o2 = DtorHelper();
    EXPECT_EQ(2, DtorHelper::counter());
  }

  EXPECT_EQ(0, DtorHelper::counter());

  Optional<int> oi;
  oi = None;
  EXPECT_TRUE(oi == false);
}

TEST(Optional, CopyOperatorEquals) {
  {
    Optional<int> oi = 0;
    oi = oi;
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(0, *oi);
  }

  {
    Optional<int> oi = 0;
    Optional<int> oi2;
    oi2 = oi;
    ASSERT_TRUE(oi2 == true);
    EXPECT_EQ(0, *oi2);

    Optional<complex<double>> ocmplx = complex<double>(0.0, 0.0);
    Optional<complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2 == true);
    EXPECT_EQ(complex<double>(0.0, 0.0), *ocmplx2);
  }

  {
    Optional<int> oi;
    Optional<int> oi2;
    oi2 = oi;
    EXPECT_TRUE(oi2 == false);

    Optional<complex<double>> ocmplx;
    Optional<complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    EXPECT_TRUE(ocmplx2 == false);
  }
}

TEST(Optional, OperatorBool) {
  {
    Optional<int> oi = 0;
    EXPECT_TRUE(oi == true);

    Optional<complex<double>> ocmplx = complex<double>(0.0, 0.0);
    EXPECT_TRUE(ocmplx == true);
  }
}

TEST(Optional, OperatorStar) {
  {
    Optional<int> oi = 0;
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(0, *oi);
    *oi = 1;
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(1, *oi);

    Optional<complex<double>> ocmplx = complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);
    EXPECT_EQ(complex<double>(0.0, 0.0), *ocmplx);
    *ocmplx = complex<double>(1.0, 1.0);
    ASSERT_TRUE(ocmplx == true);
    EXPECT_EQ(complex<double>(1.0, 1.0), *ocmplx);

    string s = "Test string";
    Optional<string> os = s;
    ASSERT_TRUE(os == true);
    EXPECT_EQ("Test string", *os);

    Optional<string> os2 = std::move(s);
    ASSERT_TRUE(os2 == true);
    EXPECT_EQ("Test string", *os2);
  }
  {
    Optional<int> oi;
    EXPECT_DEATH(*oi, "");

    Optional<complex<double>> ocmplx;
    EXPECT_DEATH(ocmplx->real(), "");
  }
  {
    Optional<int> oi;
    EXPECT_DEATH(*oi, "");

    Optional<int> ni(None);
    EXPECT_TRUE(ni == false);
    EXPECT_DEATH(*ni, "");
  }
}

TEST(Optional, ReturnByValue) {
  Optional<int> oi = ReturnOptional(10);
  ASSERT_TRUE(oi == true);
  EXPECT_EQ(10, *oi);

  Optional<complex<double>> ocmplx = ReturnOptional(complex<double>(1.0, 1.0));
  ASSERT_TRUE(ocmplx == true);
  EXPECT_EQ(complex<double>(1.0, 1.0), *ocmplx);
}

TEST(Optional, OperatorArrow) {
  {
    Optional<ArrowHelper> o = ArrowHelper();

    EXPECT_TRUE(o == true);
    o->Foo();
  }
  {
    Optional<ArrowHelper> o;
    EXPECT_DEATH(o->Foo(), "");

    Optional<ArrowHelper> ni(None);
    EXPECT_TRUE(ni == false);
    EXPECT_DEATH(ni->Foo(), "");
  }
}

}  // namespace rst
