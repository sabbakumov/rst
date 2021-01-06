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

#ifndef RST_LOGGER_LOGGER_H_
#define RST_LOGGER_LOGGER_H_

#include <cstdint>
#include <memory>
#include <string_view>
#include <utility>

#include "rst/check/check.h"
#include "rst/logger/sink.h"
#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"

// General logger component. Note that fatal logs exit the program.
//
// Example:
//
//   // Construct logger with a custom sink.
//   std::unique_ptr<Sink> sink = ...;
//   Logger logger(std::move(sink));
//
//   // To get logger macros working.
//   Logger::SetGlobalLogger(&logger);
//
//   RST_LOG_INFO("Init subsystem A");
//   // DLOG versions log only in a debug build.
//   RST_DLOG_WARNING("Init subsystem A.B");

// Helper macros for logging with the specified level.
#define RST_LOG_DEBUG(message) \
  ::rst::Logger::Log(::rst::Logger::Level::kDebug, __FILE__, __LINE__, message)

#define RST_LOG_INFO(message) \
  ::rst::Logger::Log(::rst::Logger::Level::kInfo, __FILE__, __LINE__, message)

#define RST_LOG_WARNING(message)                                         \
  ::rst::Logger::Log(::rst::Logger::Level::kWarning, __FILE__, __LINE__, \
                     message)

#define RST_LOG_ERROR(message) \
  ::rst::Logger::Log(::rst::Logger::Level::kError, __FILE__, __LINE__, message)

#define RST_LOG_FATAL(message) \
  ::rst::Logger::Log(::rst::Logger::Level::kFatal, __FILE__, __LINE__, message)

// Like RST_LOG_* macros but compiles to nothing in release build.
#if RST_BUILDFLAG(DCHECK_IS_ON)
#define RST_DLOG_DEBUG(message) RST_LOG_DEBUG(message)
#define RST_DLOG_INFO(message) RST_LOG_INFO(message)
#define RST_DLOG_WARNING(message) RST_LOG_WARNING(message)
#define RST_DLOG_ERROR(message) RST_LOG_ERROR(message)
#define RST_DLOG_FATAL(message) RST_LOG_FATAL(message)
#else  // !RST_BUILDFLAG(DCHECK_IS_ON)
#define RST_DLOG_DEBUG(message)
#define RST_DLOG_INFO(message)
#define RST_DLOG_WARNING(message)
#define RST_DLOG_ERROR(message)
#define RST_DLOG_FATAL(message)
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

namespace rst {

// The class for logging to a custom sink.
class Logger {
 public:
  // Severity levels of logging.
  enum class Level : int8_t {
    kAll = 0,
    kDebug,
    kInfo,
    kWarning,
    kError,
    kFatal,
    kOff,
  };

  explicit Logger(NotNull<std::unique_ptr<Sink>> sink)
      : sink_(std::move(sink)) {}
  ~Logger() = default;

  // Logs a |message|. If the |level| is less than |level_| nothing gets logged.
  static void Log(Level level, NotNull<const char*> filename, int line,
                  std::string_view message);

  // Sets |logger| as a global logger instance.
  static void SetGlobalLogger(NotNull<Logger*> logger);

  void set_level(const Level level) { level_ = level; }

 private:
  const NotNull<std::unique_ptr<Sink>> sink_;
  // Current severity level.
  Level level_ = Level::kAll;

  RST_DISALLOW_COPY_AND_ASSIGN(Logger);
};

}  // namespace rst

#endif  // RST_LOGGER_LOGGER_H_
