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

#ifndef RST_STATUS_STATUS_H_
#define RST_STATUS_STATUS_H_

#include <memory>
#include <string>
#include <utility>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"
#include "rst/status/status.h"

namespace rst {

// ErrorInfoBase and ErrorInfo are LLVM-like base classes for user-defined
// error types.
//
// Example:
//
//   class FileError : public ErrorInfo<FileError> {
//    public:
//     explicit FileError(std::string&& message);
//     ~FileError();
//
//     const std::string& AsString() const override;
//
//     // Important to have this field as non-const!
//     static char id_;
//
//    private:
//     const std::string message_;
//   };
//
//   // Subclass of FileOpenError.
//   class FileOpenError : public ErrorInfo<FileOpenError, FileError> {
//    public:
//     explicit FileOpenError(std::string&& message);
//     ~FileOpenError();
//
//     // Important to have this field as non-const!
//     static char id_;
//   };
//
class ErrorInfoBase {
 public:
  ErrorInfoBase();
  virtual ~ErrorInfoBase();

  static NotNull<const void*> GetClassID();

  virtual const std::string& AsString() const = 0;
  virtual NotNull<const void*> GetDynamicClassID() const = 0;

  virtual bool IsA(NotNull<const void*> class_id) const;

  template <class ErrorInfoT>
  bool IsA() const {
    return IsA(ErrorInfoT::GetClassID());
  }

 private:
  static char id_;

  RST_DISALLOW_COPY_AND_ASSIGN(ErrorInfoBase);
};

template <class T, class Parent = ErrorInfoBase>
class ErrorInfo : public Parent {
 public:
  using Parent::Parent;
  ~ErrorInfo() override = default;

  static NotNull<const void*> GetClassID() { return &T::id_; }

  // Parent:
  NotNull<const void*> GetDynamicClassID() const override { return &T::id_; }

  bool IsA(const NotNull<const void*> class_id) const override {
    return class_id == GetClassID() || Parent::IsA(class_id);
  }

 private:
  RST_DISALLOW_COPY_AND_ASSIGN(ErrorInfo);
};

// A Google-like Status class for recoverable error handling. It's impossible to
// ignore an error.
//
// Example:
//
//   Status status = Foo();
//   if (status.err())
//     return status;
//
//   // Or:
//   RST_TRY(Foo());
//
//   // Check specific error:
//   Status status = Bar();
//   if (status.err() &&
//       dyn_cast<FileOpenError>(status.GetError()) != nullptr) {
//     // File doesn't exist.
//   }
//
class [[nodiscard]] Status {
 public:
  // OK object.
  static Status OK() { return Status(); }

  // Sets as not checked by default.
#if RST_BUILDFLAG(DCHECK_IS_ON)
  Status(Status&& other) noexcept : error_(std::move(other.error_)) {
    other.was_checked_ = true;
  }
#else   // !RST_BUILDFLAG(DCHECK_IS_ON)
  Status(Status&&) noexcept = default;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  // Asserts that it was checked.
#if RST_BUILDFLAG(DCHECK_IS_ON)
  ~Status() { RST_DCHECK(was_checked_); }
#else   // !RST_BUILDFLAG(DCHECK_IS_ON)
  ~Status() = default;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  // Asserts that it was checked before and sets as not checked.
#if RST_BUILDFLAG(DCHECK_IS_ON)
  Status& operator=(Status&& rhs) noexcept {
    RST_DCHECK(was_checked_);

    error_ = std::move(rhs.error_);
    was_checked_ = false;
    rhs.was_checked_ = true;

    return *this;
  }
#else   // !RST_BUILDFLAG(DCHECK_IS_ON)
  Status& operator=(Status&&) noexcept = default;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  // Sets the object to be checked and returns whether the status is error
  // object.
  bool err() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return error_ != nullptr;
  }

  // Asserts that it was checked. Returns error information.
  NotNull<const ErrorInfoBase*> GetError() const {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    RST_DCHECK(was_checked_);
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
    return error_.get();
  }

  // Sets the object to be checked.
  void Ignore() {
#if RST_BUILDFLAG(DCHECK_IS_ON)
    was_checked_ = true;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
  }

 private:
  template <class T>
  friend class StatusOr;

  template <class Err, class... Args>
  friend Status MakeStatus(Args&&... args);

  // Sets the object as not checked by default and to be OK.
  Status() = default;

  // Sets the object as not checked by default and to be the error object.
  explicit Status(NotNull<std::unique_ptr<ErrorInfoBase>> error)
      : error_(std::move(error)) {}

  // Information about the error. nullptr if the object is OK.
  Nullable<std::unique_ptr<ErrorInfoBase>> error_;

#if RST_BUILDFLAG(DCHECK_IS_ON)
  // Whether the object was checked.
  bool was_checked_ = false;
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)

  RST_DISALLOW_COPY_AND_ASSIGN(Status);
};

// Factory function for creating Status objects.
template <class Err, class... Args>
Status MakeStatus(Args&&... args) {
  return Status(std::make_unique<Err>(std::forward<Args>(args)...));
}

}  // namespace rst

#endif  // RST_STATUS_STATUS_H_
