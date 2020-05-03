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

#include <utility>

#include "rst/check/check.h"
#include "rst/defer/defer.h"
#include "rst/stl/algorithm.h"

namespace chrono = std::chrono;

namespace rst {

ThreadPoolTaskRunner::InternalTaskRunner::InternalTaskRunner(
    std::function<chrono::milliseconds()>&& time_function)
    : time_function_(std::move(time_function)) {}

ThreadPoolTaskRunner::InternalTaskRunner::~InternalTaskRunner() = default;

void ThreadPoolTaskRunner::InternalTaskRunner::WaitAndRunTasks() {
  std::function<void()> task;
  RST_DEFER([this]() { thread_cv_.notify_one(); });

  while (true) {
    {
      std::unique_lock lock(thread_mutex_);

      if (should_exit_)
        return;

      if (!queue_.empty()) {
        const auto& item = queue_.front();
        const auto now = time_function_();
        if (now < item.time_point) {
          const auto wait_duration = item.time_point - now;
          thread_cv_.wait_for(lock, wait_duration);
        }
      } else {
        thread_cv_.wait(lock);
      }

      if (should_exit_)
        return;

      if (queue_.empty())
        continue;

      auto& item = queue_.front();
      const auto now = time_function_();
      if (now < item.time_point)
        continue;

      task = std::move(item.task);
      c_pop_heap(queue_, std::greater<>());
      queue_.pop_back();
    }

    task();
    task = nullptr;
  }
}

ThreadPoolTaskRunner::ThreadPoolTaskRunner(
    const size_t threads_num,
    std::function<chrono::milliseconds()>&& time_function)
    : task_runner_(
          std::make_shared<InternalTaskRunner>(std::move(time_function))) {
  RST_DCHECK(threads_num > 0);
  threads_.reserve(threads_num);
  for (size_t i = 0; i < threads_num; i++)
    threads_.emplace_back(&InternalTaskRunner::WaitAndRunTasks, task_runner_);
}

ThreadPoolTaskRunner::~ThreadPoolTaskRunner() {
  {
    std::mutex ending_task_mutex;
    std::condition_variable ending_task_cv;
    auto should_continue = false;

    PostTask([&ending_task_mutex, &ending_task_cv, &should_continue]() {
      std::lock_guard lock(ending_task_mutex);
      should_continue = true;
      ending_task_cv.notify_one();
    });

    std::unique_lock lock(ending_task_mutex);
    while (!should_continue)
      ending_task_cv.wait(lock);
  }

  {
    std::lock_guard lock(task_runner_->thread_mutex_);
    task_runner_->should_exit_ = true;
  }

  task_runner_->thread_cv_.notify_one();
  for (auto& thread : threads_)
    thread.join();
}

void ThreadPoolTaskRunner::PostDelayedTask(std::function<void()>&& task,
                                           const chrono::milliseconds delay) {
  RST_DCHECK(delay.count() >= 0);

  const auto now = task_runner_->time_function_();
  const auto future_time_point = now + delay;
  {
    std::lock_guard lock(task_runner_->thread_mutex_);
    task_runner_->queue_.emplace_back(
        future_time_point, task_runner_->task_id_++, std::move(task));
    c_push_heap(task_runner_->queue_, std::greater<>());
  }
  task_runner_->thread_cv_.notify_one();
}

}  // namespace rst
