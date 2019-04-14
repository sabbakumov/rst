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

#ifndef RST_LEGACY_OPTIONAL_H_
#define RST_LEGACY_OPTIONAL_H_

#include <new>
#include <utility>

#include "rst/Check/Check.h"
#include "rst/Macros/Macros.h"

namespace rst {

struct nullopt_t {
  constexpr explicit nullopt_t(int) {}
};
constexpr nullopt_t nullopt(0);

// A Boost-like optional.
template <class T>
class [[nodiscard]] optional {
 public:
  // Allows implicit conversion from T.
  optional(const T& value) : value_(value), is_valid_(true) {}
  // Allows implicit conversion from T.
  optional(T && value) : value_(std::move(value)), is_valid_(true) {}

  optional(const optional& other) : is_valid_(other.is_valid_) {
    if (is_valid_)
      Construct(other.value_);
  }

  optional(optional && other) : is_valid_(other.is_valid_) {
    if (is_valid_)
      Construct(std::move(other.value_));
  }

  // Allows implicit conversion from nullopt_t.
  optional(nullopt_t) {}

  optional() {}

  ~optional() {
    if (is_valid_)
      Destruct();
  }

  optional& operator=(const T& value) {
    if (is_valid_)
      Destruct();

    Construct(value);
    is_valid_ = true;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    return *this;
  }

  optional& operator=(T&& value) {
    if (is_valid_)
      Destruct();

    Construct(std::move(value));
    is_valid_ = true;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    return *this;
  }

  optional& operator=(const optional& rhs) {
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
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    return *this;
  }

  optional& operator=(optional&& rhs) {
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
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    return *this;
  }

  optional& operator=(nullopt_t) {
    if (is_valid_)
      Destruct();

    is_valid_ = false;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    return *this;
  }

  T& operator*() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(is_valid_);
    return value_;
  }

  const T& operator*() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(is_valid_);
    return value_;
  }

  T* operator->() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(is_valid_);
    return &value_;
  }

  T* operator->() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(is_valid_);
    return &value_;
  }

  explicit operator bool() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return is_valid_;
  }

  bool has_value() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return is_valid_;
  }

  template <class U>
  T value_or(U && default_value) const {
    if (is_valid_)
      return value_;
    return static_cast<T>(std::forward<U>(default_value));
  }

  template <class... Args>
  T& emplace(Args && ... args) {
    if (is_valid_)
      Destruct();

    Construct(T(std::forward<Args>(args)...));
    is_valid_ = true;
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

    return value_;
  }

 private:
  void Construct(const T& value) { new (&value_) T(value); }

  void Construct(T && value) { new (&value_) T(std::move(value)); }

  void Destruct() { value_.~T(); }

  union {
    T value_;
  };

  bool is_valid_ = false;
#if RST_BUILDFLAG(DCHECK_IS_ON)
  mutable bool was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
};

template <class T, class U>
bool operator==(const optional<T>& opt, const U& value) {
  return opt.has_value() ? *opt == value : false;
}

template <class T, class U>
bool operator==(const U& value, const optional<T>& opt) {
  return opt == value;
}

template <class T, class U>
bool operator!=(const optional<T>& opt, const U& value) {
  return !(opt == value);
}

template <class T, class U>
bool operator!=(const U& value, const optional<T>& opt) {
  return !(value == opt);
}

}  // namespace rst

#endif  // RST_LEGACY_OPTIONAL_H_
