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

#include <memory>

#include "DB.h"
#include "DriverManager.h"
#include "mysql/Driver.h"

#include "rst/Status/Status.h"
#include "rst/Cpp14/Memory.h"

using namespace rst;

int main() {
  DB db;
  
  Status status;
  
  auto mysql_driver = make_unique<mysql_rst_driver::Driver>();
  status = mysql_driver->Init();
  if (!status.ok()) {
    std::cout << status.ToString() << std::endl;
  }
  status = DriverManager::Register("mysql", std::move(mysql_driver));
  if (!status.ok()) {
    std::cout << status.ToString() << std::endl;
  }

  status = db.Open("mysql", "root:1234@127.0.0.1/rst");
  if (!status.ok()) {
    std::cout << status.ToString() << std::endl;
  }

  status = db.Exec(
  "CREATE TABLE IF NOT EXISTS users(name TEXT, age INTEGER)"
  );
  if (!status.ok()) {
    std::cout << status.ToString() << std::endl;
  }
  status = db.Exec(
  "INSERT INTO users(name, age) VALUES (?, ?)", "gopher", Int64(27));
  if (!status.ok()) {
    std::cout << status.ToString() << std::endl;
  }
  
  auto rows = db.Query(
      "SELECT name, 1, 2, 3 FROM users WHERE age = ?", &status, Int64(27));
  if (!status.ok()) {
    std::cout << status.ToString() << std::endl;
  }
  
  std::string name;
  std::string s1, s2, s3;
  while (rows->Next()) {
    status = rows->Scan(&name, &s1, &s2, &s3);
    if (!status.ok()) {
      std::cout << status.ToString() << std::endl;
    }
    std::cout << "Name is: " << name << " " << s1 << s2 << s3 << std::endl;
  }
  if (!rows->Err().ok()) {
    std::cout << status.ToString() << std::endl;
  }

  return 0;
}

