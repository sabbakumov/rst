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

#include "rst/Value/Value.h"

#include <cmath>
#include <limits>
#include <utility>

#include "rst/Check/Check.h"

namespace rst {
namespace {

constexpr int64_t kMaxSafeInteger =
    (int64_t{1} << std::numeric_limits<double>::digits) - 1;
static_assert(kMaxSafeInteger == (int64_t{1} << 53) - 1);

}  // namespace

Value::Value(const Type type) : type_(type) {
  switch (type_) {
    case Type::kNull:
      return;
    case Type::kBool:
      bool_ = false;
      return;
    case Type::kNumber:
      number_ = 0.0;
      return;
    case Type::kString:
      new (&string_) String();
      return;
    case Type::kArray:
      new (&array_) Array();
      return;
    case Type::kObject:
      new (&object_) Object();
      return;
  }
}

Value::Value() {}

Value::Value(const bool value) : type_(Type::kBool), bool_(value) {}

Value::Value(const int32_t value) : Value(static_cast<int64_t>(value)) {}

Value::Value(const int64_t value) : Value(static_cast<double>(value)) {
  RST_DCHECK(std::abs(value) <= kMaxSafeInteger);
}

Value::Value(const double value) : type_(Type::kNumber), number_(value) {
  RST_DCHECK(std::isfinite(value) &&
             "Non-finite (i.e. NaN or positive/negative infinity) values "
             "cannot be represented in JSON");
}

Value::Value(const char* value) : Value(String(value)) {
  RST_DCHECK(value != nullptr);
}

Value::Value(String&& value)
    : type_(Type::kString), string_(std::move(value)) {}

Value::Value(Array&& value) : type_(Type::kArray), array_(std::move(value)) {}

Value::Value(Object&& value)
    : type_(Type::kObject), object_(std::move(value)) {}

Value::Value(Value&& rhs) { MoveConstruct(std::move(rhs)); }

Value::~Value() { Cleanup(); }

Value& Value::operator=(Value&& rhs) {
  if (this == &rhs)
    return *this;

  if (type_ == rhs.type_) {
    MoveAssign(std::move(rhs));
  } else {
    Cleanup();
    MoveConstruct(std::move(rhs));
  }

  return *this;
}

Value Value::Clone() const {
  switch (type_) {
    case Type::kNull:
      return Value();
    case Type::kBool:
      return Value(bool_);
    case Type::kNumber:
      return Value(number_);
    case Type::kString:
      return Value(String(string_));
    case Type::kArray:
      return Value(Clone(array_));
    case Type::kObject:
      return Value(Clone(object_));
  }
}

// static
Value::Array Value::Clone(const Array& array) {
  Array result;
  result.reserve(array.size());
  for (const auto& val : array)
    result.emplace_back(val.Clone());
  return result;
}

// static
Value::Object Value::Clone(const Object& object) {
  Object result;
  for (const auto& pair : object)
    result.try_emplace(result.cend(), pair.first, pair.second.Clone());
  return result;
}

bool Value::IsInt64() const {
  return IsNumber() && (std::abs(number_) <= kMaxSafeInteger);
}

bool Value::IsInt() const {
  return IsNumber() && (number_ >= std::numeric_limits<int>::min()) &&
         (number_ <= std::numeric_limits<int>::max());
}

bool Value::GetBool() const {
  RST_DCHECK(IsBool());
  return bool_;
}

int64_t Value::GetInt64() const {
  RST_DCHECK(IsInt64());
  return static_cast<int64_t>(number_);
}

int Value::GetInt() const {
  RST_DCHECK(IsInt());
  return static_cast<int>(number_);
}

double Value::GetDouble() const {
  RST_DCHECK(IsNumber());
  return number_;
}

const Value::String& Value::GetString() const {
  RST_DCHECK(IsString());
  return string_;
}

Value::String& Value::GetString() {
  RST_DCHECK(IsString());
  return string_;
}

const Value::Array& Value::GetArray() const {
  RST_DCHECK(IsArray());
  return array_;
}

Value::Array& Value::GetArray() {
  RST_DCHECK(IsArray());
  return array_;
}

const Value::Object& Value::GetObject() const {
  RST_DCHECK(IsObject());
  return object_;
}

Value::Object& Value::GetObject() {
  RST_DCHECK(IsObject());
  return object_;
}

Nullable<const Value*> Value::FindKey(const std::string_view key) const {
  RST_DCHECK(IsObject());
  const auto it = object_.find(key);
  if (it == object_.cend())
    return nullptr;
  return &it->second;
}

Nullable<Value*> Value::FindKey(const std::string_view key) {
  RST_DCHECK(IsObject());
  const auto it = object_.find(key);
  if (it == object_.cend())
    return nullptr;
  return &it->second;
}

Nullable<const Value*> Value::FindKeyOfType(const std::string_view key,
                                            const Type type) const {
  const auto result = FindKey(key);
  if (result == nullptr)
    return nullptr;

  if (result->type_ != type)
    return nullptr;

  return result;
}

Nullable<Value*> Value::FindKeyOfType(const std::string_view key,
                                      const Type type) {
  const auto result = FindKey(key);
  if (result == nullptr)
    return nullptr;

  if (result->type_ != type)
    return nullptr;

  return result;
}

std::optional<bool> Value::FindBoolKey(const std::string_view key) const {
  const auto result = FindKeyOfType(key, Type::kBool);
  if (result == nullptr)
    return std::nullopt;
  return result->bool_;
}

std::optional<int64_t> Value::FindInt64Key(const std::string_view key) const {
  const auto result = FindKey(key);
  if (result == nullptr)
    return std::nullopt;

  if (!result->IsInt64())
    return std::nullopt;

  return static_cast<int64_t>(result->number_);
}

std::optional<int> Value::FindIntKey(const std::string_view key) const {
  const auto result = FindKey(key);
  if (result == nullptr)
    return std::nullopt;

  if (!result->IsInt())
    return std::nullopt;

  return static_cast<int>(result->number_);
}

std::optional<double> Value::FindDoubleKey(const std::string_view key) const {
  const auto result = FindKeyOfType(key, Type::kNumber);
  if (result == nullptr)
    return std::nullopt;
  return result->number_;
}

Nullable<const Value::String*> Value::FindStringKey(
    const std::string_view key) const {
  const auto result = FindKeyOfType(key, Type::kString);
  if (result == nullptr)
    return nullptr;
  return &result->string_;
}

Nullable<const Value*> Value::FindArrayKey(const std::string_view key) const {
  return FindKeyOfType(key, Type::kArray);
}

Nullable<const Value*> Value::FindObjectKey(const std::string_view key) const {
  return FindKeyOfType(key, Type::kObject);
}

NotNull<Value*> Value::SetKey(std::string&& key, Value&& value) {
  RST_DCHECK(IsObject());
  auto ret = object_.insert_or_assign(std::move(key), std::move(value));
  return &ret.first->second;
}

bool Value::RemoveKey(const std::string& key) {
  RST_DCHECK(IsObject());
  return object_.erase(key) != 0;
}

NotNull<Value*> Value::SetPath(const std::string_view path, Value&& value) {
  RST_DCHECK(IsObject());

  auto current_path = path;
  NotNull<Value*> current_object = this;
  for (auto delimiter_position = current_path.find('.');
       delimiter_position != std::string_view::npos;
       delimiter_position = current_path.find('.')) {
    const auto key = current_path.substr(0, delimiter_position);
    auto child_object = current_object->FindKeyOfType(key, Type::kObject);
    if (child_object == nullptr) {
      child_object =
          current_object->SetKey(std::string(key), Value(Type::kObject));
    }

    current_object = child_object;
    current_path = current_path.substr(delimiter_position + 1);
  }

  return current_object->SetKey(std::string(current_path), std::move(value));
}

void Value::MoveConstruct(Value&& rhs) {
  switch (rhs.type_) {
    case Type::kNull:
      break;
    case Type::kBool:
      bool_ = rhs.bool_;
      break;
    case Type::kNumber:
      number_ = rhs.number_;
      break;
    case Type::kString:
      new (&string_) String(std::move(rhs.string_));
      break;
    case Type::kArray:
      new (&array_) Array(std::move(rhs.array_));
      break;
    case Type::kObject:
      new (&object_) Object(std::move(rhs.object_));
      break;
  }

  type_ = rhs.type_;
}

void Value::MoveAssign(Value&& rhs) {
  RST_DCHECK(type_ == rhs.type_);

  switch (rhs.type_) {
    case Type::kNull:
      return;
    case Type::kBool:
      bool_ = rhs.bool_;
      return;
    case Type::kNumber:
      number_ = rhs.number_;
      return;
    case Type::kString:
      string_ = std::move(rhs.string_);
      return;
    case Type::kArray:
      array_ = std::move(rhs.array_);
      return;
    case Type::kObject:
      object_ = std::move(rhs.object_);
      return;
  }
}

void Value::Cleanup() {
  switch (type_) {
    case Type::kNull:
    case Type::kBool:
    case Type::kNumber:
      return;
    case Type::kString:
      string_.~String();
      return;
    case Type::kArray:
      array_.~Array();
      return;
    case Type::kObject:
      object_.~Object();
      return;
  }
}

bool operator==(const Value& lhs, const Value& rhs) {
  if (lhs.type_ != rhs.type_)
    return false;

  switch (lhs.type_) {
    case Value::Type::kNull:
      return true;
    case Value::Type::kBool:
      return lhs.bool_ == rhs.bool_;
    case Value::Type::kNumber:
      return lhs.number_ == rhs.number_;
    case Value::Type::kString:
      return lhs.string_ == rhs.string_;
    case Value::Type::kArray:
      return lhs.array_ == rhs.array_;
    case Value::Type::kObject:
      return lhs.object_ == rhs.object_;
  }
}

bool operator!=(const Value& lhs, const Value& rhs) { return !(lhs == rhs); }

bool operator<(const Value& lhs, const Value& rhs) {
  if (lhs.type_ != rhs.type_)
    return lhs.type_ < rhs.type_;

  switch (lhs.type_) {
    case Value::Type::kNull:
      return false;
    case Value::Type::kBool:
      return lhs.bool_ < rhs.bool_;
    case Value::Type::kNumber:
      return lhs.number_ < rhs.number_;
    case Value::Type::kString:
      return lhs.string_ < rhs.string_;
    case Value::Type::kArray:
      return lhs.array_ < rhs.array_;
    case Value::Type::kObject:
      return lhs.object_ < rhs.object_;
  }
}

bool operator>(const Value& lhs, const Value& rhs) { return rhs < lhs; }

bool operator<=(const Value& lhs, const Value& rhs) { return !(rhs < lhs); }

bool operator>=(const Value& lhs, const Value& rhs) { return !(lhs < rhs); }

}  // namespace rst
