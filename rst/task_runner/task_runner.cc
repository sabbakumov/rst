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

#include "rst/task_runner/task_runner.h"

#include <atomic>
#include <memory>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/threading/barrier.h"

namespace chrono = std::chrono;

namespace rst {
namespace {

struct AppliedItem {
  AppliedItem(std::function<void(size_t)>&& task, const size_t iterations)
      : barrier(iterations), task(std::move(task)) {}
  ~AppliedItem() = default;

  Barrier barrier;
  std::function<void(size_t)> task;
  std::atomic<size_t> i = 0;

 private:
  RST_DISALLOW_COPY_AND_ASSIGN(AppliedItem);
};

}  // namespace

TaskRunner::~TaskRunner() = default;

void TaskRunner::ApplyTaskSync(std::function<void(size_t)>&& task,
                               const size_t iterations) {
  RST_DCHECK(iterations != 0);

  const auto item = std::make_shared<AppliedItem>(std::move(task), iterations);

  auto f = [
#if RST_BUILDFLAG(DCHECK_IS_ON)
               iterations,
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
               item]() {
    const auto i = item->i.fetch_add(1, std::memory_order_relaxed);
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(i < iterations);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    item->task(i);
    item->barrier.CountDown();
  };

  PostDelayedTaskWithIterations(std::move(f), chrono::milliseconds::zero(),
                                iterations - 1);
  item->barrier.Wait();
}

}  // namespace rst
