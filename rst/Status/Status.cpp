// Copyright (c) 2016, Sergey Abbakumov
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

#include "rst/Cpp14/Memory.h"

using std::string;

namespace rst {

// Information about the error.
struct Status::ErrorInfo {
  // Error code.
  int error_code = 0;
  // Error message.
  std::string error_message;
};

Status::Status() : was_checked_(true) {}

Status::Status(int error_code, string error_message)
    : was_checked_(false), error_info_(rst::make_unique<ErrorInfo>()) {
  if (error_code == 0)
    std::abort();

  error_info_->error_code = error_code;
  error_info_->error_message = std::move(error_message);
}

Status::Status(Status&& rhs) : was_checked_(false) {
  error_info_ = std::move(rhs.error_info_);
  rhs.was_checked_ = true;
}

Status& Status::operator=(Status&& rhs) {
  if (!was_checked_)
    std::abort();

  if (this == &rhs)
    return *this;

  was_checked_ = false;
  error_info_ = std::move(rhs.error_info_);

  rhs.was_checked_ = true;

  return *this;
}

Status::~Status() {
  if (!was_checked_)
    std::abort();
}

bool Status::operator==(const Status& rhs) const {
  return (error_info_ == rhs.error_info_) ||
         (error_code() == rhs.error_code() &&
          error_message() == error_message());
}

const std::string& Status::error_message() const {
  if (error_info_ == nullptr)
    std::abort();
  return error_info_->error_message;
}

int Status::error_code() const {
  if (error_info_ == nullptr)
    std::abort();
  return error_info_->error_code;
}

}  // namespace rst
