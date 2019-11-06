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

#ifndef RST_MACROS_MACROS_H_
#define RST_MACROS_MACROS_H_

// Google like macros.

#define RST_DISALLOW_COPY(Class) Class(const Class&) = delete

#define RST_DISALLOW_ASSIGN(Class) Class& operator=(const Class&) = delete

#define RST_DISALLOW_COPY_AND_ASSIGN(Class) \
  RST_DISALLOW_COPY(Class);                 \
  RST_DISALLOW_ASSIGN(Class)

#define RST_DISALLOW_IMPLICIT_CONSTRUCTORS(Class) \
  Class() = delete;                               \
  RST_DISALLOW_COPY_AND_ASSIGN(Class)

#define RST_INTERNAL_CAT2(x, y) x##y
#define RST_INTERNAL_CAT(x, y) RST_INTERNAL_CAT2(x, y)

#define RST_BUILDFLAG(flag) (RST_INTERNAL_CAT(RST_BUILDFLAG_, flag)())

#endif  // RST_MACROS_MACROS_H_
