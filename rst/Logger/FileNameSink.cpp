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

using namespace rst::literals;

namespace rst {

FileNameSink::FileNameSink(const std::string& filename, Status* status) {
  RST_DCHECK(status != nullptr);

  StatusAsOutParameter sao(status);

  log_file_.reset(std::fopen(filename.c_str(), "w"));

  if (log_file_ == nullptr) {
    *status = Status(kLoggerErrorDomain, LoggerErrorCode::kOpenFileFailed,
                     "Can't open file {}"_format(filename));
    return;
  }

  *status = Status::OK();
}

// static
StatusOr<std::unique_ptr<FileNameSink>> FileNameSink::Create(
    const std::string& filename) {
  auto status = Status::OK();
  std::unique_ptr<FileNameSink> sink(new FileNameSink(filename, &status));

  if (status.err())
    return std::move(status);

  return std::move(sink);
}

void FileNameSink::Log(const std::string& message) {
  std::unique_lock<std::mutex> lock(mutex_);

  auto val = std::fprintf(log_file_.get(), "%s\n", message.c_str());
  RST_CHECK(val >= 0);

  val = std::fflush(log_file_.get());
  RST_CHECK(val >= 0);
}

}  // namespace rst
