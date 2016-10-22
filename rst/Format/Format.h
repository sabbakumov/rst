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

#ifndef RST_FORMAT_FORMAT_H_
#define RST_FORMAT_FORMAT_H_

#include <array>
#include <cassert>
#include <cstring>
#include <limits>
#include <stdexcept>
#include <string>

namespace Format
{

// Exception thrown by the library
class FormatError : public std::runtime_error
{
public:
	explicit FormatError(const std::string& message) : std::runtime_error(message) {}
	explicit FormatError(const char* message) : std::runtime_error(message) {}
};

namespace internal
{

// Class for writing values. Has static stack and dynamic buffer. By default uses static buffer
class Writer
{
public:
	// The size of the stack buffer
	static constexpr size_t kStaticBufferSize = 1024;
	
	Writer()
	{
		assert(static_buffer_.size() >= 2);
		static_buffer_[0] = '\0';
	}

	// Writes val like std::snprintf
	template <class T>
	void FormatAndWrite(char* str, size_t size, const char* format, T val)
	{
		assert(str);
		assert(format);

		const int bytes_written = std::snprintf(str, size, format, val);
		if (bytes_written < 0)
		{
			throw FormatError("snprintf() failed");
		}
		Write(str, static_cast<size_t>(bytes_written) + 1);
	}

	void Write(int val)
	{
		std::array<char, 4 * sizeof(int)> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%d", val);
	}
	
	void Write(unsigned int val)
	{
		std::array<char, 4 * sizeof(unsigned int)> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%u", val);
	}

	void Write(long val)
	{
		std::array<char, 4 * sizeof(long)> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%ld", val);
	}

	void Write(unsigned long val)
	{
		std::array<char, 4 * sizeof(unsigned long)> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%lu", val);
	}

	void Write(long long val)
	{
		std::array<char, 4 * sizeof(long long)> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%lld", val);
	}

	void Write(unsigned long long val)
	{
		std::array<char, 4 * sizeof(unsigned long long)> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%llu", val);
	}

	void Write(float val)
	{
		std::array<char, std::numeric_limits<float>::max_exponent10 + 20> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%f", val);
	}

	void Write(double val)
	{
		std::array<char, std::numeric_limits<double>::max_exponent10 + 20> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%f", val);
	}

	void Write(long double val)
	{
		std::array<char, std::numeric_limits<long double>::max_exponent10 + 20> buffer;
		FormatAndWrite(buffer.data(), buffer.size(), "%Lf", val);
	}
	
	void Write(const std::string& val)
	{
		const size_t len = val.size() + 1;
		Write(val.c_str(), len);
	}
	
	void Write(const char* val)
	{
		assert(val);
		
		const size_t len = std::strlen(val) + 1;
		Write(val, len);
	}
	
	void Write(const char val)
	{
		const char arr[] = {val, '\0'};
		constexpr size_t len = sizeof arr;
		Write(arr, len);
	}

	// Writes range [val; val + len] to the static buffer by default. When the buffer on the stack
	// gets full or the range is too long for the static buffer, allocates a dynamic buffer,
	// copies existing content to the dynamic buffer and writes current and all next ranges to the
	// dynamic buffer
	void Write(const char* val, size_t len)
	{
		assert(val);
		
		if (is_static_buffer_)
		{
			if (len <= static_buffer_.size() - size_)
			{
				std::memcpy(static_buffer_.data() + size_, val, len);
				size_ += len - 1;
			}
			else
			{
				dynamic_buffer_.reserve(size_ + len);
				dynamic_buffer_.assign(static_buffer_.data(), size_);
				dynamic_buffer_.append(val, len - 1);
				is_static_buffer_ = false;
			}
		}
		else
		{
			dynamic_buffer_.append(val, len - 1);
		}
	}
	
	// Returns a string of either static or dynamic buffer. If it's dynamic buffer performs a move
	// operation, so it's no longer valid to write to the Writer
	std::string MoveString() const
	{
		if (is_static_buffer_)
		{
			return std::string(static_buffer_.data(), size_);
		}
		else
		{
			return std::move(dynamic_buffer_);
		}
	}
	
	// Returns a string of either static or dynamic buffer. It's valid to write to the Writer more
	// data.
	std::string CopyString() const
	{
		if (is_static_buffer_)
		{
			return std::string(static_buffer_.data(), size_);
		}
		else
		{
			return dynamic_buffer_;
		}
	}

private:
	// Whether we use static of dynamic buffer
    bool is_static_buffer_ = true;

	// The current size of the static_buffer_
	size_t size_ = 0;
	// Buffer on the stack to prevent dynamic allocation
	std::array<char, kStaticBufferSize> static_buffer_;
	
	// Dynamic buffer in case of the static buffer gets full or the input is too long for the
	// static buffer
	std::string dynamic_buffer_;
};

// Writes s to the writer. "{{" -> "{", "}}" -> "}"
inline void format_impl(Writer& writer, const char* s)
{
	assert(s);
	for (char c; (c = *s) != '\0'; s++)
	{
		switch (c)
		{
			case '{':
			{
				const char s_1 = *(s + 1);
				switch (s_1)
				{
					case '{':
					{
        	    	    s++;
						break;
        	    	}
					case '}':
					{
	  					throw FormatError("Argument index out of range");
					}
					default:
					{
	  					throw FormatError("Invalid format string");
        	    	}
				}
				break;
        	}
			case '}':
			{
				const char s_1 = *(s + 1);
				switch (s_1)
				{
					case '}':
					{
        	    	    s++;
						break;
        	    	}
					default:
					{
	  					throw FormatError("Unmatched '}' in format string");
        	    	}
				}
				break;
        	}
		}
		writer.Write(c);
    }    
}

// Writes s to the writer. "{{" -> "{", "}}" -> "}"
template <class T, class... Args>
inline void format_impl(Writer& writer, const char* s, const T& value, Args&&... args)
{
	assert(s);
	char c = *s;
	if (c == '\0')
	{
	  	throw FormatError("Extra arguments");
	}
	for (; (c = *s) != '\0'; s++)
	{
		switch (c)
		{
			case '{':
			{
				const char s_1 = *(s + 1);
				switch (s_1)
				{
					case '{':
					{
        	    	    s++;
						break;
        	    	}
					case '}':
					{
						writer.Write(value);
        	    	    s += 2;
        	    	    format_impl(writer, s, std::forward<Args>(args)...);
        	    	    return;
					}
					default:
					{
	  					throw FormatError("Invalid format string");
        	    	}
				}
				break;
        	}
			case '}':
			{
				const char s_1 = *(s + 1);
				switch (s_1)
				{
					case '}':
					{
        	    	    s++;
						break;
        	    	}
					default:
					{
	  					throw FormatError("Unmatched '}' in format string");
        	    	}
				}
				break;
        	}
		}
		writer.Write(c);
    }    
}

}  // namespace internal

// A wrapper around format_impl recursive functions
template <class... Args>
inline std::string format(const char* s, Args&&... args)
{
	internal::Writer writer;
	format_impl(writer, s, std::forward<Args>(args)...);
	return writer.MoveString();
}

namespace internal
{

// Used in user defined literals
struct Format {
  const char* str;

  template <class... Args>
  auto operator()(Args&&... args) const -> decltype(format(str, std::forward<Args>(args)...)) {
    return format(str, std::forward<Args>(args)...);
  }
};

}  // namespace internal

namespace literals
{

// User defined literals
inline internal::Format operator"" _format(const char* s, size_t) {
	return {s};
}

}  // namespace literals

}  // namespace Format

#endif  // RST_FORMAT_FORMAT_H_
