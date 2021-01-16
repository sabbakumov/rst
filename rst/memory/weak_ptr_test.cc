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

#include "rst/memory/weak_ptr.h"

#include <utility>

#include <gtest/gtest.h>

namespace rst {
namespace {

struct Base : SupportsWeakPtr<Base> {};
struct Derived : Base {};

}  // namespace

TEST(WeakPtr, Basic) {
  {
    Base base;
    const auto ptr = base.AsWeakPtr();
    EXPECT_EQ(ptr.GetNullable(), &base);
  }
  {
    const Base base;
    const auto ptr = base.AsWeakPtr();
    EXPECT_EQ(ptr.GetNullable(), &base);
  }
}

TEST(WeakPtr, Comparison) {
  {
    Base base;
    const auto ptr = base.AsWeakPtr();
    const auto ptr2 = ptr;
    EXPECT_EQ(ptr.GetNullable(), ptr2.GetNullable());
  }
  {
    const Base base;
    const auto ptr = base.AsWeakPtr();
    const auto ptr2 = ptr;
    EXPECT_EQ(ptr.GetNullable(), ptr2.GetNullable());
  }
}

TEST(WeakPtr, Move) {
  {
    Base base;
    const auto ptr = base.AsWeakPtr();
    auto ptr2 = base.AsWeakPtr();
    const auto ptr3 = std::move(ptr2);
    EXPECT_EQ(ptr.GetNullable(), ptr3.GetNullable());
  }
  {
    const Base base;
    const auto ptr = base.AsWeakPtr();
    auto ptr2 = base.AsWeakPtr();
    const auto ptr3 = std::move(ptr2);
    EXPECT_EQ(ptr.GetNullable(), ptr3.GetNullable());
  }
}

TEST(WeakPtr, OutOfScope) {
  {
    WeakPtr<Base> ptr;
    EXPECT_EQ(ptr.GetNullable(), nullptr);
    {
      Base base;
      ptr = base.AsWeakPtr();
    }
    EXPECT_EQ(ptr.GetNullable(), nullptr);
  }
  {
    WeakPtr<const Base> ptr;
    EXPECT_EQ(ptr.GetNullable(), nullptr);
    {
      const Base base;
      ptr = base.AsWeakPtr();
    }
    EXPECT_EQ(ptr.GetNullable(), nullptr);
  }
}

TEST(WeakPtr, Multiple) {
  {
    WeakPtr<Base> a, b;
    {
      Base base;
      a = base.AsWeakPtr();
      b = base.AsWeakPtr();
      EXPECT_EQ(a.GetNullable(), &base);
      EXPECT_EQ(b.GetNullable(), &base);
    }
    EXPECT_EQ(a.GetNullable(), nullptr);
    EXPECT_EQ(b.GetNullable(), nullptr);
  }
  {
    WeakPtr<const Base> a, b;
    {
      const Base base;
      a = base.AsWeakPtr();
      b = base.AsWeakPtr();
      EXPECT_EQ(a.GetNullable(), &base);
      EXPECT_EQ(b.GetNullable(), &base);
    }
    EXPECT_EQ(a.GetNullable(), nullptr);
    EXPECT_EQ(b.GetNullable(), nullptr);
  }
}

TEST(WeakPtr, MultipleStaged) {
  {
    WeakPtr<Base> a;
    {
      Base base;
      a = base.AsWeakPtr();
      { const auto b = base.AsWeakPtr(); }
      EXPECT_NE(a.GetNullable(), nullptr);
    }
    EXPECT_EQ(a.GetNullable(), nullptr);
  }
  {
    WeakPtr<const Base> a;
    {
      const Base base;
      a = base.AsWeakPtr();
      { const auto b = base.AsWeakPtr(); }
      EXPECT_NE(a.GetNullable(), nullptr);
    }
    EXPECT_EQ(a.GetNullable(), nullptr);
  }
}

TEST(WeakPtr, UpCast) {
  {
    Derived derived;
    WeakPtr<Base> ptr = derived.AsWeakPtr();
    EXPECT_EQ(ptr.GetNullable(), &derived);
  }
  {
    const Derived derived;
    WeakPtr<const Base> ptr = derived.AsWeakPtr();
    EXPECT_EQ(ptr.GetNullable(), &derived);
  }
}

TEST(WeakPtr, DownCast) {
  {
    Derived derived;
    WeakPtr<Derived> ptr = AsWeakPtr(&derived);
    EXPECT_EQ(ptr.GetNullable(), &derived);
  }
  {
    const Derived derived;
    WeakPtr<const Derived> ptr = AsWeakPtr(&derived);
    EXPECT_EQ(ptr.GetNullable(), &derived);
  }
}

TEST(WeakPtr, ConstructFromNullptr) {
  {
    const WeakPtr<Base> ptr(nullptr);
    EXPECT_EQ(ptr.GetNullable(), nullptr);
  }
  {
    const WeakPtr<const Base> ptr(nullptr);
    EXPECT_EQ(ptr.GetNullable(), nullptr);
  }
}

TEST(WeakPtr, AssignNullptr) {
  {
    Base base;
    WeakPtr<Base> ptr = base.AsWeakPtr();
    ptr = nullptr;
    EXPECT_EQ(ptr.GetNullable(), nullptr);
  }
  {
    const Base base;
    WeakPtr<const Base> ptr = base.AsWeakPtr();
    ptr = nullptr;
    EXPECT_EQ(ptr.GetNullable(), nullptr);
  }
}

}  // namespace rst
