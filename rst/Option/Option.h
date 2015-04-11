// Copyright (c) 2015, Sergey Abbakumov
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

#ifndef RST_OPTION_OPTION_H_
#define RST_OPTION_OPTION_H_

#include <cassert>
#include <utility>

namespace rst {

class NoneType {
 public:
  NoneType() {}
};
const NoneType None;

template <class T>
class Option {
 public:
  // Allows implicit conversion from T.
  Option(const T& rhs) : is_valid_(true), was_checked_(false), data_(rhs) {}
  // Allows implicit conversion from T.
  Option(T&& rhs)
      : is_valid_(true), was_checked_(false), data_(std::move(rhs)) {}

  Option(const Option& rhs)
      : is_valid_(rhs.is_valid_), was_checked_(rhs.was_checked_) {
    if (is_valid_) Construct(rhs.data_);
  }

  Option(Option&& rhs)
      : is_valid_(rhs.is_valid_), was_checked_(rhs.was_checked_) {
    if (is_valid_) Construct(std::move(rhs.data_));
  }
  
  // Allows implicit conversion from NoneType.
  Option(const NoneType&) noexcept : is_valid_(false), was_checked_(false) {}
  
  Option() noexcept : is_valid_(false), was_checked_(false) {}

  ~Option() {
    if (is_valid_) Destruct();
  }

  Option& operator=(const T& rhs) {
    if (is_valid_) {
      data_ = rhs;
    } else {
      Construct(rhs);
      is_valid_ = true;
    }
    was_checked_ = false;

    return *this;
  }

  Option& operator=(T&& rhs) {
    if (is_valid_) {
      data_ = std::move(rhs);
    } else {
      Construct(std::move(rhs));
      is_valid_ = true;
    }
    was_checked_ = false;

    return *this;
  }
  
  Option& operator=(const Option& rhs) {
    if (this != &rhs) {
      if (rhs.is_valid_) {
        if (is_valid_) {
          data_ = rhs.data_;
        } else {
          Construct(rhs.data_);
          is_valid_ = true;
        }
      } else {
        if (is_valid_) {
          Destruct();
          is_valid_ = false;
        }
      }
      was_checked_ = rhs.was_checked_;
    }

    return *this;
  }

  Option& operator=(Option&& rhs) {
    if (this != &rhs) {
      if (rhs.is_valid_) {
        if (is_valid_) {
          data_ = std::move(rhs.data_);
        } else {
          Construct(std::move(rhs.data_));
          is_valid_ = true;
        }
      } else {
        if (is_valid_) {
          Destruct();
          is_valid_ = false;
        }
      }
      was_checked_ = rhs.was_checked_;
    }

    return *this;
  }
  
  Option& operator=(const NoneType&) {
    if (is_valid_) Destruct();
    is_valid_ = false;
    was_checked_ = false;

    return *this;
  }

  T& operator*() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return data_;
  }
  
  T* operator->() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return &data_;
  }

  operator bool() noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  void Ignore() { was_checked_ = true; }

 private:
  void Construct(const T& rhs) {
    new (&data_) T(rhs);
  }

  void Construct(T&& rhs) {
    new (&data_) T(std::move(rhs));
  }
  
  void Destruct() {
    data_.~T();
  }
  
  
  bool is_valid_;
  
  bool was_checked_;
  
  union {
    T data_;
  };
};

}  // namespace rst

#endif  // RST_OPTION_OPTION_H_
