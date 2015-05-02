#include "DriverManager.h"

namespace rst {

Status DriverManager::Register(
    const std::string& name, std::unique_ptr<driver::Driver> driver) {
  auto it = Drivers().find(name);
  if (it != Drivers().end()) {
    return StatusErr("Driver has already been registered");
  }

  Drivers()[name] = std::move(driver);
  return StatusOk();
}

std::map<std::string, std::unique_ptr<driver::Driver>>&
DriverManager::Drivers() {
  static std::map<std::string, std::unique_ptr<driver::Driver>> drivers;
  return drivers;
}

}  // namespace rst

