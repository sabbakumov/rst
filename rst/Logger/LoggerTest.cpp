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

#include "rst/Logger/FileNameSink.h"
#include "rst/Logger/FilePtrSink.h"
#include "rst/Logger/ISink.h"
#include "rst/Logger/LogError.h"
#include "rst/Logger/Logger.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "rst/Check/Check.h"
#include "rst/Macros/Macros.h"
#include "rst/NotNull/NotNull.h"

using testing::_;
using testing::Eq;

namespace rst {
namespace {

constexpr auto kFilename = "filename";
constexpr auto kLevelStr = "DEBUG";
constexpr auto kLine = 10;
constexpr auto kLineStr = "10";
constexpr auto kMessage = "message";

class File {
 public:
  File() { RST_CHECK(std::tmpnam(buffer_) != nullptr); }
  ~File() { std::remove(buffer_); }

  NotNull<const char*> FileName() const { return buffer_; }

 private:
  char buffer_[L_tmpnam];

  RST_DISALLOW_COPY_AND_ASSIGN(File);
};

class SinkMock : public ISink {
 public:
  MOCK_METHOD1(Log, void(std::string_view message));
};

}  // namespace

TEST(Logger, Log) {
  auto sink = std::make_unique<SinkMock>();

  EXPECT_CALL(*sink, Log(Eq(std::string("[") + kLevelStr + ":" + kFilename +
                            "(" + kLineStr + ")] " + kMessage)));

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

  testing::InSequence seq;

  EXPECT_CALL(*sink, Log(Eq(std::string("[") + "DEBUG" + ":" + kFilename + "(" +
                            kLineStr + ")] " + kMessage)));
  EXPECT_CALL(*sink, Log(Eq(std::string("[") + "INFO" + ":" + kFilename + "(" +
                            kLineStr + ")] " + kMessage)));
  EXPECT_CALL(*sink, Log(Eq(std::string("[") + "WARNING" + ":" + kFilename +
                            "(" + kLineStr + ")] " + kMessage)));
  EXPECT_CALL(*sink, Log(Eq(std::string("[") + "ERROR" + ":" + kFilename + "(" +
                            kLineStr + ")] " + kMessage)));

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
  ASSERT_FALSE(sink.err());

  (*sink)->Log("Message1");
  (*sink)->Log("Message2");
  (*sink)->Log("Message3");

  const std::vector<std::string> messages = {"Message1", "Message2",
                                             "Message3"};

  std::ifstream f(filename.get());
  ASSERT_TRUE(f.is_open());

  std::vector<std::string> strings;
  for (std::string line; std::getline(f, line);)
    strings.emplace_back(std::move(line));

  EXPECT_EQ(strings, messages);
}

TEST(FileNameSink, LogThreadSafe) {
  File file;
  const auto filename = file.FileName();

  auto sink = FileNameSink::Create(filename);
  ASSERT_FALSE(sink.err());

  std::thread t1([&sink]() {
    std::this_thread::yield();
    (*sink)->Log("Message1");
  });
  std::thread t2([&sink]() { (*sink)->Log("Message2"); });
  std::thread t3([&sink]() { (*sink)->Log("Message3"); });

  t1.join();
  t2.join();
  t3.join();

  std::vector<std::string> messages = {"Message1", "Message2", "Message3"};
  std::sort(messages.begin(), messages.end());

  std::ifstream f(filename.get());
  ASSERT_TRUE(f.is_open());

  std::vector<std::string> strings;
  for (std::string line; std::getline(f, line);)
    strings.emplace_back(std::move(line));
  std::sort(strings.begin(), strings.end());

  EXPECT_EQ(strings, messages);
}

TEST(FilePtrSink, Log) {
  const auto file = tmpfile();

  FilePtrSink sink(file);

  sink.Log("Message1");
  sink.Log("Message2");
  sink.Log("Message3");

  const std::string messages[] = {"Message1", "Message2", "Message3"};

  std::rewind(file);

  size_t i = 0;
  std::string str_line;
  for (char line[256];
       std::feof(file) == 0 && std::fgets(line, std::size(line), file); i++) {
    str_line = line;
    if (!str_line.empty() && str_line.back() == '\n')
      str_line.erase(str_line.size() - 1);
    EXPECT_EQ(str_line, messages[i]);
  }
}

TEST(FilePtrSink, LogNonClosing) {
  const auto file = tmpfile();

  FilePtrSink sink(file, FilePtrSink::ShouldClose::kNo);

  sink.Log("Message1");
  sink.Log("Message2");
  sink.Log("Message3");

  const std::string messages[] = {"Message1", "Message2", "Message3"};

  std::rewind(file);

  size_t i = 0;
  std::string str_line;
  for (char line[256];
       std::feof(file) == 0 && std::fgets(line, std::size(line), file); i++) {
    str_line = line;
    if (!str_line.empty() && str_line.back() == '\n')
      str_line.erase(str_line.size() - 1);
    EXPECT_EQ(str_line, messages[i]);
  }
}

TEST(FilePtrSink, LogThreadSafe) {
  const auto file = tmpfile();

  FilePtrSink sink(file);

  std::thread t1([&sink]() {
    std::this_thread::yield();
    sink.Log("Message1");
  });
  std::thread t2([&sink]() { sink.Log("Message2"); });
  std::thread t3([&sink]() { sink.Log("Message3"); });

  t1.join();
  t2.join();
  t3.join();

  std::vector<std::string> messages = {
      {"Message1"}, {"Message2"}, {"Message3"}};
  std::sort(messages.begin(), messages.end());

  std::vector<std::string> strings;
  std::rewind(file);

  for (char line[256];
       std::feof(file) == 0 && std::fgets(line, std::size(line), file);) {
    std::string str_line = line;
    if (!str_line.empty() && str_line.back() == '\n')
      str_line.erase(str_line.size() - 1);
    strings.emplace_back(std::move(str_line));
  }
  std::sort(strings.begin(), strings.end());

  EXPECT_EQ(strings, messages);
}

}  // namespace rst
