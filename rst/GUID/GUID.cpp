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

#include "rst/GUID/GUID.h"

#include <cstddef>
#include <random>

#include "rst/Check/Check.h"
#include "rst/NoDestructor/NoDestructor.h"

namespace rst {
namespace {

constexpr size_t kGUIDLength = 36;

bool IsHexDigit(const char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
         (c >= 'A' && c <= 'F');
}

bool IsLowerHexDigit(const char c) {
  return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
}

bool IsValidGUIDInternal(const std::string_view guid, const bool strict) {
  if (guid.size() != kGUIDLength)
    return false;

  for (size_t i = 0; i < guid.size(); i++) {
    const auto current = guid[i];
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      if (current != '-')
        return false;
    } else {
      if (strict ? !IsLowerHexDigit(current) : !IsHexDigit(current))
        return false;
    }
  }

  return true;
}

}  // namespace

std::string GenerateGUID() {
  static rst::NoDestructor<std::random_device> random_device;

  std::uniform_int_distribution<uint64_t> distribution;
  uint64_t sixteen_bytes[2] = {distribution(*random_device),
                               distribution(*random_device)};

  // Clear the version bits and set the version to 4:
  sixteen_bytes[0] &= 0xffffffff'ffff0fffULL;
  sixteen_bytes[0] |= 0x00000000'00004000ULL;

  // Set the two most significant bits (bits 6 and 7) of the
  // clock_seq_hi_and_reserved to zero and one, respectively:
  sixteen_bytes[1] &= 0x3fffffff'ffffffffULL;
  sixteen_bytes[1] |= 0x80000000'00000000ULL;

  return internal::RandomDataToGUIDString(sixteen_bytes);
}

bool IsValidGUID(const std::string_view guid) {
  return IsValidGUIDInternal(guid, false);
}

bool IsValidGUIDOutputString(const std::string_view guid) {
  return IsValidGUIDInternal(guid, true);
}

namespace internal {

std::string RandomDataToGUIDString(const uint64_t (&bytes)[2]) {
  char buffer[kGUIDLength + 1];
  const auto ret =
      std::sprintf(buffer, "%08x-%04x-%04x-%04x-%012llx",
                   static_cast<unsigned int>(bytes[0] >> 32),
                   static_cast<unsigned int>((bytes[0] >> 16) & 0x0000ffff),
                   static_cast<unsigned int>(bytes[0] & 0x0000ffff),
                   static_cast<unsigned int>(bytes[1] >> 48),
                   bytes[1] & 0x0000ffff'ffffffffULL);
  RST_DCHECK(ret == kGUIDLength);
  return buffer;
}

}  // namespace internal
}  // namespace rst
