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

namespace rst {

Logger::Logger() {}

void Logger::Log(const Level level, const char* file, const int line,
                 const char* /*function*/, const char* format, ...) {
  assert(was_opened_);
  if (static_cast<int>(level) < static_cast<int>(min_level_)) return;

  va_list args;
  va_start(args, format);
  
  mutex_.Lock();
  
  fprintf(log_file_, "%s:%d: ", file, line);
  switch (level) {
    case Level::kDebug: {
      fprintf(log_file_, "DEBUG: ");
      break;
    }
    case Level::kInfo: {
      fprintf(log_file_, "INFO: ");
      break;
    }
    case Level::kWarning: {
      fprintf(log_file_, "WARN: ");
      break;
    }
    case Level::kError: {
      fprintf(log_file_, "ERROR: ");
      break;
    }
    case Level::kFatal: {
      fprintf(log_file_, "FATAL: ");
      break;
    }
    default: {
      assert(false);
      break;
    }
  }
  vfprintf(log_file_, format, args);
  fflush(log_file_);
  
  mutex_.Unlock();
  
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

Logger::~Logger() {
  Close();
}

}  // namespace rst
