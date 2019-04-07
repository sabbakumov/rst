// Copyright (c) 2019, Sergey Abbakumov
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

#ifndef RST_FILES_FILEUTIL_H_
#define RST_FILES_FILEUTIL_H_

#include <string>
#include <string_view>

#include "rst/Macros/Macros.h"
#include "rst/NotNull/NotNull.h"
#include "rst/Status/Status.h"
#include "rst/Status/StatusOr.h"

namespace rst {

class FileError : public ErrorInfo<FileError> {
 public:
  explicit FileError(std::string&& message);
  ~FileError();

  const std::string& AsString() const override;

  static char id_;

 private:
  const std::string message_;

  RST_DISALLOW_COPY_AND_ASSIGN(FileError);
};

class FileOpenError : public ErrorInfo<FileOpenError, FileError> {
 public:
  using ErrorInfo::ErrorInfo;
  ~FileOpenError();

  static char id_;

 private:
  RST_DISALLOW_COPY_AND_ASSIGN(FileOpenError);
};

Status WriteFile(NotNull<const char*> filename, std::string_view data);

StatusOr<std::string> ReadFile(NotNull<const char*> filename);

}  // namespace rst

#endif  // RST_FILES_FILEUTIL_H_
