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
  Option(const T& value);
  Option(const Option& option);
  Option(const NoneType&);
  Option();

  ~Option();

  Option& operator=(const T& value);
  Option& operator=(const Option& option);
  Option& operator=(const NoneType&);

  operator bool() const;

  T& operator*();
  const T& operator*() const;

 private:
  union {
    T data_;
  };
  
  bool is_valid_;

  void Construct(const T& value);
  void Destruct();
};

template <class T>
Option<T>::Option(const T& value) {
  Construct(value);
  is_valid_ = true;
}

template <class T>
Option<T>::Option(const NoneType&) {
  is_valid_ = false;
}

template <class T>
Option<T>::Option() {
  is_valid_ = false;
}

template <class T>
Option<T>::Option(const Option& option) {
  if (option.is_valid_) {
    Construct(option.data_);
    is_valid_ = true;
  } else {
    is_valid_ = false;
  }
}

template <class T>
Option<T>::~Option() {
  Destruct();
}

template <class T>
Option<T>& Option<T>::operator=(const T& value) {
  Destruct();
  Construct(value);
  is_valid_ = true;

  return *this;
}

template <class T>
Option<T>& Option<T>::operator=(const NoneType&) {
  Destruct();
  is_valid_ = false;

  return *this;
}

template <class T>
Option<T>& Option<T>::operator=(const Option& option) {
  if (*this != option) {
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

template <class T>
Option<T>::operator bool() const {
  return is_valid_;
}

template <class T>
T& Option<T>::operator*() {
  assert(is_valid_);
  return data_;
}

template <class T>
const T& Option<T>::operator*() const {
  assert(is_valid_);
  return data_;
}

template <class T>
void Option<T>::Construct(const T& value) {
  new (&data_) T(value);
}

template <class T>
void Option<T>::Destruct() {
  if (is_valid_) {
    data_.~T();
  }
}

template <class T>
bool operator<(const Option<T>& a, const Option<T>& b) {
  return *a < *b;
}

}  // namespace option 

}  // namespace rst

#endif  // RST_OPTION_OPTION_H_
