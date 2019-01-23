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

#include <cstdlib>
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Format/Format.h"
#include "rst/Logger/LogError.h"

namespace rst {
namespace {

Logger* g_logger = nullptr;

}  // namespace

Logger::Logger(NotNull<std::unique_ptr<ISink>> sink) : sink_(std::move(sink)) {}

Logger::~Logger() = default;

// static
void Logger::Log(const Level level, const NotNull<const char*> filename,
                 const int line, const std::string_view message) {
  RST_DCHECK(g_logger != nullptr);
  RST_DCHECK(line > 0);

  if (static_cast<int>(level) < static_cast<int>(g_logger->level_))
    return;

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

  g_logger->sink_->Log(
      Format("[{}:{}({})] {}", level_str, filename.get(), line, message));

  if (level == Level::kFatal)
    std::abort();
}

// static
void Logger::SetLogger(const NotNull<Logger*> logger) {
  g_logger = logger.get();
}

}  // namespace rst
