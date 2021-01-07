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

#include "rst/timer/one_shot_timer.h"

#include <utility>

#include "rst/bind/bind.h"
#include "rst/check/check.h"
#include "rst/stl/function.h"

namespace chrono = std::chrono;

namespace rst {

OneShotTimer::OneShotTimer(const NotNull<TaskRunner*> task_runner)
    : task_runner_(*task_runner) {}

OneShotTimer::~OneShotTimer() = default;

void OneShotTimer::Start(std::function<void()>&& task,
                         const chrono::milliseconds delay) {
  RST_DCHECK(task != nullptr);
  task_ = std::move(task);
  is_running_ = true;
  task_runner_.PostDelayedTask(
      Bind(&OneShotTimer::RunTask, weak_factory_.GetWeakPtr(), ++task_id_),
      delay);
}

void OneShotTimer::FireNow() {
  RST_DCHECK(IsRunning());
  RunTask(task_id_);
  is_running_ = false;
}

void OneShotTimer::RunTask(const uint64_t task_id) {
  if (!is_running_ || task_id != task_id_)
    return;

  RST_DCHECK(task_ != nullptr);
  const auto task = TakeFunction(std::move(task_));

  is_running_ = false;
  task();
}

}  // namespace rst
