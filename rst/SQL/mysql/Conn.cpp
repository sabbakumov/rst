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

#include "Conn.h"

#include "Stmt.h"
#include "rst/Cpp14/Memory.h"

#include <cppconn/exception.h>

using rst::Status;
using rst::StatusErr;
using rst::StatusOk;

using sql::SQLException;

namespace mysql_rst_driver {

Conn::Conn(std::unique_ptr<sql::Connection> connection) :
    connection_(std::move(connection)) {}

std::unique_ptr<rst::driver::Stmt> Conn::Prepare(
      const std::string& query, Status* status) {
  try {
    std::unique_ptr<sql::PreparedStatement> prep_stmt(
         connection_->prepareStatement(query));
    return rst::make_unique<Stmt>(std::move(prep_stmt));
  } catch (const SQLException& e) {
    *status = StatusErr(
        "MySQL Conn Prepare: " + e.getSQLState());
    return nullptr;
  }
}

rst::Status Conn::Close() {
  return StatusErr("MySQL Conn Close: Unimplemented");
}

}  // namespace mysql_rst_driver

