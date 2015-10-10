// Copyright (c) 2015, Sergey Abbakumov
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

#ifndef RST_CPP14_MEMORY_H_
#define RST_CPP14_MEMORY_H_

#include <memory>

namespace rst {

template <class Tp>
struct unique_if {
  using unique_single = std::unique_ptr<Tp>;
};

template <class Tp>
struct unique_if<Tp[]> {
  using unique_array_unknown_bound = std::unique_ptr<Tp[]>;
};

template <class Tp, size_t Np>
struct unique_if<Tp[Np]> {
  using unique_array_known_bound = void;
};

template <class Tp, class... Args>
inline typename unique_if<Tp>::unique_single make_unique(Args&&... args) {
  return std::unique_ptr<Tp>(new Tp(std::forward<Args>(args)...));
}

template <class Tp>
inline typename unique_if<Tp>::unique_array_unknown_bound make_unique(
    size_t n) {
  using Up = typename std::remove_extent<Tp>::type;
  return std::unique_ptr<Tp>(new Up[n]());
}

template <class Tp, class... Args>
typename unique_if<Tp>::unique_array_known_bound make_unique(Args&&...) =
    delete;

}  // namespace rst

#endif  // RST_CPP14_MEMORY_H_
