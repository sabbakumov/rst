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

#include "rst/Noncopyable/Noncopyable.h"
#include "rst/Status/Status.h"

#ifndef RST_NODISCARD
#if __cplusplus > 201402L && __has_cpp_attribute(nodiscard)
#define RST_NODISCARD [[nodiscard]]
#elif !__cplusplus
#define RST_NODISCARD
#elif __has_cpp_attribute(clang::warn_unused_result)
#define RST_NODISCARD [[clang::warn_unused_result]]
#else
#define RST_NODISCARD
#endif
#endif  // RST_NODISCARD

namespace rst {

class StatusAsOutParameter;

// A Google-like Status class for error handling.
class RST_NODISCARD Status : public NonCopyable {
 public:
  static Status OK() { return Status(); }

  // Sets the object not checked by default and to be the error object with
  // error domain and code. The domain should not be nullptr and error_code
  // should not be 0.
  Status(const char* error_domain, int error_code, std::string error_message);

  // Sets the object not checked by default and moves rhs content.
  Status(Status&& rhs);

  ~Status();

  // Sets the object not checked by default and moves rhs content.
  Status& operator=(Status&& rhs);

  // Sets the object to be checked and returns whether the object is OK object.
  bool ok() const {
    set_was_checked(true);
    return error_info_ == nullptr;
  }

  const char* error_domain() const;
  int error_code() const;
  const std::string& error_message() const;

  // Sets the object to be checked.
  void Ignore() const { set_was_checked(true); }

 private:
  friend class StatusAsOutParameter;

  template <class T>
  friend class StatusOr;

  // Sets the object not checked by default and to be OK.
  Status();

#ifndef NDEBUG
  void set_was_checked(bool was_checked) const { was_checked_ = was_checked; }
#else   // NDEBUG
  void set_was_checked(bool) const {}
#endif  // NDEBUG

  // Information about the error. nullptr if the object is OK.
  struct ErrorInfo;
  std::unique_ptr<ErrorInfo> error_info_;

#ifndef NDEBUG
  // Whether the object was checked.
  mutable bool was_checked_ = false;
#endif  // NDEBUG
};

// A helper for Status used as out-parameters.
class StatusAsOutParameter : public NonCopyable {
 public:
  explicit StatusAsOutParameter(Status* status);
  ~StatusAsOutParameter();

 private:
  Status* status_ = nullptr;
};

}  // namespace rst

#endif  // RST_STATUS_STATUS_H_
