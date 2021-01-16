// Copyright (c) 2019, Sergey Abbakumov
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

#include "rst/bind/bind.h"

#include <memory>
#include <string>
#include <utility>

#include <gtest/gtest.h>

#include "rst/macros/macros.h"
#include "rst/memory/weak_ptr.h"

using namespace std::placeholders;  // NOLINT(build/namespaces)

namespace rst {
namespace {

class Weaked : public SupportsWeakPtr<Weaked> {
 public:
  Weaked() = default;
  ~Weaked() = default;

  const std::string& s() const { return s_; }

  void Foo() { s_ = "Foo"; }
  void FooConst() const { s_ = "Foo"; }
  void Bar(std::string s) { s_ = std::move(s); }
  void BarConst(std::string s) const { s_ = std::move(s); }
  void Baz(std::unique_ptr<std::string> s) { s_ = std::move(*s); }
  void BazConst(std::unique_ptr<std::string> s) const { s_ = std::move(*s); }

 private:
  mutable std::string s_;

  RST_DISALLOW_COPY_AND_ASSIGN(Weaked);
};

}  // namespace

TEST(Bind, NoArguments) {
  {
    Weaked weaked;
    auto foo = Bind(&Weaked::Foo, weaked.AsWeakPtr());

    EXPECT_TRUE(weaked.s().empty());
    foo();
    EXPECT_EQ(weaked.s(), "Foo");
  }
  {
    const Weaked weaked;
    auto foo = Bind(&Weaked::FooConst, weaked.AsWeakPtr());

    EXPECT_TRUE(weaked.s().empty());
    foo();
    EXPECT_EQ(weaked.s(), "Foo");
  }
}

TEST(Bind, NoArgumentsOnDestruction) {
  {
    std::function<void()> foo;
    {
      Weaked weaked;
      foo = Bind(&Weaked::Foo, weaked.AsWeakPtr());
    }
    foo();
  }
  {
    std::function<void()> foo;
    {
      const Weaked weaked;
      foo = Bind(&Weaked::FooConst, weaked.AsWeakPtr());
    }
    foo();
  }
}

TEST(Bind, OneArgument) {
  {
    Weaked weaked;
    auto bar = Bind(&Weaked::Bar, weaked.AsWeakPtr(), _1);

    EXPECT_TRUE(weaked.s().empty());
    bar("Bar");
    EXPECT_EQ(weaked.s(), "Bar");
  }
  {
    const Weaked weaked;
    auto bar = Bind(&Weaked::BarConst, weaked.AsWeakPtr(), _1);

    EXPECT_TRUE(weaked.s().empty());
    bar("Bar");
    EXPECT_EQ(weaked.s(), "Bar");
  }
}

TEST(Bind, OneArgumentOnDestruction) {
  {
    std::function<void(std::string)> bar;
    {
      Weaked weaked;
      bar = Bind(&Weaked::Bar, weaked.AsWeakPtr(), _1);
    }
    bar("Bar");
  }
  {
    std::function<void(std::string)> bar;
    {
      const Weaked weaked;
      bar = Bind(&Weaked::BarConst, weaked.AsWeakPtr(), _1);
    }
    bar("Bar");
  }
}

TEST(Bind, OneMoveOnlyArgument) {
  {
    Weaked weaked;
    auto baz = Bind(&Weaked::Baz, weaked.AsWeakPtr(), _1);

    EXPECT_TRUE(weaked.s().empty());
    baz(std::make_unique<std::string>("Baz"));
    EXPECT_EQ(weaked.s(), "Baz");
  }
  {
    const Weaked weaked;
    auto baz = Bind(&Weaked::BazConst, weaked.AsWeakPtr(), _1);

    EXPECT_TRUE(weaked.s().empty());
    baz(std::make_unique<std::string>("Baz"));
    EXPECT_EQ(weaked.s(), "Baz");
  }
}

TEST(Bind, OneMoveOnlyArgumentOnDestruction) {
  {
    std::function<void(std::unique_ptr<std::string>)> baz;
    {
      Weaked weaked;
      baz = Bind(&Weaked::Baz, weaked.AsWeakPtr(), _1);
    }
    baz(std::make_unique<std::string>("Baz"));
  }
  {
    std::function<void(std::unique_ptr<std::string>)> baz;
    {
      const Weaked weaked;
      baz = Bind(&Weaked::BazConst, weaked.AsWeakPtr(), _1);
    }
    baz(std::make_unique<std::string>("Baz"));
  }
}

}  // namespace rst
