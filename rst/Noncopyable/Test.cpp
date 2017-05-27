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

#include "rst/Noncopyable/Noncopyable.h"

#include <type_traits>

#include <gtest/gtest.h>

using rst::NonAssignable;
using rst::NonCopyConstructible;
using rst::NonCopyable;
using rst::NonMovable;
using rst::NonMoveAssignable;
using rst::NonMoveConstructible;

TEST(NonCopyable, NonCopyConstructible) {
  using T = NonCopyConstructible;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_assignable<T>::value);
  EXPECT_TRUE(std::is_move_constructible<T>::value);
  EXPECT_TRUE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonAssignable) {
  using T = NonAssignable;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_assignable<T>::value);
  EXPECT_TRUE(std::is_move_constructible<T>::value);
  EXPECT_TRUE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonCopyable) {
  using T = NonCopyable;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_constructible<T>::value);
  EXPECT_FALSE(std::is_copy_assignable<T>::value);
  EXPECT_TRUE(std::is_move_constructible<T>::value);
  EXPECT_TRUE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonMoveConstructible) {
  using T = NonMoveConstructible;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_assignable<T>::value);
  EXPECT_FALSE(std::is_move_constructible<T>::value);
  EXPECT_TRUE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonMoveAssignable) {
  using T = NonMoveAssignable;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_assignable<T>::value);
  EXPECT_TRUE(std::is_move_constructible<T>::value);
  EXPECT_FALSE(std::is_move_assignable<T>::value);
}

TEST(NonCopyable, NonMovable) {
  using T = NonMovable;
  EXPECT_TRUE(std::is_default_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_constructible<T>::value);
  EXPECT_TRUE(std::is_copy_assignable<T>::value);
  EXPECT_FALSE(std::is_move_constructible<T>::value);
  EXPECT_FALSE(std::is_move_assignable<T>::value);
}
