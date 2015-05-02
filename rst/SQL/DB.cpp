#include "rst/SQL/DB.h"

#include "rst/SQL/DriverManager.h"

namespace rst {

Status DB::Open(
    const std::string& driver_name, const std::string& data_source_name) {
  auto& drivers = DriverManager::Drivers();
  auto it = drivers.find(driver_name);
  if (it == drivers.end()) return StatusErr("Driver has not been registered");

  driver_ = it->second.get();

  Status status;
  status.Ignore();
  conn_ = driver_->Open(data_source_name, &status);
  return status;
}

}  // namespace rst

