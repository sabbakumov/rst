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
  status.Ignore();
  
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

