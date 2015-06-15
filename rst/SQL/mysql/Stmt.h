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

#ifndef STMT_H_
#define STMT_H_

#include <cinttypes>
#include <memory>
#include <string>

#include "rst/SQL/driver/Rows.h"
#include "rst/SQL/driver/Stmt.h"

#include "rst/Status/Status.h"

#include <cppconn/prepared_statement.h>

namespace mysql_rst_driver {

class Stmt : public rst::driver::Stmt {
 public:
  explicit Stmt(std::unique_ptr<sql::PreparedStatement> prep_statement);
  
  rst::Status Close() override;

  rst::Status Exec() override;

  std::unique_ptr<rst::driver::Rows> Query(rst::Status* status) override;

  rst::Status SetInt64(const int index, const int64_t x) override;
  rst::Status SetString(const int index, const std::string& x) override;
  rst::Status SetDouble(const int index, const double x) override;
 private:
  std::unique_ptr<sql::PreparedStatement> prep_statement_;
};

}  // namespace mysql_rst_driver

#endif  // STMT_H_
