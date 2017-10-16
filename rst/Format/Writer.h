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

#ifndef RST_FORMAT_WRITER_H_
#define RST_FORMAT_WRITER_H_

#include <algorithm>
#include <array>
#include <cstdio>
#include <string>
#include <type_traits>

#include "rst/Check/Check.h"

namespace rst {

namespace internal {

// The class for writing values. It has a static and dynamic buffer. By default
// it uses the static buffer.
class Writer {
 public:
  // The size of the stack buffer.
  static constexpr size_t kStaticBufferSize = 1024;

  Writer();

  // Writes val like std::snprintf.
  template <class T>
  void FormatAndWrite(char* str, size_t size, const char* format, T val) {
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type");
    RST_DCHECK(str != nullptr);
    RST_DCHECK(format != nullptr);
    if (size <= 1)
      return;

    const auto bytes_written = std::snprintf(str, size, format, val);
    RST_DCHECK(bytes_written >= 0);

    Write(str, std::min(static_cast<size_t>(bytes_written), size - 1));
  }

  void Write(short val);
  void Write(unsigned short val);
  void Write(int val);
  void Write(unsigned int val);
  void Write(long val);
  void Write(unsigned long val);
  void Write(long long val);
  void Write(unsigned long long val);
  void Write(float val);
  void Write(double val);
  void Write(long double val);
  void Write(const std::string& val);
  void Write(const char* val);
  void Write(char val);

  // Writes len bytes from val to the static buffer by default. When the
  // buffer on the stack gets full or the range is too long for the static
  // buffer, allocates a dynamic buffer, copies existing content to the dynamic
  // buffer and writes current and all next ranges to the dynamic buffer.
  void Write(const char* val, size_t len);

  // Returns a string of either static or dynamic buffer. If it's dynamic buffer
  // performs a move operation, so it's no longer valid to write to the Writer.
  std::string TakeString();

  // Returns a string of either static or dynamic buffer. It's valid to write to
  // the Writer more data.
  std::string CopyString() const;

 private:
#ifndef NDEBUG
  void set_moved() { moved_ = true; }

  // Whether we moved the writer via TakeString().
  bool moved_ = false;
#else   // NDEBUG
  void set_moved() {}
#endif  // NDEBUG

  // Buffer on the stack to prevent dynamic allocation.
  std::array<char, kStaticBufferSize> static_buffer_;

  // Dynamic buffer in case of the static buffer gets full or the input is too
  // long for the static buffer.
  std::string dynamic_buffer_;

  // The current size of the static_buffer_.
  size_t size_ = 0;

  // Whether we use static or dynamic buffer.
  bool is_static_buffer_ = true;
};

}  // namespace internal

}  // namespace rst

#endif  // RST_FORMAT_WRITER_H_
