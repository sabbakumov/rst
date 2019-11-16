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
//
// Example:
//
//   class NonCopyAssignable {
//    public:
//     NonCopyAssignable() = default;
//
//     void Foo();
//     void Bar();
//
//    private:
//     RST_DISALLOW_COPY_AND_ASSIGN(NonCopyAssignable);
//   };
//
//   class NonConstructible {
//    public:
//     static void Foo();
//     static void Bar();
//
//    private:
//     RST_DISALLOW_IMPLICIT_CONSTRUCTORS(NonConstructible);
//   };

// Put this in the declarations for a class to be uncopyable.
#define RST_DISALLOW_COPY(Class) Class(const Class&) = delete

// Put this in the declarations for a class to be unassignable.
#define RST_DISALLOW_ASSIGN(Class) Class& operator=(const Class&) = delete

// Put this in the declarations for a class to be uncopyable and unassignable.
#define RST_DISALLOW_COPY_AND_ASSIGN(Class) \
  RST_DISALLOW_COPY(Class);                 \
  RST_DISALLOW_ASSIGN(Class)

// A macro to disallow all the implicit constructors, namely the default
// constructor, copy constructor and operator=() functions. This is especially
// useful for classes containing only static methods.
#define RST_DISALLOW_IMPLICIT_CONSTRUCTORS(Class) \
  Class() = delete;                               \
  RST_DISALLOW_COPY_AND_ASSIGN(Class)

// This does a concatenation of two preprocessor args using ## doubly
// indirectly because using ## directly prevents macros in that parameter from
// being expanded.
#define RST_INTERNAL_CAT(x, y) x##y
#define RST_CAT(x, y) RST_INTERNAL_CAT(x, y)

// This macro un-mangles the names of the build flags in a way that looks
// natural, and gives errors if the flag is not defined. Normally in the
// preprocessor it's easy to make mistakes that interpret "you haven't done the
// setup to know what the flag is" as "flag is off".
//
// Example:
//
//   #define RST_BUILDFLAG_ENABLE_FOO() (true)
//
//   #if BUILDFLAG(ENABLE_FOO)
//     ...
//   #endif
#define RST_BUILDFLAG(flag) (RST_CAT(RST_BUILDFLAG_, flag)())

#endif  // RST_MACROS_MACROS_H_
