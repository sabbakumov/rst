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

#ifndef RST_RESULT_RESULT_H_
#define RST_RESULT_RESULT_H_

#include <cassert>

namespace rst {

namespace result {

template <class T, class E>
class Result {
 public:
  Result() = delete;
  Result(const Result& result);
  Result& operator=(const Result& result);
  ~Result();
  
  Result(const T& ok);
  Result(const E& err, const int);
  Result& operator=(const T& ok);

  operator bool() const;

  T& operator*();
  const T& operator*() const;

  E& Err();
  const E& Err() const;

 private:
  union {
    T ok_;
    E err_;
  };

  bool is_valid_;
  
  void Destruct();
};

template <class T, class E>
Result<T, E>::Result(const T& ok) : is_valid_(true) {
  new (&ok_) T(ok);
}

template <class T, class E>
Result<T, E>::~Result() {
  Destruct();
}

template <class T, class E>
Result<T, E>::Result(const E& err, const int) : is_valid_(false) {
  new (&err_) E(err);
}

template <class T, class E>
Result<T, E>::Result(const Result& result) : is_valid_(result.is_valid_) {
  if (is_valid_) {
    new (&ok_) T(result.ok_);
  } else {
    new (&err_) E(result.err_);
  }
}

template <class T, class E>
void Result<T, E>::Destruct() {
  if (is_valid_) {
    ok_.~T();
  } else {
    err_.~E();
  }
}

template <class T, class E>
Result<T, E>& Result<T, E>::operator=(const T& ok) {
  Destruct();

  new (&ok_) T(ok);
  is_valid_ = true;
  
  return *this;
}

template <class T, class E>
Result<T, E>& Result<T, E>::operator=(const Result& result) {
  if (*this != result) {
    Destruct();

    is_valid_ = result.is_valid_;
    if (is_valid_) {
      new (&ok_) T(result.ok_);
    } else {
      new (&err_) E(result.err_);
    }
  }

  return *this;
}

template <class T, class E>
Result<T, E>::operator bool() const {
  return is_valid_;
}

template <class T, class E>
T& Result<T, E>::operator*() {
  assert(is_valid_);
  return ok_;
}

template <class T, class E>
const T& Result<T, E>::operator*() const {
  assert(is_valid_);
  return ok_;
}

template <class T, class E>
E& Result<T, E>::Err() {
  assert(!is_valid_);
  return err_;
}

template <class T, class E>
const E& Result<T, E>::Err() const {
  assert(!is_valid_);
  return err_;
}

template <class T, class E>
Result<T, E> Err(const E& err) {
  return Result<T, E>(err, 0);
}

template <class T, class E>
bool operator<(const Result<T, E>& a, const Result<T, E>& b) {
  return *a < *b;
}

}  // namespace result 

}  // namespace rst

#endif  // RST_RESULT_RESULT_H_
