// Copyright (c) 2018, Sergey Abbakumov
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

#ifndef RST_STL_ALGORITHM_H_
#define RST_STL_ALGORITHM_H_

#include <algorithm>
#include <iterator>
#include <utility>

// Container-based versions of algorithmic functions within the C++ standard
// library.

namespace rst {

template <class C>
void c_sort(C& c) {  // NOLINT(runtime/references)
  std::sort(std::begin(c), std::end(c));
}

template <class C, class Compare>
void c_sort(C& c, Compare&& comp) {  // NOLINT(runtime/references)
  std::sort(std::begin(c), std::end(c), std::forward<Compare>(comp));
}

template <class C>
void c_stable_sort(C& c) {  // NOLINT(runtime/references)
  std::stable_sort(std::begin(c), std::end(c));
}

template <class C, class Compare>
void c_stable_sort(C& c, Compare&& comp) {  // NOLINT(runtime/references)
  std::stable_sort(std::begin(c), std::end(c), std::forward<Compare>(comp));
}

template <class C, class UnaryPredicate>
auto c_find_if(C& c, UnaryPredicate&& pred) {  // NOLINT(runtime/references)
  return std::find_if(std::begin(c), std::end(c),
                      std::forward<UnaryPredicate>(pred));
}

template <class C, class Compare>
void c_push_heap(C& c, Compare&& comp) {  // NOLINT(runtime/references)
  std::push_heap(std::begin(c), std::end(c), std::forward<Compare>(comp));
}

template <class C, class Compare>
void c_pop_heap(C& c, Compare&& comp) {  // NOLINT(runtime/references)
  std::pop_heap(std::begin(c), std::end(c), std::forward<Compare>(comp));
}

}  // namespace rst

#endif  // RST_STL_ALGORITHM_H_
