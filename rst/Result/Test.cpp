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

#include "rst/Result/Result.h"

#include <complex>

#include "gtest/gtest.h"

using rst::Err;
using rst::Result;

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

TEST(Result, DefaultCtor) {
  Result<int, int> oi;
  EXPECT_TRUE(oi == false);

  Result<void, int> ov;
  EXPECT_TRUE(ov == false);
}

TEST(Result, ValueCtor) {
  {
    Result<int, int> oi = 0;
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(0, *oi);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);
    EXPECT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);

    Result<char, int> oc = '\0';
    ASSERT_TRUE(oc == true);
    EXPECT_EQ('\0', *oc);

    Result<bool, int> ob = true;
    ASSERT_TRUE(ob == true);
    EXPECT_EQ(true, *ob);

    Result<void, int> ov = 0;
    EXPECT_TRUE(ov == true);
  }

  {
    Result<int, int> oi(-1, 0);
    EXPECT_TRUE(oi == false);

    Result<std::complex<double>, int> ocmplx(-1, 0);
    EXPECT_TRUE(ocmplx == false);

    Result<char, int> oc(-1, 0);
    EXPECT_TRUE(oc == false);

    Result<bool, int> ob(-1, 0);
    EXPECT_TRUE(ob == false);

    Result<void, int> ov(-1, 0);
    EXPECT_TRUE(ov == false);
  }
}

TEST(Result, MoveCtor) {
  {
    Result<int, int> oi = 0;
    Result<int, int> oi2(std::move(oi));
    ASSERT_TRUE(oi2 == true);
    EXPECT_EQ(0, *oi2);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    Result<std::complex<double>, int> ocmplx2(std::move(ocmplx));
    ASSERT_TRUE(ocmplx2 == true);
    EXPECT_EQ(std::complex<double>(0.0, 0.0), *ocmplx2);

    Result<char, int> oc = '\0';
    Result<char, int> oc2(std::move(oc));
    ASSERT_TRUE(oc2 == true);
    EXPECT_EQ('\0', *oc2);

    Result<bool, int> ob = true;
    Result<bool, int> ob2(std::move(ob));
    ASSERT_TRUE(ob2 == true);
    EXPECT_EQ(true, *ob2);

    Result<void, int> ov = 0;
    Result<void, int> ov2(std::move(ov));
    EXPECT_TRUE(ov2 == true);
  }

  {
    Result<int, int> oi = Err<int, int>(-1);
    Result<int, int> oi2(std::move(oi));
    EXPECT_TRUE(oi2 == false);

    Result<std::complex<double>, int> ocmplx =
        Err<std::complex<double>, int>(-1);
    Result<std::complex<double>, int> ocmplx2(std::move(ocmplx));
    EXPECT_TRUE(ocmplx2 == false);

    Result<char, int> oc = Err<char, int>(-1);
    Result<char, int> oc2(std::move(oc));
    EXPECT_TRUE(oc2 == false);

    Result<bool, int> ob = Err<bool, int>(-1);
    Result<bool, int> ob2(std::move(ob));
    EXPECT_TRUE(ob2 == false);

    Result<void, int> ov = Err<void, int>(-1);
    Result<void, int> ov2(std::move(ov));
    EXPECT_TRUE(ov2 == false);
  }
}

TEST(Result, Dtor) {
  EXPECT_EQ(0, DtorHelper::counter());

  {
    Result<DtorHelper, int> o = DtorHelper();
    o.Ignore();
    EXPECT_EQ(1, DtorHelper::counter());

    Result<DtorHelper, int> o2 = DtorHelper();
    o2.Ignore();
    EXPECT_EQ(2, DtorHelper::counter());
  }

  EXPECT_EQ(0, DtorHelper::counter());

  {
    Result<int, DtorHelper> o = Err<int, DtorHelper>(DtorHelper());
    o.Ignore();
    EXPECT_EQ(1, DtorHelper::counter());

    Result<int, DtorHelper> o2 = Err<int, DtorHelper>(DtorHelper());
    o2.Ignore();
    EXPECT_EQ(2, DtorHelper::counter());

    Result<void, DtorHelper> o3 = Err<void, DtorHelper>(DtorHelper());
    o3.Ignore();
    EXPECT_EQ(3, DtorHelper::counter());
  }

  EXPECT_EQ(0, DtorHelper::counter());
}

