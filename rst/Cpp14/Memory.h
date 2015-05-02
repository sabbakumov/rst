#ifndef RST_CPP14_MEMORY_H_
#define RST_CPP14_MEMORY_H_

#include <memory>

namespace rst {

template <class T, class... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
  return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

}  // namespace rst

#endif  // RST_CPP14_MEMORY_H_
