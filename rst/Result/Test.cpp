// Copyright (c) 2015, Sergey Abbakumov
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

#include "rst/Result.h"

#include <complex>
#include <list>
#include <set>
#include <vector>

#include "gtest/gtest.h"

using rst::result::Err;
using rst::result::Result;

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
  void foo() {}
};


TEST(Result, ValueCtor) {
  {
    Result<int, int> oi = 0;
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(0, *oi);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);

    Result<char, int> oc = '\0';
    ASSERT_TRUE(oc == true);
    ASSERT_EQ('\0', *oc);

    Result<bool, int> ob = true;
    ASSERT_TRUE(ob == true);
    ASSERT_EQ(true, *ob);

    Result<void, int> ov = 0;
    ASSERT_TRUE(ov == true);
  }

  {
    Result<int, int> oi(-1, 0);
    ASSERT_TRUE(oi == false);

    Result<std::complex<double>, int> ocmplx(-1, 0);
    ASSERT_TRUE(ocmplx == false);

    Result<char, int> oc(-1, 0);
    ASSERT_TRUE(oc == false);

    Result<bool, int> ob(-1, 0);
    ASSERT_TRUE(ob == false);

    Result<void, int> ov(-1, 0);
    ASSERT_TRUE(ov == false);
  }
}

TEST(Result, CopyCtor) {
  {
    Result<int, int> oi = 0;
    Result<int, int> oi2(oi);
    ASSERT_TRUE(oi2 == true);
    ASSERT_EQ(0, *oi2);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    Result<std::complex<double>, int> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2 == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx2);

    Result<char, int> oc = '\0';
    Result<char, int> oc2(oc);
    ASSERT_TRUE(oc2 == true);
    ASSERT_EQ('\0', *oc2);

    Result<bool, int> ob = true;
    Result<bool, int> ob2(ob);
    ASSERT_TRUE(ob2 == true);
    ASSERT_EQ(true, *ob2);

    Result<void, int> ov = 0;
    Result<void, int> ov2(ov);
    ASSERT_TRUE(ov2 == true);
  }

  {
    Result<int, int> oi(-1, 0);
    Result<int, int> oi2(oi);
    ASSERT_TRUE(oi2 == false);

    Result<std::complex<double>, int> ocmplx(-1, 0);
    Result<std::complex<double>, int> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2 == false);

    Result<char, int> oc(-1, 0);
    Result<char, int> oc2(oc);
    ASSERT_TRUE(oc2 == false);

    Result<bool, int> ob(-1, 0);
    Result<bool, int> ob2(ob);
    ASSERT_TRUE(ob2 == false);

    Result<void, int> ov(-1, 0);
    Result<void, int> ov2(ov);
    ASSERT_TRUE(ov2 == false);
  }
}

TEST(Result, Dtor) {
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Result<DtorHelper, int> o = DtorHelper();
    ASSERT_EQ(1, DtorHelper::counter());
    
    Result<DtorHelper, int> o2 = DtorHelper();
    ASSERT_EQ(2, DtorHelper::counter());
  }
  
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Result<int, DtorHelper> o(DtorHelper(), 0);
    ASSERT_EQ(1, DtorHelper::counter());
    
    Result<int, DtorHelper> o2(DtorHelper(), 0);
    ASSERT_EQ(2, DtorHelper::counter());

    Result<void, DtorHelper> o3(DtorHelper(), 0);
    ASSERT_EQ(3, DtorHelper::counter());
  }
  
  ASSERT_EQ(0, DtorHelper::counter());
}

TEST(Result, OperatorEquals) {
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Result<DtorHelper, int> o = DtorHelper();
    o = DtorHelper();
    ASSERT_EQ(1, DtorHelper::counter());
    
    Result<DtorHelper, int> o2 = DtorHelper();
    o2 = DtorHelper();
    ASSERT_EQ(2, DtorHelper::counter());
  }
  
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Result<int, DtorHelper> o(DtorHelper(), 0);
    ASSERT_EQ(1, DtorHelper::counter());
    o = 0;
    ASSERT_EQ(0, DtorHelper::counter());
    
    Result<DtorHelper, int> o2(-1, 0);
    o2 = DtorHelper();
    ASSERT_EQ(1, DtorHelper::counter());
  }
  
  ASSERT_EQ(0, DtorHelper::counter());
}

