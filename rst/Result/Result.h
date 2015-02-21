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
  
  Result(const Result& result) = delete;

  Result(Result&& result)
      : is_valid_(result.is_valid_), was_checked_(false) {
    result.was_checked_ = true;
    if (is_valid_) {
      new (&ok_) T(std::move(result.ok_));
    } else {
      new (&err_) E(std::move(result.err_));
    }
  }
  
  // Allows implicit conversion from T.
  Result(const T& ok) : is_valid_(true), was_checked_(false), ok_(ok) {}
  Result(T&& ok) : is_valid_(true), was_checked_(false), ok_(std::move(ok)) {}
  
  Result(const E& err, const int)
      : is_valid_(false), was_checked_(false), err_(err) {}
  Result(E&& err, const int)
      : is_valid_(false), was_checked_(false), err_(std::move(err)) {}

  ~Result() {
    assert(was_checked_);
    if (is_valid_) {
      ok_.~T();
    } else {
      err_.~E();
    }
  }
  
  Result& operator=(const Result& result) {
    assert(was_checked_);
    if (this != &result) {
      if (result.is_valid_) {
        if (!is_valid_) {
          err_.~E();
          new (&ok_) T(result.ok_);
          is_valid_ = true;
        } else {
          ok_ = result.ok_;
        }
      } else {
        if (is_valid_) {
          ok_.~T();
          new (&err_) E(result.err_);
          is_valid_ = false;
        } else {
          err_ = result.err_;
        }
      }
      was_checked_ = false;
    }
    
    return *this;
  }

  Result& operator=(Result&& result) {
    assert(was_checked_);
    if (this != &result) {
      if (result.is_valid_) {
        if (!is_valid_) {
          err_.~E();
          new (&ok_) T(std::move(result.ok_));
          is_valid_ = true;
        } else {
          ok_ = std::move(result.ok_);
        }
      } else {
        if (is_valid_) {
          ok_.~T();
          new (&err_) E(std::move(result.err_));
          is_valid_ = false;
        } else {
          err_ = std::move(result.err_);
        }
      }
      was_checked_ = false;
      result.was_checked_ = true;
    }
    
    return *this;
  }
  
  Result& operator=(const T& ok) {
    assert(was_checked_);
    if (is_valid_) {
      ok_ = ok;
    } else {
      err_.~E();
      new (&ok_) T(ok);
      is_valid_ = true;
    }
    was_checked_ = false;

    return *this;
  }

  Result& operator=(T&& ok) {
    assert(was_checked_);
    if (is_valid_) {
      ok_ = std::move(ok);
    } else {
      err_.~E();
      new (&ok_) T(std::move(ok));
      is_valid_ = true;
    }
    was_checked_ = false;

    return *this;
  }

  operator bool() const noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  T& operator*() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_;
  }
  
  const T& operator*() const noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_;
  }

  T* operator->() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return &ok_;
  }
  
  const T* operator->() const noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return &ok_;
  }

  E& Err() noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }
  
  const E& Err() const noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }

  void Ignore() const { was_checked_ = true; }

 private:
  bool is_valid_;
  
  // Allows const objects to modify 'checked' flag.
  mutable bool was_checked_;
  
  union {
    T ok_;
    E err_;
  };
};


template <class E>
class Result<void, E> {
 public:
  Result() : is_valid_(false), was_checked_(false), err_(E()) {}
  
  Result(const Result& result) = delete;

  Result(Result&& result)
      : is_valid_(result.is_valid_), was_checked_(false) {
    result.was_checked_ = true;
    if (!is_valid_) {
      new (&err_) E(std::move(result.err_));
    }
  }

  Result(const int) noexcept : is_valid_(true), was_checked_(false) {}
  
  Result(const E& err, const int)
      : is_valid_(false), was_checked_(false), err_(err) {}
  Result(E&& err, const int)
      : is_valid_(false), was_checked_(false), err_(std::move(err)) {}

  ~Result() {
    assert(was_checked_);
    if (!is_valid_) err_.~E();
  }
  
  Result& operator=(const Result& result) {
    assert(was_checked_);
    if (this != &result) {
      if (result.is_valid_) {
        if (!is_valid_) {
          err_.~E();
          is_valid_ = true;
        }
      } else {
        if (is_valid_) {
          new (&err_) E(result.err_);
          is_valid_ = false;
        } else {
          err_ = result.err_;
        }
      }
      was_checked_ = false;
    }
    
    return *this;
  }

