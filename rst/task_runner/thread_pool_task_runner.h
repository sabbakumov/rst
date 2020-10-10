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

#ifndef RST_TASK_RUNNER_THREAD_POOL_TASK_RUNNER_H_
#define RST_TASK_RUNNER_THREAD_POOL_TASK_RUNNER_H_

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <vector>

#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"
#include "rst/task_runner/item.h"
#include "rst/task_runner/task_runner.h"

namespace rst {

// Task runner that is supposed to run tasks on dedicated threads.
//
// Example:
//
//   std::function<std::chrono::milliseconds()> time_function = ...;
//   size_t threads_num = ...;
//   ThreadPoolTaskRunner task_runner(threads_num, std::move(time_function));
//   ...
//   std::function<void()> task = ...;
//   task_runner.PostTask(std::move(task));
//   ...
//
class ThreadPoolTaskRunner : public TaskRunner {
 public:
  // Takes |time_function| that returns current time and creates |threads_num|
  // threads.
  ThreadPoolTaskRunner(
      size_t threads_num,
      std::function<std::chrono::milliseconds()>&& time_function);
  ~ThreadPoolTaskRunner() override;

  // TaskRunner:
  void PostDelayedTask(std::function<void()>&& task,
                       std::chrono::milliseconds delay) final;

  size_t threads_num() const { return delayed_task_runner_->threads_num(); }

 private:
  class DelayedTaskRunner {
   public:
    explicit DelayedTaskRunner(size_t threads_num);
    ~DelayedTaskRunner();

    void PushTasks(NotNull<std::vector<std::function<void()>>*> tasks);
    void PushTask(std::function<void()>&& task);

    size_t threads_num() const { return threads_.size(); }

   private:
    void WaitAndRunTasks();

    std::queue<std::function<void()>> tasks_;

    std::condition_variable thread_cv_;
    std::mutex thread_mutex_;

    std::vector<std::thread> threads_;

    bool should_exit_ = false;

    RST_DISALLOW_COPY_AND_ASSIGN(DelayedTaskRunner);
  };

  class ServiceTaskRunner {
   public:
    ServiceTaskRunner(
        NotNull<DelayedTaskRunner*> delayed_task_runner,
        std::function<std::chrono::milliseconds()>&& time_function);
    ~ServiceTaskRunner();

    void PushTask(std::function<void()>&& task,
                  std::chrono::milliseconds delay);

   private:
    void WaitAndScheduleTasks();

    std::condition_variable thread_cv_;
    std::mutex thread_mutex_;

    // Returns current time.
    const std::function<std::chrono::milliseconds()> time_function_;

    // Priority queue of tasks.
    std::vector<internal::Item> delayed_tasks_;

    const NotNull<DelayedTaskRunner*> delayed_task_runner_;

    // Increasing task counter.
    uint64_t task_id_ = 0;

    bool should_exit_ = false;

    std::thread thread_;

    RST_DISALLOW_COPY_AND_ASSIGN(ServiceTaskRunner);
  };

  std::optional<DelayedTaskRunner> delayed_task_runner_;
  std::optional<ServiceTaskRunner> service_task_runner_;

  RST_DISALLOW_COPY_AND_ASSIGN(ThreadPoolTaskRunner);
};

}  // namespace rst

#endif  // RST_TASK_RUNNER_THREAD_POOL_TASK_RUNNER_H_
