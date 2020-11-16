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

#include "rst/logger/file_name_sink.h"

#include <limits>
#include <utility>

#include "rst/check/check.h"
#include "rst/logger/log_error.h"
#include "rst/memory/memory.h"
#include "rst/strings/str_cat.h"

namespace rst {

FileNameSink::FileNameSink() = default;
FileNameSink::~FileNameSink() = default;

// static
StatusOr<FileNameSink::Ptr> FileNameSink::Create(
    const NotNull<const char*> filename) {
  auto sink = WrapUnique(new FileNameSink());

  sink->log_file_.reset(std::fopen(filename.get(), "w"));
  if (sink->log_file_ == nullptr)
    return MakeStatus<LogError>(StrCat({"Can't open file ", filename}));

  return sink;
}

void FileNameSink::Log(const std::string_view message) {
  std::lock_guard lock(mutex_);

  RST_DCHECK(message.size() <= std::numeric_limits<int>::max());
  auto val = std::fprintf(log_file_.get(), "%.*s\n",
                          static_cast<int>(message.size()), message.data());
  RST_CHECK(val >= 0);

  val = std::fflush(log_file_.get());
  RST_CHECK(val == 0);
}

}  // namespace rst