TEST(Result, CopyOperatorEquals) {
  {
    Result<int, int> oi = 0;
    oi = oi;
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(0, *oi);
  }
  
  {
    Result<int, int> oi = 0;
    Result<int, int> oi2(-1, 0);
    oi2 = oi;
    ASSERT_TRUE(oi2 == true);
    ASSERT_EQ(0, *oi2);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    Result<std::complex<double>, int> ocmplx2(-1, 0);
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2 == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx2);

    Result<char, int> oc = '\0';
    Result<char, int> oc2(-1, 0);
    oc2 = oc;
    ASSERT_TRUE(oc2 == true);
    ASSERT_EQ('\0', *oc2);

    Result<bool, int> ob = true;
    Result<bool, int> ob2(-1, 0);
    ob2 = ob;
    ASSERT_TRUE(ob2 == true);
    ASSERT_EQ(true, *ob2);

    Result<void, int> ov = 0;
    Result<void, int> ov2(-1, 0);
    ov2 = ov;
    ASSERT_TRUE(ov2 == true);
  }

  {
    Result<int, int> oi(-1, 0);
    Result<int, int> oi2(-1, 0);
    oi2 = oi;
    ASSERT_TRUE(oi2 == false);

    Result<std::complex<double>, int> ocmplx(-1, 0);
    Result<std::complex<double>, int> ocmplx2(-1, 0);
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2 == false);

    Result<char, int> oc(-1, 0);
    Result<char, int> oc2(-1, 0);
    oc2 = oc;
    ASSERT_TRUE(oc2 == false);

    Result<bool, int> ob(-1, 0);
    Result<bool, int> ob2(-1, 0);
    ob2 = ob;
    ASSERT_TRUE(ob2 == false);

    Result<void, int> ov(-1, 0);
    Result<void, int> ov2(-1, 0);
    ov2 = ov;
    ASSERT_TRUE(ov2 == false);
  }

  {
    Result<int, int> oi = 8;
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(8, *oi);
    
    Result<int, int> oi2 = 168;
    ASSERT_TRUE(oi2 == true);
    ASSERT_EQ(168, *oi2);

    oi = std::move(oi2);
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(168, *oi);

    std::string test = "Test string! Test!";
    Result<std::string, int> os = test;
    ASSERT_TRUE(os == true);
    ASSERT_EQ("Test string! Test!", *os);

    Result<std::string, int> os2 = std::move(os);
    ASSERT_TRUE(os2 == true);
    ASSERT_EQ("Test string! Test!", *os2);
  }
}

TEST(Result, OperatorBool) {
  {
    Result<int, int> oi = 0;
    ASSERT_TRUE(oi == true);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);

    Result<char, int> oc = '\0';
    ASSERT_TRUE(oc == true);

    Result<bool, int> ob = true;
    ASSERT_TRUE(ob == true);

    Result<void, int> ov = 0;
    ASSERT_TRUE(ov == true);
  }

  {
    const Result<int, int> oi(-1, 0);
    ASSERT_TRUE(oi == false);

    const Result<std::complex<double>, int> ocmplx(-1, 0);
    ASSERT_TRUE(ocmplx == false);

    const Result<char, int> oc(-1, 0);
    ASSERT_TRUE(oc == false);

    const Result<bool, int> ob(-1, 0);
    ASSERT_TRUE(ob == false);

    const Result<void, int> ov(-1, 0);
    ASSERT_TRUE(ov == false);
  }
}

TEST(Result, OperatorStar) {
  {
    Result<int, int> oi = 0;
    ASSERT_EQ(0, *oi);
    *oi = 1;
    ASSERT_EQ(1, *oi);

    Result<std::complex<double>, int> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);
    *ocmplx = std::complex<double>(1.0, 1.0);
    ASSERT_EQ(std::complex<double>(1.0, 1.0), *ocmplx);

    Result<char, int> oc = '\0';
    ASSERT_EQ('\0', *oc);
    *oc = 'a';
    ASSERT_EQ('a', *oc);

    Result<bool, int> ob = true;
    ASSERT_EQ(true, *ob);
    *ob = false;
    ASSERT_EQ(false, *ob);
  }

  {
    const Result<int, int> oi = 0;
    ASSERT_EQ(0, *oi);

    const Result<std::complex<double>, int> ocmplx =
        std::complex<double>(0.0, 0.0);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);

    const Result<char, int> oc = '\0';
    ASSERT_EQ('\0', *oc);

    const Result<bool, int> ob = true;
    ASSERT_EQ(true, *ob);
  }
}

TEST(Result, OperatorL) {
  Result<int, int> a = 0;
  Result<int, int> b = 1;

  ASSERT_LT(*a, *b);

  a = -1;
  ASSERT_LT(*a, *b);

  a = 1;
  ASSERT_FALSE(*a < *b);

  a = 20;
  ASSERT_FALSE(*a < *b);
}

TEST(Result, UseSTL) {
  size_t i = 0;
  const int int_arr[] = {1000, 0, 10, -37, 40};
  
  std::vector<Result<int, int>> vec;
  for (const auto& e : int_arr) {
    vec.push_back(e);
  }

  i = 0;
  for (const auto& e : vec) {
    ASSERT_TRUE(e == true);
    ASSERT_EQ(int_arr[i], *e);
    i++;
  }


  std::list<Result<int, int>> lst;
  for (const auto& e : int_arr) {
    lst.push_back(e);
  }

  i = 0;
  for (const auto& e : lst) {
    ASSERT_TRUE(e == true);
    ASSERT_EQ(int_arr[i], *e);
    i++;
  }

  std::set<Result<int, int>> st;
  for (const auto& e : int_arr) {
    st.insert(e);
  }

  for (const auto& e: st) {
    ASSERT_TRUE(e == true);
  }

  auto it = st.begin();
  
  ASSERT_EQ(-37, **it);
  ++it;

  ASSERT_EQ(0, **it);
  ++it;

  ASSERT_EQ(10, **it);
  ++it;

  ASSERT_EQ(40, **it);
  ++it;

  ASSERT_EQ(1000, **it);
  ++it;
}

TEST(Result, Err) {
  Result<int, int> a = Err<int, int>(-1);
  ASSERT_TRUE(a == false);
  ASSERT_EQ(-1, a.Err());

  a.Err() = 10;
  ASSERT_TRUE(a == false);
  ASSERT_EQ(10, a.Err());

  Result<void, int> b = Err<void, int>(-1);
  ASSERT_TRUE(b == false);
  ASSERT_EQ(-1, b.Err());

  b.Err() = 10;
  ASSERT_TRUE(b == false);
  ASSERT_EQ(10, b.Err());
}

TEST(Result, OperatorArrow) {
  Result<ArrowHelper, int> r = ArrowHelper();

  r->foo();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

