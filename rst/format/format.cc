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

#include "rst/format/format.h"

#include <cstring>

#include "rst/check/check.h"

namespace rst {
namespace internal {
namespace {

template <class T, size_t N>
NotNull<char*> FormatAndWrite(char (&str)[N], const NotNull<const char*> format,
                              const T val) {
  static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");
  const auto bytes_written =
      std::sprintf(str, format.get(), val);  // NOLINT(runtime/printf)
  RST_DCHECK(bytes_written > 0);
  RST_DCHECK(static_cast<size_t>(bytes_written) < N);
  RST_DCHECK(str[bytes_written] == '\0');
  return str + bytes_written;
}

}  // namespace

std::string FormatAndReturnString(const NotNull<const char*> not_null_format,
                                  const size_t format_size,
                                  const Nullable<const Arg*> values,
                                  const size_t size) {
  auto format = not_null_format.get();

  std::string output;
  RST_DCHECK(format_size == std::strlen(format));
  auto capacity = format_size;
  for (size_t i = 0; i < size; i++) {
    RST_DCHECK(values != nullptr);
    capacity += values[i].view().size();
  }
  RST_DCHECK(capacity >= size * 2);
  capacity -= size * 2;
  output.reserve(capacity);
#if RST_BUILDFLAG(DCHECK_IS_ON)
  const auto old_capacity = output.capacity();
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  size_t arg_idx = 0;
  for (auto c = '\0'; (c = *format) != '\0'; format++) {
    switch (c) {
      case '{': {
        switch (*(format + 1)) {
          case '{': {
            format++;
            break;
          }
          case '}': {
            RST_DCHECK(arg_idx < size && "Extra arguments");
            output.append(values[arg_idx].view());
            format++;
            arg_idx++;
            continue;
          }
          default: { RST_DCHECK(false && "Invalid format string"); }
        }
        break;
      }
      case '}': {
        switch (*(format + 1)) {
          case '}': {
            format++;
            break;
          }
          default: { RST_DCHECK(false && "Unmatched '}' in format string"); }
        }
        break;
      }
    }

    output.push_back(c);
  }

  RST_DCHECK(arg_idx == size && "Numbers of parameters should match");

#if RST_BUILDFLAG(DCHECK_IS_ON)
  RST_DCHECK(output.capacity() == old_capacity);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  return output;
}

Arg::Arg(const bool value) : view_(value ? "true" : "false") {}

Arg::Arg(const char value) : view_(buffer_, 1) { buffer_[0] = value; }

Arg::Arg(const short value)  // NOLINT(runtime/int)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%hd", value).get() -
                                buffer_)) {}

Arg::Arg(const unsigned short value)  // NOLINT(runtime/int)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%hu", value).get() -
                                buffer_)) {}

Arg::Arg(const int value)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%d", value).get() -
                                buffer_)) {}

Arg::Arg(const unsigned int value)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%u", value).get() -
                                buffer_)) {}

Arg::Arg(const long value)  // NOLINT(runtime/int)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%ld", value).get() -
                                buffer_)) {}

Arg::Arg(const unsigned long value)  // NOLINT(runtime/int)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%lu", value).get() -
                                buffer_)) {}

Arg::Arg(const long long value)  // NOLINT(runtime/int)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%lld", value).get() -
                                buffer_)) {}

Arg::Arg(const unsigned long long value)  // NOLINT(runtime/int)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%llu", value).get() -
                                buffer_)) {}

Arg::Arg(const float value)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%g", value).get() -
                                buffer_)) {}

Arg::Arg(const double value)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%lg", value).get() -
                                buffer_)) {}

Arg::Arg(const long double value)
    : view_(buffer_,
            static_cast<size_t>(FormatAndWrite(buffer_, "%Lg", value).get() -
                                buffer_)) {}

Arg::Arg(const std::string_view value) : view_(value) {}

Arg::Arg(const char* value) : view_(value) { RST_DCHECK(value != nullptr); }

Arg::~Arg() = default;

}  // namespace internal

}  // namespace rst
