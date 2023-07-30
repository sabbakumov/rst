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

#ifndef RST_THREADING_BARRIER_H_
#define RST_THREADING_BARRIER_H_

#include <condition_variable>
#include <cstddef>
#include <mutex>

#include "rst/macros/macros.h"

namespace rst {

// Provides a thread-coordination mechanism that allows a set of participating
// threads to block until an operation is completed. The value of the counter
// is initialized on creation. Threads block until the counter is decremented
// to zero.
//
// Example:
//
//   #include "rst/threading/barrier.h"
//
//   rst::Barrier barrier(5);
//
//   std::vector<std::thread> threads;
//   for (auto i = 0; i < 5; i++)
//     threads.emplace_back([&barrier]() { barrier.CountDown(); });
//
//   barrier.Wait();
//   // Synchronization point.
//
class Barrier {
 public:
  explicit Barrier(size_t counter);
  ~Barrier();

  // Decrements the internal counter by 1 and blocks the calling thread until
  // the counter reaches zero.
  void CountDownAndWait();
  // Decrements the internal counter by 1.
  void CountDown();
  // Blocks the calling thread until the internal counter reaches 0.
  void Wait();

 private:
  std::condition_variable cv_;
  std::mutex mutex_;
  size_t counter_ = 0;

  RST_DISALLOW_COPY_AND_ASSIGN(Barrier);
};

}  // namespace rst

#endif  // RST_THREADING_BARRIER_H_
