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

#include "rst/Option.h"

#include <cassert>
#include <complex>
#include <list>
#include <set>
#include <vector>

#include "gtest/gtest.h"

using rst::None;
using rst::Option;

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


template <class T>
Option<T> ReturnOption(const T& val) {
  return Option<T>(val);
}


TEST(Option, ValueCtor) {
  Option<int> oi = 0;
  ASSERT_TRUE(oi == true);

  Option<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
  ASSERT_TRUE(ocmplx == true);

  Option<char> oc = '\0';
  ASSERT_TRUE(oc == true);

  Option<bool> ob = true;
  ASSERT_TRUE(ob == true);

  Option<int> ni(None);
  ASSERT_TRUE(ni == false);
}

TEST(Option, DefaultCtor) {
  Option<int> oi;
  ASSERT_TRUE(oi == false);

  Option<std::complex<double>> ocmplx;
  ASSERT_TRUE(ocmplx == false);

  Option<char> oc;
  ASSERT_TRUE(oc == false);

  Option<bool> ob;
  ASSERT_TRUE(ob == false);
}

TEST(Option, CopyCtor) {
  {
    Option<int> oi = 0;
    Option<int> oi2(oi);
    ASSERT_TRUE(oi2 == true);
    ASSERT_EQ(0, *oi2);

    Option<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    Option<std::complex<double>> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2 == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx2);

    Option<char> oc = '\0';
    Option<char> oc2(oc);
    ASSERT_TRUE(oc2 == true);
    ASSERT_EQ('\0', *oc2);

    Option<bool> ob = true;
    Option<bool> ob2(ob);
    ASSERT_TRUE(ob2 == true);
    ASSERT_EQ(true, *ob2);
  }

  {
    Option<int> oi;
    Option<int> oi2(oi);
    ASSERT_TRUE(oi2 == false);

    Option<std::complex<double>> ocmplx;
    Option<std::complex<double>> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2 == false);

    Option<char> oc;
    Option<char> oc2(oc);
    ASSERT_TRUE(oc2 == false);

    Option<bool> ob;
    Option<bool> ob2(ob);
    ASSERT_TRUE(ob2 == false);
  }
}

TEST(Option, Dtor) {
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Option<DtorHelper> o = DtorHelper();
    ASSERT_EQ(1, DtorHelper::counter());
    
    Option<DtorHelper> o2 = DtorHelper();
    ASSERT_EQ(2, DtorHelper::counter());
  }
  
  ASSERT_EQ(0, DtorHelper::counter());
}

TEST(Option, OperatorEquals) {
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Option<DtorHelper> o;
    o = DtorHelper();
    ASSERT_EQ(1, DtorHelper::counter());
    
    Option<DtorHelper> o2;
    o2 = DtorHelper();
    ASSERT_EQ(2, DtorHelper::counter());
  }
  
  ASSERT_EQ(0, DtorHelper::counter());

  Option<int> oi;
  oi = None;
  ASSERT_TRUE(oi == false);
}

TEST(Option, CopyOperatorEquals) {
  {
    Option<int> oi = 0;
    oi = oi;
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(0, *oi);
  }
  
  {
    Option<int> oi = 0;
    Option<int> oi2;
    oi2 = oi;
    ASSERT_TRUE(oi2 == true);
    ASSERT_EQ(0, *oi2);

    Option<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    Option<std::complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2 == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx2);

    Option<char> oc = '\0';
    Option<char> oc2;
    oc2 = oc;
    ASSERT_TRUE(oc2 == true);
    ASSERT_EQ('\0', *oc2);

    Option<bool> ob = true;
    Option<bool> ob2;
    ob2 = ob;
    ASSERT_TRUE(ob2 == true);
    ASSERT_EQ(true, *ob2);
  }

  {
    Option<int> oi;
    Option<int> oi2;
    oi2 = oi;
    ASSERT_TRUE(oi2 == false);

    Option<std::complex<double>> ocmplx;
    Option<std::complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2 == false);

    Option<char> oc;
    Option<char> oc2;
    oc2 = oc;
    ASSERT_TRUE(oc2 == false);

    Option<bool> ob;
    Option<bool> ob2;
    ob2 = ob;
    ASSERT_TRUE(ob2 == false);
  }
}

TEST(Option, OperatorBool) {
  {
    Option<int> oi = 0;
    ASSERT_TRUE(oi == true);

    Option<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);

    Option<char> oc = '\0';
    ASSERT_TRUE(oc == true);

    Option<bool> ob = true;
    ASSERT_TRUE(ob == true);
  }

  {
    const Option<int> oi;
    ASSERT_TRUE(oi == false);

    const Option<std::complex<double>> ocmplx;
    ASSERT_TRUE(ocmplx == false);

    const Option<char> oc;
    ASSERT_TRUE(oc == false);

    const Option<bool> ob;
    ASSERT_TRUE(ob == false);
  }
}

TEST(Option, OperatorStar) {
  {
    Option<int> oi = 0;
    ASSERT_EQ(0, *oi);
    *oi = 1;
    ASSERT_EQ(1, *oi);

    Option<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);
    *ocmplx = std::complex<double>(1.0, 1.0);
    ASSERT_EQ(std::complex<double>(1.0, 1.0), *ocmplx);

    Option<char> oc = '\0';
    ASSERT_EQ('\0', *oc);
    *oc = 'a';
    ASSERT_EQ('a', *oc);

    Option<bool> ob = true;
    ASSERT_EQ(true, *ob);
    *ob = false;
    ASSERT_EQ(false, *ob);
  }

  {
    const Option<int> oi = 0;
    ASSERT_EQ(0, *oi);

    const Option<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);

    const Option<char> oc = '\0';
    ASSERT_EQ('\0', *oc);

    const Option<bool> ob = true;
    ASSERT_EQ(true, *ob);
  }
}

TEST(Option, ReturnByValue) {
  Option<int> oi = ReturnOption(10);
  ASSERT_TRUE(oi == true);
  ASSERT_EQ(10, *oi);

  Option<std::complex<double>> ocmplx =
      ReturnOption(std::complex<double>(1.0, 1.0));
  ASSERT_TRUE(ocmplx == true);
  ASSERT_EQ(std::complex<double>(1.0, 1.0), *ocmplx);
}

TEST(Option, OperatorL) {
  Option<int> a = 0;
  Option<int> b = 1;

  ASSERT_LT(*a, *b);

  a = -1;
  ASSERT_LT(*a, *b);

  a = 1;
  ASSERT_FALSE(*a < *b);

  a = 20;
  ASSERT_FALSE(*a < *b);
}


TEST(Option, UseSTL) {
  size_t i = 0;
  const int int_arr[] = {1000, 0, 10, -37, 40};
  
  std::vector<Option<int>> vec;
  for (const auto& e : int_arr) {
    vec.push_back(e);
  }

  i = 0;
  for (const auto& e : vec) {
    ASSERT_TRUE(e == true);
    ASSERT_EQ(int_arr[i], *e);
    i++;
  }


  std::list<Option<int>> lst;
  for (const auto& e : int_arr) {
    lst.push_back(e);
  }

  i = 0;
  for (const auto& e : lst) {
    ASSERT_TRUE(e == true);
    ASSERT_EQ(int_arr[i], *e);
    i++;
  }

  std::set<Option<int>> st;
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

TEST(Option, OperatorArrow) {
  Option<ArrowHelper> o = ArrowHelper();

  o->foo();

  const Option<ArrowHelper> o2 = ArrowHelper();

  o2->foo();
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

