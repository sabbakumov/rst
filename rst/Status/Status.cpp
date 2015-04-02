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

#include "rst/Status/Status.h"

#include <cassert>

namespace rst {

Status::Status() : was_checked_(false), code_(Code::kOk) {}

Status::Status(const Code code, const std::string& message)
    : was_checked_(false), code_(code), message_(message) {}

Status::Status(Status&& rhs) {
  was_checked_ = false;
  code_ = rhs.code_;
  message_ = std::move(rhs.message_);

  rhs.was_checked_ = true;
}

Status& Status::operator=(Status&& rhs) {
  assert(was_checked_);
  
  if (this != &rhs) {
    was_checked_ = false;
    code_ = rhs.code_;
    message_ = std::move(rhs.message_);

    rhs.was_checked_ = true;
  }
  
  return *this;
}

Status::~Status() {
  assert(was_checked_);
}

#define CASE(code) case Code::code: return #code
static std::string CodeToString(const Code code) {
  switch (code) {
    CASE(kOk);
    CASE(kCanceled);
    CASE(kInvalidArgument);
    CASE(kDeadlineExceeded);
    CASE(kNotFound);
    CASE(kAlreadyExists);
    CASE(kPermissionDenied);
    CASE(kResourceExhausted);
    CASE(kFailedPrecondition);
    CASE(kAborted);
    CASE(kOutOfRange);
    CASE(kUnimplemented);
    CASE(kInternal);
    CASE(kUnavailable);
    CASE(kDataLoss);
    default: {
      std::string result = "Error #";
      result.append(std::to_string(static_cast<int>(code)));
      
      return result;
    }
  }
}
#undef CASE

std::string Status::ToString() const {
  std::string result = CodeToString(code_);
  if (!message_.empty()) {
    result.append(": ");
    result.append(message_);
  }

  return result;
}

}  // namespace rst

