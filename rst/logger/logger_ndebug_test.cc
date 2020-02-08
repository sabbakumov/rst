// Copyright (c) 2017, Sergey Abbakumov
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

#undef NDEBUG
#define NDEBUG
#include "rst/logger/logger.h"
#include "rst/logger/sink.h"

#include <memory>
#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using testing::_;

namespace rst {
namespace {

class SinkMock : public Sink {
 public:
  MOCK_METHOD(void, Log, (std::string_view message), (override));
};

}  // namespace

TEST(Logger, DebugMacrosNDebug) {
  auto sink = std::make_unique<SinkMock>();

  EXPECT_CALL(*sink, Log(_)).Times(0);

  Logger logger(std::move(sink));
  Logger::SetGlobalLogger(&logger);

  RST_DLOG_DEBUG(kMessage);
  RST_DLOG_INFO(kMessage);
  RST_DLOG_WARNING(kMessage);
  RST_DLOG_ERROR(kMessage);
  RST_DLOG_FATAL(kMessage);
}

}  // namespace rst
