#ifndef RST_SQL_DB_H_
#define RST_SQL_DB_H_

#include <memory>
#include <string>

#include "rst/Cpp14/Memory.h"
#include "rst/SQL/Rows.h"
#include "rst/SQL/Stmt.h"
#include "rst/SQL/driver/Conn.h"
#include "rst/SQL/driver/Driver.h"
#include "rst/Util/Int64.h"

#include "rst/Status/Status.h"

namespace rst {

class DB {
 public:
  DB() = default;

  Status Open(
      const std::string& driver_name, const std::string& data_source_name);
  
  Status Close();

  template <class... Args>
  Status Exec(const std::string& query, const Args&... args) {
    Status status;
    auto stmt = conn_->Prepare(query, &status);
    if (!status.ok()) return status;

    status = Bind(stmt.get(), 1, args...);
    if (!status.ok()) return status;

    return stmt->Exec();
  }

  template <class... Args>
  std::unique_ptr<Rows> Query(
      const std::string& query, Status* status, const Args&... args) {
    Status my_status;
    auto stmt = conn_->Prepare(query, &my_status);
    if (!my_status.ok()) {
      *status = std::move(my_status);
      return nullptr;
    }

    my_status = Bind(stmt.get(), 1, args...);
    if (!my_status.ok()) {
      *status = std::move(my_status);
      return nullptr;
    }

    auto rows = stmt->Query(&my_status);
    if (!my_status.ok()) {
      *status = std::move(my_status);
      return nullptr;
    }

    *status = StatusOk();
    return rst::make_unique<Rows>(std::move(rows));
  }

  std::unique_ptr<Stmt> Prepare(const std::string& query, Status* status);

 private:
  Status Bind(driver::Stmt* /* stmt */, const int /* index */) {
    return StatusOk();
  }
  
  template <class T, class... Args>
  Status Bind(
      driver::Stmt* stmt, const int index, const T& val, const Args&... args) {
    Status status = Set(stmt, index, val);
    if (!status.ok()) return status;

    return Bind(stmt, index + 1, args...);
  }

  Status Set(driver::Stmt* stmt, const int index, const Int64& x) {
    return stmt->SetInt64(index, x.value());
  }
  Status Set(driver::Stmt* stmt, const int index, const std::string& x) {
    return stmt->SetString(index, x);
  }
  Status Set(driver::Stmt* stmt, const int index, const double x) {
    return stmt->SetDouble(index, x);
  }

  driver::Driver* driver_ = nullptr;
  std::unique_ptr<driver::Conn> conn_;
};

}  // namespace rst

#endif  // RST_SQL_DB_H_
