#ifndef RST_SQL_STMT_H_
#define RST_SQL_STMT_H_

#include <memory>
#include <string>

#include "rst/SQL/Rows.h"

#include "rst/Status/Status.h"

namespace rst {

class Stmt {
 public:
  Status Close();

  template <class... Args>
  Status Exec(Args... args);

  template <class... Args>
  std::unique_ptr<Rows> Query(Status* status, Args... args);
};

}  // namespace rst

#endif  // RST_SQL_STMT_H_
