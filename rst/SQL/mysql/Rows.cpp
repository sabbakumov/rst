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

