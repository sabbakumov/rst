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

ThreadPoolTaskRunner::DelayedTaskRunner::DelayedTaskRunner(
    const size_t threads_num) {
  RST_DCHECK(threads_num > 0);
  threads_.reserve(threads_num);
  for (size_t i = 0; i < threads_num; i++)
    threads_.emplace_back(&DelayedTaskRunner::WaitAndRunTasks, this);
}

ThreadPoolTaskRunner::DelayedTaskRunner::~DelayedTaskRunner() {
  {
    std::lock_guard lock(thread_mutex_);
    should_exit_ = true;
  }

  thread_cv_.notify_one();
  for (auto& thread : threads_)
    thread.join();
}

void ThreadPoolTaskRunner::DelayedTaskRunner::WaitAndRunTasks() {
  std::function<void()> task;
  RST_DEFER([this]() { thread_cv_.notify_one(); });

  while (true) {
    auto had_items = false;

    {
      std::unique_lock lock(thread_mutex_);

      while (tasks_.empty() && !should_exit_)
        thread_cv_.wait(lock);

      if (should_exit_)
        return;

      RST_DCHECK(!tasks_.empty());
      auto& front = tasks_.front();
      if (front.iterations == 0) {
        task = std::move(front.task);
        tasks_.pop();
      } else {
        task = front.task;
        front.iterations--;
      }

      had_items = !tasks_.empty();
    }

    if (had_items)
      thread_cv_.notify_one();

    task();
    task = nullptr;
  }
}

void ThreadPoolTaskRunner::DelayedTaskRunner::PushTasks(
    const NotNull<std::vector<internal::IterationItem>*> tasks) {
  RST_DCHECK(!tasks->empty());

  {
    std::lock_guard lock(thread_mutex_);
    for (auto& task : *tasks)
      tasks_.emplace(std::move(task));
  }

  thread_cv_.notify_one();
}

void ThreadPoolTaskRunner::DelayedTaskRunner::PushTask(
    internal::IterationItem task) {
  {
    std::lock_guard lock(thread_mutex_);
    tasks_.emplace(std::move(task));
  }

  thread_cv_.notify_one();
}

ThreadPoolTaskRunner::ServiceTaskRunner::ServiceTaskRunner(
    const NotNull<DelayedTaskRunner*> delayed_task_runner,
    std::function<std::chrono::milliseconds()>&& time_function)
    : time_function_(std::move(time_function)),
      delayed_task_runner_(delayed_task_runner),
#pragma warning(push)
#pragma warning(disable : 4355)
      thread_(&ServiceTaskRunner::WaitAndScheduleTasks, this)
#pragma warning(pop)
{
}

ThreadPoolTaskRunner::ServiceTaskRunner::~ServiceTaskRunner() {
  {
    std::lock_guard lock(thread_mutex_);
    should_exit_ = true;
  }

  thread_cv_.notify_one();
  thread_.join();
}

void ThreadPoolTaskRunner::ServiceTaskRunner::WaitAndScheduleTasks() {
  std::vector<internal::IterationItem> tasks;

  while (true) {
    {
      std::unique_lock lock(thread_mutex_);

      if (should_exit_)
        return;

      if (!delayed_tasks_.empty()) {
        const auto& item = delayed_tasks_.front();
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

      if (delayed_tasks_.empty())
        continue;

      const auto now = time_function_();
      RST_DCHECK(tasks.empty());
      while (!delayed_tasks_.empty()) {
        auto& item = delayed_tasks_.front();
        if (now < item.time_point)
          break;

        tasks.emplace_back(std::move(item.task), item.iterations);
        c_pop_heap(delayed_tasks_, std::greater<>());
        delayed_tasks_.pop_back();
      }
    }

    if (!tasks.empty()) {
      delayed_task_runner_->PushTasks(&tasks);
      tasks.clear();
    }
  }
}

void ThreadPoolTaskRunner::ServiceTaskRunner::PushTask(
    std::function<void()>&& task, const std::chrono::milliseconds delay,
    const size_t iterations) {
  RST_DCHECK(delay.count() > 0);

  const auto now = time_function_();
  const auto future_time_point = now + delay;
  {
    std::lock_guard lock(thread_mutex_);
    delayed_tasks_.emplace_back(std::move(task), future_time_point, task_id_++,
                                iterations);
    c_push_heap(delayed_tasks_, std::greater<>());
  }

  thread_cv_.notify_one();
}

ThreadPoolTaskRunner::ThreadPoolTaskRunner(
    const size_t threads_num,
    std::function<chrono::milliseconds()>&& time_function)
    : delayed_task_runner_(threads_num),
      service_task_runner_(std::in_place, &*delayed_task_runner_,
                           std::move(time_function)) {}

ThreadPoolTaskRunner::~ThreadPoolTaskRunner() {
  std::mutex ending_task_mutex;
  std::condition_variable ending_task_cv;
  auto should_continue = false;

  PostTask([&ending_task_mutex, &ending_task_cv, &should_continue]() {
    std::lock_guard lock(ending_task_mutex);
    should_continue = true;
    // Notify under the lock since the cv can be destroyed otherwise.
    ending_task_cv.notify_one();
  });

  std::unique_lock lock(ending_task_mutex);
  while (!should_continue)
    ending_task_cv.wait(lock);
}

void ThreadPoolTaskRunner::PostDelayedTaskWithIterations(
    std::function<void()>&& task, const chrono::milliseconds delay,
    const size_t iterations) {
  if (delay == chrono::milliseconds::zero()) {
    delayed_task_runner_->PushTask(
        internal::IterationItem(std::move(task), iterations));
  } else {
    service_task_runner_->PushTask(std::move(task), delay, iterations);
  }
}

}  // namespace rst
