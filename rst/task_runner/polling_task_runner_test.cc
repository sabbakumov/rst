// Copyright (c) 2019, Sergey Abbakumov
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

#include "rst/task_runner/polling_task_runner.h"

#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "rst/bind/bind_helpers.h"
#include "rst/stl/algorithm.h"

namespace chrono = std::chrono;

namespace rst {

TEST(PollingTaskRunner, IsTaskRunner) {
  const PollingTaskRunner task_runner(
      []() -> chrono::milliseconds { return chrono::milliseconds(0); });
  const TaskRunner& i_task_runner = task_runner;
  (void)i_task_runner;
}

TEST(PollingTaskRunner, InvalidPostTaskDelay) {
  PollingTaskRunner task_runner(
      []() -> chrono::milliseconds { return chrono::milliseconds(0); });
  EXPECT_DEATH(
      task_runner.PostDelayedTask(DoNothing(), chrono::milliseconds(-1)), "");
}

TEST(PollingTaskRunner, PostTaskInOrder) {
  PollingTaskRunner task_runner(
      []() -> chrono::milliseconds { return chrono::milliseconds(0); });

  std::string str, expected;
  for (auto i = 0; i < 1000; i++) {
    task_runner.PostTask([i, &str]() { str += std::to_string(i); });
    expected += std::to_string(i);
  }

  task_runner.RunPendingTasks();
  EXPECT_EQ(str, expected);
}

TEST(PollingTaskRunner, DestructorRunsPendingTasks) {
  std::string str, expected;

  {
    PollingTaskRunner task_runner(
        []() -> chrono::milliseconds { return chrono::milliseconds(0); });

    for (auto i = 0; i < 1000; i++) {
      task_runner.PostTask([i, &str]() { str += std::to_string(i); });
      expected += std::to_string(i);
    }
  }

  EXPECT_EQ(str, expected);
}

TEST(PollingTaskRunner, PostDelayedTaskInOrder) {
  auto ms = 0;
  PollingTaskRunner task_runner(
      [&ms]() -> chrono::milliseconds { return chrono::milliseconds(ms); });

  std::string str, first_half;
  for (auto i = 0; i < 500; i++) {
    task_runner.PostDelayedTask([i, &str]() { str += std::to_string(i); },
                                chrono::milliseconds(1));
    first_half += std::to_string(i);
  }

  auto expected = first_half;

  for (auto i = 500; i < 1000; i++) {
    task_runner.PostDelayedTask([i, &str]() { str += std::to_string(i); },
                                chrono::milliseconds(2));
    expected += std::to_string(i);
  }

  task_runner.RunPendingTasks();
  EXPECT_EQ(str, std::string());

  ms = 1;
  task_runner.RunPendingTasks();
  EXPECT_EQ(str, first_half);

  ms = 2;
  task_runner.RunPendingTasks();
  EXPECT_EQ(str, expected);
}

TEST(PollingTaskRunner, PostTaskConcurrently) {
  PollingTaskRunner task_runner(
      []() -> chrono::milliseconds { return chrono::milliseconds(0); });

  std::string str, expected;
  std::vector<std::thread> threads;
  static constexpr size_t kMaxThreadNumber = 10;
  threads.reserve(kMaxThreadNumber);
  for (size_t i = 0; i < kMaxThreadNumber; i++) {
    std::thread t([&task_runner, i, &str]() {
      task_runner.PostTask([i, &str]() { str += std::to_string(i); });
    });
    threads.emplace_back(std::move(t));
    expected += std::to_string(i);
  }

  task_runner.RunPendingTasks();

  for (auto& t : threads)
    t.join();

  task_runner.RunPendingTasks();

  sort(str);
  sort(expected);
  EXPECT_EQ(str, expected);
}

}  // namespace rst
