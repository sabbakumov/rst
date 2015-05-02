#ifndef RST_SQL_DRIVERMANAGER_H_
#define RST_SQL_DRIVERMANAGER_H_

#include <map>
#include <memory>
#include <string>

#include "rst/SQL/driver/Driver.h"

#include "rst/Status/Status.h"

namespace rst {

class DriverManager {
 public:
  static Status Register(
      const std::string& name, std::unique_ptr<driver::Driver> driver);
  static std::map<std::string, std::unique_ptr<driver::Driver>>& Drivers();
};

}  // namespace rst

#endif  // RST_SQL_DRIVERMANAGER_H_
