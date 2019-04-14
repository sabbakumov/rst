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

#ifndef RST_CHECK_CHECK_H_
#define RST_CHECK_CHECK_H_

#include <cassert>
#include <cstdlib>

#if defined(NDEBUG)
#define RST_BUILDFLAG_DCHECK_IS_ON() (false)

#define RST_DCHECK(condition)                                    \
  do {                                                           \
    false ? static_cast<void>(condition) : static_cast<void>(0); \
  } while (false)
#else
#define RST_BUILDFLAG_DCHECK_IS_ON() (true)

#define RST_DCHECK(condition)                            \
  do {                                                   \
    (condition) ? static_cast<void>(0)                   \
                : [] { assert(false && #condition); }(); \
  } while (false)
#endif

#define RST_NOTREACHED() RST_DCHECK(false)

#define RST_CHECK(condition) \
  do {                       \
    if (!(condition))        \
      std::abort();          \
  } while (false)

#endif  // RST_CHECK_CHECK_H_
