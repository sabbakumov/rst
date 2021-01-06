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

#ifndef RST_GUID_GUID_H_
#define RST_GUID_GUID_H_

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

// Chromium-like GUID helpers.
namespace rst {

// Generates a 128-bit random GUID in the form of version 4 as described in RFC
// 4122, section 4.4. The format of GUID version 4 must be
// xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx, where y is one of [8, 9, A, B]. The
// hexadecimal values "a" through "f" are output as lower case characters.
std::string GenerateGuid();

// Returns true if the input string conforms to the version 4 GUID format.
// Note that this does not check if the hexadecimal values "a" through "f" are
// in lower case characters, as Version 4 RFC says onput they're case
// insensitive. (Use IsValidGuidOutputString() for checking if the given string
// is valid output string)
bool IsValidGuid(std::string_view guid);

// Returns true if the input string is valid version 4 GUID output string.
// This also checks if the hexadecimal values "a" through "f" are in lower case
// characters.
bool IsValidGuidOutputString(std::string_view guid);

namespace internal {

std::string RandomDataToGuidString(std::array<uint64_t, 2> bytes);

}  // namespace internal
}  // namespace rst

#endif  // RST_GUID_GUID_H_
