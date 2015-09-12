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
#include <utility>

namespace rst {

class Status {
 public:
  Status();

  Status(int code, std::string message);

  Status(Status&& rhs);

  Status& operator=(Status&& rhs);
  bool operator==(const Status& rhs) const {
    return code_ == rhs.code_;
  }

  ~Status();

  bool ok() {
    was_checked_ = true;
    return code_ == 0;
  }

  const std::string& ToString() const { return message_; }
  int code() const { return code_; }

  void Ignore() { was_checked_ = true; }

 private:
  Status(const Status&) = delete;
  Status& operator=(const Status&) = delete;

  bool was_checked_;
  int code_;
  std::string message_;
};

inline Status StatusOk() { return Status(); }

inline Status StatusErr(std::string message) {
  return Status(-1, std::move(message));
}

inline Status StatusErrWithCode(int code, std::string message) {
  return Status(code, std::move(message));
}

}  // namespace rst

#endif  // RST_STATUS_STATUS_H_
