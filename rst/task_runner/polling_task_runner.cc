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

#include <utility>

#include "rst/check/check.h"
#include "rst/stl/algorithm.h"

namespace chrono = std::chrono;

namespace rst {

PollingTaskRunner::PollingTaskRunner(
    std::function<chrono::milliseconds()>&& time_function)
    : time_function_(std::move(time_function)) {}

PollingTaskRunner::~PollingTaskRunner() { RunPendingTasks(); }

void PollingTaskRunner::PostDelayedTask(std::function<void()>&& task,
                                        const chrono::milliseconds delay) {
  RST_DCHECK(delay.count() >= 0);

  const auto now = time_function_();
  const auto future_time_point = now + delay;
  std::lock_guard lock(mutex_);
  queue_.emplace_back(future_time_point, task_id_++, std::move(task));
  c_push_heap(queue_, std::greater<>());
}

void PollingTaskRunner::RunPendingTasks() {
  {
    std::lock_guard lock(mutex_);

    const auto now = time_function_();
    RST_DCHECK(pending_tasks_.empty());
    while (!queue_.empty()) {
      auto& item = queue_.front();
      if (now < item.time_point)
        break;

      auto task = std::move(item.task);
      c_pop_heap(queue_, std::greater<>());
      queue_.pop_back();
      pending_tasks_.emplace_back(std::move(task));
    }
  }

  for (const auto& task : pending_tasks_)
    task();

  pending_tasks_.clear();
}

}  // namespace rst
