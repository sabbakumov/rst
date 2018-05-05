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

#include <array>
#include <limits>
#include <string>

#include <gtest/gtest.h>

using std::array;
using std::numeric_limits;
using std::string;

namespace rst {

using namespace literals;
using namespace internal;

TEST(FormatterTest, Escape) {
  EXPECT_EQ("{", format("{{"));
  EXPECT_EQ("before {", format("before {{"));
  EXPECT_EQ("{ after", format("{{ after"));
  EXPECT_EQ("before { after", format("before {{ after"));

  EXPECT_EQ("}", format("}}"));
  EXPECT_EQ("before }", format("before }}"));
  EXPECT_EQ("} after", format("}} after"));
  EXPECT_EQ("before } after", format("before }} after"));

  EXPECT_EQ("{}", format("{{}}"));
  EXPECT_EQ("{42}", format("{{{}}}", 42));
}

TEST(FormatterTest, UnmatchedBraces) {
  EXPECT_DEATH(format("{"), "");

  EXPECT_DEATH(format("}"), "");

  EXPECT_DEATH(format("{0{}"), "");
}

TEST(FormatterTest, UnmatchedBracesWithArgs) {
  EXPECT_DEATH(format("{", 1), "");

  EXPECT_DEATH(format("}", 1), "");

  EXPECT_DEATH(format("{0{}", 1), "");
}

TEST(FormatterTest, NoArgs) { EXPECT_EQ("test", format("test")); }

TEST(FormatterTest, ArgsInDifferentPositions) {
  EXPECT_EQ("42", format("{}", 42));
  EXPECT_EQ("before 42", format("before {}", 42));
  EXPECT_EQ("42 after", format("{} after", 42));
  EXPECT_EQ("before 42 after", format("before {} after", 42));
  EXPECT_EQ("answer = 42", format("{} = {}", "answer", 42));
  EXPECT_EQ("42 is the answer", format("{} is the {}", 42, "answer"));
  EXPECT_EQ("abracadabra", format("{}{}{}", "abra", "cad", "abra"));
}

TEST(FormatterTest, ArgErrors) {
  EXPECT_DEATH(format("{"), "");

  EXPECT_DEATH(format("{?}"), "");

  EXPECT_DEATH(format("{0"), "");

  EXPECT_DEATH(format("{}"), "");
}

TEST(FormatterTest, ArgErrorsWithArgs) {
  EXPECT_DEATH(format("{", 1), "");

  EXPECT_DEATH(format("{?}", 1), "");

  EXPECT_DEATH(format("{0", 1), "");
}

TEST(FormatTest, Variadic) { EXPECT_EQ("abc1", format("{}c{}", "ab", 1)); }

TEST(FormatTest, MaxArgs) {
  EXPECT_EQ("0123456789abcde1.2300001.230000",
            format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}", 0, 1, 2, 3, 4, 5, 6, 7,
                   8, 9, 'a', 'b', 'c', 'd', 'e', 1.23, 1.23f));
}

TEST(FormatTest, ExtraArgument) {
  EXPECT_DEATH(format("", 1), "");
  EXPECT_DEATH(format("string", 1), "");
  EXPECT_DEATH(format("string{}{}", 1), "");
}

TEST(FormatTest, Strings) {
  const string s = "string";
  EXPECT_EQ("string", "{}"_format(s));
  EXPECT_EQ("stringstring", "{}{}"_format(s, s));
  EXPECT_EQ("temp", "{}"_format(string("temp")));
}

TEST(Literals, Common) {
  EXPECT_EQ("", ""_format());
  EXPECT_EQ("1234 Hello, world",
            "{} {}, {}{}{}"_format(1234, "Hello", "wor", 'l', 'd'));
}

TEST(Writer, DefaultCtor) {
  Writer writer;
  EXPECT_EQ("", writer.CopyString());
  EXPECT_EQ("", writer.TakeString());
}

TEST(Writer, FormatAndWriteNullStr) {
  Writer writer;
  EXPECT_DEATH(writer.FormatAndWrite(nullptr, 10, "format", 0), "");
}

TEST(Writer, FormatAndWriteNullFormat) {
  Writer writer;
  array<char, 10> str;
  EXPECT_DEATH(writer.FormatAndWrite(str.data(), str.size(), nullptr, 0), "");
}

