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

#include "rst/Check/Check.h"

namespace rst {

char ErrorInfoBase::id_ = 0;

ErrorInfoBase::ErrorInfoBase() = default;
ErrorInfoBase::~ErrorInfoBase() = default;

// static
NotNull<const void*> ErrorInfoBase::GetClassID() { return &id_; }

bool ErrorInfoBase::IsA(const NotNull<const void*> class_id) const {
  return class_id == GetClassID();
}

Status::Status() = default;

Status::Status(NotNull<std::unique_ptr<ErrorInfoBase>> error)
    : error_(error.Take()) {
  RST_DCHECK(error_ != nullptr);
}

Status::Status(Status&& other) {
  error_ = std::move(other.error_);
  other.set_was_checked(true);
}

Status& Status::operator=(Status&& rhs) {
  RST_DCHECK(was_checked_);

  if (this == &rhs)
    return *this;

  set_was_checked(false);
  error_ = std::move(rhs.error_);

  rhs.set_was_checked(true);

  return *this;
}

Status::~Status() { RST_DCHECK(was_checked_); }

const ErrorInfoBase& Status::GetError() const {
  RST_DCHECK(was_checked_);
  RST_DCHECK(error_ != nullptr);
  return *error_;
}

StatusAsOutParameter::StatusAsOutParameter(const NotNull<Status*> status)
    : status_(status) {
  RST_DCHECK(!status_->was_checked_);
  status_->set_was_checked(true);
}

StatusAsOutParameter::~StatusAsOutParameter() {
  status_->set_was_checked(false);
}

}  // namespace rst
