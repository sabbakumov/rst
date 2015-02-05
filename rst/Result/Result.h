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
#include <new>
#include <utility>

namespace rst {

namespace result {

template <class T, class E>
class Result {
 public:
  
  Result() = delete;
  
  Result(const Result& result) : is_valid_(result.is_valid_) {
    if (is_valid_) {
      new (&ok_) T(result.ok_);
    } else {
      new (&err_) E(result.err_);
    }
  }

  Result(Result&& result) : is_valid_(result.is_valid_) {
    if (is_valid_) {
      new (&ok_) T(std::move(result.ok_));
    } else {
      new (&err_) E(std::move(result.err_));
    }
  }
  
  // Allows implicit conversion from T.
  Result(const T& ok) : is_valid_(true), ok_(ok) {}
  Result(T&& ok) : is_valid_(true), ok_(std::move(ok)) {}
  
  Result(const E& err, const int) : is_valid_(false), err_(err) {}
  Result(E&& err, const int) : is_valid_(false), err_(std::move(err)) {}

  ~Result() {
    Destruct();
  }
  
  Result& operator=(const Result& result) {
    if (this != &result) {
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

  Result& operator=(Result&& result) {
    if (this != &result) {
      Destruct();

      is_valid_ = result.is_valid_;
      if (is_valid_) {
        new (&ok_) T(std::move(result.ok_));
      } else {
        new (&err_) E(std::move(result.err_));
      }
    }
    
    return *this;
  }
  
  Result& operator=(const T& ok) {
    Destruct();

    new (&ok_) T(ok);
    is_valid_ = true;

    return *this;
  }

  Result& operator=(T&& ok) {
    Destruct();

    new (&ok_) T(std::move(ok));
    is_valid_ = true;

    return *this;
  }

  operator bool() const noexcept {
    return is_valid_;
  }

  T& operator*() noexcept {
    assert(is_valid_);
    return ok_;
  }
  
  const T& operator*() const noexcept {
    assert(is_valid_);
    return ok_;
  }

  T* operator->() noexcept {
    assert(is_valid_);
    return &ok_;
  }
  
  const T* operator->() const noexcept {
    assert(is_valid_);
    return &ok_;
  }

  E& Err() noexcept {
    assert(!is_valid_);
    return err_;
  }
  
  const E& Err() const noexcept {
    assert(!is_valid_);
    return err_;
  }

 private:

  void Destruct() {
    if (is_valid_) {
      ok_.~T();
    } else {
      err_.~E();
    }
  }
  
  
  bool is_valid_;
  
  union {
    T ok_;
    E err_;
  };
};

template <class E>
class Result<void, E> {
 public:
  
  Result() = delete;
  
  Result(const Result& result) : is_valid_(result.is_valid_) {
    if (!is_valid_) {
      new (&err_) E(result.err_);
    }
  }

  Result(Result&& result) : is_valid_(result.is_valid_) {
    if (!is_valid_) {
      new (&err_) E(std::move(result.err_));
    }
  }

  Result(const int) : is_valid_(true) {}
  
  Result(const E& err, const int) : is_valid_(false), err_(err) {}
  Result(E&& err, const int) : is_valid_(false), err_(std::move(err)) {}

  ~Result() {
    Destruct();
  }
  
  Result& operator=(const Result& result) {
    if (this != &result) {
      Destruct();

      is_valid_ = result.is_valid_;
      if (!is_valid_) {
        new (&err_) E(result.err_);
      }
    }
    
    return *this;
  }

  Result& operator=(Result&& result) {
    if (this != &result) {
      Destruct();

      is_valid_ = result.is_valid_;
      if (!is_valid_) {
        new (&err_) E(std::move(result.err_));
      }
    }
    
    return *this;
  }
  
  operator bool() const noexcept {
    return is_valid_;
  }

  E& Err() noexcept {
    assert(!is_valid_);
    return err_;
  }
  
  const E& Err() const noexcept {
    assert(!is_valid_);
    return err_;
  }

 private:
  
  void Destruct() {
    if (!is_valid_) {
      err_.~E();
    }
  }
  bool is_valid_;
  
  union {
    E err_;
  };
};

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
