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

#ifndef RST_BIND_BIND_H_
#define RST_BIND_BIND_H_

#include <functional>
#include <utility>

#include "rst/memory/weak_ptr.h"

namespace rst {

// Like std::bind() but doesn't call |f| when |weak_ptr| is invalidated.
//
// Example:
//
//   class Controller {
//    public:
//     void SpawnWorker() {
//       Worker::StartNew(Bind(&Controller::WorkComplete,
//                             weak_factory_.GetWeakPtr()));
//     }
//     void WorkComplete(const Result& result) { ... }
//
//    private:
//     WeakPtrFactory<Controller> weak_factory_{this};
//   };
//
//   class Worker {
//    public:
//     static void StartNew(std::function<void(const Result&)>&& callback) {
//       new Worker(std::move(callback));
//       // Asynchronous processing...
//     }
//
//    private:
//     Worker(std::function<void(const Result&)>&& callback) {
//         : callback_(std::move(callback)) {}
//
//     void DidCompleteAsynchronousProcessing(const Result& result) {
//       callback_(result);  // Does nothing if controller has been deleted.
//       delete this;
//     }
//
//     const std::function<void(const Result&)> callback_;
//   };
//
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshadow-uncaptured-local"
template <class F, class T, class... Args>
auto Bind(F&& f, WeakPtr<T>&& weak_ptr, Args&&... args) {
  return std::bind(
      [](const F& f, const WeakPtr<T>& weak_ptr, auto&&... args) {
        const auto nullable_self = weak_ptr.GetNullable();
        if (const auto self = nullable_self.get(); self != nullptr)
          std::invoke(f, self, std::forward<decltype(args)>(args)...);
      },
      std::forward<F>(f), std::move(weak_ptr), std::forward<Args>(args)...);
}
#pragma clang diagnostic pop

}  // namespace rst

#endif  // RST_BIND_BIND_H_
