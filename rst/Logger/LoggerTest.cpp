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
#include "rst/Logger/FileNameSink.h"
#include "rst/Logger/FilePtrSink.h"
#include "rst/Logger/ISink.h"
#include "rst/Logger/LogError.h"

#include <algorithm>
#include <array>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <memory>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "rst/Check/Check.h"
#include "rst/Defer/Defer.h"
#include "rst/Noncopyable/Noncopyable.h"
#include "rst/Status/StatusOr.h"

using std::array;
using std::ifstream;
using std::string;
using std::thread;
using std::unique_ptr;
using std::vector;

using namespace rst;
using namespace testing;

namespace {

class File : public rst::NonCopyable, public rst::NonMovable {
 public:
  File() { RST_CHECK(std::tmpnam(buffer_.data()) != nullptr); }
  ~File() { std::remove(buffer_.data()); }

  const char* FileName() const { return buffer_.data(); }

 private:
  array<char, L_tmpnam> buffer_;
};

class SinkMock : public ISink {
 public:
  MOCK_METHOD5(Log,
               void(const char* filename, int line, const char* severity_level,
                    const char* format, va_list args));
};

void Log(ISink& sink, const char* filename, int line,
         const char* severity_level, const char* format, ...) {
  ASSERT_NE(nullptr, filename);
  ASSERT_GT(line, 0);
  ASSERT_NE(nullptr, severity_level);
  ASSERT_NE(nullptr, format);

  va_list args;
  va_start(args, format);
  RST_DEFER([&args]() -> void { va_end(args); });

  sink.Log(filename, line, severity_level, format, args);
}

}  // namespace

TEST(Logger, ConstructorNullSink) { EXPECT_DEATH(Logger(nullptr), ""); }

TEST(Logger, LogNullLogger) {
  auto sink = std::make_unique<SinkMock>();
  Logger logger(std::move(sink));
  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, "filename", 1, "format"), "");
}

TEST(Logger, LogNullFilename) {
  auto sink = std::make_unique<SinkMock>();
  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);
  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, nullptr, 1, "format"), "");
}

TEST(Logger, LogZeroLine) {
  auto sink = std::make_unique<SinkMock>();
  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);
  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, "filename", 0, "format"), "");
}

TEST(Logger, LogNegativeLine) {
  auto sink = std::make_unique<SinkMock>();
  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);
  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, "filename", -1, "format"),
               "");
}

TEST(Logger, LogNullFormat) {
  auto sink = std::make_unique<SinkMock>();
  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);
  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, "filename", 1, nullptr), "");
}

TEST(Logger, Log) {
  auto sink = std::make_unique<SinkMock>();
  static constexpr auto kFilename = "filename";
  static constexpr auto kLevelStr = "DEBUG";
  static constexpr auto kLine = 10;
  static constexpr auto kFormat = "format";

  EXPECT_CALL(
      *sink, Log(StrEq(kFilename), kLine, StrEq(kLevelStr), StrEq(kFormat), _));

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);
  Logger::Log(Logger::Level::kDebug, kFilename, kLine, kFormat);
}

TEST(Logger, LogSeverityLevelComparison) {
  auto sink = std::make_unique<SinkMock>();
  static constexpr auto kFilename = "filename";
  static constexpr auto kLine = 10;
  static constexpr auto kFormat = "format";

  EXPECT_CALL(*sink, Log(_, _, _, _, _)).Times(0);

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  logger.set_level(Logger::Level::kInfo);
  Logger::Log(Logger::Level::kDebug, kFilename, kLine, kFormat);

  logger.set_level(Logger::Level::kWarning);
  Logger::Log(Logger::Level::kInfo, kFilename, kLine, kFormat);

  logger.set_level(Logger::Level::kError);
  Logger::Log(Logger::Level::kWarning, kFilename, kLine, kFormat);

  logger.set_level(Logger::Level::kFatal);
  Logger::Log(Logger::Level::kError, kFilename, kLine, kFormat);

  logger.set_level(Logger::Level::kOff);
  Logger::Log(Logger::Level::kFatal, kFilename, kLine, kFormat);
}

