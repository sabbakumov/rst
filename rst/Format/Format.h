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

#include <string>
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Format/FormatError.h"
#include "rst/Format/Writer.h"

namespace rst {

namespace internal {

// Handles character c in the string s. Returns false if there's {} in s.
bool HandleCharacter(char c, const char*& s);

// Writes s to the writer. "{{" -> "{", "}}" -> "}".
void Format(Writer& writer, const char* s);

// Writes s to the writer. "{{" -> "{", "}}" -> "}".
template <class T, class... Args>
inline void Format(Writer& writer, const char* s, const T& value,
                   Args&&... args) {
  RST_CHECK(s != nullptr, FormatError);

  auto c = *s;
  if (c == '\0')
    throw FormatError("Extra arguments");
  for (; (c = *s) != '\0'; s++) {
    if (!HandleCharacter(c, s)) {
      writer.Write(value);
      s += 2;
      Format(writer, s, std::forward<Args>(args)...);
      return;
    }
    writer.Write(c);
  }
}

}  // namespace internal

// A wrapper around Format recursive functions.
template <class... Args>
inline std::string format(const char* s, Args&&... args) {
  RST_CHECK(s != nullptr, FormatError);
  internal::Writer writer;
  Format(writer, s, std::forward<Args>(args)...);
  return writer.MoveString();
}

namespace internal {

// Used in user defined literals.
class Formatter {
 public:
  explicit Formatter(const char* str) : str_(str) {
    RST_CHECK(str_ != nullptr, FormatError);
  }

  template <class... Args>
  std::string operator()(Args&&... args) const {
    return format(str_, std::forward<Args>(args)...);
  }

 private:
  const char* str_ = nullptr;
};

}  // namespace internal

namespace literals {

// User defined literals.
inline internal::Formatter operator"" _format(const char* s, size_t) {
  RST_CHECK(s != nullptr, FormatError);
  return internal::Formatter(s);
}

}  // namespace literals

}  // namespace rst

#endif  // RST_FORMAT_FORMAT_H_
