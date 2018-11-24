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
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Format/Writer.h"
#include "rst/Macros/Macros.h"

namespace rst {
namespace internal {

// Handles character c in the string s. Returns false if there's {} in s.
bool HandleCharacter(char c, const char** s);

// Writes s to the writer. "{{" -> "{", "}}" -> "}".
void Format(Writer* writer, const char* s);

class Value {
 public:
  Value(short short_val);
  Value(unsigned short unsigned_short_val);
  Value(int int_val);
  Value(unsigned int unsigned_int_val);
  Value(long long_val);
  Value(unsigned long unsigned_long_val);
  Value(long long long_long_val);
  Value(unsigned long long unsigned_long_long_val);
  Value(float float_val);
  Value(double double_val);
  Value(long double long_double_val);
  Value(std::string_view string_view_val);
  Value(char char_val);

  ~Value();

  void Write(Writer* writer) const;

 private:
  enum class Type : unsigned int {
    kInt,
    kDouble,
    kStringView,
    kChar,
    kShort,
    kFloat,
    kUnsignedShort,
    kUnsignedInt,
    kLong,
    kUnsignedLong,
    kLongLong,
    kUnsignedLongLong,
    kLongDouble,
  };

  Type type_;

  union {
    short short_val_;
    unsigned short unsigned_short_val_;
    int int_val_;
    unsigned int unsigned_int_val_;
    long long_val_;
    unsigned long unsigned_long_val_;
    long long long_long_val_;
    unsigned long long unsigned_long_long_val_;
    float float_val_;
    double double_val_;
    long double long_double_val_;
    std::string_view string_view_val_;
    char char_val_;
  };

  RST_DISALLOW_IMPLICIT_CONSTRUCTORS(Value);
};

// Writes s to the writer. "{{" -> "{", "}}" -> "}".
template <class... Args>
inline void Format(Writer* writer, const char* s, Args&&... args) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(s != nullptr);

  const Value values[] = {Value(std::forward<Args>(args))...};

  size_t arg_idx = 0;
  for (auto c = '\0'; (c = *s) != '\0'; s++) {
    if (!HandleCharacter(c, &s)) {
      RST_DCHECK(arg_idx < std::size(values) && "Extra arguments");
      values[arg_idx].Write(writer);
      s++;
      arg_idx++;
      continue;
    }
    writer->Write(c);
  }

  RST_DCHECK(arg_idx == std::size(values) &&
             "Numbers of parameters should match");
}

}  // namespace internal

// A wrapper around Format recursive functions.
template <class... Args>
inline std::string format(const char* s, Args&&... args) {
  RST_DCHECK(s != nullptr);
  internal::Writer writer;
  Format(&writer, s, std::forward<Args>(args)...);
  return writer.TakeString();
}

namespace internal {

// Used in user defined literals.
class Formatter {
 public:
  explicit Formatter(const char* str);
  ~Formatter();

  template <class... Args>
  std::string operator()(Args&&... args) const {
    return format(str_, std::forward<Args>(args)...);
  }

 private:
  const char* str_ = nullptr;

  RST_DISALLOW_COPY_AND_ASSIGN(Formatter);
};

}  // namespace internal

namespace literals {

// User defined literals.
inline internal::Formatter operator"" _format(const char* s, size_t) {
  RST_DCHECK(s != nullptr);
  return internal::Formatter(s);
}

}  // namespace literals

}  // namespace rst

#endif  // RST_FORMAT_FORMAT_H_
