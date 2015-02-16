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

#ifndef RST_LOGGER_LOGGER_H_
#define RST_LOGGER_LOGGER_H_

#include <cstdio>
#include <mutex>

#define LOG_DEBUG(...) \
Logger::Instance().Log(Logger::Level::kDebug, __FILE__, __LINE__, \
                       __FUNCTION__, __VA_ARGS__)

#define LOG_INFO(...) \
Logger::Instance().Log(Logger::Level::kInfo, __FILE__, __LINE__, \
                       __FUNCTION__, __VA_ARGS__)

#define LOG_WARN(...) \
Logger::Instance().Log(Logger::Level::kWarning, __FILE__, __LINE__, \
                       __FUNCTION__, __VA_ARGS__)

#define LOG_ERROR(...) \
Logger::Instance().Log(Logger::Level::kError, __FILE__, __LINE__, \
                       __FUNCTION__, __VA_ARGS__)

#define LOG_FATAL(...) \
Logger::Instance().Log(Logger::Level::kFatal, __FILE__, __LINE__, \
                       __FUNCTION__, __VA_ARGS__)

namespace rst {

class Logger {
 public:
  enum class Level {
    kAll = 0,
    kDebug = 1,
    kInfo = 2,
    kWarning = 3,
    kError = 4,
    kFatal = 5,
    kOff = 6,
  };

  static Logger& Instance();
  
  void Log(const Level level, const char* file, const int line,
           const char* function, const char* format, ...);

  bool Open(const char* log_file);
  bool Open(FILE* file);

  void SetMinLevel(const Level min_level) { min_level_ = min_level; }
 
 private:
  Logger() = default;
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  
  ~Logger();
  
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;

  bool was_opened_ = false;
  Level min_level_ = Level::kAll;
  FILE* log_file_ = nullptr;
  std::mutex mutex_;
};

}  // namespace rst

#endif  // RST_LOGGER_LOGGER_H_
