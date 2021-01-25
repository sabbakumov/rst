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

#include "rst/task_runner/thread_pool_task_runner.h"

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "rst/bind/bind_helpers.h"
#include "rst/not_null/not_null.h"
#include "rst/stl/algorithm.h"

namespace chrono = std::chrono;

namespace rst {
namespace {

void Wait(const NotNull<ThreadPoolTaskRunner*> task_runner) {
  std::mutex mtx;
  std::condition_variable cv;
  auto should_continue = false;

  task_runner->PostTask([&mtx, &cv, &should_continue]() {
    std::lock_guard lock(mtx);
    should_continue = true;
    cv.notify_one();
  });

  std::unique_lock lock(mtx);
  while (!should_continue)
    cv.wait(lock);
}

}  // namespace

TEST(ThreadPoolTaskRunner, IsTaskRunner) {
  const ThreadPoolTaskRunner task_runner(
      1, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
      chrono::seconds(60));
  const TaskRunner& i_task_runner = task_runner;
  (void)i_task_runner;
}

TEST(ThreadPoolTaskRunner, InvalidPostTaskDelay) {
  ThreadPoolTaskRunner task_runner(
      1, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
      chrono::seconds(60));
  EXPECT_DEATH(
      task_runner.PostDelayedTask(DoNothing(), chrono::nanoseconds(-1)), "");
}

TEST(ThreadPoolTaskRunner, PostTaskInOrder) {
  std::mutex mtx;
  ThreadPoolTaskRunner task_runner(
      1, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
      chrono::seconds(60));

  std::vector<int> result, expected;
  for (auto i = 0; i < 1000; i++) {
    task_runner.PostTask([i, &mtx, &result]() {
      std::lock_guard lock(mtx);
      result.emplace_back(i);
    });
    expected.emplace_back(i);
  }

  Wait(&task_runner);
  EXPECT_EQ(result, expected);
}

TEST(ThreadPoolTaskRunner, PostDelayedTaskInOrder) {
  std::mutex mtx;
  std::atomic<int> ns = 0;
  ThreadPoolTaskRunner task_runner(
      1,
      [&ns]() -> chrono::nanoseconds {
        return chrono::nanoseconds(ns.load(std::memory_order_relaxed));
      },
      chrono::seconds(60));

  std::vector<int> result, first_half, expected;
  for (auto i = 0; i < 500; i++) {
    task_runner.PostDelayedTask(
        [i, &mtx, &result]() {
          std::lock_guard lock(mtx);
          result.emplace_back(i);
        },
        chrono::nanoseconds(100));
    first_half.emplace_back(i);
    expected.emplace_back(i);
  }

  for (auto i = 500; i < 1000; i++) {
    task_runner.PostDelayedTask(
        [i, &mtx, &result]() {
          std::lock_guard lock(mtx);
          result.emplace_back(i);
        },
        chrono::nanoseconds(200));
    expected.emplace_back(i);
  }

  {
    std::lock_guard lock(mtx);
    EXPECT_TRUE(result.empty());
  }

  ns.store(100, std::memory_order_relaxed);
  while (true) {
    std::lock_guard lock(mtx);
    if (result == first_half)
      break;
  }

  ns.store(200, std::memory_order_relaxed);
  while (true) {
    std::lock_guard lock(mtx);
    if (result == expected)
      break;
  }
}

TEST(ThreadPoolTaskRunner, PostTaskConcurrently) {
  std::mutex mtx;
  ThreadPoolTaskRunner task_runner(
      1, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
      chrono::seconds(60));

  std::vector<size_t> result, expected;
  std::vector<std::thread> threads;
  static constexpr size_t kMaxThreadNumber = 10;
  threads.reserve(kMaxThreadNumber);
  for (size_t i = 0; i < kMaxThreadNumber; i++) {
    std::thread t([&task_runner, i, &mtx, &result]() {
      task_runner.PostTask([i, &mtx, &result]() {
        std::lock_guard lock(mtx);
        result.emplace_back(i);
      });
    });
    threads.emplace_back(std::move(t));
    expected.emplace_back(i);
  }

  for (auto& t : threads)
    t.join();

  c_sort(expected);
  while (true) {
    std::lock_guard lock(mtx);
    c_sort(result);
    if (result == expected)
      break;
  }
}

TEST(ThreadPoolTaskRunner, MultipleThreads) {
  for (size_t t = 1; t <= 24; t++) {
    std::mutex mtx;
    ThreadPoolTaskRunner task_runner(
        t, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
        chrono::seconds(60));

    std::vector<int> result, expected;
    for (auto i = 0; i < 100; i++) {
      task_runner.PostTask([i, &mtx, &result]() {
        std::lock_guard lock(mtx);
        result.emplace_back(i);
      });
      expected.emplace_back(i);
    }

    c_sort(expected);
    while (true) {
      std::unique_lock lock(mtx);
      c_sort(result);
      if (result == expected)
        break;
    }
  }
}

TEST(ThreadPoolTaskRunner, ApplyTaskSync) {
  for (size_t t = 1; t <= 24; t++) {
    for (auto i = 1; i <= 24; i++) {
      std::mutex mtx;
      ThreadPoolTaskRunner task_runner(
          t, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
          chrono::seconds(60));

      std::vector<int> result, expected;
      for (auto j = 0; j < i; j++)
        expected.emplace_back(j);

      task_runner.ApplyTaskSync(
          [&mtx, &result](const size_t iteration) {
            std::lock_guard lock(mtx);
            result.emplace_back(static_cast<int>(iteration));
          },
          static_cast<size_t>(i));

      c_sort(expected);
      c_sort(result);
      EXPECT_EQ(result, expected);
    }
  }
}

TEST(ThreadPoolTaskRunner, CrashOnZeroIteration) {
  ThreadPoolTaskRunner task_runner(
      1, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
      chrono::seconds(60));

  EXPECT_DEATH(task_runner.ApplyTaskSync(DoNothing(), 0), "");
}

TEST(ThreadPoolTaskRunner, Timeout) {
  std::mutex mtx;
  ThreadPoolTaskRunner task_runner(
      1, []() -> chrono::nanoseconds { return chrono::nanoseconds(0); },
      chrono::seconds(1));

  std::vector<int> result, expected{0, 1};
  task_runner.PostTask([&mtx, &result]() {
    std::lock_guard lock(mtx);
    result.emplace_back(0);
  });

  std::this_thread::sleep_for(chrono::seconds(2));

  task_runner.PostTask([&mtx, &result]() {
    std::lock_guard lock(mtx);
    result.emplace_back(1);
  });

  while (true) {
    std::unique_lock lock(mtx);
    if (result == expected)
      break;
  }
}

}  // namespace rst
