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

#include "rst/guid/guid.h"
#include "rst/status/status_macros.h"
#include "rst/stl/resize_uninitialized.h"
#include "rst/strings/str_cat.h"

namespace filesystem = std::filesystem;

namespace rst {
namespace {

[[nodiscard]] bool Rename(const filesystem::path& old_filename,
                          const filesystem::path& new_filename) {
  std::error_code ec;
  filesystem::rename(old_filename, new_filename, ec);
  return !ec;
}

[[nodiscard]] bool Remove(const filesystem::path& filename) {
  std::error_code ec;
  return filesystem::remove(filename, ec);
}

std::optional<size_t> GetFileSize(const filesystem::path& filename) {
  std::error_code ec;
  const auto size = filesystem::file_size(filename, ec);
  if (ec)
    return std::nullopt;

  return size;
}

Status WriteFile(const NotNull<const char*> filename,
                 const NotNull<const char*> mode, const std::string_view data) {
  std::unique_ptr<std::FILE, void (*)(std::FILE*)> file(
      std::fopen(filename.get(), mode.get()), [](std::FILE* f) {
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
  return WriteFile(filename, "wb", data);
}

Status WriteImportantFile(const filesystem::path& filename,
                          const std::string_view data) {
  const filesystem::path temp_filename(
      StrCat({filename.native(), Guid().AsStringView().value(), ".tmp"}));
  RST_TRY(WriteFile(temp_filename.c_str(), "wxb", data));

  if (!Rename(temp_filename, filename)) {
    (void)Remove(temp_filename);
    return MakeStatus<FileError>(
        StrCat({"Can't rename temp file ", temp_filename.native()}));
  }

  return Status::OK();
}

StatusOr<std::string> ReadFile(const filesystem::path& filename) {
  std::unique_ptr<std::FILE, void (*)(std::FILE*)> file(
      std::fopen(filename.c_str(), "rb"), [](std::FILE* f) {
        if (f != nullptr)
          (void)std::fclose(f);
      });

  if (file == nullptr) {
    return MakeStatus<FileOpenError>(
        StrCat({"Can't open file ", filename.native()}));
  }

  static constexpr size_t kDefaultChunkSize = 128 * 1024 - 1;
  auto chunk_size = GetFileSize(filename).value_or(kDefaultChunkSize);
  if (chunk_size == 0)  // Some files return 0 bytes (/etc/*).
    chunk_size = kDefaultChunkSize;
  chunk_size++;  // Try to read file at once including the EOF flag.

  std::string content;
  size_t bytes_read_so_far = 0;
  for (size_t bytes_read_this_pass = 0;
       std::feof(file.get()) == 0 && std::ferror(file.get()) == 0;
       bytes_read_so_far += bytes_read_this_pass) {
    RST_DCHECK(content.size() == bytes_read_so_far);
    StringResizeUninitialized(&content, bytes_read_so_far + chunk_size);
    bytes_read_this_pass = std::fread(content.data() + bytes_read_so_far, 1,
                                      chunk_size, file.get());
  }

  if (std::ferror(file.get()) != 0) {
    return MakeStatus<FileError>(
        StrCat({"Can't read file ", filename.native()}));
  }

  const auto raw_file = file.release();
  if (std::fclose(raw_file) != 0) {
    return MakeStatus<FileError>(
        StrCat({"Can't close file ", filename.native()}));
  }

  content.resize(bytes_read_so_far);
  return content;
}

}  // namespace rst
