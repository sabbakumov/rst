#ifndef RST_SQL_DRIVER_STMT_H_
#define RST_SQL_DRIVER_STMT_H_

#include <cinttypes>
#include <memory>
#include <string>

#include "rst/SQL/driver/Rows.h"

#include "rst/Status/Status.h"

namespace rst {

namespace driver {

class Stmt {
public:
  virtual ~Stmt() {}

  virtual Status Close() = 0;

  virtual Status Exec() = 0;

  virtual std::unique_ptr<Rows> Query(Status* status) = 0;

  virtual Status SetInt64(const int index, const int64_t x) = 0;
  virtual Status SetString(const int index, const std::string& x) = 0;
  virtual Status SetDouble(const int index, const double x) = 0;
};

}  // namespace driver

}  // namespace rst

#endif  // RST_SQL_DRIVER_STMT_H_