TEST(Logger, LogSeverityLevelComparisonPass) {
  auto sink = std::make_unique<SinkMock>();
  static constexpr auto filename = "filename";
  static constexpr auto line = 10;
  static constexpr auto format = "format";

  EXPECT_CALL(*sink, Log(StrEq(filename), line, _, StrEq(format), _)).Times(8);

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  logger.set_level(Logger::Level::kAll);
  Logger::Log(Logger::Level::kDebug, filename, line, format);

  logger.set_level(Logger::Level::kDebug);
  Logger::Log(Logger::Level::kInfo, filename, line, format);

  logger.set_level(Logger::Level::kDebug);
  Logger::Log(Logger::Level::kDebug, filename, line, format);

  logger.set_level(Logger::Level::kInfo);
  Logger::Log(Logger::Level::kWarning, filename, line, format);

  logger.set_level(Logger::Level::kInfo);
  Logger::Log(Logger::Level::kInfo, filename, line, format);

  logger.set_level(Logger::Level::kWarning);
  Logger::Log(Logger::Level::kError, filename, line, format);

  logger.set_level(Logger::Level::kWarning);
  Logger::Log(Logger::Level::kWarning, filename, line, format);

  logger.set_level(Logger::Level::kError);
  EXPECT_DEATH(Logger::Log(Logger::Level::kFatal, filename, line, format), "");

  logger.set_level(Logger::Level::kError);
  Logger::Log(Logger::Level::kError, filename, line, format);
}

TEST(Logger, LogEnumToString) {
  auto sink = std::make_unique<SinkMock>();
  static constexpr auto filename = "filename";
  static constexpr auto line = 10;
  static constexpr auto format = "format";

  EXPECT_CALL(*sink,
              Log(StrEq(filename), line, StrEq("DEBUG"), StrEq(format), _));
  EXPECT_CALL(*sink,
              Log(StrEq(filename), line, StrEq("INFO"), StrEq(format), _));
  EXPECT_CALL(*sink,
              Log(StrEq(filename), line, StrEq("WARNING"), StrEq(format), _));
  EXPECT_CALL(*sink,
              Log(StrEq(filename), line, StrEq("ERROR"), StrEq(format), _));

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  Logger::Log(Logger::Level::kDebug, filename, line, format);
  Logger::Log(Logger::Level::kInfo, filename, line, format);
  Logger::Log(Logger::Level::kWarning, filename, line, format);
  Logger::Log(Logger::Level::kError, filename, line, format);
  EXPECT_DEATH(Logger::Log(Logger::Level::kFatal, filename, line, format), "");
}

TEST(Logger, LogEnumToStringIncorrectCases) {
  auto sink = std::make_unique<SinkMock>();
  static constexpr auto kFilename = "filename";
  static constexpr auto kLine = 10;
  static constexpr auto kFormat = "format";

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  EXPECT_DEATH(Logger::Log(Logger::Level::kAll, kFilename, kLine, kFormat), "");
  EXPECT_DEATH(Logger::Log(Logger::Level::kOff, kFilename, kLine, kFormat), "");
}

