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

#include "rst/Threading/Barrier.h"

#include "rst/Check/Check.h"

namespace rst {

Barrier::Barrier(const size_t counter) : counter_(counter) {
  RST_DCHECK(counter > 0);
}

Barrier::~Barrier() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
  std::unique_lock<std::mutex> lock(mutex_);
  RST_DCHECK(counter_ == 0);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
}

void Barrier::CountDownAndWait() {
  {
    std::unique_lock<std::mutex> lock(mutex_);
    RST_DCHECK(counter_ > 0);

    counter_--;
    while (counter_ != 0)
      cv_.wait(lock);
  }

  cv_.notify_one();
}

}  // namespace rst
