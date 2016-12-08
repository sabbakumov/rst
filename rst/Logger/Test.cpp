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

#include <array>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "FileNameSink.h"
#include "FilePtrSink.h"
#include "ISink.h"
#include "LogError.h"
#include "Logger.h"

#include "rst/Cpp14/Memory.h"

using std::FILE;
using std::array;
using std::ifstream;
using std::runtime_error;
using std::string;
using std::thread;
using std::unique_ptr;
using std::vector;

using namespace rst;
using namespace testing;

class File {
 public:
  File() {
    if (std::tmpnam(buffer_.data()) == nullptr) {
      throw runtime_error("Can not generate filename");
    }
  }
  ~File() { std::remove(buffer_.data()); }

  string FileName() const { return string(buffer_.data()); }

 private:
  File(const File&) = delete;
  File& operator=(const File&) = delete;

  File(File&&) = delete;
  File& operator=(File&&) = delete;

  array<char, L_tmpnam> buffer_;
};

class ISinkMock : public ISink {
 public:
  MOCK_METHOD5(Log,
               void(const char* filename, int line, const char* severity_level,
                    const char* format, va_list args));
};

TEST(Logger, ConstructorNullSink) { EXPECT_THROW(Logger(nullptr), LogError); }

TEST(Logger, LogNullFilename) {
  auto sink = make_unique<ISinkMock>();
  Logger logger(std::move(sink));
  EXPECT_THROW(logger.Log(Logger::Level::kDebug, nullptr, 0, "format"),
               LogError);
}

TEST(Logger, LogNullFormat) {
  auto sink = make_unique<ISinkMock>();
  Logger logger(std::move(sink));
  EXPECT_THROW(logger.Log(Logger::Level::kDebug, "filename", 0, nullptr),
               LogError);
}

TEST(Logger, Log) {
  auto sink = make_unique<ISinkMock>();
  const char* filename = "filename";
  const char* level_str = "DEBUG";
  const auto line = 10;
  const char* format = "format";

  EXPECT_CALL(*sink, Log(StrEq(filename), Eq(line), StrEq(level_str),
                         StrEq(format), _));

  Logger logger(std::move(sink));
  logger.Log(Logger::Level::kDebug, filename, line, format);
}

TEST(Logger, LogSeverityLevelComparison) {
  auto sink = make_unique<ISinkMock>();
  const char* filename = "filename";
  const auto line = 10;
  const char* format = "format";

  EXPECT_CALL(*sink, Log(_, _, _, _, _)).Times(0);

  Logger logger(std::move(sink));

  logger.set_level(Logger::Level::kInfo);
  logger.Log(Logger::Level::kDebug, filename, line, format);

  logger.set_level(Logger::Level::kWarning);
  logger.Log(Logger::Level::kInfo, filename, line, format);

  logger.set_level(Logger::Level::kError);
  logger.Log(Logger::Level::kWarning, filename, line, format);

  logger.set_level(Logger::Level::kFatal);
  logger.Log(Logger::Level::kError, filename, line, format);

  logger.set_level(Logger::Level::kOff);
  logger.Log(Logger::Level::kFatal, filename, line, format);
}

TEST(Logger, LogSeverityLevelComparisonPass) {
  auto sink = make_unique<ISinkMock>();
  const char* filename = "filename";
  const auto line = 10;
  const char* format = "format";

  EXPECT_CALL(*sink, Log(StrEq(filename), Eq(line), _, StrEq(format), _))
      .Times(9);

  Logger logger(std::move(sink));

  logger.set_level(Logger::Level::kAll);
  logger.Log(Logger::Level::kDebug, filename, line, format);

  logger.set_level(Logger::Level::kDebug);
  logger.Log(Logger::Level::kInfo, filename, line, format);

  logger.set_level(Logger::Level::kDebug);
  logger.Log(Logger::Level::kDebug, filename, line, format);

  logger.set_level(Logger::Level::kInfo);
  logger.Log(Logger::Level::kWarning, filename, line, format);

  logger.set_level(Logger::Level::kInfo);
  logger.Log(Logger::Level::kInfo, filename, line, format);

  logger.set_level(Logger::Level::kWarning);
  logger.Log(Logger::Level::kError, filename, line, format);

  logger.set_level(Logger::Level::kWarning);
  logger.Log(Logger::Level::kWarning, filename, line, format);

  logger.set_level(Logger::Level::kError);
  logger.Log(Logger::Level::kFatal, filename, line, format);

  logger.set_level(Logger::Level::kError);
  logger.Log(Logger::Level::kError, filename, line, format);
}

