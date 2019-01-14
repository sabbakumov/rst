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

#include "rst/Format/Format.h"

#include <iterator>
#include <limits>
#include <string>
#include <string_view>

#include <gtest/gtest.h>

namespace rst {

using namespace literals;
using namespace internal;

TEST(FormatterTest, Escape) {
  EXPECT_EQ(DoFormat("{{"), "{");
  EXPECT_EQ(DoFormat("before {{"), "before {");
  EXPECT_EQ(DoFormat("{{ after"), "{ after");
  EXPECT_EQ(DoFormat("before {{ after"), "before { after");

  EXPECT_EQ(DoFormat("}}"), "}");
  EXPECT_EQ(DoFormat("before }}"), "before }");
  EXPECT_EQ(DoFormat("}} after"), "} after");
  EXPECT_EQ(DoFormat("before }} after"), "before } after");

  EXPECT_EQ(DoFormat("{{}}"), "{}");
  EXPECT_EQ(DoFormat("{{{}}}", 42), "{42}");
}

TEST(FormatterTest, UnmatchedBraces) {
  EXPECT_DEATH(DoFormat("{"), "");

  EXPECT_DEATH(DoFormat("}"), "");

  EXPECT_DEATH(DoFormat("{0{}"), "");
}

TEST(FormatterTest, UnmatchedBracesWithArgs) {
  EXPECT_DEATH(DoFormat("{", 1), "");

  EXPECT_DEATH(DoFormat("}", 1), "");

  EXPECT_DEATH(DoFormat("{0{}", 1), "");
}

TEST(FormatterTest, NoArgs) { EXPECT_EQ(DoFormat("test"), "test"); }

TEST(FormatterTest, ArgsInDifferentPositions) {
  EXPECT_EQ(DoFormat("{}", 42), "42");
  EXPECT_EQ(DoFormat("before {}", 42), "before 42");
  EXPECT_EQ(DoFormat("{} after", 42), "42 after");
  EXPECT_EQ(DoFormat("before {} after", 42), "before 42 after");
  EXPECT_EQ(DoFormat("{} = {}", "answer", 42), "answer = 42");
  EXPECT_EQ(DoFormat("{} is the {}", 42, "answer"), "42 is the answer");
  EXPECT_EQ(DoFormat("{}{}{}", "abra", "cad", "abra"), "abracadabra");
}

TEST(FormatterTest, ArgErrors) {
  EXPECT_DEATH(DoFormat("{"), "");

  EXPECT_DEATH(DoFormat("{?}"), "");

  EXPECT_DEATH(DoFormat("{0"), "");

  EXPECT_DEATH(DoFormat("{}"), "");
}

TEST(FormatterTest, ArgErrorsWithArgs) {
  EXPECT_DEATH(DoFormat("{", 1), "");

  EXPECT_DEATH(DoFormat("{?}", 1), "");

  EXPECT_DEATH(DoFormat("{0", 1), "");
}

TEST(FormatTest, Variadic) { EXPECT_EQ(DoFormat("{}c{}", "ab", 1), "abc1"); }

TEST(FormatTest, MaxArgs) {
  EXPECT_EQ(DoFormat("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}", 0, 1, 2, 3, 4, 5, 6,
                     7, 8, 9, 'a', 'b', 'c', 'd', 'e', 1.23, 1.23f),
            "0123456789abcde1.2300001.230000");
}

TEST(FormatTest, ExtraArgument) {
  EXPECT_DEATH(DoFormat("", 1), "");
  EXPECT_DEATH(DoFormat("string", 1), "");
  EXPECT_DEATH(DoFormat("string{}{}", 1), "");
}

TEST(FormatTest, Strings) {
  const std::string s = "string";
  EXPECT_EQ("{}"_format(s), "string");
  EXPECT_EQ("{}{}"_format(s, s), "stringstring");
  EXPECT_EQ("{}"_format(std::string("temp")), "temp");
  EXPECT_EQ("{}"_format(std::string_view("temp")), "temp");
}

TEST(Literals, Common) {
  EXPECT_EQ(""_format(), std::string());
  EXPECT_EQ("{} {}, {}{}{}"_format(1234, "Hello", "wor", 'l', 'd'),
            "1234 Hello, world");
}

TEST(Writer, DefaultCtor) {
  Writer writer;
  EXPECT_EQ(writer.CopyString(), std::string());
  EXPECT_EQ(writer.TakeString(), std::string());
}

TEST(Writer, FormatAndWriteZeroSize) {
  Writer writer;
  char str[10];
  writer.FormatAndWrite(str, 0, "%d", 0);
  EXPECT_EQ(writer.CopyString(), std::string());
}

TEST(Writer, FormatAndWriteOneSize) {
  Writer writer;
  char str[10];
  writer.FormatAndWrite(str, 1, "%d", 0);
  EXPECT_EQ(writer.CopyString(), std::string());
}

TEST(Writer, FormatAndWriteTwoSize) {
  Writer writer;
  char str[10];
  writer.FormatAndWrite(str, 2, "%d", 0);
  EXPECT_EQ(writer.CopyString(), "0");
}

TEST(Writer, FormatAndWriteThreeSize) {
  Writer writer;
  char str[10];
  writer.FormatAndWrite(str, 3, "%d", 10);
  EXPECT_EQ(writer.CopyString(), "10");
}

TEST(Writer, FormatAndWriteThreeSizeNotFull) {
  Writer writer;
  char str[10];
  writer.FormatAndWrite(str, 3, "%d", 103);
  EXPECT_EQ(writer.CopyString(), "10");
}

TEST(FormatTemplate, SNullPtr) {
  Writer writer;
  EXPECT_DEATH(Format(&writer, nullptr, 0), "");
}

TEST(Writer, Numbers) {
  static constexpr short short_val = 0;
  static constexpr int int_val = 1;
  static constexpr long long_val = 2;
  static constexpr long long long_long_val = 3;
  static constexpr unsigned short unsigned_short_val = 4;
  static constexpr unsigned int unsigned_int_val = 5;
  static constexpr unsigned long unsigned_long_val = 6;
  static constexpr unsigned long long unsigned_long_long_val = 7;
  static constexpr float float_val = 8.0f;
  static constexpr double double_val = 9.0;
  static constexpr long double long_double_val = 10.0L;

  Writer writer;
  writer.Write(short_val);
  writer.Write(int_val);
  writer.Write(long_val);
  writer.Write(long_long_val);
  writer.Write(unsigned_short_val);
  writer.Write(unsigned_int_val);
  writer.Write(unsigned_long_val);
  writer.Write(unsigned_long_long_val);
  writer.Write(float_val);
  writer.Write(double_val);
  writer.Write(long_double_val);
  EXPECT_EQ(writer.CopyString(), "012345678.0000009.00000010.000000");
}

TEST(Writer, MinMax) {
  Writer writer;
  writer.Write(std::numeric_limits<short>::min());
  writer.Write(std::numeric_limits<short>::max());
  writer.Write(std::numeric_limits<int>::min());
  writer.Write(std::numeric_limits<int>::max());
  writer.Write(std::numeric_limits<long>::min());
  writer.Write(std::numeric_limits<long>::max());
  writer.Write(std::numeric_limits<long long>::min());
  writer.Write(std::numeric_limits<long long>::max());
  writer.Write(std::numeric_limits<unsigned short>::min());
  writer.Write(std::numeric_limits<unsigned short>::max());
  writer.Write(std::numeric_limits<unsigned int>::min());
  writer.Write(std::numeric_limits<unsigned int>::max());
  writer.Write(std::numeric_limits<unsigned long>::min());
  writer.Write(std::numeric_limits<unsigned long>::max());
  writer.Write(std::numeric_limits<unsigned long long>::min());
  writer.Write(std::numeric_limits<unsigned long long>::max());
  writer.Write(std::numeric_limits<float>::min());
  writer.Write(std::numeric_limits<float>::max());
  writer.Write(std::numeric_limits<double>::min());
  writer.Write(std::numeric_limits<double>::max());
  writer.Write(std::numeric_limits<long double>::min());
  writer.Write(std::numeric_limits<long double>::max());

  std::string result;
  result += std::to_string(std::numeric_limits<short>::min());
  result += std::to_string(std::numeric_limits<short>::max());
  result += std::to_string(std::numeric_limits<int>::min());
  result += std::to_string(std::numeric_limits<int>::max());
  result += std::to_string(std::numeric_limits<long>::min());
  result += std::to_string(std::numeric_limits<long>::max());
  result += std::to_string(std::numeric_limits<long long>::min());
  result += std::to_string(std::numeric_limits<long long>::max());
  result += std::to_string(std::numeric_limits<unsigned short>::min());
  result += std::to_string(std::numeric_limits<unsigned short>::max());
  result += std::to_string(std::numeric_limits<unsigned int>::min());
  result += std::to_string(std::numeric_limits<unsigned int>::max());
  result += std::to_string(std::numeric_limits<unsigned long>::min());
  result += std::to_string(std::numeric_limits<unsigned long>::max());
  result += std::to_string(std::numeric_limits<unsigned long long>::min());
  result += std::to_string(std::numeric_limits<unsigned long long>::max());
  result += std::to_string(std::numeric_limits<float>::min());
  result += std::to_string(std::numeric_limits<float>::max());
  result += std::to_string(std::numeric_limits<double>::min());
  result += std::to_string(std::numeric_limits<double>::max());
  result += std::to_string(std::numeric_limits<long double>::min());
  result += std::to_string(std::numeric_limits<long double>::max());

  EXPECT_EQ(writer.CopyString(), result);
}

TEST(Writer, EmptyStdString) {
  Writer writer;
  const std::string str;
  writer.Write(str);
  EXPECT_EQ(writer.CopyString(), std::string());
}

TEST(Writer, EmptyCharPtr) {
  Writer writer;
  static constexpr auto str = "";
  writer.Write(str);
  EXPECT_EQ(writer.CopyString(), std::string());
}

TEST(Writer, NormalStdString) {
  Writer writer;
  const std::string str = "Normal";
  writer.Write(str);
  EXPECT_EQ(writer.CopyString(), "Normal");
}

TEST(Writer, NormalCharPtr) {
  Writer writer;
  static constexpr auto str = "Normal";
  writer.Write(str);
  EXPECT_EQ(writer.CopyString(), "Normal");
}

TEST(Writer, StringView) {
  Writer writer;
  const std::string_view str = "Normal";
  writer.Write(str);
  EXPECT_EQ(writer.CopyString(), "Normal");
}

TEST(Writer, NormalChar) {
  Writer writer;
  static constexpr auto c = 'C';
  writer.Write(c);
  EXPECT_EQ(writer.CopyString(), "C");
}

TEST(Writer, BigStdString) {
  Writer writer;
  const std::string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  EXPECT_EQ(writer.CopyString(), str);
}

TEST(Writer, BigCharPtr) {
  Writer writer;
  const std::string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str.c_str());
  EXPECT_EQ(writer.CopyString(), str);
}

