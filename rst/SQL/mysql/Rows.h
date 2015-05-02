#ifndef ROWS_H_
#define ROWS_H_

#include <cinttypes>
#include <memory>
#include <string>

#include "rst/SQL/driver/Rows.h"
#include "rst/Status/Status.h"

#include <cppconn/resultset.h>

namespace mysql_rst_driver {

class Rows : public rst::driver::Rows {
 public:
  explicit Rows(std::unique_ptr<sql::ResultSet> result_set);
  bool Next(rst::Status* status) override;
  
  std::string GetString(const int index, rst::Status* status) override;
  int64_t GetInt64(const int index, rst::Status* status) override;
  double GetDouble(const int index, rst::Status* status) override;

 private:
  std::unique_ptr<sql::ResultSet> result_set_;  
};

}  // namespace mysql_rst_driver

#endif  // ROWS_H_
