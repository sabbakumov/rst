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

#include <iterator>
#include <limits>

namespace rst {
namespace internal {

Writer::Writer() { RST_DCHECK(std::size(static_buffer_) >= 2); }

Writer::~Writer() = default;

void Writer::Write(const short val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%hd", val);
}

void Writer::Write(const unsigned short val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%hu", val);
}

void Writer::Write(const int val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%d", val);
}

void Writer::Write(const unsigned int val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%u", val);
}

void Writer::Write(const long val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%ld", val);
}

void Writer::Write(const unsigned long val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%lu", val);
}

void Writer::Write(const long long val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%lld", val);
}

void Writer::Write(const unsigned long long val) {
  char buffer[4 * sizeof val];
  FormatAndWrite(buffer, std::size(buffer), "%llu", val);
}

void Writer::Write(const float val) {
  char buffer[std::numeric_limits<decltype(val)>::max_exponent10 + 20];
  FormatAndWrite(buffer, std::size(buffer), "%f", val);
}

void Writer::Write(const double val) {
  char buffer[std::numeric_limits<decltype(val)>::max_exponent10 + 20];
  FormatAndWrite(buffer, std::size(buffer), "%f", val);
}

void Writer::Write(const long double val) {
  char buffer[std::numeric_limits<decltype(val)>::max_exponent10 + 20];
  FormatAndWrite(buffer, std::size(buffer), "%Lf", val);
}

void Writer::Write(const std::string_view val) {
  Write(val.data(), val.size());
}

void Writer::Write(const char val) { Write(&val, 1); }

void Writer::Write(const char* val, const size_t len) {
  RST_DCHECK(val != nullptr);
  if (len == 0)
    return;

  if (is_static_buffer_) {
    if (len < std::size(static_buffer_) - size_) {
      std::copy(val, val + len, static_buffer_ + size_);
      size_ += len;
      static_buffer_[size_] = '\0';
    } else {
      dynamic_buffer_.reserve(size_ + len + 1);
      dynamic_buffer_.assign(static_buffer_, size_);
      dynamic_buffer_.append(val, len);
      is_static_buffer_ = false;
    }
  } else {
    dynamic_buffer_.append(val, len);
  }
}

std::string Writer::TakeString() {
  RST_DCHECK(!moved_ && "String has been already moved");

  if (is_static_buffer_) {
    std::string val(static_buffer_, size_);
    set_moved();
    return val;
  }

  set_moved();
  return std::move(dynamic_buffer_);
}

std::string Writer::CopyString() const {
  if (is_static_buffer_)
    return std::string(static_buffer_, size_);
  return dynamic_buffer_;
}

}  // namespace internal
}  // namespace rst
