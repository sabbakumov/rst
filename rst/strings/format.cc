// Copyright (c) 2016, Sergey Abbakumov
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

#include "rst/strings/format.h"

#include <algorithm>
#include <cstring>

#include "rst/check/check.h"
#include "rst/stl/resize_uninitialized.h"

namespace rst {
namespace internal {

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

template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                      short val);  // NOLINT(runtime/int)
template std::string_view IntToString(
    char (&str)[Arg::kBufferSize],
    unsigned short val);  // NOLINT(runtime/int)
template std::string_view IntToString(char (&str)[Arg::kBufferSize], int val);
template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                      unsigned int val);
template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                      long val);  // NOLINT(runtime/int)
template std::string_view IntToString(
    char (&str)[Arg::kBufferSize],
    unsigned long val);  // NOLINT(runtime/int)
template std::string_view IntToString(char (&str)[Arg::kBufferSize],
                                      long long val);  // NOLINT(runtime/int)
template std::string_view IntToString(
    char (&str)[Arg::kBufferSize],
    unsigned long long val);  // NOLINT(runtime/int)

template <class Float, size_t N>
std::string_view FloatToString(char (&str)[N],
                               const NotNull<const char*> format,
                               const Float val) {
  static_assert(std::is_floating_point<Float>::value);
  const auto bytes_written =
      std::sprintf(str, format.get(), val);  // NOLINT(runtime/printf)
  RST_DCHECK(bytes_written > 0);
  RST_DCHECK(static_cast<size_t>(bytes_written) < N);
  RST_DCHECK(str[bytes_written] == '\0');
  return std::string_view(str, static_cast<size_t>(bytes_written));
}

template std::string_view FloatToString(char (&str)[Arg::kBufferSize],
                                        NotNull<const char*> format, float val);
template std::string_view FloatToString(char (&str)[Arg::kBufferSize],
                                        NotNull<const char*> format,
                                        double val);
template std::string_view FloatToString(char (&str)[Arg::kBufferSize],
                                        NotNull<const char*> format,
                                        long double val);

std::string FormatAndReturnString(
    const NotNull<const char*> not_null_format, const size_t format_size,
    const Nullable<const std::string_view*> values, const size_t size) {
  auto format = not_null_format.get();

  RST_DCHECK(format_size == std::strlen(format));
  auto new_size = format_size;
  for (size_t i = 0; i < size; i++) {
    RST_DCHECK(values != nullptr);
    new_size += values[i].size();
  }
  RST_DCHECK(new_size >= size * 2);
  new_size -= size * 2;

  std::string output;
  StringResizeUninitialized(NotNull(&output), new_size);

  size_t arg_idx = 0;
  auto target = output.data();
  for (auto c = '\0'; (c = *format) != '\0'; format++) {
    switch (c) {
      case '{': {
        switch (*(format + 1)) {
          case '{': {
            format++;
            *target++ = '{';
            break;
          }
          case '}': {
            RST_DCHECK(arg_idx < size && "Extra arguments");
            const auto src = values[arg_idx];
            target = std::copy_n(src.data(), src.size(), target);
            format++;
            arg_idx++;
            break;
          }
          default: { RST_DCHECK(false && "Invalid format string"); }
        }
        break;
      }
      case '}': {
        switch (*(format + 1)) {
          case '}': {
            format++;
            *target++ = '}';
            break;
          }
          default: { RST_DCHECK(false && "Unmatched '}' in format string"); }
        }
        break;
      }
      default: {
        *target++ = c;
        break;
      }
    }
  }

  RST_DCHECK(arg_idx == size && "Numbers of parameters should match");

  output.resize(static_cast<size_t>(target - output.data()));
  return output;
}

}  // namespace internal
}  // namespace rst
