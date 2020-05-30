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

#ifndef RST_FILES_FILE_UTILS_H_
#define RST_FILES_FILE_UTILS_H_

#include <string>
#include <string_view>

#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"
#include "rst/status/status.h"
#include "rst/status/status_or.h"

namespace rst {

class FileError : public ErrorInfo<FileError> {
 public:
  explicit FileError(std::string&& message);
  ~FileError() override;

  // ErrorInfo:
  const std::string& AsString() const override;

  static char id_;

 private:
  const std::string message_;

  RST_DISALLOW_COPY_AND_ASSIGN(FileError);
};

class FileOpenError final : public ErrorInfo<FileOpenError, FileError> {
 public:
  explicit FileOpenError(std::string&& message);
  ~FileOpenError() override;

  static char id_;

 private:
  RST_DISALLOW_COPY_AND_ASSIGN(FileOpenError);
};

// Writes |data| to |filename|. Returns FileError on error.
Status WriteFile(NotNull<const char*> filename, std::string_view data);

// Like WriteFile() but ensures that the file won't be corrupted by application
// crash during write.
Status WriteImportantFile(NotNull<const char*> filename, std::string_view data);

// Reads content from |filename|. Returns FileOpenError if the file can not be
// opened, FileError on other error.
StatusOr<std::string> ReadFile(NotNull<const char*> filename);

}  // namespace rst

#endif  // RST_FILES_FILE_UTILS_H_
