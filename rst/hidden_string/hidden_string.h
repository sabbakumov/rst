// Copyright (c) 2017, Sergey Abbakumov
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

#ifndef RST_HIDDEN_STRING_HIDDEN_STRING_H_
#define RST_HIDDEN_STRING_HIDDEN_STRING_H_

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"
#include "rst/stl/resize_uninitialized.h"

// Compile time encrypted string modified implementation originally taken from
// https://stackoverflow.com/questions/7270473/compile-time-string-encryption.
//
// Strings encrypted with this method are not visible directly in the binary.
//
// Example:
//
//   RST_HIDDEN_STRING(kHidden, "Not visible");
//   RST_DCHECK(kHidden.Decrypt() == "Not visible");
//
#define RST_HIDDEN_STRING(var, str)                                 \
  static constexpr ::rst::internal::HiddenString<                   \
      ::rst::internal::ConstructIndexList<sizeof(str) - 1>::Result> \
      var(str)

namespace rst {
namespace internal {

constexpr uint64_t GetSeed() {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdate-time"
  return (__TIME__[7] - '0') * uint64_t{1} +
         (__TIME__[6] - '0') * uint64_t{10} +
         (__TIME__[4] - '0') * uint64_t{60} +
         (__TIME__[3] - '0') * uint64_t{600} +
         (__TIME__[1] - '0') * uint64_t{3600} +
         (__TIME__[0] - '0') * uint64_t{36000};
#pragma clang diagnostic pop
}

constexpr uint64_t LinearCongruentGenerator(const int rounds) {
  auto result = GetSeed();

  for (auto i = 0; i < rounds; i++)
    result = result * uint64_t{1664525} + uint64_t{1013904223};

  return result;
}

constexpr uint64_t Random() { return LinearCongruentGenerator(10); }

constexpr uint64_t RandomNumber(const uint64_t min, const uint64_t max) {
  return min + Random() % (max - min + 1);
}

template <size_t... Pack>
struct IndexList {};

template <class IndexList, size_t Right>
struct Append;

template <size_t... Left, size_t Right>
struct Append<IndexList<Left...>, Right> {
  using Result = IndexList<Left..., Right>;
};

template <size_t N>
struct ConstructIndexList {
  using Result = typename Append<typename ConstructIndexList<N - 1>::Result,
                                 N - 1>::Result;
};

template <>
struct ConstructIndexList<0> {
  using Result = IndexList<>;
};

constexpr auto kXorKey =
    static_cast<char>(RandomNumber(std::numeric_limits<unsigned char>::min(),
                                   std::numeric_limits<unsigned char>::max()));

constexpr char ProcessCharacter(const char c, const size_t i) {
  return c ^ static_cast<char>(static_cast<size_t>(kXorKey) + i);
}

template <class IndexList>
class HiddenString;

template <size_t... Index>
class HiddenString<IndexList<Index...>> {
 public:
  explicit constexpr HiddenString(const NotNull<const char*> str)
      : str_{ProcessCharacter(str[Index], Index)...} {}

  std::string Decrypt() const {
    std::string result;
    StringResizeUninitialized(&result, sizeof str_);

    for (size_t i = 0; i < sizeof str_; i++)
      result[i] = ProcessCharacter(str_[i], i);

    return result;
  }

 private:
  const char str_[sizeof...(Index)];

  RST_DISALLOW_COPY_AND_ASSIGN(HiddenString);
};

}  // namespace internal
}  // namespace rst

#endif  // RST_HIDDEN_STRING_HIDDEN_STRING_H_
