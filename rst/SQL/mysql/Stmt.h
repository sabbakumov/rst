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
