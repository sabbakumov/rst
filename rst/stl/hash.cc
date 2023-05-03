// Copyright (c) 2021, Sergey Abbakumov
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

#include "rst/stl/hash.h"

#include <cstdint>

namespace rst {
namespace {

uint32_t Rotl32(const uint32_t x, const uint32_t r) {
  return (x << r) | (x >> (32 - r));
}

[[maybe_unused]] uint32_t HashCombineSizeT(uint32_t h, uint32_t k) {
  static constexpr uint32_t kC1 = 0xcc9e2d51;
  static constexpr uint32_t kC2 = 0x1b873593;

  k *= kC1;
  k = Rotl32(k, 15);
  k *= kC2;

  h ^= k;
  h = Rotl32(h, 13);
  h = h * 5 + 0xe6546b64;

  return h;
}

[[maybe_unused]] uint64_t HashCombineSizeT(uint64_t h, uint64_t k) {
  static constexpr uint64_t kM = 0xc6a4a7935bd1e995ULL;
  static constexpr int kR = 47;

  k *= kM;
  k ^= k >> kR;
  k *= kM;

  h ^= k;
  h *= kM;

  h += 0xe6546b64;

  return h;
}

}  // namespace

size_t HashCombine(const std::initializer_list<internal::Hash> hashes) {
  auto it = hashes.begin();
  const auto end = hashes.end();
  if (it == end)
    return 0;

  auto result = it->hash();
  ++it;

  for (; it != end; ++it)
    result = HashCombineSizeT(result, it->hash());

  return result;
}

}  // namespace rst
