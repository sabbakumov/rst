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

#include "rst/Format/Writer.h"
#include "rst/Macros/Macros.h"
#include "rst/NotNull/NotNull.h"

namespace rst {
namespace internal {

class Value {
 public:
  explicit Value(short short_val);
  explicit Value(unsigned short unsigned_short_val);
  explicit Value(int int_val);
  explicit Value(unsigned int unsigned_int_val);
  explicit Value(long long_val);
  explicit Value(unsigned long unsigned_long_val);
  explicit Value(long long long_long_val);
  explicit Value(unsigned long long unsigned_long_long_val);
  explicit Value(float float_val);
  explicit Value(double double_val);
  explicit Value(long double long_double_val);
  explicit Value(std::string_view string_view_val);
  explicit Value(char char_val);

  ~Value();

  void Write(NotNull<Writer*> writer) const;

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

  union {
    const short short_val_;
    const unsigned short unsigned_short_val_;
    const int int_val_;
    const unsigned int unsigned_int_val_;
    const long long_val_;
    const unsigned long unsigned_long_val_;
    const long long long_long_val_;
    const unsigned long long unsigned_long_long_val_;
    const float float_val_;
    const double double_val_;
    const long double long_double_val_;
    const std::string_view string_view_val_;
    const char char_val_;
  };
  const Type type_;

  RST_DISALLOW_IMPLICIT_CONSTRUCTORS(Value);
};

// Handles character c in the string s. Returns false if there's {} in s.
bool HandleCharacter(char c, NotNull<const char**> s);

// Writes s to the writer.
void Format(NotNull<Writer*> writer, const char* s);
// Writes s to the writer. "{{" -> "{", "}}" -> "}".
void Format(NotNull<Writer*> writer, const char* s,
            NotNull<const Value*> values, size_t size);

}  // namespace internal

// A wrapper around Format functions.
std::string Format(NotNull<const char*> s);

template <class... Args>
inline std::string Format(const NotNull<const char*> s, Args&&... args) {
  internal::Writer writer;
  const internal::Value values[] = {
      internal::Value(std::forward<Args>(args))...};
  internal::Format(&writer, s.get(), values, std::size(values));
  return writer.TakeString();
}

namespace internal {

// Used in user defined literals.
class Formatter {
 public:
  explicit Formatter(NotNull<const char*> str);
  ~Formatter();

  template <class... Args>
  std::string operator()(Args&&... args) const {
    return Format(str_, std::forward<Args>(args)...);
  }

 private:
  const NotNull<const char*> str_;

  RST_DISALLOW_COPY_AND_ASSIGN(Formatter);
};

}  // namespace internal

namespace literals {

// User defined literals.
inline internal::Formatter operator"" _format(const char* s, size_t) {
  return internal::Formatter(s);
}

}  // namespace literals
}  // namespace rst

#endif  // RST_FORMAT_FORMAT_H_
