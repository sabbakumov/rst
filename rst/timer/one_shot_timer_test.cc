// Copyright (c) 2020, Sergey Abbakumov
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

#include "rst/timer/one_shot_timer.h"

#include <functional>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "rst/task_runner/task_runner.h"

namespace chrono = std::chrono;

using testing::_;
using testing::SaveArg;

namespace rst {
namespace {

class MockTaskRunner : public TaskRunner {
 public:
  MOCK_METHOD(void, PostDelayedTaskWithIterations,
              (std::function<void()> && task, chrono::milliseconds delay,
               size_t iterations),
              (override));
};

class Callee {
 public:
  MOCK_METHOD(void, Run, ());
};

}  // namespace

class OneShotTimerTest : public testing::Test {
 public:
  ~OneShotTimerTest() override;

 protected:
  MockTaskRunner task_runner_;
  Callee callee_;
};

OneShotTimerTest::~OneShotTimerTest() = default;

TEST_F(OneShotTimerTest, Test) {
  OneShotTimer timer(&task_runner_);
  EXPECT_FALSE(timer.IsRunning());

  std::function<void()> task;
  EXPECT_CALL(task_runner_,
              PostDelayedTaskWithIterations(_, chrono::milliseconds(1), 0))
      .WillOnce(SaveArg<0>(&task));

  EXPECT_FALSE(timer.IsRunning());
  timer.Start(std::bind(&Callee::Run, &callee_), chrono::milliseconds(1));
  EXPECT_TRUE(timer.IsRunning());

  EXPECT_CALL(callee_, Run());
  task();
  EXPECT_FALSE(timer.IsRunning());
}

TEST_F(OneShotTimerTest, OutOfScope) {
  std::function<void()> task;

  {
    OneShotTimer timer(&task_runner_);
    EXPECT_FALSE(timer.IsRunning());

    EXPECT_CALL(task_runner_,
                PostDelayedTaskWithIterations(_, chrono::milliseconds(1), 0))
        .WillOnce(SaveArg<0>(&task));

    EXPECT_FALSE(timer.IsRunning());
    timer.Start(std::bind(&Callee::Run, &callee_), chrono::milliseconds(1));
    EXPECT_TRUE(timer.IsRunning());
  }

  EXPECT_CALL(callee_, Run()).Times(0);
  task();
}

TEST_F(OneShotTimerTest, Restart) {
  OneShotTimer timer(&task_runner_);
  EXPECT_FALSE(timer.IsRunning());

  std::function<void()> task1, task2;
  EXPECT_CALL(task_runner_,
              PostDelayedTaskWithIterations(_, chrono::milliseconds(1), 0))
      .WillOnce(SaveArg<0>(&task1))
      .WillOnce(SaveArg<0>(&task2));

  Callee callee2;
  EXPECT_FALSE(timer.IsRunning());
  timer.Start(std::bind(&Callee::Run, &callee_), chrono::milliseconds(1));
  EXPECT_TRUE(timer.IsRunning());
  timer.Start(std::bind(&Callee::Run, &callee2), chrono::milliseconds(1));
  EXPECT_TRUE(timer.IsRunning());

  EXPECT_CALL(callee_, Run()).Times(0);
  task1();
  EXPECT_TRUE(timer.IsRunning());

  EXPECT_CALL(callee2, Run());
  task2();
  EXPECT_FALSE(timer.IsRunning());
}

TEST_F(OneShotTimerTest, FireNow) {
  OneShotTimer timer(&task_runner_);
  EXPECT_FALSE(timer.IsRunning());

  std::function<void()> task;
  EXPECT_CALL(task_runner_,
              PostDelayedTaskWithIterations(_, chrono::milliseconds(1), 0))
      .WillOnce(SaveArg<0>(&task));

  EXPECT_FALSE(timer.IsRunning());
  timer.Start(std::bind(&Callee::Run, &callee_), chrono::milliseconds(1));
  EXPECT_TRUE(timer.IsRunning());

  EXPECT_CALL(callee_, Run());
  timer.FireNow();
  EXPECT_FALSE(timer.IsRunning());
  testing::Mock::VerifyAndClearExpectations(&callee_);

  task();
}

TEST_F(OneShotTimerTest, StartNullFunction) {
  OneShotTimer timer(&task_runner_);
  EXPECT_DEATH(timer.Start(nullptr, chrono::milliseconds(1)), "");
}

TEST_F(OneShotTimerTest, FireNowNotRunning) {
  OneShotTimer timer(&task_runner_);
  EXPECT_DEATH(timer.FireNow(), "");
}

TEST_F(OneShotTimerTest, NullTaskRunner) {
  TaskRunner* task_runner = nullptr;
  EXPECT_DEATH((OneShotTimer(task_runner)), "");
}

}  // namespace rst
