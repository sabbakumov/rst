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

#ifndef RST_DEFER_DEFER_H_
#define RST_DEFER_DEFER_H_

#include <utility>

#include "rst/macros/macros.h"

// Executes a function object on scope exit.
//
// Example:
//
//   #include "rst/defer/defer.h"
//
//   void Foo() {
//     std::FILE* f = std::fopen(...);
//     RST_DEFER([f]() { std::fclose(f); });
//   }
//
#define RST_DEFER(f)                                          \
  const auto RST_CAT(RST_INTERNAL_DEFER_VAR_NAME, __LINE__) = \
      ::rst::internal::Defer(f)

namespace rst {
namespace internal {

template <class F>
class DeferredAction {
 public:
  explicit DeferredAction(F&& action) : action_(std::forward<F>(action)) {}

  DeferredAction(DeferredAction&&) noexcept = default;
  DeferredAction& operator=(DeferredAction&&) noexcept = delete;

  ~DeferredAction() { action_(); }

 private:
  const F action_;

  RST_DISALLOW_COPY_AND_ASSIGN(DeferredAction);
};

template <class F>
DeferredAction<F> Defer(F&& f) {
  return DeferredAction<F>(std::forward<F>(f));
}

}  // namespace internal
}  // namespace rst

#endif  // RST_DEFER_DEFER_H_
