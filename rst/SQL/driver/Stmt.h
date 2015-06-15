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

#ifndef RST_SQL_DRIVER_STMT_H_
#define RST_SQL_DRIVER_STMT_H_

#include <cinttypes>
#include <memory>
#include <string>

#include "rst/SQL/driver/Rows.h"

#include "rst/Status/Status.h"

namespace rst {

namespace driver {

class Stmt {
public:
  virtual ~Stmt() {}

  virtual Status Close() = 0;

  virtual Status Exec() = 0;

  virtual std::unique_ptr<Rows> Query(Status* status) = 0;

  virtual Status SetInt64(const int index, const int64_t x) = 0;
  virtual Status SetString(const int index, const std::string& x) = 0;
  virtual Status SetDouble(const int index, const double x) = 0;
};

}  // namespace driver

}  // namespace rst

#endif  // RST_SQL_DRIVER_STMT_H_
