#ifndef CONN_H_
#define CONN_H_

#include <memory>
#include <string>

#include "rst/SQL/driver/Conn.h"
#include "rst/SQL/driver/Stmt.h"

#include "rst/Status/Status.h"

#include <mysql_connection.h>

namespace mysql_rst_driver {

class Conn : public rst::driver::Conn {
 public:
  explicit Conn(std::unique_ptr<sql::Connection> connection);
  
  std::unique_ptr<rst::driver::Stmt> Prepare(
      const std::string& query, rst::Status* status) override;

  rst::Status Close() override;
 
 private:
  std::unique_ptr<sql::Connection> connection_;
};

}  // namespace mysql_rst_driver

#endif  // CONN_H_
