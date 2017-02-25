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

#ifndef RST_NONCOPYABLE_NONCOPYABLE_H_
#define RST_NONCOPYABLE_NONCOPYABLE_H_

namespace rst {

class NonCopyConstructible {
 public:
  NonCopyConstructible() = default;
  NonCopyConstructible(const NonCopyConstructible&) = delete;
  NonCopyConstructible(NonCopyConstructible&&) = default;
  NonCopyConstructible& operator=(const NonCopyConstructible&) = default;
  NonCopyConstructible& operator=(NonCopyConstructible&&) = default;
};

class NonAssignable {
 public:
  NonAssignable() = default;
  NonAssignable(const NonAssignable&) = default;
  NonAssignable(NonAssignable&&) = default;
  NonAssignable& operator=(const NonAssignable&) = delete;
  NonAssignable& operator=(NonAssignable&&) = default;
};

class NonCopyable {
 public:
  NonCopyable() = default;
  NonCopyable(const NonCopyable&) = delete;
  NonCopyable(NonCopyable&&) = default;
  NonCopyable& operator=(const NonCopyable&) = delete;
  NonCopyable& operator=(NonCopyable&&) = default;
};

class NonMoveConstructible {
 public:
  NonMoveConstructible() = default;
  NonMoveConstructible(const NonMoveConstructible&) = default;
  NonMoveConstructible(NonMoveConstructible&&) = delete;
  NonMoveConstructible& operator=(const NonMoveConstructible&) = default;
  NonMoveConstructible& operator=(NonMoveConstructible&&) = default;
};

class NonMoveAssignable {
 public:
  NonMoveAssignable() = default;
  NonMoveAssignable(const NonMoveAssignable&) = default;
  NonMoveAssignable(NonMoveAssignable&&) = default;
  NonMoveAssignable& operator=(const NonMoveAssignable&) = default;
  NonMoveAssignable& operator=(NonMoveAssignable&&) = delete;
};

class NonMovable {
 public:
  NonMovable() = default;
  NonMovable(const NonMovable&) = default;
  NonMovable(NonMovable&&) = delete;
  NonMovable& operator=(const NonMovable&) = default;
  NonMovable& operator=(NonMovable&&) = delete;
};

}  // namespace rst

#endif  // RST_NONCOPYABLE_NONCOPYABLE_H_