TEST(Result, OperatorEquals) {
  EXPECT_EQ(0, DtorHelper::counter());

  {
    Result<DtorHelper, int> o = DtorHelper();
    o.Ignore();
    o = DtorHelper();
    o.Ignore();
    EXPECT_EQ(1, DtorHelper::counter());

    Result<DtorHelper, int> o2 = DtorHelper();
    o2.Ignore();
    o2 = DtorHelper();
    o2.Ignore();
    EXPECT_EQ(2, DtorHelper::counter());
  }

  EXPECT_EQ(0, DtorHelper::counter());

  {
    Result<int, DtorHelper> o = Err<int, DtorHelper>(DtorHelper());
    EXPECT_EQ(1, DtorHelper::counter());
    o.Ignore();
    o = 0;
    o.Ignore();
    EXPECT_EQ(0, DtorHelper::counter());

    Result<DtorHelper, int> o2 = Err<DtorHelper, int>(-1);
    o2.Ignore();
    o2 = DtorHelper();
    o2.Ignore();
    EXPECT_EQ(1, DtorHelper::counter());
  }

  EXPECT_EQ(0, DtorHelper::counter());
}

TEST(Result, CopyOperatorEquals) {
  {
    Result<int, int> oi = 8;
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(8, *oi);

    Result<int, int> oi2 = 168;
    ASSERT_TRUE(oi2 == true);
    EXPECT_EQ(168, *oi2);

    oi = std::move(oi2);
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(168, *oi);

    std::string test = "Test string! Test!";
    Result<std::string, int> os = test;
    ASSERT_TRUE(os == true);
    EXPECT_EQ("Test string! Test!", *os);

    Result<std::string, int> os2 = std::move(os);
    ASSERT_TRUE(os2 == true);
    EXPECT_EQ("Test string! Test!", *os2);
  }
}

TEST(Result, OperatorBool) {
  {
    Result<int, int> oi = 0;
    EXPECT_TRUE(oi == true);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    EXPECT_TRUE(ocmplx == true);

    Result<char, int> oc = '\0';
    EXPECT_TRUE(oc == true);

    Result<bool, int> ob = true;
    EXPECT_TRUE(ob == true);

    Result<void, int> ov = 0;
    EXPECT_TRUE(ov == true);
  }
}

TEST(Result, OperatorStar) {
  {
    Result<int, int> oi = 0;
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(0, *oi);
    *oi = 1;
    ASSERT_TRUE(oi == true);
    EXPECT_EQ(1, *oi);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);
    EXPECT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);
    *ocmplx = std::complex<double>(1.0, 1.0);
    ASSERT_TRUE(ocmplx == true);
    EXPECT_EQ(std::complex<double>(1.0, 1.0), *ocmplx);

    Result<char, int> oc = '\0';
    ASSERT_TRUE(oc == true);
    EXPECT_EQ('\0', *oc);
    *oc = 'a';
    ASSERT_TRUE(oc == true);
    EXPECT_EQ('a', *oc);

    Result<bool, int> ob = true;
    ASSERT_TRUE(ob == true);
    EXPECT_EQ(true, *ob);
    *ob = false;
    ASSERT_TRUE(ob == true);
    EXPECT_EQ(false, *ob);
  }
}

TEST(Result, Err) {
  Result<int, int> a = Err<int, int>(-1);
  ASSERT_TRUE(a == false);
  EXPECT_EQ(-1, a.Err());

  a.Err() = 10;
  ASSERT_TRUE(a == false);
  EXPECT_EQ(10, a.Err());

  Result<void, int> b = Err<void, int>(-1);
  ASSERT_TRUE(b == false);
  EXPECT_EQ(-1, b.Err());

  b.Err() = 10;
  ASSERT_TRUE(b == false);
  EXPECT_EQ(10, b.Err());
}

TEST(Result, OperatorArrow) {
  Result<ArrowHelper, int> r = ArrowHelper();
  EXPECT_TRUE(r == true);
  r->foo();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

