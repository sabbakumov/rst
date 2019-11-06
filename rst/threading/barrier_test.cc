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

#include "rst/threading/barrier.h"

#include <cstddef>
#include <optional>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

namespace rst {

TEST(Barrier, Normal) {
  static constexpr size_t kMaxThreadNumber = 20;
  std::vector<std::thread> threads;
  threads.reserve(kMaxThreadNumber);

  for (size_t i = 1; i <= kMaxThreadNumber; i++) {
    Barrier barrier(i);

    threads.clear();
    for (size_t j = 0; j < i; j++)
      threads.emplace_back([&barrier]() { barrier.CountDownAndWait(); });

    for (auto& thread : threads)
      thread.join();
  }
}

TEST(Barrier, ZeroCounter) { EXPECT_DEATH(Barrier(0), ""); }

TEST(Barrier, CalledMoreTimesThanNeeded) {
  Barrier barrier(1);

  barrier.CountDownAndWait();
  EXPECT_DEATH(barrier.CountDownAndWait(), "");
}

TEST(Barrier, CalledLessTimesThanNeeded) { EXPECT_DEATH(Barrier(1), ""); }

}  // namespace rst
