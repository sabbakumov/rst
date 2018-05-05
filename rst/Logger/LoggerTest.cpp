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
#include "rst/Noncopyable/Noncopyable.h"

using std::array;
using std::ifstream;
using std::string;
using std::thread;
using std::unique_ptr;
using std::vector;

using namespace testing;

namespace rst {

namespace {

constexpr auto kFilename = "filename";
constexpr auto kLevelStr = "DEBUG";
constexpr auto kLine = 10;
constexpr auto kLineStr = "10";
constexpr auto kMessage = "message";

class File : public rst::NonCopyable {
 public:
  File() { RST_CHECK(std::tmpnam(buffer_.data()) != nullptr); }
  ~File() { std::remove(buffer_.data()); }

  const char* FileName() const { return buffer_.data(); }

 private:
  array<char, L_tmpnam> buffer_;
};

class SinkMock : public ISink {
 public:
  MOCK_METHOD1(Log, void(const string& message));
};

}  // namespace

TEST(Logger, ConstructorNullSink) { EXPECT_DEATH(Logger(nullptr), ""); }

TEST(Logger, Log) {
  auto sink = std::make_unique<SinkMock>();

  EXPECT_CALL(*sink, Log(string("[") + kLevelStr + ":" + kFilename + "(" +
                         kLineStr + ")] " + kMessage));

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);
  Logger::Log(Logger::Level::kDebug, kFilename, kLine, kMessage);
}

TEST(Logger, LogSeverityLevelComparison) {
  auto sink = std::make_unique<SinkMock>();

  EXPECT_CALL(*sink, Log(_)).Times(0);

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  logger.set_level(Logger::Level::kInfo);
  Logger::Log(Logger::Level::kDebug, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kWarning);
  Logger::Log(Logger::Level::kInfo, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kError);
  Logger::Log(Logger::Level::kWarning, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kFatal);
  Logger::Log(Logger::Level::kError, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kOff);
  Logger::Log(Logger::Level::kFatal, kFilename, kLine, kMessage);
}

TEST(Logger, LogSeverityLevelComparisonPass) {
  auto sink = std::make_unique<SinkMock>();

  EXPECT_CALL(*sink, Log(_)).Times(8);

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  logger.set_level(Logger::Level::kAll);
  Logger::Log(Logger::Level::kDebug, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kDebug);
  Logger::Log(Logger::Level::kInfo, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kDebug);
  Logger::Log(Logger::Level::kDebug, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kInfo);
  Logger::Log(Logger::Level::kWarning, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kInfo);
  Logger::Log(Logger::Level::kInfo, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kWarning);
  Logger::Log(Logger::Level::kError, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kWarning);
  Logger::Log(Logger::Level::kWarning, kFilename, kLine, kMessage);

  logger.set_level(Logger::Level::kError);
  EXPECT_DEATH(Logger::Log(Logger::Level::kFatal, kFilename, kLine, kMessage),
               "");

  logger.set_level(Logger::Level::kError);
  Logger::Log(Logger::Level::kError, kFilename, kLine, kMessage);
}

TEST(Logger, LogEnumToString) {
  auto sink = std::make_unique<SinkMock>();

  InSequence seq;

  EXPECT_CALL(*sink, Log(string("[") + "DEBUG" + ":" + kFilename + "(" +
                         kLineStr + ")] " + kMessage));
  EXPECT_CALL(*sink, Log(string("[") + "INFO" + ":" + kFilename + "(" +
                         kLineStr + ")] " + kMessage));
  EXPECT_CALL(*sink, Log(string("[") + "WARNING" + ":" + kFilename + "(" +
                         kLineStr + ")] " + kMessage));
  EXPECT_CALL(*sink, Log(string("[") + "ERROR" + ":" + kFilename + "(" +
                         kLineStr + ")] " + kMessage));

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  Logger::Log(Logger::Level::kDebug, kFilename, kLine, kMessage);
  Logger::Log(Logger::Level::kInfo, kFilename, kLine, kMessage);
  Logger::Log(Logger::Level::kWarning, kFilename, kLine, kMessage);
  Logger::Log(Logger::Level::kError, kFilename, kLine, kMessage);
  EXPECT_DEATH(Logger::Log(Logger::Level::kFatal, kFilename, kLine, kMessage),
               "");
}