TEST(Logger, LogEnumToString) {
  auto sink = make_unique<ISinkMock>();
  const char* filename = "filename";
  const auto line = 10;
  const char* format = "format";

  EXPECT_CALL(*sink,
              Log(StrEq(filename), Eq(line), StrEq("DEBUG"), StrEq(format), _));
  EXPECT_CALL(*sink,
              Log(StrEq(filename), Eq(line), StrEq("INFO"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(StrEq(filename), Eq(line), StrEq("WARNING"),
                         StrEq(format), _));
  EXPECT_CALL(*sink,
              Log(StrEq(filename), Eq(line), StrEq("ERROR"), StrEq(format), _));
  EXPECT_CALL(*sink,
              Log(StrEq(filename), Eq(line), StrEq("FATAL"), StrEq(format), _));

  Logger logger(std::move(sink));

  logger.Log(Logger::Level::kDebug, filename, line, format);
  logger.Log(Logger::Level::kInfo, filename, line, format);
  logger.Log(Logger::Level::kWarning, filename, line, format);
  logger.Log(Logger::Level::kError, filename, line, format);
  logger.Log(Logger::Level::kFatal, filename, line, format);
}

TEST(Logger, LogEnumToStringIncorrectCases) {
  auto sink = make_unique<ISinkMock>();
  const char* filename = "filename";
  const auto line = 10;
  const char* format = "format";

  Logger logger(std::move(sink));

  EXPECT_THROW(logger.Log(Logger::Level::kAll, filename, line, format),
               LogError);
  EXPECT_THROW(logger.Log(Logger::Level::kOff, filename, line, format),
               LogError);
}

TEST(Logger, LogThrow) {
  auto sink = make_unique<ISinkMock>();
  const char* filename = "filename";
  const char* level_str = "DEBUG";
  const auto line = 10;
  const char* format = "format";

  EXPECT_CALL(
      *sink, Log(StrEq(filename), Eq(line), StrEq(level_str), StrEq(format), _))
      .WillOnce(Throw(runtime_error("")));

  Logger logger(std::move(sink));
  EXPECT_THROW(logger.Log(Logger::Level::kDebug, filename, line, format),
               runtime_error);
}

TEST(Logger, Macros) {
  auto sink = make_unique<ISinkMock>();
  const char* format = "%s";
  const char* message = "message";

  EXPECT_CALL(*sink, Log(_, _, StrEq("DEBUG"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(_, _, StrEq("INFO"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(_, _, StrEq("WARNING"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(_, _, StrEq("ERROR"), StrEq(format), _));
  EXPECT_CALL(*sink, Log(_, _, StrEq("FATAL"), StrEq(format), _));

  Logger logger(std::move(sink));

  LOG_DEBUG(logger, format, message);
  LOG_INFO(logger, format, message);
  LOG_WARNING(logger, format, message);
  LOG_ERROR(logger, format, message);
  LOG_FATAL(logger, format, message);
}

TEST(FileNameSink, LogNullFilename) {
  File file;
  const string filename = file.FileName();
  const string prologue_format;
  va_list args;

  FileNameSink sink(filename, prologue_format);
  EXPECT_THROW(sink.Log(nullptr, 0, "level", "format", args), LogError);
}

TEST(FileNameSink, LogNullSeverity) {
  File file;
  const string filename = file.FileName();
  const string prologue_format;
  va_list args;

  FileNameSink sink(filename, prologue_format);
  EXPECT_THROW(sink.Log("filename", 0, nullptr, "format", args), LogError);
}

TEST(FileNameSink, LogNullFormat) {
  File file;
  const string filename = file.FileName();
  const string prologue_format;
  va_list args;

  FileNameSink sink(filename, prologue_format);
  EXPECT_THROW(sink.Log("filename", 0, "level", nullptr, args), LogError);
}

void Log(ISink& sink, const char* filename, int line,
         const char* severity_level, const char* format, ...) {
  va_list plain_args;
  unique_ptr<va_list, void (*)(va_list*)> args{
      &plain_args, [](va_list* list) { va_end(*list); }};
  va_start(*args, format);

  sink.Log(filename, line, severity_level, format, *args);
}

TEST(FileNameSink, Log) {
  File file;
  const string filename = file.FileName();
  string prologue_format = "%s:%d %s: ";

  FileNameSink sink(filename, std::move(prologue_format));

  Log(sink, "filename", 10, "level", "%s", "Message");
  Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);

  array<string, 3> messages = {{"filename:10 level: Message",
                                "filename2:20 level2: Message2",
                                "filename3:30 level3: Message3"}};

  ifstream f;
  f.open(filename);

  size_t i = 0;
  for (string line; std::getline(f, line); i++) {
    EXPECT_EQ(messages[i], line);
  }
}

TEST(FileNameSink, LogThreadSafe) {
  File file;
  const string filename = file.FileName();
  string prologue_format = "%s:%d %s: ";

  FileNameSink sink(filename, std::move(prologue_format));

  thread t1([&sink]() {
    std::this_thread::yield();
    Log(sink, "filename", 10, "level", "%s", "Message");
  });
  thread t2([&sink]() {
    Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  });
  thread t3([&sink]() {
    Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);
  });

  t1.join();
  t2.join();
  t3.join();

  vector<string> messages = {"filename:10 level: Message",
                             "filename2:20 level2: Message2",
                             "filename3:30 level3: Message3"};
  sort(messages.begin(), messages.end());

  ifstream f;
  f.open(filename);

  vector<string> strings;
  for (string line; std::getline(f, line);) {
    strings.emplace_back(line);
  }
  sort(strings.begin(), strings.end());

  EXPECT_EQ(messages, strings);
}

TEST(FilePtrSink, ConstructorNullFile) {
  EXPECT_THROW(FilePtrSink(nullptr, ""), LogError);
}

TEST(FilePtrSink, LogNullFilename) {
  FILE* file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_THROW(sink.Log(nullptr, 0, "level", "format", args), LogError);
}

TEST(FilePtrSink, LogNullSeverity) {
  FILE* file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_THROW(sink.Log("filename", 0, nullptr, "format", args), LogError);
}

TEST(FilePtrSink, LogNullFormat) {
  FILE* file = tmpfile();
  const string prologue_format;
  va_list args;

  FilePtrSink sink(file, prologue_format);
  EXPECT_THROW(sink.Log("filename", 0, "level", nullptr, args), LogError);
}

TEST(FilePtrSink, Log) {
  FILE* file = tmpfile();
  string prologue_format = "%s:%d %s: ";

  FilePtrSink sink(file, std::move(prologue_format));

  Log(sink, "filename", 10, "level", "%s", "Message");
  Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);

  array<string, 3> messages = {{"filename:10 level: Message",
                                "filename2:20 level2: Message2",
                                "filename3:30 level3: Message3"}};

  std::rewind(file);

  size_t i = 0;
  string str_line;
  for (array<char, 256> line;
       std::feof(file) == 0 && std::fgets(line.data(), line.size(), file);
       i++) {
    str_line = line.data();
    if (!str_line.empty() && str_line.back() == '\n') {
      str_line.erase(str_line.size() - 1);
    }
    EXPECT_EQ(messages[i], str_line);
  }
}

TEST(FilePtrSink, LogNonClosing) {
  FILE* file = tmpfile();
  string prologue_format = "%s:%d %s: ";

  FilePtrSink sink(file, std::move(prologue_format), false);

  Log(sink, "filename", 10, "level", "%s", "Message");
  Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);

  array<string, 3> messages = {{"filename:10 level: Message",
                                "filename2:20 level2: Message2",
                                "filename3:30 level3: Message3"}};

  std::rewind(file);

  size_t i = 0;
  string str_line;
  for (array<char, 256> line;
       std::feof(file) == 0 && std::fgets(line.data(), line.size(), file);
       i++) {
    str_line = line.data();
    if (!str_line.empty() && str_line.back() == '\n') {
      str_line.erase(str_line.size() - 1);
    }
    EXPECT_EQ(messages[i], str_line);
  }

  std::fclose(file);
}

TEST(FilePtrSink, LogThreadSafe) {
  FILE* file = tmpfile();
  string prologue_format = "%s:%d %s: ";

  FilePtrSink sink(file, std::move(prologue_format));

  thread t1([&sink]() {
    std::this_thread::yield();
    Log(sink, "filename", 10, "level", "%s", "Message");
  });
  thread t2([&sink]() {
    Log(sink, "filename2", 20, "level2", "%s%d", "Message", 2);
  });
  thread t3([&sink]() {
    Log(sink, "filename3", 30, "level3", "%s%d", "Message", 3);
  });

  t1.join();
  t2.join();
  t3.join();

  vector<string> messages = {{"filename:10 level: Message"},
                             {"filename2:20 level2: Message2"},
                             {"filename3:30 level3: Message3"}};
  sort(messages.begin(), messages.end());

  vector<string> strings;
  std::rewind(file);

  for (array<char, 256> line;
       std::feof(file) == 0 && std::fgets(line.data(), line.size(), file);) {
    string str_line = line.data();
    if (!str_line.empty() && str_line.back() == '\n') {
      str_line.erase(str_line.size() - 1);
    }
    strings.emplace_back(std::move(str_line));
  }
  sort(strings.begin(), strings.end());

  EXPECT_EQ(messages, strings);
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