  Result& operator=(Result&& result) {
    assert(was_checked_);
    if (this != &result) {
      if (result.is_valid_) {
        if (!is_valid_) {
          err_.~E();
          is_valid_ = true;
        }
      } else {
        if (is_valid_) {
          new (&err_) E(std::move(result.err_));
          is_valid_ = false;
        } else {
          err_ = std::move(result.err_);
        }
      }
      was_checked_ = false;
      result.was_checked_ = true;
    }
    
    return *this;
  }
  
  operator bool() const noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  E& Err() noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }
  
  const E& Err() const noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }

  void Ignore() const { was_checked_ = true; }

 private:
  bool is_valid_;
  
  // Allows const objects to modify 'checked' flag.
  mutable bool was_checked_;
  
  union {
    E err_;
  };
};


template <class T, class E>
class Result<T*, E> {
 public:
  Result() : is_valid_(false), was_checked_(false), err_(E()) {}
  Result(const Result&) = delete;

  Result(Result&& result)
      : is_valid_(result.is_valid_), was_checked_(false) {
    result.was_checked_ = true;
    if (is_valid_) {
      new (&ok_) std::unique_ptr<T>(std::move(result.ok_));
    } else {
      new (&err_) E(std::move(result.err_));
    }
  }
  
  // Allows implicit conversion from T*.
  Result(T* ok) noexcept : is_valid_(true), was_checked_(false), ok_(ok) {}
  
  Result(const E& err, const int)
      : is_valid_(false), was_checked_(false), err_(err) {}
  Result(E&& err, const int)
      : is_valid_(false), was_checked_(false), err_(std::move(err)) {}

  ~Result() {
    assert(was_checked_);
    if (is_valid_) {
      using std::unique_ptr;
      ok_.unique_ptr<T, std::default_delete<T>>::~unique_ptr();
    } else {
      err_.~E();
    }
  }
  
  Result& operator=(const Result&) = delete;

  Result& operator=(Result&& result) {
    assert(was_checked_);
    if (this != &result) {
      if (result.is_valid_) {
        if (!is_valid_) {
          err_.~E();
          new (&ok_) std::unique_ptr<T>(std::move(result.ok_));
          is_valid_ = true;
        } else {
          ok_ = std::move(result.ok_);
        }
      } else {
        if (is_valid_) {
          using std::unique_ptr;
          ok_.unique_ptr<T, std::default_delete<T>>::~unique_ptr();
          new (&err_) E(std::move(result.err_));
          is_valid_ = false;
        } else {
          err_ = std::move(result.err_);
        }
      }
      was_checked_ = false;
      result.was_checked_ = true;
    }
    
    return *this;
  }
  
  operator bool() const noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  T& operator*() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    assert(ok_.get());
    return *ok_;
  }
  
  const T& operator*() const noexcept {
    assert(was_checked_);
    assert(is_valid_);
    assert(ok_.get());
    return *ok_;
  }

  T* operator->() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_.get();
  }
  
  const T* operator->() const noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_.get();
  }

  E& Err() noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }
  
  const E& Err() const noexcept {
    assert(was_checked_);
    assert(!is_valid_);
    return err_;
  }

  T* get() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_.get();
  }
  
  const T* get() const noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_.get();
  }

  T* release() noexcept {
    assert(was_checked_);
    assert(is_valid_);
    return ok_.release();
  }

  void reset(T* ok = nullptr) noexcept {
    assert(was_checked_);
    assert(is_valid_);
    ok_.reset(ok);
  }

  void Ignore() const { was_checked_ = true; }

 private:
  bool is_valid_;
  
  // Allows const objects to modify 'checked' flag.
  mutable bool was_checked_;
  
  union {
    std::unique_ptr<T> ok_;
    E err_;
  };
};

template <class T, class E>
Result<T, E> Err(E&& err) {
  return Result<T, E>(std::forward<E>(err), 0);
}

template <class T, class E>
bool operator<(const Result<T, E>& a, const Result<T, E>& b) {
  a.Ignore();
  b.Ignore();
  return *a < *b;
}

}  // namespace rst

#endif  // RST_RESULT_RESULT_H_
