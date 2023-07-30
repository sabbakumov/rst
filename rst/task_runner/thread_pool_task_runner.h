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
#include <queue>
#include <thread>
#include <unordered_map>
#include <vector>

#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"
#include "rst/task_runner/item.h"
#include "rst/task_runner/iteration_item.h"
#include "rst/task_runner/task_runner.h"

namespace rst {

// Task runner that is supposed to run tasks on dedicated threads that have
// their keep alive time. After that time of inactivity the threads stop.
//
// Example:
//
//   #include "rst/task_runner/thread_pool_task_runner.h"
//
//   std::function<std::chrono::nanoseconds()> time_function = ...;
//   size_t max_threads_num = ...;
//   std::chrono::nanoseconds keep_alive_time = ...;
//   rst::ThreadPoolTaskRunner task_runner(max_threads_num,
//                                         std::move(time_function),
//                                         keep_alive_time);
//   ...
//   std::function<void()> task = ...;
//   task_runner.PostTask(std::move(task));
//   task = ...;
//   task_runner.PostDelayedTask(std::move(task), std::chrono::seconds(1));
//   ...
//
//   // Posts a single |task| and waits for all |iterations| to complete before
//   // returning. The current index of iteration is passed to each invocation.
//   std::function<void(size_t)> task = ...;
//   constexpr size_t iterations = 100;
//   task_runner.ApplyTaskSync(std::move(task), iterations);
//
class ThreadPoolTaskRunner : public TaskRunner {
 public:
  // Takes |time_function| that returns current time. Up to |max_threads_num|
  // threads can be created during the pool lifetime. Threads will be
  // terminated if they have been idle for more than the |keep_alive_time|.
  ThreadPoolTaskRunner(
      size_t max_threads_num,
      std::function<std::chrono::nanoseconds()>&& time_function,
      std::chrono::nanoseconds keep_alive_time);
  ~ThreadPoolTaskRunner() override;

 private:
  // TaskRunner:
  void PostDelayedTaskWithIterations(std::function<void()>&& task,
                                     std::chrono::nanoseconds delay,
                                     size_t iterations) final;
  class DelayedTaskRunner {
   public:
    DelayedTaskRunner(size_t max_threads_num,
                      std::chrono::nanoseconds keep_alive_time);
    ~DelayedTaskRunner();

    void PushTasks(NotNull<std::vector<internal::IterationItem>*> items);
    void PushTask(internal::IterationItem item);

   private:
    void WaitAndRunTasks();

    std::queue<internal::IterationItem> tasks_;

    std::condition_variable thread_cv_;
    std::mutex thread_mutex_;

    std::unordered_map<std::thread::id, std::thread> thread_id_to_thread_map_;
    const size_t max_threads_num_;
    const std::chrono::nanoseconds keep_alive_time_;
    size_t waiting_threads_num_ = 0;

    bool should_exit_ = false;

    RST_DISALLOW_COPY_AND_ASSIGN(DelayedTaskRunner);
  };

  class ServiceTaskRunner {
   public:
    ServiceTaskRunner(
        NotNull<DelayedTaskRunner*> delayed_task_runner,
        std::function<std::chrono::nanoseconds()>&& time_function);
    ~ServiceTaskRunner();

    void PushTask(std::function<void()>&& task, std::chrono::nanoseconds delay,
                  size_t iterations);

   private:
    void WaitAndScheduleTasks();

    std::condition_variable thread_cv_;
    std::mutex thread_mutex_;

    // Returns current time.
    const std::function<std::chrono::nanoseconds()> time_function_;

    // Priority queue of tasks.
    std::vector<internal::Item> delayed_tasks_;

    DelayedTaskRunner& delayed_task_runner_;

    // Increasing task counter.
    uint64_t task_id_ = 0;

    bool should_exit_ = false;

    std::thread thread_;

    RST_DISALLOW_COPY_AND_ASSIGN(ServiceTaskRunner);
  };

  DelayedTaskRunner delayed_task_runner_;
  ServiceTaskRunner service_task_runner_;

  RST_DISALLOW_COPY_AND_ASSIGN(ThreadPoolTaskRunner);
};

}  // namespace rst

#endif  // RST_TASK_RUNNER_THREAD_POOL_TASK_RUNNER_H_
