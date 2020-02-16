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

#ifndef RST_TASK_RUNNER_POLLING_TASK_RUNNER_H_
#define RST_TASK_RUNNER_POLLING_TASK_RUNNER_H_

#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>

#include "rst/macros/macros.h"
#include "rst/task_runner/item.h"
#include "rst/task_runner/task_runner.h"

namespace rst {

// Task runner that is supposed to run tasks on the same thread.
//
// Example:
//
//   std::function<std::chrono::milliseconds()> time_function = ...;
//   PollingTaskRunner task_runner(std::move(time_function));
//   for (;; task_runner.RunPendingTasks()) {
//     ...
//     std::function<void()> task = ...;
//     task_runner.PostTask(std::move(task));
//     ...
//   }
//
class PollingTaskRunner : public TaskRunner {
 public:
  // Takes |time_function| that returns current time.
  explicit PollingTaskRunner(
      std::function<std::chrono::milliseconds()>&& time_function);
  ~PollingTaskRunner();

  void PostDelayedTask(std::function<void()>&& task,
                       std::chrono::milliseconds delay) final;

  // Runs all pending tasks in interval (-inf, time_function_()].
  void RunPendingTasks();

 private:
  // Returns current time.
  const std::function<std::chrono::milliseconds()> time_function_;
  // Used to not to allocate memory on every RunPendingTasks() call.
  std::vector<std::function<void()>> pending_tasks_;
  std::mutex mutex_;
  // Priority queue of tasks.
  std::vector<internal::Item> queue_;
  // Increasing task counter.
  uint64_t task_id_ = 0;

  RST_DISALLOW_COPY_AND_ASSIGN(PollingTaskRunner);
};

}  // namespace rst

#endif  // RST_TASK_RUNNER_POLLING_TASK_RUNNER_H_
