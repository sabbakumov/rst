#ifndef RST_SQL_DRIVER_ROWS_H_
#define RST_SQL_DRIVER_ROWS_H_

#include <cinttypes>
#include <string>

#include "rst/Status/Status.h"

namespace rst {

namespace driver {

class Rows {
 public:
  virtual ~Rows() {}

  virtual bool Next(Status* status) = 0;
  
  virtual std::string GetString(const int index, Status* status) = 0;
  virtual int64_t GetInt64(const int index, Status* status) = 0;
  virtual double GetDouble(const int index, Status* status) = 0;
};

}  // namespace driver

}  // namespace rst

#endif  // RST_SQL_DRIVER_ROWS_H_