TEST(Writer, AppendBigStdString) {
  Writer writer;
  const std::string initial_str = "Initial";
  const std::string str(Writer::kStaticBufferSize, 'A');
  writer.Write(initial_str);
  writer.Write(str);
  writer.Write(initial_str);
  EXPECT_EQ(writer.CopyString(), initial_str + str + initial_str);
}

TEST(Writer, AppendBigCharPtr) {
  Writer writer;
  static constexpr auto initial_str = "Initial";
  const std::string str(Writer::kStaticBufferSize, 'A');
  writer.Write(initial_str);
  writer.Write(str.c_str());
  writer.Write(initial_str);
  EXPECT_EQ(writer.CopyString(), initial_str + str + initial_str);
}

TEST(Writer, AppendBigChar) {
  Writer writer;
  const std::string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  static constexpr auto c = 'C';
  writer.Write(c);
  EXPECT_EQ(writer.CopyString(), str + c);
}

TEST(Writer, WriteZeroSize) {
  Writer writer;
  static constexpr auto str = "Initial";
  writer.Write(std::string_view(str, 0));
  EXPECT_EQ(writer.CopyString(), std::string());
}

TEST(Writer, TakeStringStatic) {
  Writer writer;
  writer.Write("Initial");
  EXPECT_EQ(writer.TakeString(), "Initial");
  EXPECT_DEATH(writer.TakeString(), "");
}

