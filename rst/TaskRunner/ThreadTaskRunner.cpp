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

#include "rst/TaskRunner/ThreadTaskRunner.h"

#include <utility>

#include "rst/Check/Check.h"

namespace chrono = std::chrono;

namespace rst {

ThreadTaskRunner::InternalTaskRunner::InternalTaskRunner(
    std::function<chrono::milliseconds()>&& time_function)
    : time_function_(std::move(time_function)) {}

ThreadTaskRunner::InternalTaskRunner::~InternalTaskRunner() = default;

void ThreadTaskRunner::InternalTaskRunner::WaitAndRunTasks() {
  while (true) {
    {
      std::unique_lock<std::mutex> lock(thread_mutex_);

      if (should_exit_)
        return;

      if (!queue_.empty()) {
        const auto& item = queue_.top();
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

      const auto now = time_function_();
      while (!queue_.empty()) {
        const auto& item = queue_.top();
        if (now < item.time_point)
          break;

        auto task = item.task;
        queue_.pop();
        pending_tasks_.emplace_back(std::move(task));
      }
    }

    for (const auto& task : pending_tasks_)
      task();

    pending_tasks_.clear();
  }
}

ThreadTaskRunner::ThreadTaskRunner(
    std::function<chrono::milliseconds()>&& time_function)
    : task_runner_(
          std::make_shared<InternalTaskRunner>(std::move(time_function))),
      thread_(&InternalTaskRunner::WaitAndRunTasks,
              task_runner_.Clone().Take()) {}

ThreadTaskRunner::~ThreadTaskRunner() {
  if (thread_.joinable()) {
    std::mutex ending_task_mutex;
    std::condition_variable ending_task_cv;
    auto should_continue = false;

    PostTask(
        std::bind([&ending_task_mutex, &ending_task_cv, &should_continue]() {
          std::lock_guard<std::mutex> lock(ending_task_mutex);
          should_continue = true;
          ending_task_cv.notify_one();
        }));

    std::unique_lock<std::mutex> lock(ending_task_mutex);
    while (!should_continue)
      ending_task_cv.wait(lock);
  }

  {
    std::lock_guard<std::mutex> lock(task_runner_->thread_mutex_);
    task_runner_->should_exit_ = true;
  }

  task_runner_->thread_cv_.notify_one();
  if (thread_.joinable())
    thread_.join();
}

void ThreadTaskRunner::PostDelayedTask(std::function<void()>&& task,
                                       const chrono::milliseconds delay) {
  RST_DCHECK(delay.count() >= 0);

  const auto now = task_runner_->time_function_();
  const auto future_time_point = now + delay;
  {
    std::lock_guard<std::mutex> lock(task_runner_->thread_mutex_);
    task_runner_->queue_.emplace(future_time_point, task_runner_->task_id_,
                                 std::move(task));
    task_runner_->task_id_++;
  }
  task_runner_->thread_cv_.notify_one();
}

void ThreadTaskRunner::Detach() { thread_.detach(); }

}  // namespace rst
