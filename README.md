# RST C++ library

This is a RST C++11 library.

It is licensed under the _Simplified BSD License_.

Now the library contains the following components:

## Optional
  A lightweight analogy of _Boost.optional_ and _Option_ enum in Rust. It is
  used to support one more optional value for every existing type. It uses
  pointer-like semantics by overloading `operator*` and `operator->`. The
  object of the class aborts if it has not been checked.

## Result
  An analogy of _Result_ enum in Rust. Also it is similar to Go's (value,
  error) return semantics. It is used as a return value of any method or
  function. The component can be helpful to support error handling without C++
  exceptions. It uses pointer-like semantics by overloading `operator*` and
  `operator->`. A Result template class supports:
  * `Result<ValueType, ErrorType>`
  * `Result<void, ErrorType>` - doesn't have `*` and `->` operator overloads.

  The object of the class asserts if it has been checked. So, it's impossible
  now to ignore an error.

## Logger
  A fast flexible thread-safe Logger component for C++11.

## Status
  A Google-like _Status_ class for error handling with small improvements. It's
  impossible now to ignore an error.

## Cpp14
  A set of C++14 features unavaliable for C++11 compilers.

## Format
  A Python-like string format library

## NonCopyable
  A set of utility classes: _NonCopyConstructible_, _NonAssignable_,
  _NonCopyable_, _NonMoveConstructible_, _NonMoveAssignable_, _NonMovable_.

## Defer
  A Go-like (also a Microsoft GSL finally-like) method of deferring the action
  execution at the end of the function.

## Check
  A set of macros for better error handling.

## Hidden String
  A compile time string encryption. It's useful when you need to have a
  "secret" strings in a program that are not findable in a hexeditor.

## BuildFlag
  A Chromium-like build-flag support.
