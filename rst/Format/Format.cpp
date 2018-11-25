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
namespace {

void WriteShort(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const short*>(ptr);
  writer->Write(val);
}

void WriteUnsignedShort(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const unsigned short*>(ptr);
  writer->Write(val);
}

void WriteInt(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const int*>(ptr);
  writer->Write(val);
}

void WriteUnsignedInt(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const unsigned int*>(ptr);
  writer->Write(val);
}

void WriteLong(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const long*>(ptr);
  writer->Write(val);
}

void WriteUnsignedLong(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const unsigned long*>(ptr);
  writer->Write(val);
}

void WriteLongLong(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const long long*>(ptr);
  writer->Write(val);
}

void WriteUnsignedLongLong(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const unsigned long long*>(ptr);
  writer->Write(val);
}

void WriteFloat(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const float*>(ptr);
  writer->Write(val);
}

void WriteDouble(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const double*>(ptr);
  writer->Write(val);
}

void WriteLongDouble(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const long double*>(ptr);
  writer->Write(val);
}

void WriteStringView(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const std::string_view*>(ptr);
  writer->Write(val);
}

void WriteChar(Writer* writer, const void* ptr) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(ptr != nullptr);

  const auto val = *static_cast<const char*>(ptr);
  writer->Write(val);
}

}  // namespace

Formatter::Formatter(const char* str) : str_(str) {
  RST_DCHECK(str_ != nullptr);
}

Formatter::~Formatter() = default;

bool HandleCharacter(const char c, const char** s) {
  RST_DCHECK(s != nullptr);
  const char*& s_ref = *s;
  RST_DCHECK(s_ref != nullptr);
  RST_DCHECK(c == *s_ref);

  switch (c) {
    case '{': {
      const auto s_1 = *(s_ref + 1);
      switch (s_1) {
        case '{': {
          s_ref++;
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
      const auto s_1 = *(s_ref + 1);
      switch (s_1) {
        case '}': {
          s_ref++;
          break;
        }
        default: { RST_DCHECK(false && "Unmatched '}' in format string"); }
      }
      break;
    }
  }
  return true;
}

void Format(Writer* writer, const char* s) {
  RST_DCHECK(writer != nullptr);
  RST_DCHECK(s != nullptr);

  for (auto c = '\0'; (c = *s) != '\0'; s++) {
    const auto result = HandleCharacter(c, &s);
    RST_DCHECK(result && "Argument index out of range");
    writer->Write(c);
  }
}

Value::Value(const short short_val)
    : short_val_(short_val), type_(Type::kShort) {}

Value::Value(const unsigned short unsigned_short_val)
    : unsigned_short_val_(unsigned_short_val), type_(Type::kUnsignedShort) {}

Value::Value(const int int_val) : int_val_(int_val), type_(Type::kInt) {}

Value::Value(const unsigned int unsigned_int_val)
    : unsigned_int_val_(unsigned_int_val), type_(Type::kUnsignedInt) {}

Value::Value(const long long_val) : long_val_(long_val), type_(Type::kLong) {}

Value::Value(const unsigned long unsigned_long_val)
    : unsigned_long_val_(unsigned_long_val), type_(Type::kUnsignedLong) {}

Value::Value(const long long long_long_val)
    : long_long_val_(long_long_val), type_(Type::kLongLong) {}

Value::Value(const unsigned long long unsigned_long_long_val)
    : unsigned_long_long_val_(unsigned_long_long_val),
      type_(Type::kUnsignedLongLong) {}

Value::Value(const float float_val)
    : float_val_(float_val), type_(Type::kFloat) {}

Value::Value(const double double_val)
    : double_val_(double_val), type_(Type::kDouble) {}

Value::Value(const long double long_double_val)
    : long_double_val_(long_double_val), type_(Type::kLongDouble) {}

Value::Value(const std::string_view string_view_val)
    : string_view_val_(string_view_val), type_(Type::kStringView) {}

Value::Value(const char char_val) : char_val_(char_val), type_(Type::kChar) {}

Value::~Value() = default;

void Value::Write(Writer* writer) const {
  RST_DCHECK(writer != nullptr);

  using WriteFunction = void(Writer * writer, const void* ptr);
  // clang-format off
  static constexpr WriteFunction* kFuncs[] = {
      &WriteInt,
      &WriteDouble,
      &WriteStringView,
      &WriteChar,
      &WriteShort,
      &WriteFloat,
      &WriteUnsignedShort,
      &WriteUnsignedInt,
      &WriteLong,
      &WriteUnsignedLong,
      &WriteLongLong,
      &WriteUnsignedLongLong,
      &WriteLongDouble,
  };
  // clang-format on

  const auto index = static_cast<size_t>(type_);
  RST_DCHECK(index < std::size(kFuncs));

  const auto func = kFuncs[index];
  func(writer, &short_val_);
}

}  // namespace internal

}  // namespace rst
