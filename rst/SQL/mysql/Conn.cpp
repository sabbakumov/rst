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

