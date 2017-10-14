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

#include "rst/Logger/FileNameSink.h"

#include <utility>

#include "rst/Check/Check.h"
#include "rst/Format/Format.h"
#include "rst/Logger/LogError.h"
#include "rst/Status/Status.h"
#include "rst/Status/StatusOr.h"

using std::mutex;
using std::string;
using std::unique_lock;

using namespace rst::literals;

namespace rst {

FileNameSinkData::FileNameSinkData(string prologue_format)
    : prologue_format_(std::move(prologue_format)) {}

FileNameSink::FileNameSink(const string& filename, string prologue_format,
                           Status& status)
    : FileNameSinkData(std::move(prologue_format)) {
  log_file_.reset(fopen(filename.c_str(), "w"));

  if (log_file_ == nullptr)
    status = Status(kLoggerErrorDomain,
                    static_cast<int>(LoggerErrorCode::kOpenFileFailed),
                    "Can't open file {}"_format(filename));

  status = Status::OK();
}

FileNameSink::FileNameSink(FileNameSink&& rhs)
    : FileNameSinkData(std::move(rhs)) {}

FileNameSink& FileNameSink::operator=(FileNameSink&& rhs) {
  if (this == &rhs)
    return *this;

  static_cast<FileNameSinkData&>(*this) = std::move(rhs);
  return *this;
}

// static
StatusOr<FileNameSink> FileNameSink::Create(const std::string& filename,
                                            std::string prologue_format) {
  auto status = Status::OK();
  status.Ignore();
  FileNameSink sink(filename, std::move(prologue_format), status);

  if (!status.ok())
    return std::move(status);

  return std::move(sink);
}

void FileNameSink::Log(const char* filename, int line,
                       const char* severity_level, const char* format,
                       va_list args) {
  RST_DCHECK(filename != nullptr);
  RST_DCHECK(line > 0);
  RST_DCHECK(severity_level != nullptr);
  RST_DCHECK(format != nullptr);

  unique_lock<mutex> lock(mutex_);

  auto val = std::fprintf(log_file_.get(), prologue_format_.c_str(), filename,
                          line, severity_level);
  RST_CHECK(val >= 0);

  val = std::vfprintf(log_file_.get(), format, args);
  RST_CHECK(val >= 0);

  val = std::fprintf(log_file_.get(), "\n");
  RST_CHECK(val >= 0);

  val = std::fflush(log_file_.get());
  RST_CHECK(val >= 0);
}

}  // namespace rst
