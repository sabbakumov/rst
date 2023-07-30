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

#ifndef RST_TYPE_TYPE_H_
#define RST_TYPE_TYPE_H_

#include <cstddef>
#include <functional>
#include <utility>

namespace rst {

// A Chromium-like `StrongAlias` type.
//
// A type-safe alternative for a typedef or a using directive like in Golang.
//
// The motivation is to disallow several classes of errors:
//
//   using Orange = int;
//   using Apple = int;
//   Apple apple(2);
//   Orange orange = apple;  // Orange should not be able to become an Apple.
//   Orange x = orange + apple;  // Shouldn't add Oranges and Apples.
//   if (orange > apple);  // Shouldn't compare Apples to Oranges.
//   void foo(Orange);
//   void foo(Apple);  // Redefinition.
//
// Type may instead be used as follows:
//
//   #include "rst/type/type.h"
//
//   using Orange = rst::Type<class OrangeTag, int>;
//   using Apple = rst::Type<class AppleTag, int>;
//   Apple apple(2);
//   Orange orange = apple;  // Does not compile.
//   Orange other_orange = orange;  // Compiles, types match.
//   Orange x = orange + apple;  // Does not compile.
//   Orange y = Orange(orange.value() + apple.value());  // Compiles.
//   if (orange > apple);  // Does not compile.
//   if (orange > other_orange);  // Compiles.
//   void foo(Orange);
//   void foo(Apple);  // Compiles into separate overload.
//
// TagType is an empty tag class (also called "phantom type") that only serves
// the type system to differentiate between different instantiations of the
// template.
template <class TagType, class UnderlyingType>
class Type {
 public:
  Type() = default;

  explicit Type(const UnderlyingType& value) : value_(value) {}
  explicit Type(UnderlyingType&& value) noexcept : value_(std::move(value)) {}

  Type(const Type&) = default;
  Type(Type&&) noexcept = default;

  ~Type() = default;

  Type& operator=(const Type&) = default;
  Type& operator=(Type&&) noexcept = default;

  const UnderlyingType& value() const { return value_; }
  explicit operator UnderlyingType() const { return value_; }

  bool operator==(const Type& other) const { return value_ == other.value_; }
  bool operator!=(const Type& other) const { return value_ != other.value_; }
  bool operator<(const Type& other) const { return value_ < other.value_; }
  bool operator<=(const Type& other) const { return value_ <= other.value_; }
  bool operator>(const Type& other) const { return value_ > other.value_; }
  bool operator>=(const Type& other) const { return value_ >= other.value_; }

 protected:
  UnderlyingType value_;
};

}  // namespace rst

namespace std {

template <class TagType, class UnderlyingType>
struct hash<rst::Type<TagType, UnderlyingType>> {
  size_t operator()(const rst::Type<TagType, UnderlyingType>& type) const {
    return std::hash<UnderlyingType>()(type.value());
  }
};

}  // namespace std

#endif  // RST_TYPE_TYPE_H_
