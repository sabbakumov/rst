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

#include <cstdio>
#include <string>

#include <gtest/gtest.h>

#include "rst/check/check.h"
#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"
#include "rst/rtti/rtti.h"

namespace rst {
namespace {

class File {
 public:
  File() { RST_DCHECK(std::tmpnam(buffer_) != nullptr); }
  ~File() { std::remove(buffer_); }

  NotNull<const char*> FileName() const { return buffer_; }

 private:
  char buffer_[L_tmpnam];

  RST_DISALLOW_COPY_AND_ASSIGN(File);
};

}  // namespace

TEST(FileUtils, WriteRead) {
  std::string content;
  for (auto i = 0; i < 150; content += std::to_string(i), i++) {
    File file;
    auto status = WriteFile(file.FileName(), content);
    ASSERT_FALSE(status.err());

    auto string = ReadFile(file.FileName().get());
    ASSERT_FALSE(string.err());
    EXPECT_EQ(*string, content);
  }
}

TEST(FileUtils, WriteReadToTheSameFile) {
  File file;
  std::string content;
  for (auto i = 0; i < 150; content += std::to_string(i), i++) {
    auto status = WriteFile(file.FileName(), content);
    ASSERT_FALSE(status.err());

    auto string = ReadFile(file.FileName().get());
    ASSERT_FALSE(string.err());
    EXPECT_EQ(*string, content);
  }
}

TEST(FileUtils, WriteImportantRead) {
  std::string content;
  for (auto i = 0; i < 150; content += std::to_string(i), i++) {
    File file;
    auto status = WriteImportantFile(file.FileName().get(), content);
    ASSERT_FALSE(status.err());

    auto string = ReadFile(file.FileName().get());
    ASSERT_FALSE(string.err());
    EXPECT_EQ(*string, content);
  }
}

TEST(FileUtils, WriteImportantReadToTheSameFile) {
  File file;
  std::string content;
  for (auto i = 0; i < 150; content += std::to_string(i), i++) {
    auto status = WriteImportantFile(file.FileName().get(), content);
    ASSERT_FALSE(status.err());

    auto string = ReadFile(file.FileName().get());
    ASSERT_FALSE(string.err());
    EXPECT_EQ(*string, content);
  }
}

TEST(FileUtils, OpenFailed) {
  File file;
  auto string = ReadFile(file.FileName().get());
  ASSERT_TRUE(string.err());
  EXPECT_NE(dyn_cast<FileOpenError>(string.status().GetError()), nullptr);
}

}  // namespace rst
