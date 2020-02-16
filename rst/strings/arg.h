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
#include <cstdio>
#include <string>
#include <string_view>
#include <type_traits>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"

namespace rst {
namespace internal {

template <class Float, size_t N>
std::string_view FloatToString(char (&str)[N],
                               const NotNull<const char*> format,
                               const Float val) {
  static_assert(std::is_floating_point<Float>::value);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wformat-nonliteral"
#pragma warning(push)
#pragma warning(disable : 4774)
  const auto bytes_written =
      std::sprintf(str, format.get(), val);  // NOLINT(runtime/printf)
#pragma warning(pop)
#pragma clang diagnostic pop
  RST_DCHECK(bytes_written > 0);
  RST_DCHECK(static_cast<size_t>(bytes_written) < N);
  RST_DCHECK(str[bytes_written] == '\0');
  return std::string_view(str, static_cast<size_t>(bytes_written));
}

template <class Int, size_t N>
std::string_view IntToString(char (&str)[N], const Int val) {
  static_assert(std::is_integral<Int>::value);

  auto res = static_cast<typename std::make_unsigned<Int>::type>(val);

  auto p = str + N;
  do {
    --p;
    RST_DCHECK(p != str);
    *p = static_cast<char>((res % 10) + '0');
    res /= 10;
  } while (res != 0);

  if (val < 0) {
    --p;
    RST_DCHECK(p != str);
    *p = '-';
  }
  return std::string_view(p, static_cast<size_t>(str + N - p));
}

class Arg {
 public:
  static constexpr size_t kBufferSize = 21;

  Arg(const bool value)  // NOLINT(runtime/explicit)
      : view_(value ? "true" : "false") {}

  Arg(const char value)  // NOLINT(runtime/explicit)
      : view_(buffer_, 1) {
    buffer_[0] = value;
  }

  Arg(const short value) : view_(IntToString(buffer_, value)) {}  // NOLINT(*)

  Arg(const unsigned short value)  // NOLINT(*)
      : view_(IntToString(buffer_, value)) {}

  Arg(const int value)  // NOLINT(runtime/explicit)
      : view_(IntToString(buffer_, value)) {}

  Arg(const unsigned int value)  // NOLINT(runtime/explicit)
      : view_(IntToString(buffer_, value)) {}

  Arg(const long value) : view_(IntToString(buffer_, value)) {}  // NOLINT(*)

  Arg(const unsigned long value)  // NOLINT(*)
      : view_(IntToString(buffer_, value)) {}

  Arg(const long long value)  // NOLINT(*)
      : view_(IntToString(buffer_, value)) {}

  Arg(const unsigned long long value)  // NOLINT(*)
      : view_(IntToString(buffer_, value)) {}

  Arg(const float value)  // NOLINT(runtime/explicit)
      : Arg(static_cast<double>(value)) {}

  Arg(const double value)  // NOLINT(runtime/explicit)
      : view_(FloatToString(buffer_, "%g", value)) {}

  Arg(const long double value)  // NOLINT(runtime/explicit)
      : view_(FloatToString(buffer_, "%Lg", value)) {}

  Arg(const std::string_view value)  // NOLINT(runtime/explicit)
      : view_(value) {}
  Arg(const std::string& value) : view_(value) {}  // NOLINT(runtime/explicit)

  // Provides const char* overload since otherwise it will be implicitly
  // converted to bool.
  Arg(const char* value)  // NOLINT(runtime/explicit)
      : view_(value) {
    RST_DCHECK(value != nullptr);
  }
  Arg(const NotNull<const char*> value)  // NOLINT(runtime/explicit)
      : view_(value.get()) {}

  template <class T, class = typename std::enable_if<std::is_enum<T>{}>::type>
  Arg(const T e)  // NOLINT(runtime/explicit)
      : Arg(static_cast<typename std::underlying_type<T>::type>(e)) {}

  // Prevents Arg(pointer) from accidentally producing a bool.
  Arg(void*) = delete;  // NOLINT(runtime/explicit)

  ~Arg() = default;

  std::string_view view() const { return view_; }
  size_t size() const { return view_.size(); }

 private:
  const std::string_view view_;
  char buffer_[kBufferSize];  // Can store 2^64 - 1 that is
                              // 18,446,744,073,709,551,615 with '\0'.

  RST_DISALLOW_COPY_AND_ASSIGN(Arg);
};

extern template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                             short val);  // NOLINT(runtime/int)
extern template std::string_view IntToString(
    char (&str)[Arg::kBufferSize],
    unsigned short val);  // NOLINT(runtime/int)
extern template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                             int val);
extern template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                             unsigned int val);
extern template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                             long val);  // NOLINT(runtime/int)
extern template std::string_view IntToString(
    char (&str)[Arg::kBufferSize],
    unsigned long val);  // NOLINT(runtime/int)
extern template std::string_view IntToString(
    char (&str)[Arg::kBufferSize],
    long long val);  // NOLINT(runtime/int)
extern template std::string_view IntToString(
    char (&str)[Arg::kBufferSize],
    unsigned long long val);  // NOLINT(runtime/int)

}  // namespace internal
}  // namespace rst

#endif  // RST_STRINGS_ARG_H_
