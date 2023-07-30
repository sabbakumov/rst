// Copyright (c) 2018, Sergey Abbakumov
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

#ifndef RST_RTTI_RTTI_H_
#define RST_RTTI_RTTI_H_

#include "rst/check/check.h"
#include "rst/not_null/not_null.h"

// LLVM-based RTTI.
//
// Example:
//
//   #include "rst/rtti/rtti.h"
//
//   class FileError : public rst::ErrorInfo<FileError> {
//    public:
//     explicit FileError(std::string&& message);
//     ~FileError();
//
//     const std::string& AsString() const override;
//
//     // Important to have this non-const field!
//     static char id_;
//
//    private:
//     const std::string message_;
//   };
//
//   rst::Status status = Bar();
//   if (status.err() &&
//       rst::dyn_cast<FileError>(status.GetError()) != nullptr) {
//     // File doesn't exist.
//   }
//
namespace rst {

template <class T, class U>
Nullable<T*> dyn_cast(const NotNull<U*> ptr) {
  if (ptr->template IsA<T>())
    return static_cast<T*>(ptr.get());
  return nullptr;
}

template <class T, class U>
Nullable<T*> dyn_cast(U* ptr) {
  return dyn_cast<T, U>(NotNull(ptr));
}

template <class T, class U>
Nullable<const T*> dyn_cast(const NotNull<const U*> ptr) {
  if (ptr->template IsA<T>())
    return static_cast<const T*>(ptr.get());
  return nullptr;
}

template <class T, class U>
Nullable<const T*> dyn_cast(const U* ptr) {
  return dyn_cast<T, U>(NotNull(ptr));
}

}  // namespace rst

#endif  // RST_RTTI_RTTI_H_
