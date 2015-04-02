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

#ifndef RST_STATUS_STATUS_H_
#define RST_STATUS_STATUS_H_

#include <cassert>
#include <string>

namespace rst {

enum class Code {
  kOk = 0,
  kCanceled,
  kUnknown,
  kInvalidArgument,
  kDeadlineExceeded,
  kNotFound,
  kAlreadyExists,
  kPermissionDenied,
  kResourceExhausted,
  kFailedPrecondition,
  kAborted,
  kOutOfRange,
  kUnimplemented,
  kInternal,
  kUnavailable,
  kDataLoss,
};


class Status {
 public:
  Status();

  Status(const Code code, const std::string& message);

  Status(Status&& rhs);

  Status& operator=(Status&& rhs);

  ~Status();

  bool ok() const {
    was_checked_ = true;
    return code_ == Code::kOk;
  }

  const std::string& error_message() const { return message_; }

  Code error_code() const { return code_; }

  std::string ToString() const;

  void Ignore() const { was_checked_ = true; }

 private:
  Status(const Status&) = delete;
  Status& operator=(const Status&) = delete;

  mutable bool was_checked_;
  Code code_;
  std::string message_;
};

inline Status StatusOk() { return Status(); }

inline Status StatusAborted(const std::string& message) {
  return Status(Code::kAborted, message);
}

inline Status StatusCanceled(const std::string& message) {
  return Status(Code::kCanceled, message);
}

inline Status StatusDataLoss(const std::string& message) {
  return Status(Code::kDataLoss, message);
}

inline Status StatusDeadlineExceeded(const std::string& message) {
  return Status(Code::kDeadlineExceeded, message);
}

inline Status StatusInternalError(const std::string& message) {
  return Status(Code::kInternal, message);
}

inline Status StatusInvalidArgument(const std::string& message) {
  return Status(Code::kInvalidArgument, message);
}

inline Status StatusOutOfRange(const std::string& message) {
  return Status(Code::kOutOfRange, message);
}

inline Status StatusPermissionDenied(const std::string& message) {
  return Status(Code::kPermissionDenied, message);
}

inline Status StatusUnimplemented(const std::string& message) {
  return Status(Code::kUnimplemented, message);
}

inline Status StatusUnknown(const std::string& message) {
  return Status(Code::kUnknown, message);
}

inline Status StatusResourceExhausted(const std::string& message) {
  return Status(Code::kResourceExhausted, message);
}

inline Status StatusFailedPrecondition(const std::string& message) {
  return Status(Code::kFailedPrecondition, message);
}

}  // namespace rst

#endif  // RST_STATUS_STATUS_H_
