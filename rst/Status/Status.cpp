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

#include <utility>

#include "rst/Check/Check.h"

namespace rst {

// Information about the error.
struct Status::ErrorInfo {
  // Error domain.
  const char* error_domain = nullptr;
  // Error code.
  int error_code = 0;
  // Error message.
  std::string error_message;
};

Status::Status() = default;

Status::Status(const char* error_domain, int error_code,
               std::string error_message)
    : error_info_(std::make_unique<ErrorInfo>()) {
  RST_DCHECK(error_domain != nullptr);
  RST_DCHECK(error_code != 0);

  error_info_->error_domain = error_domain;
  error_info_->error_code = error_code;
  error_info_->error_message = std::move(error_message);
}

Status::Status(Status&& rhs) {
  error_info_ = std::move(rhs.error_info_);
  rhs.set_was_checked(true);
}

Status& Status::operator=(Status&& rhs) {
  RST_DCHECK(was_checked_);

  if (this == &rhs)
    return *this;

  set_was_checked(false);
  error_info_ = std::move(rhs.error_info_);

  rhs.set_was_checked(true);

  return *this;
}

Status::~Status() { RST_DCHECK(was_checked_); }

const char* Status::error_domain() const {
  RST_DCHECK(was_checked_);
  RST_DCHECK(error_info_ != nullptr);
  return error_info_->error_domain;
}

int Status::error_code() const {
  RST_DCHECK(was_checked_);
  RST_DCHECK(error_info_ != nullptr);
  return error_info_->error_code;
}

const std::string& Status::error_message() const {
  RST_DCHECK(was_checked_);
  RST_DCHECK(error_info_ != nullptr);
  return error_info_->error_message;
}

StatusAsOutParameter::StatusAsOutParameter(Status* status) : status_(status) {
  RST_DCHECK(status_ != nullptr);
  RST_DCHECK(!status_->was_checked_);
  status_->set_was_checked(true);
}

StatusAsOutParameter::~StatusAsOutParameter() {
  status_->set_was_checked(false);
}

}  // namespace rst
