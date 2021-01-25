// Copyright (c) 2020, Sergey Abbakumov
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

#ifndef RST_TIMER_ONE_SHOT_TIMER_H_
#define RST_TIMER_ONE_SHOT_TIMER_H_

#include <chrono>
#include <cstdint>
#include <functional>

#include "rst/macros/macros.h"
#include "rst/memory/weak_ptr.h"
#include "rst/task_runner/task_runner.h"

namespace rst {

// OneShotTimer provides a simple timer API. As the name suggests, OneShotTimer
// calls back once after a time delay expires.
//
// OneShotTimer cancels the timer when it goes out of scope, which makes it
// easy to ensure that you do not get called when your object has gone out of
// scope. Just instantiate a timer as a member variable of the class for which
// you wish to receive timer events.
//
// Example:
//
//   class MyClass {
//    public:
//     void DelayDoingStuff() {
//       timer_.Start(std::bind(&MyClass::DoStuff, this),
//                    std::chrono::seconds(1));
//     }
//
//    private:
//     void DoStuff() {
//       // This method is called after 1 second.
//     }
//
//     OneShotTimer timer_{&GetTaskRunner};
//   };
//
class OneShotTimer : public SupportsWeakPtr<OneShotTimer> {
 public:
  explicit OneShotTimer(std::function<TaskRunner&()>&& get_task_runner_fn);
  ~OneShotTimer();

  // Starts the timer to run the |task| at the given |delay| from now. If the
  // timer is already running, it will be replaced to call the given |task|.
  void Start(std::function<void()>&& task, std::chrono::nanoseconds delay);

  // Returns true if the timer is running.
  bool IsRunning() const { return is_running_; }

  // Runs the scheduled task immediately. The timer needs to be running.
  void FireNow();

 private:
  void RunTask(uint64_t task_id);

  const std::function<TaskRunner&()> get_task_runner_fn_;
  std::function<void()> task_;
  uint64_t task_id_ = 0;
  bool is_running_ = false;

  RST_DISALLOW_COPY_AND_ASSIGN(OneShotTimer);
};

}  // namespace rst

#endif  // RST_TIMER_ONE_SHOT_TIMER_H_
