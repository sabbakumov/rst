#ifndef RST_UTIL_INT64_H_
#define RST_UTIL_INT64_H_

#include <cinttypes>

namespace rst {

class Int64 {
 public:
  explicit Int64(const int64_t value) : value_(value) {}
  int64_t value() const { return value_; }
 private:
  int64_t value_;
};

}  // namespace rst

#endif  // RST_UTIL_INT64_H_