TEST(Logger, LogEnumToStringIncorrectCases) {
  auto sink = std::make_unique<SinkMock>();

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  EXPECT_DEATH(Logger::Log(Logger::Level::kAll, kFilename, kLine, kMessage),
               "");
  EXPECT_DEATH(Logger::Log(Logger::Level::kOff, kFilename, kLine, kMessage),
               "");
}

TEST(Logger, Macros) {
  auto sink = std::make_unique<SinkMock>();

  EXPECT_CALL(*sink, Log(_)).Times(4);

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  LOG_DEBUG(kMessage);
  LOG_INFO(kMessage);
  LOG_WARNING(kMessage);
  LOG_ERROR(kMessage);
  EXPECT_DEATH(LOG_FATAL(kMessage), "");
}

TEST(Logger, DebugMacros) {
  auto sink = std::make_unique<SinkMock>();

  EXPECT_CALL(*sink, Log(_)).Times(4);

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  DLOG_DEBUG(kMessage);
  DLOG_INFO(kMessage);
  DLOG_WARNING(kMessage);
  DLOG_ERROR(kMessage);
  EXPECT_DEATH(DLOG_FATAL(kMessage), "");
}

TEST(Logger, SetNullLogger) { EXPECT_DEATH(Logger::SetLogger(nullptr), ""); }

TEST(Logger, NullFilename) {
  auto sink = std::make_unique<SinkMock>();

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, nullptr, kLine, kMessage),
               "");
}

TEST(Logger, ZeroLine) {
  auto sink = std::make_unique<SinkMock>();

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, kFilename, 0, kMessage), "");
}

TEST(Logger, NegativeLine) {
  auto sink = std::make_unique<SinkMock>();

  Logger logger(std::move(sink));
  Logger::SetLogger(&logger);

  EXPECT_DEATH(Logger::Log(Logger::Level::kDebug, kFilename, -1, kMessage), "");
}

TEST(FileNameSink, Log) {
  File file;
  const auto filename = file.FileName();

  auto sink = FileNameSink::Create(filename);
  ASSERT_TRUE(sink.ok());

  (*sink)->Log("Message1");
  (*sink)->Log("Message2");
  (*sink)->Log("Message3");

  const vector<string> messages = {"Message1", "Message2", "Message3"};

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

  auto sink = FileNameSink::Create(filename);
  ASSERT_TRUE(sink.ok());

  thread t1([&sink]() -> void {
    std::this_thread::yield();
    (*sink)->Log("Message1");
  });
  thread t2([&sink]() -> void { (*sink)->Log("Message2"); });
  thread t3([&sink]() -> void { (*sink)->Log("Message3"); });

  t1.join();
  t2.join();
  t3.join();

  vector<string> messages = {"Message1", "Message2", "Message3"};
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

TEST(FilePtrSink, Log) {
  const auto file = tmpfile();

  FilePtrSink sink(file);

  sink.Log("Message1");
  sink.Log("Message2");
  sink.Log("Message3");

  const array<string, 3> messages = {{"Message1", "Message2", "Message3"}};

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

  FilePtrSink sink(file, false);

  sink.Log("Message1");
  sink.Log("Message2");
  sink.Log("Message3");

  const array<string, 3> messages = {{"Message1", "Message2", "Message3"}};

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

  FilePtrSink sink(file);

  thread t1([&sink]() -> void {
    std::this_thread::yield();
    sink.Log("Message1");
  });
  thread t2([&sink]() -> void { sink.Log("Message2"); });
  thread t3([&sink]() -> void { sink.Log("Message3"); });

  t1.join();
  t2.join();
  t3.join();

  vector<string> messages = {{"Message1"}, {"Message2"}, {"Message3"}};
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

}  // namespace rst
