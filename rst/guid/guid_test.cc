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

#include "rst/guid/guid.h"

#include <array>
#include <cstdint>
#include <limits>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace rst {
namespace {

bool IsGuidv4(const std::string_view guid) {
  return Guid::IsValid(guid) && guid[14] == '4' &&
         (guid[19] == '8' || guid[19] == '9' || guid[19] == 'A' ||
          guid[19] == 'a' || guid[19] == 'B' || guid[19] == 'b');
}

char ToLowerASCII(const char c) {
  return (c >= 'A' && c <= 'Z') ? static_cast<char>(c + ('a' - 'A')) : c;
}

char ToUpperASCII(const char c) {
  return (c >= 'a' && c <= 'z') ? static_cast<char>(c + ('A' - 'a')) : c;
}

std::string ToLowerASCII(const std::string_view str) {
  std::string ret;
  ret.reserve(str.size());
  for (const auto c : str)
    ret.push_back(ToLowerASCII(c));
  return ret;
}

std::string ToUpperASCII(const std::string_view str) {
  std::string ret;
  ret.reserve(str.size());
  for (const auto c : str)
    ret.push_back(ToUpperASCII(c));
  return ret;
}

void CheckValid(const std::string_view guid) {
    EXPECT_TRUE(Guid::IsValid(guid));
    EXPECT_TRUE(Guid::IsValidOutputString(guid));
    EXPECT_TRUE(Guid::IsValid(ToLowerASCII(guid)));
    EXPECT_TRUE(Guid::IsValid(ToUpperASCII(guid)));
}

}  // namespace

TEST(GUID, GUIDGeneratesAllZeroes) {
  const std::array<uint64_t, 2> bytes = {0, 0};
  const auto guid = internal::GuidInternal(bytes);
  EXPECT_EQ(guid.value(), "00000000-0000-0000-0000-000000000000");
}

TEST(GUID, GUIDGeneratesCorrectly) {
  const std::array<uint64_t, 2> bytes = {uint64_t{0x0123456789ABCDEF},
                                         uint64_t{0xFEDCBA9876543210}};
  const auto guid = internal::GuidInternal(bytes);
  EXPECT_EQ(guid.value(), "01234567-89ab-cdef-fedc-ba9876543210");
}

TEST(GUID, GUIDCorrectlyFormatted) {
  for (auto i = 0; i < 10; i++) {
    const Guid guid;
    CheckValid(guid.AsString());
    CheckValid(guid.AsStringView().value());
  }
}

TEST(GUID, GUIDBasicUniqueness) {
  for (auto i = 0; i < 10; i++) {
    const auto guid1 = Guid().AsString();
    const auto guid2 = Guid().AsString();
    EXPECT_EQ(guid1.size(), 36U);
    EXPECT_EQ(guid2.size(), 36U);
    EXPECT_NE(guid1, guid2);
    EXPECT_TRUE(IsGuidv4(guid1));
    EXPECT_TRUE(IsGuidv4(guid2));
  }
}

}  // namespace rst
