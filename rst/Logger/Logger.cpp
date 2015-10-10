// Copyright (c) 2015, Sergey Abbakumov
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

using std::mutex;
using std::unique_lock;

namespace rst {

Logger::Logger() {}

void Logger::Log(Level level, const char* file, int line,
                 const char* /*function*/, const char* format, ...) {
  assert(was_opened_);
  if (static_cast<int>(level) < static_cast<int>(min_level_)) return;

  va_list args;
  va_start(args, format);

  const char* level_str = nullptr;
  switch (level) {
    case Level::kDebug: {
      level_str = "DEBUG: ";
      break;
    }
    case Level::kInfo: {
      level_str = "INFO: ";
      break;
    }
    case Level::kWarning: {
      level_str = "WARN: ";
      break;
    }
    case Level::kError: {
      level_str = "ERROR: ";
      break;
    }
    case Level::kFatal: {
      level_str = "FATAL: ";
      break;
    }
    default: {
      assert(false);
      break;
    }
  }

  unique_lock<mutex> lock(mutex_);

  fprintf(log_file_, "%s:%d: %s: ", file, line, level_str);
  vfprintf(log_file_, format, args);
  fflush(log_file_);

  lock.unlock();

  va_end(args);
}

Logger& Logger::Instance() {
  static Logger logger;

  return logger;
}

bool Logger::OpenFile(const char* filename) {
  if (was_opened_) return false;

  log_file_ = fopen(filename, "w");

  if (!log_file_) return false;

  was_opened_ = true;
  return true;
}

bool Logger::OpenFilePtr(FILE* file) {
  if (was_opened_) return false;
  if (!file) return false;

  log_file_ = file;

  was_opened_ = true;
  return true;
}

void Logger::Close() {
  if (log_file_) fclose(log_file_);
}

Logger::~Logger() { Close(); }

}  // namespace rst