TEST(Logger, Macros) {
  auto sink = std::make_unique<SinkMock>();
  static constexpr auto format = "%s";
  static constexpr auto message = "message";

  EXPECT_CALL(*sink, Log(_, _, StrEq("DEBUG"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(_, _, StrEq("INFO"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(_, _, StrEq("WARNING"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(_, _, StrEq("ERROR"), StrEq(format), _));

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  LOG_DEBUG(format, message);
  LOG_INFO(format, message);
  LOG_WARNING(format, message);
  LOG_ERROR(format, message);
  EXPECT_DEATH(LOG_FATAL(format, message), "");
}

TEST(Logger, SetNullLogger) { EXPECT_DEATH(Logger::SetLogger(nullptr), ""); }

TEST(FileNameSink, LogNullFilename) {
  File file;
  const auto filename = file.FileName();
  const string prologue_format;
  va_list args;

  auto sink = FileNameSink::Create(filename, prologue_format);
  ASSERT_TRUE(sink.ok());
  EXPECT_DEATH(sink->Log(nullptr, 1, "level", "format", args), "");
}

TEST(FileNameSink, LogZeroLine) {
  File file;
  const auto filename = file.FileName();
  const string prologue_format;
  va_list args;

  auto sink = FileNameSink::Create(filename, prologue_format);
  ASSERT_TRUE(sink.ok());
  EXPECT_DEATH(sink->Log("filename", 0, "level", "format", args), "");
}

TEST(FileNameSink, LogNegativeLine) {
  File file;
  const auto filename = file.FileName();
  const string prologue_format;
  va_list args;

  auto sink = FileNameSink::Create(filename, prologue_format);
  ASSERT_TRUE(sink.ok());
  EXPECT_DEATH(sink->Log("filename", -1, "level", "format", args), "");
}

TEST(FileNameSink, LogNullSeverity) {
  File file;
  const auto filename = file.FileName();
  const string prologue_format;
  va_list args;

  auto sink = FileNameSink::Create(filename, prologue_format);
  ASSERT_TRUE(sink.ok());
  EXPECT_DEATH(sink->Log("filename", 1, nullptr, "format", args), "");
}

TEST(FileNameSink, LogNullFormat) {
  File file;
  const auto filename = file.FileName();
  const string prologue_format;
  va_list args;

  auto sink = FileNameSink::Create(filename, prologue_format);
  ASSERT_TRUE(sink.ok());
  EXPECT_DEATH(sink->Log("filename", 1, "level", nullptr, args), "");
}

TEST(FileNameSink, Log) {
  File file;
  const auto filename = file.FileName();
  static constexpr auto kPrologueFormat = "%s:%d %s: ";

  auto sink = FileNameSink::Create(filename, kPrologueFormat);
  ASSERT_TRUE(sink.ok());

  Log(*sink, "filename", 10, "level", "%s", "Message");
  Log(*sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  Log(*sink, "filename3", 30, "level3", "%s%d", "Message", 3);

  const vector<string> messages = {"filename:10 level: Message",
                                   "filename2:20 level2: Message2",
                                   "filename3:30 level3: Message3"};

  ifstream f(filename);
  ASSERT_TRUE(f.is_open());

  vector<string> strings;
  for (string line; std::getline(f, line);)
    strings.emplace_back(std::move(line));

  EXPECT_EQ(messages, strings);
}

TEST(FileNameSink, LogThreadSafe) {
  File file;
  const auto filename = file.FileName();
  static constexpr auto kPrologueFormat = "%s:%d %s: ";

  auto sink = FileNameSink::Create(filename, kPrologueFormat);
  ASSERT_TRUE(sink.ok());

  thread t1([&sink]() -> void {
    std::this_thread::yield();
    Log(*sink, "filename", 10, "level", "%s", "Message");
  });
  thread t2([&sink]() -> void {
    Log(*sink, "filename3", 30, "level3", "%s%d", "Message", 3);
  });
  thread t3([&sink]() -> void {
    Log(*sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  });

  t1.join();
  t2.join();
  t3.join();

  vector<string> messages = {"filename:10 level: Message",
                             "filename2:20 level2: Message2",
                             "filename3:30 level3: Message3"};
  std::sort(messages.begin(), messages.end());

  ifstream f(filename);
  ASSERT_TRUE(f.is_open());

  vector<string> strings;
  for (string line; std::getline(f, line);)
    strings.emplace_back(std::move(line));
  std::sort(strings.begin(), strings.end());

  EXPECT_EQ(messages, strings);
}

TEST(FilePtrSink, ConstructorNullFile) {
  EXPECT_DEATH(FilePtrSink(nullptr, ""), "");
}

TEST(FilePtrSink, LogNullFilename) {
  const auto file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_DEATH(sink.Log(nullptr, 1, "level", "format", args), "");
}

TEST(FilePtrSink, LogZeroLine) {
  const auto file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_DEATH(sink.Log("filename", 0, "level", "format", args), "");
}

TEST(FilePtrSink, LogNegativeLine) {
  const auto file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_DEATH(sink.Log("filename", -1, "level", "format", args), "");
}

TEST(FilePtrSink, LogNullSeverity) {
  const auto file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_DEATH(sink.Log("filename", 1, nullptr, "format", args), "");
}

TEST(FilePtrSink, LogNullFormat) {
  const auto file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_DEATH(sink.Log("filename", 1, "level", nullptr, args), "");
}

TEST(FilePtrSink, Log) {
  const auto file = tmpfile();
  static constexpr auto kPrologueFormat = "%s:%d %s: ";

  FilePtrSink sink(file, kPrologueFormat);

  Log(sink, "filename", 10, "level", "%s", "Message");
  Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);

  const array<string, 3> messages = {{"filename:10 level: Message",
                                      "filename2:20 level2: Message2",
                                      "filename3:30 level3: Message3"}};

  std::rewind(file);

  size_t i = 0;
  string str_line;
  for (array<char, 256> line;
       std::feof(file) == 0 && std::fgets(line.data(), line.size(), file);
       i++) {
    str_line = line.data();
    if (!str_line.empty() && str_line.back() == '\n')
      str_line.erase(str_line.size() - 1);
    EXPECT_EQ(messages[i], str_line);
  }
}

TEST(FilePtrSink, LogNonClosing) {
  const auto file = tmpfile();
  static constexpr auto kPrologueFormat = "%s:%d %s: ";

  FilePtrSink sink(file, kPrologueFormat, false);

  Log(sink, "filename", 10, "level", "%s", "Message");
  Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);

  const array<string, 3> messages = {{"filename:10 level: Message",
                                      "filename2:20 level2: Message2",
                                      "filename3:30 level3: Message3"}};

  std::rewind(file);

  size_t i = 0;
  string str_line;
  for (array<char, 256> line;
       std::feof(file) == 0 && std::fgets(line.data(), line.size(), file);
       i++) {
    str_line = line.data();
    if (!str_line.empty() && str_line.back() == '\n')
      str_line.erase(str_line.size() - 1);
    EXPECT_EQ(messages[i], str_line);
  }
}

TEST(FilePtrSink, LogThreadSafe) {
  const auto file = tmpfile();
  static constexpr auto kPrologueFormat = "%s:%d %s: ";

  FilePtrSink sink(file, kPrologueFormat);

  thread t1([&sink]() -> void {
    std::this_thread::yield();
    Log(sink, "filename", 10, "level", "%s", "Message");
  });
  thread t2([&sink]() -> void {
    Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  });
  thread t3([&sink]() -> void {
    Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);
  });

  t1.join();
  t2.join();
  t3.join();

  vector<string> messages = {{"filename:10 level: Message"},
                             {"filename2:20 level2: Message2"},
                             {"filename3:30 level3: Message3"}};
  std::sort(messages.begin(), messages.end());

  vector<string> strings;
  std::rewind(file);

  for (array<char, 256> line;
       std::feof(file) == 0 && std::fgets(line.data(), line.size(), file);) {
    string str_line = line.data();
    if (!str_line.empty() && str_line.back() == '\n')
      str_line.erase(str_line.size() - 1);
    strings.emplace_back(std::move(str_line));
  }
  std::sort(strings.begin(), strings.end());

  EXPECT_EQ(messages, strings);
}
