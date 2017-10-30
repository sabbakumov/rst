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

using std::string;

namespace rst {

namespace internal {

namespace {

using WriteFunction = void(Writer& writer, const void* ptr);

void WriteShort(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const short*>(ptr);
  writer.Write(val);
}

void WriteUnsignedShort(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const unsigned short*>(ptr);
  writer.Write(val);
}

void WriteInt(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const int*>(ptr);
  writer.Write(val);
}

void WriteUnsignedInt(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const unsigned int*>(ptr);
  writer.Write(val);
}

void WriteLong(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const long*>(ptr);
  writer.Write(val);
}

void WriteUnsignedLong(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const unsigned long*>(ptr);
  writer.Write(val);
}

void WriteLongLong(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const long long*>(ptr);
  writer.Write(val);
}

void WriteUnsignedLongLong(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const unsigned long long*>(ptr);
  writer.Write(val);
}

void WriteFloat(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const float*>(ptr);
  writer.Write(val);
}

void WriteDouble(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const double*>(ptr);
  writer.Write(val);
}

void WriteLongDouble(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const long double*>(ptr);
  writer.Write(val);
}

void WriteString(Writer& writer, const void* ptr) {
  const auto& val = **static_cast<const string* const*>(ptr);
  writer.Write(val);
}

void WriteCharPtr(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const char* const*>(ptr);
  writer.Write(val);
}

void WriteChar(Writer& writer, const void* ptr) {
  const auto val = *static_cast<const char*>(ptr);
  writer.Write(val);
}

}  // namespace

bool HandleCharacter(char c, const char*& s) {
  RST_DCHECK(s != nullptr);
  RST_DCHECK(c == *s);

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
        default: { RST_DCHECK(false && "Invalid format string"); }
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
        default: { RST_DCHECK(false && "Unmatched '}' in format string"); }
      }
      break;
    }
  }
  return true;
}

void Format(Writer& writer, const char* s) {
  RST_DCHECK(s != nullptr);

  for (auto c = '\0'; (c = *s) != '\0'; s++) {
    const auto result = HandleCharacter(c, s);
    RST_DCHECK(result && "Argument index out of range");
    (void)result;
    writer.Write(c);
  }
}

Value::Value(short short_val) : type_(Type::kShort), short_val_(short_val) {}

Value::Value(unsigned short unsigned_short_val)
    : type_(Type::kUnsignedShort), unsigned_short_val_(unsigned_short_val) {}

Value::Value(int int_val) : type_(Type::kInt), int_val_(int_val) {}

Value::Value(unsigned int unsigned_int_val)
    : type_(Type::kUnsignedInt), unsigned_int_val_(unsigned_int_val) {}

Value::Value(long long_val) : type_(Type::kLong), long_val_(long_val) {}

Value::Value(unsigned long unsigned_long_val)
    : type_(Type::kUnsignedLong), unsigned_long_val_(unsigned_long_val) {}

Value::Value(long long long_long_val)
    : type_(Type::kLongLong), long_long_val_(long_long_val) {}

Value::Value(unsigned long long unsigned_long_long_val)
    : type_(Type::kUnsignedLongLong),
      unsigned_long_long_val_(unsigned_long_long_val) {}

Value::Value(float float_val) : type_(Type::kFloat), float_val_(float_val) {}

Value::Value(double double_val)
    : type_(Type::kDouble), double_val_(double_val) {}

Value::Value(long double long_double_val)
    : type_(Type::kLongDouble), long_double_val_(long_double_val) {}

Value::Value(const string& string_val)
    : type_(Type::kString), string_val_(&string_val) {}

Value::Value(const char* char_ptr_val)
    : type_(Type::kCharPtr), char_ptr_val_(char_ptr_val) {
  RST_DCHECK(char_ptr_val != nullptr);
}

Value::Value(char char_val) : type_(Type::kChar), char_val_(char_val) {}

void Value::Write(Writer& writer) const {
  static constexpr WriteFunction* funcs[] = {&WriteInt,
                                             &WriteDouble,
                                             &WriteString,
                                             &WriteCharPtr,
                                             &WriteChar,
                                             &WriteShort,
                                             &WriteFloat,
                                             &WriteUnsignedShort,
                                             &WriteUnsignedInt,
                                             &WriteLong,
                                             &WriteUnsignedLong,
                                             &WriteLongLong,
                                             &WriteUnsignedLongLong,
                                             &WriteLongDouble};

  static constexpr auto size = sizeof funcs / sizeof funcs[0];
  const auto index = static_cast<size_t>(type_);
  RST_DCHECK(index < size);

  const auto func = funcs[index];
  func(writer, &short_val_);
}

}  // namespace internal

}  // namespace rst
