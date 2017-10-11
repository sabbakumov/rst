// Copyright (c) 2017, Sergey Abbakumov
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef RST_OPTIONAL_OPTIONAL_H_
#define RST_OPTIONAL_OPTIONAL_H_

#include <new>
#include <utility>

#include "rst/Check/Check.h"

#ifndef RST_NODISCARD
#if __cplusplus > 201402L && __has_cpp_attribute(nodiscard)
#define RST_NODISCARD [[nodiscard]]
#elif !__cplusplus
#define RST_NODISCARD
#elif __has_cpp_attribute(clang::warn_unused_result)
#define RST_NODISCARD [[clang::warn_unused_result]]
#else
#define RST_NODISCARD
#endif
#endif  // RST_NODISCARD

namespace rst {

struct NoneType {
  constexpr NoneType(int) {}
};
constexpr NoneType None(0);

// A Boost-like optional.
template <class T>
class RST_NODISCARD Optional {
 public:
  // Allows implicit conversion from T.
  Optional(const T& value) : value_(value), is_valid_(true) {}
  // Allows implicit conversion from T.
  Optional(T&& value) : value_(std::move(value)), is_valid_(true) {}

  Optional(const Optional& rhs) : is_valid_(rhs.is_valid_) {
    if (is_valid_)
      Construct(rhs.value_);
  }

  Optional(Optional&& rhs) : is_valid_(rhs.is_valid_) {
    if (is_valid_)
      Construct(std::move(rhs.value_));
  }

  // Allows implicit conversion from NoneType.
  Optional(NoneType) : is_valid_(false) {}

  Optional() : is_valid_(false) {}

  ~Optional() {
    if (is_valid_)
      Destruct();
  }

  Optional& operator=(const T& value) {
    if (is_valid_)
      Destruct();

    Construct(value);
    is_valid_ = true;
    set_was_checked(false);

    return *this;
  }

  Optional& operator=(T&& value) {
    if (is_valid_)
      Destruct();

    Construct(std::move(value));
    is_valid_ = true;
    set_was_checked(false);

    return *this;
  }

  Optional& operator=(const Optional& rhs) {
    if (this == &rhs)
      return *this;

    if (is_valid_)
      Destruct();

    if (rhs.is_valid_) {
      Construct(rhs.value_);
      is_valid_ = true;
    } else {
      is_valid_ = false;
    }
    set_was_checked(false);

    return *this;
  }

  Optional& operator=(Optional&& rhs) {
    if (this == &rhs)
      return *this;

    if (is_valid_)
      Destruct();

    if (rhs.is_valid_) {
      Construct(std::move(rhs.value_));
      is_valid_ = true;
    } else {
      is_valid_ = false;
    }
    set_was_checked(false);

    return *this;
  }

  Optional& operator=(NoneType) {
    if (is_valid_)
      Destruct();

    is_valid_ = false;
    set_was_checked(false);

    return *this;
  }

  T& operator*() {
    RST_DCHECK(was_checked_);
    RST_DCHECK(is_valid_);
    return value_;
  }

  T* operator->() {
    RST_DCHECK(was_checked_);
    RST_DCHECK(is_valid_);
    return &value_;
  }

  operator bool() {
    set_was_checked(true);
    return is_valid_;
  }

 private:
  void Construct(const T& value) { new (&value_) T(value); }

  void Construct(T&& value) { new (&value_) T(std::move(value)); }

  void Destruct() { value_.~T(); }

#ifndef NDEBUG
  void set_was_checked(bool was_checked) { was_checked_ = was_checked; }
#else   // NDEBUG
  void set_was_checked(bool) {}
#endif  // NDEBUG

  union {
    T value_;
  };

  bool is_valid_ : 1;
#ifndef NDEBUG
  bool was_checked_ = false;
#endif  // NDEBUG
};

}  // namespace rst

#endif  // RST_OPTIONAL_OPTIONAL_H_