TEST(Writer, TakeStringDynamic) {
  Writer writer;
  const std::string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  EXPECT_EQ(writer.TakeString(), str);
  EXPECT_DEATH(writer.TakeString(), "");
}

TEST(Writer, CopyStringStatic) {
  Writer writer;
  writer.Write("Initial");
  EXPECT_EQ(writer.CopyString(), "Initial");
  EXPECT_EQ(writer.CopyString(), "Initial");
}

TEST(Writer, CopyStringDynamic) {
  Writer writer;
  const std::string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  EXPECT_EQ(writer.CopyString(), str);
  EXPECT_EQ(writer.CopyString(), str);
}

TEST(HandleCharacter, SRefNullPtr) {
  const char* s = nullptr;
  EXPECT_DEATH(HandleCharacter('a', &s), "");
}

TEST(HandleCharacter, AnotherString) {
  auto s = "abc";
  EXPECT_DEATH(HandleCharacter('d', &s), "");
}

TEST(HandleCharacter, CaseOpenOpen) {
  auto s = "{{";
  EXPECT_TRUE(HandleCharacter(*s, &s));
  EXPECT_STREQ(s, "{");
}

TEST(HandleCharacter, CaseOpenClose) {
  auto s = "{}";
  EXPECT_FALSE(HandleCharacter(*s, &s));
  EXPECT_STREQ(s, "{}");
}

TEST(HandleCharacter, CaseOpenOther) {
  auto s = "{o";
  EXPECT_DEATH(HandleCharacter(*s, &s), "");
}

TEST(HandleCharacter, CaseCloseClose) {
  auto s = "}}";
  EXPECT_TRUE(HandleCharacter(*s, &s));
  EXPECT_STREQ(s, "}");
}

TEST(HandleCharacter, CaseCloseOther) {
  auto s = "}o";
  EXPECT_DEATH(HandleCharacter(*s, &s), "");
}

TEST(Format, SNullPtr) {
  Writer writer;
  EXPECT_DEATH(Format(&writer, nullptr), "");
}

}  // namespace rst
