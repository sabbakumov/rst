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

using rst::NonCopyConstructible;
using rst::NonAssignable;
using rst::NonCopyable;
using rst::NonMoveConstructible;
using rst::NonMoveAssignable;
using rst::NonMovable;

TEST(NonCopyConstructible, Test) {
  EXPECT_FALSE(std::is_copy_constructible<NonCopyConstructible>::value);
  EXPECT_TRUE(std::is_copy_assignable<NonCopyConstructible>::value);
  EXPECT_TRUE(std::is_move_constructible<NonCopyConstructible>::value);
  EXPECT_TRUE(std::is_move_assignable<NonCopyConstructible>::value);
}

TEST(NonAssignable, Test) {
  EXPECT_TRUE(std::is_copy_constructible<NonAssignable>::value);
  EXPECT_FALSE(std::is_copy_assignable<NonAssignable>::value);
  EXPECT_TRUE(std::is_move_constructible<NonAssignable>::value);
  EXPECT_TRUE(std::is_move_assignable<NonAssignable>::value);
}

TEST(NonCopyable, Test) {
  EXPECT_FALSE(std::is_copy_constructible<NonCopyable>::value);
  EXPECT_FALSE(std::is_copy_assignable<NonCopyable>::value);
  EXPECT_TRUE(std::is_move_constructible<NonCopyable>::value);
  EXPECT_TRUE(std::is_move_assignable<NonCopyable>::value);
}

TEST(NonMoveConstructible, Test) {
  EXPECT_TRUE(std::is_copy_constructible<NonMoveConstructible>::value);
  EXPECT_TRUE(std::is_copy_assignable<NonMoveConstructible>::value);
  EXPECT_FALSE(std::is_move_constructible<NonMoveConstructible>::value);
  EXPECT_TRUE(std::is_move_assignable<NonMoveConstructible>::value);
}

TEST(NonMoveAssignable, Test) {
  EXPECT_TRUE(std::is_copy_constructible<NonMoveAssignable>::value);
  EXPECT_TRUE(std::is_copy_assignable<NonMoveAssignable>::value);
  EXPECT_TRUE(std::is_move_constructible<NonMoveAssignable>::value);
  EXPECT_FALSE(std::is_move_assignable<NonMoveAssignable>::value);
}

TEST(NonMovable, Test) {
  EXPECT_TRUE(std::is_copy_constructible<NonMovable>::value);
  EXPECT_TRUE(std::is_copy_assignable<NonMovable>::value);
  EXPECT_FALSE(std::is_move_constructible<NonMovable>::value);
  EXPECT_FALSE(std::is_move_assignable<NonMovable>::value);
}
