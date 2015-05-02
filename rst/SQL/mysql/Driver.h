#ifndef DRIVER_H_
#define DRIVER_H_

#include <memory>
#include <string>

#include "rst/SQL/driver/Conn.h"
#include "rst/SQL/driver/Driver.h"
#include "rst/Status/Status.h"

#include <mysql_driver.h>

namespace mysql_rst_driver {

class Driver : public rst::driver::Driver {
 public:
   rst::Status Init();
  
  std::unique_ptr<rst::driver::Conn> Open(
      const std::string& name, rst::Status* status) override;
 private:
  sql::mysql::MySQL_Driver* driver_ = nullptr;
};

}  // namespace mysql_rst_driver

#endif  // DRIVER_H_
