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

#include <cstddef>
#include <random>

#include "rst/check/check.h"
#include "rst/random/random_device.h"
#include "rst/type/type.h"

namespace rst {
namespace {

bool IsHexDigit(const char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

bool IsLowerHexDigit(const char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

using Strict = Type<class StrictTag, bool>;

bool IsValidGuidInternal(const std::string_view guid, const Strict strict) {
  if (guid.size() != internal::kGuidLength)
    return false;

  for (size_t i = 0; i < guid.size(); i++) {
    const auto current = guid[i];
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      if (current != '-')
        return false;
    } else {
      if (strict) {
        if (!IsLowerHexDigit(current))
          return false;
      } else {
        if (!IsHexDigit(current))
          return false;
      }
    }
  }

  return true;
}

}  // namespace

namespace internal {

GuidInternal::GuidInternal(const std::array<uint64_t, 2> bytes) {
  const auto ret = std::sprintf(  // NOLINT(runtime/printf)
      buffer_, "%08x-%04x-%04x-%04x-%012llx",
      static_cast<unsigned int>(bytes[0] >> 32),
      static_cast<unsigned int>((bytes[0] >> 16) & 0x0000ffff),
      static_cast<unsigned int>(bytes[0] & 0x0000ffff),
      static_cast<unsigned int>(bytes[1] >> 48),
      bytes[1] & 0x0000ffff'ffffffffULL);
  RST_DCHECK(ret == kGuidLength);
}

}  // namespace internal

Guid::Guid() {
  auto& random_device = GetRandomDevice();

  std::uniform_int_distribution<uint64_t> distribution;
  std::array<uint64_t, 2> bytes = {distribution(random_device),
                                   distribution(random_device)};

  // Clear the version bits and set the version to 4:
  bytes[0] &= 0xffffffff'ffff0fffULL;
  bytes[0] |= 0x00000000'00004000ULL;

  // Set the two most significant bits (bits 6 and 7) of the
  // clock_seq_hi_and_reserved to zero and one, respectively:
  bytes[1] &= 0x3fffffff'ffffffffULL;
  bytes[1] |= 0x80000000'00000000ULL;

  bytes_ = bytes;
}

std::string Guid::AsString() const {
  return std::string(AsStringView().value());
}

internal::GuidInternal Guid::AsStringView() const {
  return internal::GuidInternal(bytes_);
}

// static
bool Guid::IsValid(const std::string_view guid) {
  return IsValidGuidInternal(guid, Strict(false));
}

// static
bool Guid::IsValidOutputString(const std::string_view guid) {
  return IsValidGuidInternal(guid, Strict(true));
}

}  // namespace rst
