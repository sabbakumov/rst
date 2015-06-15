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

#include "Rows.h"

#include <cppconn/exception.h>

using rst::Status;
using rst::StatusErr;
using rst::StatusOk;
using sql::SQLException;

namespace mysql_rst_driver {

Rows::Rows(std::unique_ptr<sql::ResultSet> result_set) :
    result_set_(std::move(result_set)) {}

bool Rows::Next(Status* status) {
  try {
    bool has_next = result_set_->next();
    *status = StatusOk();
    return has_next;
  } catch (const SQLException& e) {
    *status = StatusErr("MySQL Rows Next: " + e.getSQLState());
    return false;
  }
}

std::string Rows::GetString(const int index, Status* status) {
  try {
    std::string value = result_set_->getString(static_cast<uint32_t>(index));
    *status = StatusOk();
    return value;
  } catch (const SQLException& e) {
    *status = StatusErr("MySQL Rows GetString: " + e.getSQLState());
    return std::string();
  }
}

int64_t Rows::GetInt64(const int index, Status* status) {
  try {
    int64_t value = result_set_->getInt64(static_cast<uint32_t>(index));
    *status = StatusOk();
    return value;
  } catch (const SQLException& e) {
    *status = StatusErr("MySQL Rows GetInt64: " + e.getSQLState());
    return -1;
  }
}

double Rows::GetDouble(const int index, Status* status) {
  try {
    double value = static_cast<double>(
        result_set_->getDouble(static_cast<uint32_t>(index)));
    *status = StatusOk();
    return value;
  } catch (const SQLException& e) {
    *status = StatusErr("MySQL Rows GetDouble: " + e.getSQLState());
    return -1.0;
  }
}

}  // namespace mysql_rst_driver

