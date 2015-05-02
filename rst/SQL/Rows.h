#ifndef RST_SQL_ROWS_H_
#define RST_SQL_ROWS_H_

#include <memory>

#include "rst/SQL/driver/Rows.h"
#include "rst/Status/Status.h"
#include "rst/Util/Int64.h"

namespace rst {

class Rows {
 public:
  explicit Rows(std::unique_ptr<driver::Rows> rows);
  
  bool Next();
  Status& Err() { return status_; }

  template <class... Args>
  Status Scan(const Args&... args) {
    return Unbind(1, args...);
  }

 private:
  Status Unbind(const int /* index */) {
    return StatusOk();
  }
  
  template <class T, class... Args>
  Status Unbind(const int index, T* val, const Args&... args) {
    Status status = Get(index, val);
    if (!status.ok()) return status;

    return Unbind(index + 1, args...);
  }

  Status Get(const int index, Int64* x) {
    Status status;
    status.Ignore();
    
    int64_t value = rows_->GetInt64(index, &status);
    if (!status.ok()) return status;

    *x = Int64(value);
    return StatusOk();
  }

  Status Get(const int index, std::string* x) {
    Status status;
    status.Ignore();
    
    *x = rows_->GetString(index, &status);
    return status;
  }

  Status Get(const int index, double* x) {
    Status status;
    status.Ignore();
    
    *x = rows_->GetDouble(index, &status);
    return status;
  }
  
  std::unique_ptr<driver::Rows> rows_;
  
  Status status_;
};

}  // namespace rst

#endif  // RST_SQL_ROWS_H_
