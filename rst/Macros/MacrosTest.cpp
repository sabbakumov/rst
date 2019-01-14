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

#include "rst/Macros/Macros.h"

#include <type_traits>

#include <gtest/gtest.h>

namespace rst {
namespace {

class NonCopyable {
 public:
  NonCopyable() = default;

 private:
  RST_DISALLOW_COPY(NonCopyable);
};

class NonAssignable {
 public:
  NonAssignable() = default;

 private:
  RST_DISALLOW_ASSIGN(NonAssignable);
};

class NonCopyAssignable {
 public:
  NonCopyAssignable() = default;

 private:
  RST_DISALLOW_COPY_AND_ASSIGN(NonCopyAssignable);
};

class NonConstructible {
 private:
  RST_DISALLOW_IMPLICIT_CONSTRUCTORS(NonConstructible);
};

}  // namespace

TEST(NonCopyable, NonCopyable) {
  using T = NonCopyable;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_assignable<T>::value);
  EXPECT_FALSE(std::is_move_constructible<T>::value);
  EXPECT_TRUE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonAssignable) {
  using T = NonAssignable;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_assignable<T>::value);
  EXPECT_TRUE(std::is_move_constructible<T>::value);
  EXPECT_FALSE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonCopyAssignable) {
  using T = NonCopyAssignable;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_assignable<T>::value);
  EXPECT_FALSE(std::is_move_constructible<T>::value);
  EXPECT_FALSE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonConstructible) {
  using T = NonConstructible;
  EXPECT_FALSE(std::is_default_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_assignable<T>::value);
  EXPECT_FALSE(std::is_move_constructible<T>::value);
  EXPECT_FALSE(std::is_move_assignable<T>::value);
}

#define RST_BUILDFLAG_FOO() (true)
#define RST_BUILDFLAG_BAR() (false)

TEST(BuildFlag, TurnOn) {
#if RST_BUILDFLAG(FOO)
  SUCCEED();
#else   // RST_BUILDFLAG(FOO)
  FAIL();
#endif  // RST_BUILDFLAG(FOO)
}

TEST(BuildFlag, TurnOff) {
#if !RST_BUILDFLAG(BAR)
  SUCCEED();
#else   // !RST_BUILDFLAG(BAR)
  FAIL();
#endif  // !RST_BUILDFLAG(BAR)
}

}  // namespace rst