TEST(Writer, FormatAndWriteZeroSize) {
  Writer writer;
  array<char, 10> str;
  writer.FormatAndWrite(str.data(), 0, "%d", 0);
  EXPECT_EQ("", writer.CopyString());
}

TEST(Writer, FormatAndWriteOneSize) {
  Writer writer;
  array<char, 10> str;
  writer.FormatAndWrite(str.data(), 1, "%d", 0);
  EXPECT_EQ("", writer.CopyString());
}

TEST(Writer, FormatAndWriteTwoSize) {
  Writer writer;
  array<char, 10> str;
  writer.FormatAndWrite(str.data(), 2, "%d", 0);
  EXPECT_EQ("0", writer.CopyString());
}

TEST(Writer, FormatAndWriteThreeSize) {
  Writer writer;
  array<char, 10> str;
  writer.FormatAndWrite(str.data(), 3, "%d", 10);
  EXPECT_EQ("10", writer.CopyString());
}

TEST(Writer, FormatAndWriteThreeSizeNotFull) {
  Writer writer;
  array<char, 10> str;
  writer.FormatAndWrite(str.data(), 3, "%d", 103);
  EXPECT_EQ("10", writer.CopyString());
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
  EXPECT_EQ("012345678.0000009.00000010.000000", writer.CopyString());
}

TEST(Writer, MinMax) {
  Writer writer;
  writer.Write(numeric_limits<short>::min());
  writer.Write(numeric_limits<short>::max());
  writer.Write(numeric_limits<int>::min());
  writer.Write(numeric_limits<int>::max());
  writer.Write(numeric_limits<long>::min());
  writer.Write(numeric_limits<long>::max());
  writer.Write(numeric_limits<long long>::min());
  writer.Write(numeric_limits<long long>::max());
  writer.Write(numeric_limits<unsigned short>::min());
  writer.Write(numeric_limits<unsigned short>::max());
  writer.Write(numeric_limits<unsigned int>::min());
  writer.Write(numeric_limits<unsigned int>::max());
  writer.Write(numeric_limits<unsigned long>::min());
  writer.Write(numeric_limits<unsigned long>::max());
  writer.Write(numeric_limits<unsigned long long>::min());
  writer.Write(numeric_limits<unsigned long long>::max());
  writer.Write(numeric_limits<float>::min());
  writer.Write(numeric_limits<float>::max());
  writer.Write(numeric_limits<double>::min());
  writer.Write(numeric_limits<double>::max());
  writer.Write(numeric_limits<long double>::min());
  writer.Write(numeric_limits<long double>::max());

  string result;
  result += std::to_string(numeric_limits<short>::min());
  result += std::to_string(numeric_limits<short>::max());
  result += std::to_string(numeric_limits<int>::min());
  result += std::to_string(numeric_limits<int>::max());
  result += std::to_string(numeric_limits<long>::min());
  result += std::to_string(numeric_limits<long>::max());
  result += std::to_string(numeric_limits<long long>::min());
  result += std::to_string(numeric_limits<long long>::max());
  result += std::to_string(numeric_limits<unsigned short>::min());
  result += std::to_string(numeric_limits<unsigned short>::max());
  result += std::to_string(numeric_limits<unsigned int>::min());
  result += std::to_string(numeric_limits<unsigned int>::max());
  result += std::to_string(numeric_limits<unsigned long>::min());
  result += std::to_string(numeric_limits<unsigned long>::max());
  result += std::to_string(numeric_limits<unsigned long long>::min());
  result += std::to_string(numeric_limits<unsigned long long>::max());
  result += std::to_string(numeric_limits<float>::min());
  result += std::to_string(numeric_limits<float>::max());
  result += std::to_string(numeric_limits<double>::min());
  result += std::to_string(numeric_limits<double>::max());
  result += std::to_string(numeric_limits<long double>::min());
  result += std::to_string(numeric_limits<long double>::max());

  EXPECT_EQ(result, writer.CopyString());
}

TEST(Writer, EmptyStdString) {
  Writer writer;
  const string str;
  writer.Write(str);
  EXPECT_EQ("", writer.CopyString());
}

TEST(Writer, EmptyCharPtr) {
  Writer writer;
  static constexpr auto str = "";
  writer.Write(str);
  EXPECT_EQ("", writer.CopyString());
}

