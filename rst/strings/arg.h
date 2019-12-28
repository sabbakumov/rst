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

#ifndef RST_STRINGS_ARG_H_
#define RST_STRINGS_ARG_H_

#include <cstddef>
#include <string_view>
#include <type_traits>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"

namespace rst {
namespace internal {

template <class Int, size_t N>
std::string_view IntToString(char (&str)[N], Int val);

template <class Float, size_t N>
std::string_view FloatToString(char (&str)[N], NotNull<const char*> format,
                               Float val);

class Arg {
 public:
  static constexpr size_t kBufferSize = 21;

  explicit Arg(const bool value) : view_(value ? "true" : "false") {}

  explicit Arg(const char value) : view_(buffer_, 1) { buffer_[0] = value; }

  explicit Arg(const short value)  // NOLINT(runtime/int)
      : view_(IntToString(buffer_, value)) {}

  explicit Arg(const unsigned short value)  // NOLINT(runtime/int)
      : view_(IntToString(buffer_, value)) {}

  explicit Arg(const int value) : view_(IntToString(buffer_, value)) {}

  explicit Arg(const unsigned int value) : view_(IntToString(buffer_, value)) {}

  explicit Arg(const long value)  // NOLINT(runtime/int)
      : view_(IntToString(buffer_, value)) {}

  explicit Arg(const unsigned long value)  // NOLINT(runtime/int)
      : view_(IntToString(buffer_, value)) {}

  explicit Arg(const long long value)  // NOLINT(runtime/int)
      : view_(IntToString(buffer_, value)) {}

  explicit Arg(const unsigned long long value)  // NOLINT(runtime/int)
      : view_(IntToString(buffer_, value)) {}

  explicit Arg(const float value)
      : view_(FloatToString(buffer_, "%g", value)) {}

  explicit Arg(const double value)
      : view_(FloatToString(buffer_, "%lg", value)) {}

  explicit Arg(const long double value)
      : view_(FloatToString(buffer_, "%Lg", value)) {}

  explicit Arg(const std::string_view value) : view_(value) {}

  // Provides const char* overload since otherwise it will be implicitly
  // converted to bool.
  explicit Arg(const char* value) : view_(value) {
    RST_DCHECK(value != nullptr);
  }

  template <class T, class = typename std::enable_if<std::is_enum<T>{}>::type>
  explicit Arg(const T e)
      : Arg(static_cast<typename std::underlying_type<T>::type>(e)) {}

  // Prevents Arg(pointer) from accidentally producing a bool.
  explicit Arg(void*) = delete;

  ~Arg() = default;

  std::string_view view() const { return view_; }

 private:
  const std::string_view view_;
  char buffer_[kBufferSize];  // Can store 2^64 - 1 that is
                              // 18,446,744,073,709,551,615 with '\0'.

  RST_DISALLOW_COPY_AND_ASSIGN(Arg);
};

}  // namespace internal
}  // namespace rst

#endif  // RST_STRINGS_ARG_H_
