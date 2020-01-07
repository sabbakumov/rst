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

#include "rst/files/file_utils.h"

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <optional>
#include <utility>

#include "rst/macros/os.h"
#include "rst/status/status_macros.h"
#include "rst/strings/str_cat.h"

#if RST_BUILDFLAG(OS_WIN)
#include <Windows.h>
#else
#include <sys/stat.h>
#endif

namespace rst {
namespace {

#if RST_BUILDFLAG(OS_WIN)
bool Replace(const NotNull<const char*> old_filename,
             const NotNull<const char*> new_filename) {
  return ::MoveFileEx(old_filename.get(), new_filename.get(),
                      MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0;
}

std::optional<int64_t> GetFileSize(const NotNull<const char*> filename) {
  ::WIN32_FILE_ATTRIBUTE_DATA attr;
  if (::GetFileAttributesEx(filename.get(), ::GetFileExInfoStandard, &attr) ==
      0) {
    return std::nullopt;
  }

  return static_cast<int64_t>(attr.nFileSizeHigh) << 32 |
         static_cast<int64_t>(attr.nFileSizeLow);
}

#else

bool Replace(const NotNull<const char*> old_filename,
             const NotNull<const char*> new_filename) {
  return std::rename(old_filename.get(), new_filename.get()) == 0;
}

std::optional<int64_t> GetFileSize(const NotNull<const char*> filename) {
  struct ::stat buf;
  if (::stat(filename.get(), &buf) != 0)
    return std::nullopt;

  return buf.st_size;
}
#endif

}  // namespace

char FileError::id_ = '\0';

FileError::FileError(std::string&& message) : message_(std::move(message)) {}

FileError::~FileError() = default;

const std::string& FileError::AsString() const { return message_; }

char FileOpenError::id_ = '\0';

FileOpenError::FileOpenError(std::string&& message)
    : ErrorInfo(std::move(message)) {}

FileOpenError::~FileOpenError() = default;

Status WriteFile(const NotNull<const char*> filename,
                 const std::string_view data) {
  std::unique_ptr<std::FILE, void (*)(std::FILE*)> file(
      std::fopen(filename.get(), "wb"), [](std::FILE* f) {
        if (f != nullptr)
          (void)std::fclose(f);
      });

  if (file == nullptr)
    return MakeStatus<FileOpenError>(StrCat({"Can't open file ", filename}));

  if (std::fwrite(data.data(), 1, data.size(), file.get()) != data.size())
    return MakeStatus<FileError>(StrCat({"Can't write file ", filename}));

  const auto raw_file = file.release();
  if (std::fclose(raw_file) != 0)
    return MakeStatus<FileError>(StrCat({"Can't close file ", filename}));

  return Status::OK();
}

Status WriteImportantFile(const NotNull<const char*> filename,
                          const std::string_view data) {
  const auto temp_filename = StrCat({filename, "_tmp_"});
  RST_TRY(WriteFile(temp_filename.c_str(), data));

  if (!Replace(temp_filename.c_str(), filename.get())) {
    (void)std::remove(temp_filename.c_str());
    return MakeStatus<FileError>(
        StrCat({"Can't rename temp file ", temp_filename}));
  }

  return Status::OK();
}

StatusOr<std::string> ReadFile(const NotNull<const char*> filename) {
  std::unique_ptr<std::FILE, void (*)(std::FILE*)> file(
      std::fopen(filename.get(), "rb"), [](std::FILE* f) {
        if (f != nullptr)
          (void)std::fclose(f);
      });

  if (file == nullptr)
    return MakeStatus<FileOpenError>(StrCat({"Can't open file ", filename}));

  static constexpr int64_t kDefaultChunkSize = 128 * 1024 - 1;
  auto chunk_size = GetFileSize(filename).value_or(kDefaultChunkSize);
  RST_DCHECK(chunk_size >= 0);
  if (chunk_size == 0)  // Some files return 0 bytes (/etc/*).
    chunk_size = kDefaultChunkSize;
  chunk_size++;

  std::rewind(file.get());

  std::string content;
  size_t bytes_read_so_far = 0;
  for (size_t bytes_read_this_pass = 0;
       std::feof(file.get()) == 0 && std::ferror(file.get()) == 0;
       bytes_read_so_far += bytes_read_this_pass) {
    RST_DCHECK(content.size() == bytes_read_so_far);
    content.resize(bytes_read_so_far + static_cast<size_t>(chunk_size));
    bytes_read_this_pass =
        std::fread(content.data() + bytes_read_so_far, 1,
                   static_cast<size_t>(chunk_size), file.get());
  }

  if (std::ferror(file.get()) != 0)
    return MakeStatus<FileError>(StrCat({"Can't read file ", filename}));

  const auto raw_file = file.release();
  if (std::fclose(raw_file) != 0)
    return MakeStatus<FileError>(StrCat({"Can't close file ", filename}));

  content.resize(bytes_read_so_far);
  return content;
}

}  // namespace rst
