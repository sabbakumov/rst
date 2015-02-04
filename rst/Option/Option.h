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

namespace option {

class NoneType {
 public:
  NoneType() {}
};
const NoneType None;

template <class T>
class Option {
 public:
  
  Option(const T& value) : data_(value), is_valid_(true) {}
  Option(T&& value) : data_(std::move(value)), is_valid_(true) {}

  Option(const Option& option) : is_valid_(option.is_valid_) {
    if (is_valid_) {
      Construct(option.data_);
    }
  }

  Option(Option&& option) : is_valid_(option.is_valid_) {
    if (is_valid_) {
      Construct(std::move(option.data_));
    }
  }
  
  Option(const NoneType&) : is_valid_(false) {}
  
  Option() : is_valid_(false) {}

  ~Option() {
    Destruct();
  }

  Option& operator=(const T& value) {
    Destruct();
    Construct(value);
    is_valid_ = true;

    return *this;
  }

  Option& operator=(T&& value) {
    Destruct();
    Construct(std::move(value));
    is_valid_ = true;

    return *this;
  }
  
  Option& operator=(const Option& option) {
    if (this != &option) {
      Destruct();

      if (option.is_valid_) {
        Construct(option.data_);
        is_valid_ = true;
      } else {
        is_valid_ = false;
      }
    }

    return *this;
  }

  Option& operator=(Option&& option) {
    if (this != &option) {
      Destruct();

      if (option.is_valid_) {
        Construct(std::move(option.data_));
        is_valid_ = true;
      } else {
        is_valid_ = false;
      }
    }

    return *this;
  }
  
  Option& operator=(const NoneType&) {
    Destruct();
    is_valid_ = false;

    return *this;
  }

  operator bool() const noexcept {
    return is_valid_;
  }

  T& operator*() noexcept {
    assert(is_valid_);
    return data_;
  }
  
  const T& operator*() const noexcept {
    assert(is_valid_);
    return data_;
  }

  T* operator->() noexcept {
    assert(is_valid_);
    return &data_;
  }

  const T* operator->() const noexcept {
    assert(is_valid_);
    return &data_;
  }

 private:
  
  union {
    T data_;
  };
  
  bool is_valid_;

  void Construct(const T& value) {
    new (&data_) T(value);
  }

  void Construct(T&& value) {
    new (&data_) T(std::move(value));
  }
  
  void Destruct() {
    if (is_valid_) {
      data_.~T();
    }
  }
};

template <class T>
bool operator<(const Option<T>& a, const Option<T>& b) {
  return *a < *b;
}

}  // namespace option 

}  // namespace rst

#endif  // RST_OPTION_OPTION_H_
