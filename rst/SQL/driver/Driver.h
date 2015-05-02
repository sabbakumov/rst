#ifndef RST_SQL_DRIVER_DRIVER_H_
#define RST_SQL_DRIVER_DRIVER_H_

#include <memory>
#include <string>

#include "rst/SQL/driver/Conn.h"
#include "rst/Status/Status.h"

namespace rst {

namespace driver {

enum class Errors {
  kBadConn,
};

class Driver {
 public:
  virtual ~Driver() {}
  
  virtual std::unique_ptr<Conn> Open(const std::string& name,
                                     Status* status) = 0;
};

}  // namespace driver

}  // namespace rst

#endif  // RST_SQL_DRIVER_DRIVER_H_
