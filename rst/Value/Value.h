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

#ifndef RST_VALUE_VALUE_H_
#define RST_VALUE_VALUE_H_

#include <cstdint>
#include <functional>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "rst/Macros/Macros.h"
#include "rst/NotNull/NotNull.h"

namespace rst {

// A Chromium-like JSON Value class.
class Value {
 public:
  using String = std::string;
  using Array = std::vector<Value>;
  // std::less allows to use heterogeneous lookup.
  using Object = std::map<std::string, Value, std::less<>>;

  enum class Type : int8_t {
    kNull,
    kBool,
    kNumber,
    kString,
    kArray,
    kObject,
  };

  explicit Value(Type type);

  Value();  // A null value.
  explicit Value(bool value);
  explicit Value(int32_t value);
  // Can store |2^53 - 1| at maximum since it's a max safe integer that can
  // stored in JavaScript.
  explicit Value(int64_t value);
  explicit Value(double value);
  // Provides const char* overload since otherwise it will be implicitly
  // converted to bool.
  explicit Value(const char* value);
  explicit Value(String&& value);
  explicit Value(Array&& value);
  explicit Value(Object&& value);

  // Prevents Value(pointer) from accidentally producing a bool.
  explicit Value(void*) = delete;

  Value(Value&& rhs);

  ~Value();

  Value& operator=(Value&& rhs);

  Value Clone() const;

  static Array Clone(const Array& array);
  static Object Clone(const Object& object);

  Type type() const { return type_; }

  bool IsNull() const { return type() == Type::kNull; }
  bool IsBool() const { return type() == Type::kBool; }
  bool IsNumber() const { return type() == Type::kNumber; }
  bool IsInt64() const;
  bool IsInt() const;
  bool IsString() const { return type() == Type::kString; }
  bool IsArray() const { return type() == Type::kArray; }
  bool IsObject() const { return type() == Type::kObject; }

  bool GetBool() const;
  int64_t GetInt64() const;
  int GetInt() const;
  double GetDouble() const;
  const String& GetString() const;
  String& GetString();
  const Array& GetArray() const;
  Array& GetArray();
  const Object& GetObject() const;
  Object& GetObject();

  Nullable<const Value*> FindKey(std::string_view key) const;
  Nullable<Value*> FindKey(std::string_view key);

  Nullable<const Value*> FindKeyOfType(std::string_view key, Type type) const;
  Nullable<Value*> FindKeyOfType(std::string_view key, Type type);

  std::optional<bool> FindBoolKey(std::string_view key) const;
  std::optional<int64_t> FindInt64Key(std::string_view key) const;
  std::optional<int> FindIntKey(std::string_view key) const;
  std::optional<double> FindDoubleKey(std::string_view key) const;

  Nullable<const String*> FindStringKey(std::string_view key) const;
  Nullable<const Value*> FindArrayKey(std::string_view key) const;
  Nullable<const Value*> FindObjectKey(std::string_view key) const;

  NotNull<Value*> SetKey(std::string&& key, Value&& value);

  bool RemoveKey(const std::string& key);

  // Sets the |value| associated with the given |path| starting from this
  // object. A |path| has the form "<key>" or "<key>.<key>.[...]", where "."
  // indexes into the next Value down. Obviously, "." can't be used within a
  // key, but there are no other restrictions on keys. If the key at any step
  // of the way doesn't exist, or exists but isn't an Object, a new Value will
  // be created and attached to the path in that location.
  NotNull<Value*> SetPath(std::string_view path, Value&& value);

  // Finds the value associated with the given |path| starting from this
  // object. A |path| has the form "<key>" or "<key>.<key>.[...]", where "."
  // indexes into the next Value down. If the |path| can be resolved
  // successfully, the value for the last key in the |path| will be returned.
  // Otherwise, it will return nullptr.
  Nullable<const Value*> FindPath(std::string_view path) const;
  Nullable<Value*> FindPath(std::string_view path);

 private:
  friend bool operator==(const Value& lhs, const Value& rhs);
  friend bool operator<(const Value& lhs, const Value& rhs);

  void MoveConstruct(Value&& rhs);
  void MoveAssign(Value&& rhs);
  void Cleanup();

  Type type_ = Type::kNull;
  union {
    bool bool_;
    double number_;
    String string_;
    Array array_;
    Object object_;
  };

  RST_DISALLOW_COPY_AND_ASSIGN(Value);
};

bool operator==(const Value& lhs, const Value& rhs);
bool operator!=(const Value& lhs, const Value& rhs);
bool operator<(const Value& lhs, const Value& rhs);
bool operator>(const Value& lhs, const Value& rhs);
bool operator<=(const Value& lhs, const Value& rhs);
bool operator>=(const Value& lhs, const Value& rhs);

}  // namespace rst

#endif  // RST_VALUE_VALUE_H_
