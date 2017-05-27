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

#include "rst/Format/Format.h"

namespace rst {

namespace internal {

bool HandleCharacter(char c, const char*& s) {
  RST_CHECK(s != nullptr, FormatError);
  RST_CHECK(c == *s, FormatError);

  switch (c) {
    case '{': {
      const auto s_1 = *(s + 1);
      switch (s_1) {
        case '{': {
          s++;
          break;
        }
        case '}': {
          return false;
        }
        default: { throw FormatError("Invalid format string"); }
      }
      break;
    }
    case '}': {
      const auto s_1 = *(s + 1);
      switch (s_1) {
        case '}': {
          s++;
          break;
        }
        default: { throw FormatError("Unmatched '}' in format string"); }
      }
      break;
    }
  }
  return true;
}

void Format(Writer& writer, const char* s) {
  RST_CHECK(s != nullptr, FormatError);

  for (char c; (c = *s) != '\0'; s++) {
    if (!HandleCharacter(c, s))
      throw FormatError("Argument index out of range");
    writer.Write(c);
  }
}

}  // namespace internal

}  // namespace rst
