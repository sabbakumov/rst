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

