// Copyright (c) 2016, Sergey Abbakumov
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

#include <cstdlib>
#include <new>
#include <type_traits>
#include <utility>

namespace rst {

// A Rust-like Result<T, E> class for error handling.
template <class T, class E>
class Result {
 public:
  // By default: not valid, not checked.
  Result() : is_valid_(false), was_checked_(false), err_(E()) {}

  // Sets rhs as checked, copies its validity and moves its content. Not checked
  // by default.
  Result(Result&& rhs) : is_valid_(rhs.is_valid_), was_checked_(false) {
    rhs.was_checked_ = true;
    if (is_valid_) {
      new (&ok_) T(std::move(rhs.ok_));
    } else {
      new (&err_) E(std::move(rhs.err_));
    }
  }

  // Allows implicit conversion from T. Types T and T2 should be equal.
  template <class T2>
  Result(T2&& rhs)
      : is_valid_(true), was_checked_(false), ok_(std::forward<T>(rhs)) {
    using Type = typename std::remove_reference<T2>::type;
    static_assert(std::is_same<T, Type>::value, "Types should match");
  }

  // Types E and E2 should be equal.
  template <class E2>
  Result(E2&& rhs, int)
      : is_valid_(false), was_checked_(false), err_(std::forward<E>(rhs)) {
    using Type = typename std::remove_reference<E2>::type;
    static_assert(std::is_same<E, Type>::value, "Types should match");
  }

  Result(const Result&) = delete;

  // If the object has not been checked, aborts.
  ~Result() {
    if (!was_checked_)
      std::abort();

    if (is_valid_) {
      ok_.~T();
    } else {
      err_.~E();
    }
  }

  // Sets rhs as checked, copies its validity and moves its content. Not checked
  // by default. If the object has not been checked, aborts.
  Result& operator=(Result&& rhs) {
    if (!was_checked_)
      std::abort();

    if (this == &rhs)
      return *this;

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

    return *this;
  }

  Result& operator=(const Result& rhs) = delete;

  // If the object has not been checked, aborts. Not checked by default.
  template <class T2>
  Result& operator=(T2&& rhs) {
    using Type = typename std::remove_reference<T2>::type;
    static_assert(std::is_same<T, Type>::value, "Types should match");

    if (!was_checked_)
      std::abort();

    if (is_valid_) {
      ok_ = std::forward<T>(rhs);
    } else {
      err_.~E();
      new (&ok_) T(std::forward<T>(rhs));
      is_valid_ = true;
    }
    was_checked_ = false;

    return *this;
  }

  // Sets the object to be checked. Returns its validity.
  operator bool() noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  // If the object has not been checked or it's invalid, aborts.
  T& operator*() noexcept {
    if (!was_checked_)
      std::abort();

    if (!is_valid_)
      std::abort();

    return ok_;
  }

  // If the object has not been checked or it's invalid, aborts.
  T* operator->() noexcept {
    if (!was_checked_)
      std::abort();

    if (!is_valid_)
      std::abort();

    return &ok_;
  }

  // If the object has not been checked or it's valid, aborts.
  E& Err() noexcept {
    if (!was_checked_)
      std::abort();

    if (is_valid_)
      std::abort();

    return err_;
  }

  // Sets the object to be checked.
  void Ignore() { was_checked_ = true; }

 private:
  // Object's validity.
  bool is_valid_ = false;

  // Whether the object is checked.
  bool was_checked_ = true;

  union {
    // OK object.
    T ok_;
    // Error object.
    E err_;
  };
};

// A Rust-like Result<void, E> class for error handling.
template <class E>
class Result<void, E> {
 public:
  // By default: not valid, not checked.
  Result() : is_valid_(false), was_checked_(false), err_(E()) {}

  // Sets rhs as checked, copies its validity and moves its content. Not checked
  // by default.
  Result(Result&& rhs) : is_valid_(rhs.is_valid_), was_checked_(false) {
    rhs.was_checked_ = true;
    if (!is_valid_)
      new (&err_) E(std::move(rhs.err_));
  }

  // Sets the object to be valid and not checked by default.
  explicit Result(int) noexcept : is_valid_(true), was_checked_(false) {}

  // Sets the object to be invalid and not checked by default.
  template <class E2>
  Result(E2&& rhs, int)
      : is_valid_(false), was_checked_(false), err_(std::forward<E2>(rhs)) {
    using Type = typename std::remove_reference<E2>::type;
    static_assert(std::is_same<E, Type>::value, "Types should match");
  }

  Result(const Result&) = delete;

  // If the object has not been checked, aborts.
  ~Result() {
    if (!was_checked_)
      std::abort();

    if (!is_valid_)
      err_.~E();
  }

  // Sets rhs as checked, copies its validity and moves its content. Not checked
  // by default. If the object has not been checked, aborts.
  Result& operator=(Result&& rhs) {
    if (!was_checked_)
      std::abort();

    if (this == &rhs)
      return *this;

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

    return *this;
  }

  Result& operator=(const Result& rhs) = delete;

  // Sets the object to be checked. Returns its validity.
  operator bool() noexcept {
    was_checked_ = true;
    return is_valid_;
  }

  // If the object has not been checked or it's valid, aborts.
  E& Err() noexcept {
    if (!was_checked_)
      std::abort();

    if (is_valid_)
      std::abort();

    return err_;
  }

  // Sets the object to be checked.
  void Ignore() { was_checked_ = true; }

 private:
  // Object's validity.
  bool is_valid_ : 1;

  // Whether the object is checked.
  bool was_checked_ : 1;

  union {
    // Error object.
    E err_;
  };
};

// Construct the Result object to be error.
template <class T, class E>
Result<T, E> Err(E&& err) {
  return Result<T, E>(std::forward<E>(err), 0);
}

}  // namespace rst

#endif  // RST_RESULT_RESULT_H_
