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

#include "rst/value/value.h"

namespace rst {

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

Value::Value(Value&& other) noexcept { MoveConstruct(std::move(other)); }

Value::~Value() { Cleanup(); }

Value& Value::operator=(Value&& rhs) noexcept {
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

  RST_NOTREACHED();
  return Value();
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

Nullable<const Value*> Value::FindKey(const std::string_view key) const {
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

NotNull<Value*> Value::SetKey(std::string&& key, Value&& value) {
  RST_DCHECK(IsObject());
  auto ret = object_.insert_or_assign(std::move(key), std::move(value));
  return &ret.first->second;
}

bool Value::RemoveKey(const std::string_view key) {
  RST_DCHECK(IsObject());
  const auto it = object_.find(key);
  if (it == object_.cend())
    return false;

  object_.erase(it);
  return true;
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

    RST_DCHECK(child_object != nullptr);
    current_object = child_object;
    current_path = current_path.substr(delimiter_position + 1);
  }

  return current_object->SetKey(std::string(current_path), std::move(value));
}

Nullable<const Value*> Value::FindPath(const std::string_view path) const {
  RST_DCHECK(IsObject());

  auto current_path = path;
  NotNull<const Value*> current_object = this;
  for (auto delimiter_position = current_path.find('.');
       delimiter_position != std::string_view::npos;
       delimiter_position = current_path.find('.')) {
    const auto key = current_path.substr(0, delimiter_position);
    auto child_object = current_object->FindKeyOfType(key, Type::kObject);
    if (child_object == nullptr)
      return nullptr;

    current_object = child_object;
    current_path = current_path.substr(delimiter_position + 1);
  }

  return current_object->FindKey(current_path);
}

void Value::MoveConstruct(Value&& other) {
  type_ = other.type_;

  switch (other.type_) {
    case Type::kNull:
      break;
    case Type::kBool:
      bool_ = other.bool_;
      break;
    case Type::kNumber:
      number_ = other.number_;
      break;
    case Type::kString:
      new (&string_) String(std::move(other.string_));
      break;
    case Type::kArray:
      new (&array_) Array(std::move(other.array_));
      break;
    case Type::kObject:
      new (&object_) Object(std::move(other.object_));
      break;
  }
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
      return std::fabs(lhs.number_ - rhs.number_) <
             std::numeric_limits<double>::epsilon();
    case Value::Type::kString:
      return lhs.string_ == rhs.string_;
    case Value::Type::kArray:
      return lhs.array_ == rhs.array_;
    case Value::Type::kObject:
      return lhs.object_ == rhs.object_;
  }

  RST_NOTREACHED();
  return false;
}

bool operator<(const Value& lhs, const Value& rhs) {
  if (lhs.type_ != rhs.type_)
    return lhs.type_ < rhs.type_;

  switch (lhs.type_) {
    case Value::Type::kNull:
      return false;
    case Value::Type::kBool:
      return static_cast<int>(lhs.bool_) < static_cast<int>(rhs.bool_);
    case Value::Type::kNumber:
      return lhs.number_ < rhs.number_;
    case Value::Type::kString:
      return lhs.string_ < rhs.string_;
    case Value::Type::kArray:
      return lhs.array_ < rhs.array_;
    case Value::Type::kObject:
      return lhs.object_ < rhs.object_;
  }

  RST_NOTREACHED();
  return false;
}

}  // namespace rst
