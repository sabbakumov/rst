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

#ifndef RST_STATUS_STATUS_MACROS_H_
#define RST_STATUS_STATUS_MACROS_H_

#include <utility>

#include "rst/macros/macros.h"
#include "rst/macros/optimization.h"
#include "rst/status/status.h"
#include "rst/status/status_or.h"

namespace rst {

// Evaluates |statement| to a Status object and if it results in an error,
// returns that error. Uses |status| as a variable name to avoid shadowing.
#define RST_INTERNAL_TRY(status, statement)                    \
  do {                                                         \
    if (auto status = (statement); RST_UNLIKELY(status.err())) \
      return status;                                           \
  } while (false)

// Macro to allow exception-like handling of `rst::Status` return values.
//
// If the evaluation of statement results in an error, returns that error from
// the current function.
//
// Example:
//
//   #include "rst/status/status_macros.h"
//
//   rst::Status Foo();
//
//   rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
//     RST_TRY(Foo());
//     ...
//   }
//
#define RST_TRY(statement) \
  RST_INTERNAL_TRY(RST_CAT(RST_INTERNAL_TRY_STATUS_NAME, __LINE__), statement)

// Macro to allow exception-like handling of `rst::StatusOr` return values.
//
// Assigns the result of evaluation of statement to lvalue and if it results in
// an error, returns that error from the current function.
//
// lvalue should be an existing non-const variable accessible in the current
// scope.
//
// `RST_TRY_ASSIGN()` expands into multiple statements; it cannot be used in a
// single statement (e.g. as the body of an if statement without {})!
//
// Example:
//
//   #include "rst/status/status_macros.h"
//
//   rst::StatusOr<MyType> Foo();
//
//   rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
//     rst::StatusOr<MyType> existing_var = ...;
//     RST_TRY_ASSIGN(existing_var, Foo());
//     ...
//   }
//
#define RST_TRY_ASSIGN(lhs, statement) \
  lhs = (statement);                   \
  if (RST_UNLIKELY(lhs.err()))         \
  return std::move(lhs).TakeStatus()

// Evaluates |statement| to a StatusOr object and if it results in an error,
// returns that error. Otherwise assings the result to |lhs|. Uses |status_or|
// as a variable name to avoid shadowing.
#define RST_INTERNAL_TRY_ASSIGN_UNWRAP(lhs, status_or, statement)    \
  do {                                                               \
    if (auto status_or = (statement); RST_UNLIKELY(status_or.err())) \
      return std::move(status_or).TakeStatus();                      \
    else                                                             \
      lhs = std::move(*status_or);                                   \
  } while (false)

// Macro to allow exception-like handling of `rst::StatusOr` return values.
//
// Assigns the unwrapped result of evaluation of statement to lvalue and if it
// results in an error, returns that error from the current function.
//
// lvalue should be an existing non-const variable accessible in the current
// scope.
//
// Example:
//
//   #include "rst/status/status_macros.h"
//
//   rst::StatusOr<MyType> Foo();
//
//   rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
//     MyType existing_var = ...;
//     RST_TRY_ASSIGN_UNWRAP(existing_var, Foo());
//     ...
//   }
//
#define RST_TRY_ASSIGN_UNWRAP(lhs, statement)                                \
  RST_INTERNAL_TRY_ASSIGN_UNWRAP(                                            \
      lhs, RST_CAT(RST_INTERNAL_TRY_ASSIGN_UNWRAP_STATUS_OR_NAME, __LINE__), \
      statement)

// Macro to allow exception-like handling of `rst::StatusOr` return values.
//
// Creates lvalue and assigns the result of evaluation of statement to it and
// if it results in an error, returns that error from the current function.
//
// lvalue should be a new variable.
//
// `RST_TRY_CREATE()` expands into multiple statements; it cannot be used in a
// single statement (e.g. as the body of an if statement without {})!
//
// Example:
//
//   #include "rst/status/status_macros.h"
//
//   rst::StatusOr<MyType> Foo();
//
//   rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
//     RST_TRY_CREATE(rst::StatusOr<MyType>, var1, Foo());
//     RST_TRY_CREATE(auto, var2, Foo());
//     ...
//   }
//
#define RST_TRY_CREATE(type, lhs, statement) \
  type lhs = (statement);                    \
  if (RST_UNLIKELY(lhs.err()))               \
  return std::move(lhs).TakeStatus()

}  // namespace rst

#endif  // RST_STATUS_STATUS_MACROS_H_
