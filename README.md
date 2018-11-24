# RST C++ library

This is a RST C++11 library.

It is licensed under the _Simplified BSD License_.

Now the library contains the following components:

## BuildFlag
  A Chromium-like build-flag support.

## Check
  A set of macros for better programming error handling.

## Defer
  A Go-like (also a Microsoft GSL finally-like) method of deferring the action
  execution at the end of the function.

## GUID
  GUID generation.

## Format
  A Python-like string format library

## Hidden String
  A compile time string encryption. It's useful when you need to have a
  "secret" strings in a program that are not findable in a hex editor.

## Legacy
  A set of features unavaliable for C++11 compilers:
  * make_unique<T>
  * Optional<T>

## Logger
  A fast flexible thread-safe logger for C++11.

## Macros
  A Google-like RST_DISALLOW_COPY_AND_ASSIGN macros.

## Memory
  A set of memory related utilities like WeakPtr, WrapUnique.

## NoDestructor
  A Chromium-like static storage duration support.

## RTTI
  LLVM-like custom RTTI support.

## Status
  A Google-like _Status_ class for recoverable error handling with small
  improvements. It's impossible now to ignore an error.

## STL
  A set of STL utilities.
