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

#ifndef RST_SQL_DB_H_
#define RST_SQL_DB_H_

#include <memory>
#include <string>

#include "rst/Cpp14/Memory.h"
#include "rst/SQL/Rows.h"
#include "rst/SQL/Stmt.h"
#include "rst/SQL/driver/Conn.h"
#include "rst/SQL/driver/Driver.h"
#include "rst/Util/Int64.h"

#include "rst/Status/Status.h"

namespace rst {

class DB {
 public:
  DB() = default;

  Status Open(
      const std::string& driver_name, const std::string& data_source_name);
  
  Status Close();

  template <class... Args>
  Status Exec(const std::string& query, const Args&... args) {
    Status status;
    auto stmt = conn_->Prepare(query, &status);
    if (!status.ok()) return status;

    status = Bind(stmt.get(), 1, args...);
    if (!status.ok()) return status;

    return stmt->Exec();
  }

  template <class... Args>
  std::unique_ptr<Rows> Query(
      const std::string& query, Status* status, const Args&... args) {
    Status my_status;
    auto stmt = conn_->Prepare(query, &my_status);
    if (!my_status.ok()) {
      *status = std::move(my_status);
      return nullptr;
    }

    my_status = Bind(stmt.get(), 1, args...);
    if (!my_status.ok()) {
      *status = std::move(my_status);
      return nullptr;
    }

    auto rows = stmt->Query(&my_status);
    if (!my_status.ok()) {
      *status = std::move(my_status);
      return nullptr;
    }

    *status = StatusOk();
    return rst::make_unique<Rows>(std::move(rows));
  }

  std::unique_ptr<Stmt> Prepare(const std::string& query, Status* status);

 private:
  Status Bind(driver::Stmt* /* stmt */, const int /* index */) {
    return StatusOk();
  }
  
  template <class T, class... Args>
  Status Bind(
      driver::Stmt* stmt, const int index, const T& val, const Args&... args) {
    Status status = Set(stmt, index, val);
    if (!status.ok()) return status;

    return Bind(stmt, index + 1, args...);
  }

  Status Set(driver::Stmt* stmt, const int index, const Int64& x) {
    return stmt->SetInt64(index, x.value());
  }
  Status Set(driver::Stmt* stmt, const int index, const std::string& x) {
    return stmt->SetString(index, x);
  }
  Status Set(driver::Stmt* stmt, const int index, const double x) {
    return stmt->SetDouble(index, x);
  }

  driver::Driver* driver_ = nullptr;
  std::unique_ptr<driver::Conn> conn_;
};

}  // namespace rst

#endif  // RST_SQL_DB_H_
