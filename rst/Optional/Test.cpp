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

#include "rst/Optional/Optional.h"

#include <cassert>
#include <complex>
#include <list>
#include <set>
#include <vector>

#include "gtest/gtest.h"

using rst::None;
using rst::Optional;

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
  void foo() const {}
};

template <class T>
Optional<T> ReturnOptional(const T& val) {
  return Optional<T>(val);
}

}  // namespace

TEST(Optional, ValueCtor) {
  Optional<int> oi = 0;
  ASSERT_TRUE(oi == true);

  Optional<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
  ASSERT_TRUE(ocmplx == true);

  Optional<char> oc = '\0';
  ASSERT_TRUE(oc == true);

  Optional<bool> ob = true;
  ASSERT_TRUE(ob == true);

  Optional<int> ni(None);
  ASSERT_TRUE(ni == false);
}

TEST(Optional, DefaultCtor) {
  Optional<int> oi;
  ASSERT_TRUE(oi == false);

  Optional<std::complex<double>> ocmplx;
  ASSERT_TRUE(ocmplx == false);

  Optional<char> oc;
  ASSERT_TRUE(oc == false);

  Optional<bool> ob;
  ASSERT_TRUE(ob == false);
}

TEST(Optional, CopyCtor) {
  {
    Optional<int> oi = 0;
    Optional<int> oi2(oi);
    ASSERT_TRUE(oi2 == true);
    ASSERT_EQ(0, *oi2);

    Optional<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    Optional<std::complex<double>> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2 == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx2);

    Optional<char> oc = '\0';
    Optional<char> oc2(oc);
    ASSERT_TRUE(oc2 == true);
    ASSERT_EQ('\0', *oc2);

    Optional<bool> ob = true;
    Optional<bool> ob2(ob);
    ASSERT_TRUE(ob2 == true);
    ASSERT_EQ(true, *ob2);
  }

  {
    Optional<int> oi;
    Optional<int> oi2(oi);
    ASSERT_TRUE(oi2 == false);

    Optional<std::complex<double>> ocmplx;
    Optional<std::complex<double>> ocmplx2(ocmplx);
    ASSERT_TRUE(ocmplx2 == false);

    Optional<char> oc;
    Optional<char> oc2(oc);
    ASSERT_TRUE(oc2 == false);

    Optional<bool> ob;
    Optional<bool> ob2(ob);
    ASSERT_TRUE(ob2 == false);
  }
}

TEST(Optional, Dtor) {
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Optional<DtorHelper> o = DtorHelper();
    ASSERT_EQ(1, DtorHelper::counter());

    Optional<DtorHelper> o2 = DtorHelper();
    ASSERT_EQ(2, DtorHelper::counter());
  }

  ASSERT_EQ(0, DtorHelper::counter());
}

TEST(Optional, OperatorEquals) {
  ASSERT_EQ(0, DtorHelper::counter());

  {
    Optional<DtorHelper> o;
    o = DtorHelper();
    ASSERT_EQ(1, DtorHelper::counter());

    Optional<DtorHelper> o2;
    o2 = DtorHelper();
    ASSERT_EQ(2, DtorHelper::counter());
  }

  ASSERT_EQ(0, DtorHelper::counter());

  Optional<int> oi;
  oi = None;
  ASSERT_TRUE(oi == false);
}

TEST(Optional, CopyOperatorEquals) {
  {
    Optional<int> oi = 0;
    oi = oi;
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(0, *oi);
  }

  {
    Optional<int> oi = 0;
    Optional<int> oi2;
    oi2 = oi;
    ASSERT_TRUE(oi2 == true);
    ASSERT_EQ(0, *oi2);

    Optional<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    Optional<std::complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2 == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx2);

    Optional<char> oc = '\0';
    Optional<char> oc2;
    oc2 = oc;
    ASSERT_TRUE(oc2 == true);
    ASSERT_EQ('\0', *oc2);

    Optional<bool> ob = true;
    Optional<bool> ob2;
    ob2 = ob;
    ASSERT_TRUE(ob2 == true);
    ASSERT_EQ(true, *ob2);
  }

  {
    Optional<int> oi;
    Optional<int> oi2;
    oi2 = oi;
    ASSERT_TRUE(oi2 == false);

    Optional<std::complex<double>> ocmplx;
    Optional<std::complex<double>> ocmplx2;
    ocmplx2 = ocmplx;
    ASSERT_TRUE(ocmplx2 == false);

    Optional<char> oc;
    Optional<char> oc2;
    oc2 = oc;
    ASSERT_TRUE(oc2 == false);

    Optional<bool> ob;
    Optional<bool> ob2;
    ob2 = ob;
    ASSERT_TRUE(ob2 == false);
  }
}

TEST(Optional, OperatorBool) {
  {
    Optional<int> oi = 0;
    ASSERT_TRUE(oi == true);

    Optional<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);

    Optional<char> oc = '\0';
    ASSERT_TRUE(oc == true);

    Optional<bool> ob = true;
    ASSERT_TRUE(ob == true);
  }
}

TEST(Optional, OperatorStar) {
  {
    Optional<int> oi = 0;
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(0, *oi);
    *oi = 1;
    ASSERT_TRUE(oi == true);
    ASSERT_EQ(1, *oi);

    Optional<std::complex<double>> ocmplx = std::complex<double>(0.0, 0.0);
    ASSERT_TRUE(ocmplx == true);
    ASSERT_EQ(std::complex<double>(0.0, 0.0), *ocmplx);
    *ocmplx = std::complex<double>(1.0, 1.0);
    ASSERT_TRUE(ocmplx == true);
    ASSERT_EQ(std::complex<double>(1.0, 1.0), *ocmplx);

    Optional<char> oc = '\0';
    ASSERT_TRUE(oc == true);
    ASSERT_EQ('\0', *oc);
    *oc = 'a';
    ASSERT_TRUE(oc == true);
    ASSERT_EQ('a', *oc);

    Optional<bool> ob = true;
    ASSERT_TRUE(ob == true);
    ASSERT_EQ(true, *ob);
    *ob = false;
    ASSERT_TRUE(ob == true);
    ASSERT_EQ(false, *ob);
  }
}

TEST(Optional, ReturnByValue) {
  Optional<int> oi = ReturnOptional(10);
  ASSERT_TRUE(oi == true);
  ASSERT_EQ(10, *oi);

  Optional<std::complex<double>> ocmplx =
      ReturnOptional(std::complex<double>(1.0, 1.0));
  ASSERT_TRUE(ocmplx == true);
  ASSERT_EQ(std::complex<double>(1.0, 1.0), *ocmplx);
}

TEST(Optional, OperatorArrow) {
  Optional<ArrowHelper> o = ArrowHelper();

  ASSERT_TRUE(o == true);
  o->foo();
}
