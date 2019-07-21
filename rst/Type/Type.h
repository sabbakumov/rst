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

#include <functional>
#include <utility>

namespace rst {

// Chromium-like StrongAlias type.
template <class TagType, class UnderlyingType>
class Type {
 public:
  Type() = default;

  explicit Type(const UnderlyingType& value) : value_(value) {}
  explicit Type(UnderlyingType&& value) : value_(std::move(value)) {}

  Type(const Type&) = default;
  Type(Type&&) = default;

  ~Type() = default;

  Type& operator=(const Type&) = default;
  Type& operator=(Type&&) = default;

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
