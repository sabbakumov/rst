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

#include "Driver.h"

#include <sstream>

#include "Conn.h"
#include "rst/Cpp14/Memory.h"

#include <cppconn/exception.h>
#include <mysql_connection.h>

using rst::Status;
using rst::StatusErr;
using rst::StatusOk;
using sql::mysql::get_mysql_driver_instance;
using sql::SQLException;

namespace mysql_rst_driver {

Status Driver::Init() {
  try {
    driver_ = get_mysql_driver_instance();
  } catch (const SQLException& e) {
    return StatusErr("MySQL Driver Init: " + e.getSQLState());
  }

  return StatusOk();
}

std::unique_ptr<rst::driver::Conn> Driver::Open(
    const std::string& name, Status* status) {
  std::istringstream in(name);
  
  std::string user;
  std::string password;
  std::string server;
  std::string schema;

  if (!std::getline(in, user, ':') || !std::getline(in, password, '@') ||
      !std::getline(in, server, '/') || !std::getline(in, schema)) {
    *status = StatusErr("MySQL Driver Open: Invalid name");
    return nullptr;
  }
  
  try {
    std::unique_ptr<sql::Connection> con(
        driver_->connect(server, user, password));
    con->setSchema(schema);
    return rst::make_unique<Conn>(std::move(con));
  } catch (const SQLException& e) {
    *status = StatusErr("MySQL Driver Open: " + e.getSQLState());
    return nullptr;
  }
}

}  // namespace mysql_rst_driver

