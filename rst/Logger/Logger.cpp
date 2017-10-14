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

#include <cstdarg>
#include <cstdlib>
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Defer/Defer.h"
#include "rst/Logger/ISink.h"
#include "rst/Logger/LogError.h"

using std::unique_ptr;

namespace rst {

namespace {

Logger* g_logger = nullptr;

}  // namespace

Logger::Logger(unique_ptr<ISink> sink) : sink_(std::move(sink)) {
  RST_DCHECK(sink_ != nullptr);
}

// static
void Logger::Log(Level level, const char* filename, int line,
                 const char* format, ...) {
  RST_DCHECK(g_logger != nullptr);
  RST_DCHECK(filename != nullptr);
  RST_DCHECK(line > 0);
  RST_DCHECK(format != nullptr);

  if (static_cast<int>(level) < static_cast<int>(g_logger->level_))
    return;

  va_list args;
  va_start(args, format);
  RST_DEFER([&args]() -> void { va_end(args); });

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
    default: { RST_DCHECK(false && "Unexpected level"); }
  }
  RST_DCHECK(level_str != nullptr);

  g_logger->sink_->Log(filename, line, level_str, format, args);

  if (level == Level::kFatal) {
    g_logger->sink_.reset();
    std::abort();
  }
}

// static
void Logger::SetLogger(Logger* logger) {
  RST_DCHECK(logger != nullptr);
  g_logger = logger;
}

}  // namespace rst
