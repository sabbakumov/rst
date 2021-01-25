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

#ifndef RST_TASK_RUNNER_TASK_RUNNER_H_
#define RST_TASK_RUNNER_TASK_RUNNER_H_

#include <chrono>
#include <cstddef>
#include <functional>
#include <utility>

namespace rst {

// An object that runs posted tasks in sequence (in the form of
// std::function<void()> objects). All methods are thread-safe. Implementations
// should use a tick clock, rather than wall clock time, to implement delay.
class TaskRunner {
 public:
  virtual ~TaskRunner();

  // Like |PostTask()|, but tries to run the posted |task| only after |delay|
  // has passed.
  void PostDelayedTask(std::function<void()>&& task,
                       std::chrono::nanoseconds delay) {
    PostDelayedTaskWithIterations(std::move(task), delay, 0);
  }

  // Posts the given |task| to be run.
  void PostTask(std::function<void()>&& task) {
    PostDelayedTask(std::move(task), std::chrono::nanoseconds::zero());
  }

  // Posts a single |task| and waits for all |iterations| to complete before
  // returning. The current index of iteration is passed to each invocation.
  void ApplyTaskSync(std::function<void(size_t)>&& task, size_t iterations);

 protected:
  virtual void PostDelayedTaskWithIterations(std::function<void()>&& task,
                                             std::chrono::nanoseconds delay,
                                             size_t iterations) = 0;
};

}  // namespace rst

#endif  // RST_TASK_RUNNER_TASK_RUNNER_H_
