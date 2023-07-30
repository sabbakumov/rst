// Copyright (c) 2020, Sergey Abbakumov
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

#ifndef RST_MACROS_THREAD_ANNOTATIONS_H_
#define RST_MACROS_THREAD_ANNOTATIONS_H_

// This header file contains macro definitions for thread safety annotations
// that allow developers to document the locking policies of multi-threaded
// code. The annotations can also help program analysis tools to identify
// potential thread safety issues.

#if defined(__clang__)
#define RST_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(x) __attribute__((x))
#else  // !defined(__clang__)
#define RST_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(x)
#endif  // defined(__clang__)

// Documents if a shared field or global variable needs to be protected by a
// mutex. Allows the user to specify a particular mutex that should be held
// when accessing the annotated variable.
//
// Example:
//
//   #include "rst/macros/thread_annotations.h"
//
//   std::mutex mtx;
//   int i RST_GUARDED_BY(mtx);
//
#define RST_GUARDED_BY(mtx) \
  RST_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(guarded_by(mtx))

// Documents if the memory location pointed to by a pointer should be guarded
// by a mutex when dereferencing the pointer.
//
// Example:
//
//   #include "rst/macros/thread_annotations.h"
//
//   std::mutex mtx;
//   int* p RST_PT_GUARDED_BY(mtx);
//   std::unique_ptr<int> p2 RST_PT_GUARDED_BY(mtx);
//
// Note that a pointer variable to a shared memory location could itself be a
// shared variable.
//
// Example:
//
//   #include "rst/macros/thread_annotations.h"
//
//   // |q|, guarded by |mtx1|, points to a shared memory location that is
//   // guarded by |mtx2|:
//   int* q RST_GUARDED_BY(mtx1) RST_PT_GUARDED_BY(mtx2);
//
#define RST_PT_GUARDED_BY(mtx) \
  RST_INTERNAL_THREAD_ANNOTATION_ATTRIBUTE(pt_guarded_by(mtx))

#endif  // RST_MACROS_THREAD_ANNOTATIONS_H_
