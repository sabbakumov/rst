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
#include <memory>
#include <new>
#include <utility>

namespace rst {

template <class T, class E>
class Result {
 public:
  Result() : is_valid_(false), was_checked_(false), err_(E()) {}
  
  Result(Result&& rhs)
      : is_valid_(rhs.is_valid_), was_checked_(false) {
    rhs.was_checked_ = true;
    if (is_valid_) {
      new (&ok_) T(std::move(rhs.ok_));
    } else {
      new (&err_) E(std::move(rhs.err_));
    }
  }
  
  // Allows implicit conversion from T.
  Result(const T& rhs) : is_valid_(true), was_checked_(false), ok_(rhs) {}
  // Allows implicit conversion from T.
  Result(T&& rhs) : is_valid_(true), was_checked_(false), ok_(std::move(rhs)) {}
  
  Result(const E& rhs, const int)
      : is_valid_(false), was_checked_(false), err_(rhs) {}
  Result(E&& rhs, const int)
      : is_valid_(false), was_checked_(false), err_(std::move(rhs)) {}

  ~Result() {
    assert(was_checked_);
    if (is_valid_) {
      ok_.~T();
    } else {
      err_.~E();
    }
  }
  
  Result& operator=(Result&& rhs) {
    assert(was_checked_);
    if (this != &rhs) {
      if (rhs.is_valid_) {
        if (!is_valid_) {
          err_.~E();
          new (&ok_) T(std::move(rhs.ok_));
          is_valid_ = true;
        } else {
          ok_ = std::move(rhs.ok_);
        }
      } else {
        if (is_valid_) {
          ok_.~T();
          new (&err_) E(std::move(rhs.err_));
          is_valid_ = false;
        } else {
          err_ = std::move(rhs.err_);
        }
      }
      was_checked_ = false;
      rhs.was_checked_ = true;
    }
    
    return *this;
  }
  
  Result& operator=(const T& rhs) {
    assert(was_checked_);
    if (is_valid_) {
      ok_ = rhs;
    } else {
      err_.~E();
      new (&ok_) T(rhs);
      is_valid_ = true;
    }
    was_checked_ = false;

    return *this;
  }

  Result& operator=(T&& rhs) {
    assert(was_checked_);
    if (is_valid_) {
      ok_ = std::move(rhs);
    } else {
      err_.~E();
      new (&ok_) T(std::move(rhs));
      is_valid_ = true;
    }
    was_checked_ = false;

    return *this;
  }

  operator bool() noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  T& operator*() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_;
  }

  T* operator->() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return &ok_;
  }
  
  E& Err() noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }
  
  void Ignore() { was_checked_ = true; }

 private:
  Result(const Result&) = delete;
  Result& operator=(const Result& rhs) = delete;

  bool is_valid_;
  
  bool was_checked_;
  
  union {
    T ok_;
    E err_;
  };
};


template <class E>
class Result<void, E> {
 public:
  Result() : is_valid_(false), was_checked_(false), err_(E()) {}
  
  Result(Result&& rhs)
      : is_valid_(rhs.is_valid_), was_checked_(false) {
    rhs.was_checked_ = true;
    if (!is_valid_) {
      new (&err_) E(std::move(rhs.err_));
    }
  }

  Result(const int) noexcept : is_valid_(true), was_checked_(false) {}
  
  Result(const E& rhs, const int)
      : is_valid_(false), was_checked_(false), err_(rhs) {}
  Result(E&& rhs, const int)
      : is_valid_(false), was_checked_(false), err_(std::move(rhs)) {}

  ~Result() {
    assert(was_checked_);
    if (!is_valid_) err_.~E();
  }
  
  Result& operator=(Result&& rhs) {
    assert(was_checked_);
    if (this != &rhs) {
      if (rhs.is_valid_) {
        if (!is_valid_) {
          err_.~E();
          is_valid_ = true;
        }
      } else {
        if (is_valid_) {
          new (&err_) E(std::move(rhs.err_));
          is_valid_ = false;
        } else {
          err_ = std::move(rhs.err_);
        }
      }
      was_checked_ = false;
      rhs.was_checked_ = true;
    }
    
    return *this;
  }
  
  operator bool() noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  E& Err() noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }

  void Ignore() { was_checked_ = true; }

 private:
  Result(const Result&) = delete;
  Result& operator=(const Result& rhs) = delete;

  bool is_valid_;
  
  bool was_checked_;
  
  union {
    E err_;
  };
};

template <class T, class E>
Result<T, E> Err(E&& err) {
  return Result<T, E>(std::forward<E>(err), 0);
}

}  // namespace rst

#endif  // RST_RESULT_RESULT_H_
