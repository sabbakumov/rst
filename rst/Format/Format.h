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

#ifndef RST_FORMAT_FORMAT_H_
#define RST_FORMAT_FORMAT_H_

#include <iterator>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "rst/Macros/Macros.h"
#include "rst/NotNull/NotNull.h"

namespace rst {
namespace internal {

class Arg {
 public:
  explicit Arg(bool value);
  explicit Arg(char value);
  explicit Arg(short value);
  explicit Arg(unsigned short value);
  explicit Arg(int value);
  explicit Arg(unsigned int value);
  explicit Arg(long value);
  explicit Arg(unsigned long value);
  explicit Arg(long long value);
  explicit Arg(unsigned long long value);
  explicit Arg(float value);
  explicit Arg(double value);
  explicit Arg(long double value);
  explicit Arg(std::string_view value);
  explicit Arg(const char* value);

  template <class T, class = typename std::enable_if<std::is_enum<T>{}>::type>
  explicit Arg(const T e)
      : Arg(static_cast<typename std::underlying_type<T>::type>(e)) {}

  Arg(void*) = delete;

  ~Arg();

  std::string_view view() const { return view_; }
  size_t size() const { return view_.size(); }

 private:
  char buffer_[21];
  const std::string_view view_;

  RST_DISALLOW_COPY_AND_ASSIGN(Arg);
};

std::string FormatAndReturnString(const char* format,
                                  Nullable<const Arg*> values, size_t size);

}  // namespace internal

std::string Format(NotNull<const char*> format);

template <class... Args>
inline std::string Format(const NotNull<const char*> format, Args&&... args) {
  const internal::Arg values[] = {internal::Arg(std::forward<Args>(args))...};
  return internal::FormatAndReturnString(format.get(), values,
                                         std::size(values));
}

}  // namespace rst

#endif  // RST_FORMAT_FORMAT_H_
