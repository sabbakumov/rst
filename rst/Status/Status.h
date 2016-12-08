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

#ifndef RST_STATUS_STATUS_H_
#define RST_STATUS_STATUS_H_

#include <memory>
#include <string>
#include <utility>

namespace rst {

// Information about the error
struct ErrorInfo {
  // Error code
  int error_code = 0;
  // Error message
  std::string error_message;
};

// A Google-like Status class for error handling
class Status {
 public:
  // Sets the object checked by default and to be OK
  Status();

  // Sets the object not checked by default and to be error object with error
  // code. If the error_code is 0, aborts
  Status(int error_code, std::string error_message);

  // Sets the object not checked by default and moves rhs content
  Status(Status&& rhs);
  
  Status(const Status&) = delete;

  // Sets the object not checked by default and moves rhs content. If the
  // object has not been checked, aborts
  Status& operator=(Status&& rhs);
  bool operator==(const Status& rhs) const {
    return (error_info_ == rhs.error_info_) ||
           (error_code() == rhs.error_code() &&
            error_message() == error_message());
  }

  Status& operator=(const Status&) = delete;

  // If the object has not been checked, aborts
  ~Status();

  // Sets the object to be checked and returns whether the object is OK object
  bool ok() {
    was_checked_ = true;
    return error_info_ == nullptr;
  }

  const std::string& error_message() const {
    if (error_info_ == nullptr) {
      return empty_string_;
    }
    return error_info_->error_message;
  }

  int error_code() const {
    if (error_info_ == nullptr) {
      return 0;
    }
    return error_info_->error_code;
  }

  // Sets the object to be checked
  void Ignore() { was_checked_ = true; }

 private:
  // Whether the object was checked
  bool was_checked_;

  // Information about the error. nullptr if the object is OK
  std::unique_ptr<ErrorInfo> error_info_;

  // The empty string in case of calling error_message() on OK object
  static const std::string empty_string_;
};

inline Status StatusOk() { return Status(); }

inline Status StatusErr(std::string error_message) {
  return Status(-1, std::move(error_message));
}

inline Status StatusErrWithCode(int error_code, std::string error_message) {
  return Status(error_code, std::move(error_message));
}

}  // namespace rst

#endif  // RST_STATUS_STATUS_H_
