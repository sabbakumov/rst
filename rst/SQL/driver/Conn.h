#ifndef RST_SQL_DRIVER_CONN_H_
#define RST_SQL_DRIVER_CONN_H_

#include <memory>
#include <string>

#include "rst/SQL/driver/Stmt.h"

#include "rst/Status/Status.h"

namespace rst {

namespace driver {

class Conn {
 public:
  virtual ~Conn() {}
  
  virtual std::unique_ptr<Stmt> Prepare(const std::string& query, 
                                        Status* status) = 0;

  virtual Status Close() = 0;
};

}  // namespace driver

}  // namespace rst

#endif  // RST_SQL_DRIVER_CONN_H_
