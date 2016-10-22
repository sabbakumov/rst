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

#include <cassert>
#include <limits>
#include <string>

#include "Format.h"

using namespace Format;

#define AssertEq(val1, val2) \
do \
{ \
	assert(val1 == val2); \
} \
while (false)

#define AssertThrowMsg(expr, exception_type, exception_message) \
do \
{ \
	try \
	{ \
		expr; \
	} \
	catch (const exception_type& e) \
	{ \
		const std::string message = e.what(); \
		assert(message == exception_message); \
		break; \
	} \
	catch (...) \
	{ \
		assert(false); \
	} \
	assert(false); \
} \
while (false)

int main()
{
	// TEST(FormatterTest, Escape)
	{
	  AssertEq("{", format("{{"));
	  AssertEq("before {", format("before {{"));
	  AssertEq("{ after", format("{{ after"));
	  AssertEq("before { after", format("before {{ after"));
	
	  AssertEq("}", format("}}"));
	  AssertEq("before }", format("before }}"));
	  AssertEq("} after", format("}} after"));
	  AssertEq("before } after", format("before }} after"));
	
	  AssertEq("{}", format("{{}}"));
	  AssertEq("{42}", format("{{{}}}", 42));
	}

	// TEST(FormatterTest, UnmatchedBraces)
	{
	  AssertThrowMsg(format("{"), FormatError, "Invalid format string");

	  AssertThrowMsg(format("}"), FormatError, "Unmatched '}' in format string");

	  AssertThrowMsg(format("{0{}"), FormatError, "Invalid format string");
	}
	
	// TEST(FormatterTest, UnmatchedBraces)
	{
	  AssertThrowMsg(format("{", 1), FormatError, "Invalid format string");

	  AssertThrowMsg(format("}", 1), FormatError, "Unmatched '}' in format string");

	  AssertThrowMsg(format("{0{}", 1), FormatError, "Invalid format string");
	}

	// TEST(FormatterTest, NoArgs)
	{
	  AssertEq("test", format("test"));
	}

	// TEST(FormatterTest, ArgsInDifferentPositions)
	{
	  AssertEq("42", format("{}", 42));
	  AssertEq("before 42", format("before {}", 42));
	  AssertEq("42 after", format("{} after", 42));
	  AssertEq("before 42 after", format("before {} after", 42));
	  AssertEq("answer = 42", format("{} = {}", "answer", 42));
	  AssertEq("42 is the answer", format("{} is the {}", 42, "answer"));
	  AssertEq("abracadabra", format("{}{}{}", "abra", "cad", "abra"));
	}

	// TEST(FormatterTest, ArgErrors)
	{
	  AssertThrowMsg(format("{"), FormatError, "Invalid format string");

	  AssertThrowMsg(format("{?}"), FormatError, "Invalid format string");

	  AssertThrowMsg(format("{0"), FormatError, "Invalid format string");

	  AssertThrowMsg(format("{}"), FormatError, "Argument index out of range");
	}

	// TEST(FormatterTest, ArgErrors)
	{
	  AssertThrowMsg(format("{", 1), FormatError, "Invalid format string");

	  AssertThrowMsg(format("{?}", 1), FormatError, "Invalid format string");

	  AssertThrowMsg(format("{0", 1), FormatError, "Invalid format string");
	}

	// TEST(FormatTest, Variadic)
	{
	  AssertEq("abc1", format("{}c{}", "ab", 1));
	}

	// TEST(FormatTest, MaxArgs)
	{
	  AssertEq("0123456789abcde1.2300001.230000",
	            format("{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}{}",
	                        0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 'a', 'b', 'c', 'd', 'e', 1.23, 1.23f));
	}

	// TEST(FormatTest, ExtraArgument)
	{
	  AssertThrowMsg(format("", 1), FormatError, "Extra arguments");
	}

	// TEST(Literals, Commong)
	{
		using namespace literals;
		AssertEq("", ""_format());
		AssertEq("1234 Hello, world", "{} {}, {}{}{}"_format(1234, "Hello", "wor", 'l', 'd'));
	}

	// TEST(Writer, DefaultCtor)
	{
		using namespace internal;
		Writer writer;
		AssertEq("", writer.CopyString());
	}

	// TEST(Writer, Numbers)
	{
		using namespace internal;
		
		const int int_val = 1;
		const long long_val = 2;
		const long long long_long_val = 3;
		const unsigned int unsigned_int_val = 4;
		const unsigned long unsigned_long_val = 5;
		const unsigned long long unsigned_long_long_val = 6;
		const float float_val = 7.0f;
		const double double_val = 8.0;
		const long double long_double_val = 9.0;
		
		Writer writer;
		writer.Write(int_val);
		writer.Write(long_val);
		writer.Write(long_long_val);
		writer.Write(unsigned_int_val);
		writer.Write(unsigned_long_val);
		writer.Write(unsigned_long_long_val);
		writer.Write(float_val);
		writer.Write(double_val);
		writer.Write(long_double_val);
		AssertEq("1234567.0000008.0000009.000000", writer.CopyString());
	}

	// TEST(Writer, MinMax)
	{
		using namespace internal;
		
		const int min_int_val = std::numeric_limits<int>::min();
		const int max_int_val = std::numeric_limits<int>::max();
		const long min_long_val = std::numeric_limits<long>::min();
		const long max_long_val = std::numeric_limits<long>::max();
		const long long min_long_long_val = std::numeric_limits<long long>::min();
		const long long max_long_long_val = std::numeric_limits<long long>::max();
		const unsigned int min_unsigned_int_val = std::numeric_limits<unsigned int>::min();
		const unsigned int max_unsigned_int_val = std::numeric_limits<unsigned int>::max();
		const unsigned long min_unsigned_long_val = std::numeric_limits<unsigned long>::min();
		const unsigned long max_unsigned_long_val = std::numeric_limits<unsigned long>::max();
		const unsigned long long min_unsigned_long_long_val = std::numeric_limits<unsigned long long>::min();
		const unsigned long long max_unsigned_long_long_val = std::numeric_limits<unsigned long long>::max();
		const float min_float_val = std::numeric_limits<float>::min();
		const float max_float_val = std::numeric_limits<float>::max();
		const double min_double_val = std::numeric_limits<double>::min();
		const double max_double_val = std::numeric_limits<double>::max();
		const long double min_long_double_val = std::numeric_limits<long double>::min();
		const long double max_long_double_val = std::numeric_limits<long double>::max();
		
		Writer writer;
		writer.Write(min_int_val);
		writer.Write(max_int_val);
		writer.Write(min_long_val);
		writer.Write(max_long_val);
		writer.Write(min_long_long_val);
		writer.Write(max_long_long_val);
		writer.Write(min_unsigned_int_val);
		writer.Write(max_unsigned_int_val);
		writer.Write(min_unsigned_long_val);
		writer.Write(max_unsigned_long_val);
		writer.Write(min_unsigned_long_long_val);
		writer.Write(max_unsigned_long_long_val);
		writer.Write(min_float_val);
		writer.Write(max_float_val);
		writer.Write(min_double_val);
		writer.Write(max_double_val);
		writer.Write(min_long_double_val);
		writer.Write(max_long_double_val);

		std::string result;
		result += std::to_string(std::numeric_limits<int>::min());
		result += std::to_string(std::numeric_limits<int>::max());
		result += std::to_string(std::numeric_limits<long>::min());
		result += std::to_string(std::numeric_limits<long>::max());
		result += std::to_string(std::numeric_limits<long long>::min());
		result += std::to_string(std::numeric_limits<long long>::max());
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
		
		AssertEq(result, writer.CopyString());
	}

	// TEST(Writer, EmptyStdString)
	{
		using namespace internal;
		
		Writer writer;
		const std::string str;
		writer.Write(str);
		AssertEq("", writer.CopyString());
	}
	// TEST(Writer, EmptyCharPtr)
	{
		using namespace internal;
		
		Writer writer;
		const char* str = "";
		writer.Write(str);
		AssertEq("", writer.CopyString());
	}

	// TEST(Writer, NormalStdString)
	{
		using namespace internal;
		
		Writer writer;
		const std::string str = "Normal";
		writer.Write(str);
		AssertEq("Normal", writer.CopyString());
	}
	// TEST(Writer, NormalCharPtr)
	{
		using namespace internal;
		
		Writer writer;
		const char* str = "Normal";
		writer.Write(str);
		AssertEq("Normal", writer.CopyString());
	}
	// TEST(Writer, NormalChar)
	{
		using namespace internal;
		
		Writer writer;
		const char c = 'C';
		writer.Write(c);
		AssertEq("C", writer.CopyString());
	}

	// TEST(Writer, BigStdString)
	{
		using namespace internal;
		
		Writer writer;
		const std::string str(Writer::kStaticBufferSize, 'A');
		writer.Write(str);
		AssertEq(str, writer.CopyString());
	}
	// TEST(Writer, BigCharPtr)
	{
		using namespace internal;
		
		Writer writer;
		const std::string str(Writer::kStaticBufferSize, 'A');
		writer.Write(str.c_str());
		AssertEq(str, writer.CopyString());
	}
	
	// TEST(Writer, AppendBigStdString)
	{
		using namespace internal;
		
		Writer writer;
		const std::string initial_str = "Initial";
		const std::string str(Writer::kStaticBufferSize, 'A');
		writer.Write(initial_str);
		writer.Write(str);
		writer.Write(initial_str);
		AssertEq(initial_str + str + initial_str, writer.CopyString());
	}
	// TEST(Writer, AppendBigCharPtr)
	{
		using namespace internal;
		
		Writer writer;
		const char* initial_str = "Initial";
		const std::string str(Writer::kStaticBufferSize, 'A');
		writer.Write(initial_str);
		writer.Write(str.c_str());
		writer.Write(initial_str);
		AssertEq(initial_str + str + initial_str, writer.CopyString());
	}
	// TEST(Writer, AppendBigChar)
	{
		using namespace internal;
		
		Writer writer;
		const std::string str(Writer::kStaticBufferSize, 'A');
		writer.Write(str);
		const char c = 'C';
		writer.Write(c);
		AssertEq(str + c, writer.CopyString());
	}
}