TEST(Writer, NullCharPtr) {
  Writer writer;
  EXPECT_DEATH(writer.Write(nullptr), "");
}

TEST(Writer, NormalStdString) {
  Writer writer;
  const string str = "Normal";
  writer.Write(str);
  EXPECT_EQ("Normal", writer.CopyString());
}

TEST(Writer, NormalCharPtr) {
  Writer writer;
  static constexpr auto str = "Normal";
  writer.Write(str);
  EXPECT_EQ("Normal", writer.CopyString());
}

TEST(Writer, NormalChar) {
  Writer writer;
  static constexpr auto c = 'C';
  writer.Write(c);
  EXPECT_EQ("C", writer.CopyString());
}

TEST(Writer, BigStdString) {
  Writer writer;
  const string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  EXPECT_EQ(str, writer.CopyString());
}

TEST(Writer, BigCharPtr) {
  Writer writer;
  const string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str.c_str());
  EXPECT_EQ(str, writer.CopyString());
}

TEST(Writer, AppendBigStdString) {
  Writer writer;
  const string initial_str = "Initial";
  const string str(Writer::kStaticBufferSize, 'A');
  writer.Write(initial_str);
  writer.Write(str);
  writer.Write(initial_str);
  EXPECT_EQ(initial_str + str + initial_str, writer.CopyString());
}

TEST(Writer, AppendBigCharPtr) {
  Writer writer;
  static constexpr auto initial_str = "Initial";
  const string str(Writer::kStaticBufferSize, 'A');
  writer.Write(initial_str);
  writer.Write(str.c_str());
  writer.Write(initial_str);
  EXPECT_EQ(initial_str + str + initial_str, writer.CopyString());
}

TEST(Writer, AppendBigChar) {
  Writer writer;
  const string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  static constexpr auto c = 'C';
  writer.Write(c);
  EXPECT_EQ(str + c, writer.CopyString());
}

TEST(Writer, WriteNullPtr) {
  Writer writer;
  EXPECT_DEATH(writer.Write(nullptr, 10), "");
}

TEST(Writer, WriteZeroSize) {
  Writer writer;
  static constexpr auto str = "Initial";
  writer.Write(str, 0);
  EXPECT_EQ("", writer.CopyString());
}

TEST(Writer, TakeStringStatic) {
  Writer writer;
  writer.Write("Initial");
  EXPECT_EQ("Initial", writer.TakeString());
  EXPECT_DEATH(writer.TakeString(), "");
}

TEST(Writer, TakeStringDynamic) {
  Writer writer;
  const string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  EXPECT_EQ(str, writer.TakeString());
  EXPECT_DEATH(writer.TakeString(), "");
}

TEST(Writer, CopyStringStatic) {
  Writer writer;
  writer.Write("Initial");
  EXPECT_EQ("Initial", writer.CopyString());
  EXPECT_EQ("Initial", writer.CopyString());
}

TEST(Writer, CopyStringDynamic) {
  Writer writer;
  const string str(Writer::kStaticBufferSize, 'A');
  writer.Write(str);
  EXPECT_EQ(str, writer.CopyString());
  EXPECT_EQ(str, writer.CopyString());
}

TEST(HandleCharacter, SNullPtr) {
  EXPECT_DEATH(HandleCharacter('a', nullptr), "");
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
  EXPECT_STREQ("{", s);
}

TEST(HandleCharacter, CaseOpenClose) {
  auto s = "{}";
  EXPECT_FALSE(HandleCharacter(*s, &s));
  EXPECT_STREQ("{}", s);
}

TEST(HandleCharacter, CaseOpenOther) {
  auto s = "{o";
  EXPECT_DEATH(HandleCharacter(*s, &s), "");
}

TEST(HandleCharacter, CaseCloseClose) {
  auto s = "}}";
  EXPECT_TRUE(HandleCharacter(*s, &s));
  EXPECT_STREQ("}", s);
}

TEST(HandleCharacter, CaseCloseOther) {
  auto s = "}o";
  EXPECT_DEATH(HandleCharacter(*s, &s), "");
}

TEST(Format, WriterNullPtr) {
  EXPECT_DEATH(Format(nullptr, ""), "");
}

TEST(Format, SNullPtr) {
  Writer writer;
  EXPECT_DEATH(Format(&writer, nullptr), "");
}

}  // namespace rst
