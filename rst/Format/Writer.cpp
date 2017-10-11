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

#include "rst/Format/Writer.h"

#include <cstring>
#include <limits>

using std::array;
using std::numeric_limits;
using std::string;

namespace rst {

namespace internal {

Writer::Writer() { RST_DCHECK(static_buffer_.size() >= 2); }

void Writer::Write(short val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%hd", val);
}

void Writer::Write(unsigned short val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%hu", val);
}

void Writer::Write(int val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%d", val);
}

void Writer::Write(unsigned int val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%u", val);
}

void Writer::Write(long val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%ld", val);
}

void Writer::Write(unsigned long val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%lu", val);
}

void Writer::Write(long long val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%lld", val);
}

void Writer::Write(unsigned long long val) {
  array<char, 4 * sizeof val> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%llu", val);
}

void Writer::Write(float val) {
  array<char, numeric_limits<decltype(val)>::max_exponent10 + 20> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%f", val);
}

void Writer::Write(double val) {
  array<char, numeric_limits<decltype(val)>::max_exponent10 + 20> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%f", val);
}

void Writer::Write(long double val) {
  array<char, numeric_limits<decltype(val)>::max_exponent10 + 20> buffer;
  FormatAndWrite(buffer.data(), buffer.size(), "%Lf", val);
}

void Writer::Write(const string& val) { Write(val.c_str(), val.size()); }

void Writer::Write(const char* val) {
  RST_DCHECK(val != nullptr);

  Write(val, std::strlen(val));
}

void Writer::Write(char val) { Write(&val, 1); }

void Writer::Write(const char* val, size_t len) {
  RST_DCHECK(val != nullptr);
  if (len == 0)
    return;

  if (is_static_buffer_) {
    if (len < static_buffer_.size() - size_) {
      std::copy(val, val + len, static_buffer_.data() + size_);
      size_ += len;
      static_buffer_[size_] = '\0';
    } else {
      dynamic_buffer_.reserve(size_ + len + 1);
      dynamic_buffer_.assign(static_buffer_.data(), size_);
      dynamic_buffer_.append(val, len);
      is_static_buffer_ = false;
    }
  } else {
    dynamic_buffer_.append(val, len);
  }
}

string Writer::TakeString() {
  RST_DCHECK(!moved_ && "String has been already moved");

  if (is_static_buffer_) {
    string val(static_buffer_.data(), size_);
    set_moved();
    return val;
  }

  set_moved();
  return std::move(dynamic_buffer_);
}

string Writer::CopyString() const {
  if (is_static_buffer_)
    return string(static_buffer_.data(), size_);
  return dynamic_buffer_;
}

}  // namespace internal

}  // namespace rst
