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

#include "rst/Logger/Logger.h"

#include <cassert>
#include <cstdarg>

#include "rst/Logger/LogError.h"

using std::unique_ptr;

namespace rst {

Logger::Logger(unique_ptr<ISink> sink) : sink_(std::move(sink)) {
  if (sink_ == nullptr)
    throw LogError("sink is nullptr");
}

void Logger::Log(Level level, const char* filename, int line,
                 const char* format, ...) {
  if (filename == nullptr)
    throw LogError("filename is nullptr");

  if (format == nullptr)
    throw LogError("format is nullptr");

  if (static_cast<int>(level) < static_cast<int>(level_))
    return;

  va_list plain_args;
  unique_ptr<va_list, void (*)(va_list*)> args{
      &plain_args, [](va_list* list) { va_end(*list); }};
  va_start(*args, format);

  const char* level_str = nullptr;
  switch (level) {
    case Level::kDebug: {
      level_str = "DEBUG";
      break;
    }
    case Level::kInfo: {
      level_str = "INFO";
      break;
    }
    case Level::kWarning: {
      level_str = "WARNING";
      break;
    }
    case Level::kError: {
      level_str = "ERROR";
      break;
    }
    case Level::kFatal: {
      level_str = "FATAL";
      break;
    }
    default: { throw LogError("Unexpected level"); }
  }
  assert(level_str != nullptr);

  sink_->Log(filename, line, level_str, format, *args);
}

}  // namespace rst
