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

#ifndef RST_SQL_ROWS_H_
#define RST_SQL_ROWS_H_

#include <memory>

#include "rst/SQL/driver/Rows.h"
#include "rst/Status/Status.h"
#include "rst/Util/Int64.h"

namespace rst {

class Rows {
 public:
  explicit Rows(std::unique_ptr<driver::Rows> rows);
  
  bool Next();
  Status& Err() { return status_; }

  template <class... Args>
  Status Scan(const Args&... args) {
    return Unbind(1, args...);
  }

 private:
  Status Unbind(const int /* index */) {
    return StatusOk();
  }
  
  template <class T, class... Args>
  Status Unbind(const int index, T* val, const Args&... args) {
    Status status = Get(index, val);
    if (!status.ok()) return status;

    return Unbind(index + 1, args...);
  }

  Status Get(const int index, Int64* x) {
    Status status;
    int64_t value = rows_->GetInt64(index, &status);
    if (!status.ok()) return status;

    *x = Int64(value);
    return StatusOk();
  }

  Status Get(const int index, std::string* x) {
    Status status;
    *x = rows_->GetString(index, &status);
    return status;
  }

  Status Get(const int index, double* x) {
    Status status;
    *x = rows_->GetDouble(index, &status);
    return status;
  }
  
  std::unique_ptr<driver::Rows> rows_;
  
  Status status_;
};

}  // namespace rst

#endif  // RST_SQL_ROWS_H_
