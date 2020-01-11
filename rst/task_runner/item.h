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

#ifndef RST_TASK_RUNNER_ITEM_H_
#define RST_TASK_RUNNER_ITEM_H_

#include <chrono>
#include <cstdint>
#include <functional>
#include <tuple>
#include <type_traits>
#include <utility>

#include "rst/macros/macros.h"

namespace rst {
namespace internal {

// Used in implementations of TaskRunner interface to maintain an ordered queue
// of tasks.
struct Item {
  using Function = std::function<void()>;

  Item(const std::chrono::milliseconds time_point, const uint64_t task_id,
       std::function<void()>&& task)
      : time_point(time_point), task_id(task_id), task(std::move(task)) {}
  Item(Item&&) noexcept(std::is_nothrow_move_constructible<Function>::value) =
      default;
  ~Item() = default;

  Item& operator=(Item&&) noexcept(
      std::is_nothrow_move_assignable<Function>::value) = default;
  bool operator>(const Item& item) const {
    return std::make_tuple(time_point, task_id) >
           std::make_tuple(item.time_point, item.task_id);
  }

  std::chrono::milliseconds time_point;
  uint64_t task_id = 0;
  Function task;

 private:
  RST_DISALLOW_COPY_AND_ASSIGN(Item);
};

}  // namespace internal
}  // namespace rst

#endif  // RST_TASK_RUNNER_ITEM_H_
