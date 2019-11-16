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

#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"

namespace rst {

// A Chromium-like JSON Value class.

// This is a recursive data storage class intended for storing settings and
// other persistable data.
//
// A Value represents something that can be stored in JSON or passed to/from
// JavaScript. As such, it is not a generalized variant type, since only the
// types supported by JavaScript/JSON are supported.
//
// In particular this means that there is no support for int64_t or unsigned
// numbers. Writing JSON with such types would violate the spec. If you need
// something like this, either use a double or make a string value containing
// the number you want.
class Value {
 public:
  using String = std::string;
  using Array = std::vector<Value>;
  // std::less allows to use heterogeneous lookup.
  using Object = std::map<std::string, Value, std::less<>>;

  // Types supported by JSON.
  enum class Type : int8_t {
    kNull,
    kBool,
    kNumber,
    kString,
    kArray,
    kObject,
  };

  // Constructs the default value of a given type.
  explicit Value(Type type);

  Value();  // A null value.
  explicit Value(bool value);
  explicit Value(int32_t value);
  // Can store |2^53 - 1| at maximum since it's a max safe integer that can be
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

  Value(Value&& other) noexcept;

  ~Value();

  Value& operator=(Value&& rhs) noexcept;

  // Creates an explicit copy.
  Value Clone() const;
  static Array Clone(const Array& array);
  static Object Clone(const Object& object);

  // Returns the type of the stored value.
  Type type() const { return type_; }

  // Returns true if the current value represents a given type.
  bool IsNull() const { return type() == Type::kNull; }
  bool IsBool() const { return type() == Type::kBool; }
  bool IsNumber() const { return type() == Type::kNumber; }
  bool IsInt64() const;
  bool IsInt() const;
  bool IsString() const { return type() == Type::kString; }
  bool IsArray() const { return type() == Type::kArray; }
  bool IsObject() const { return type() == Type::kObject; }

  // These will all assert that the type matches.
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

  // Looks up |key| in the underlying dictionary. Asserts that the value is
  // object.
  Nullable<const Value*> FindKey(std::string_view key) const;
  Nullable<Value*> FindKey(std::string_view key);

  // Similar to FindKey(), but it also requires the found value to have type
  // |type|. Asserts that the value is object.
  Nullable<const Value*> FindKeyOfType(std::string_view key, Type type) const;
  Nullable<Value*> FindKeyOfType(std::string_view key, Type type);

  // These are convenience forms of FindKeyOfType(). Asserts that the value is
  // object.
  std::optional<bool> FindBoolKey(std::string_view key) const;
  std::optional<int64_t> FindInt64Key(std::string_view key) const;
  std::optional<int> FindIntKey(std::string_view key) const;
  std::optional<double> FindDoubleKey(std::string_view key) const;
  Nullable<const String*> FindStringKey(std::string_view key) const;
  Nullable<const Value*> FindArrayKey(std::string_view key) const;
  Nullable<const Value*> FindObjectKey(std::string_view key) const;

  // Looks up |key| in the underlying dictionary and sets the mapped value to
  // |value|. If |key| could not be found, a new element is inserted. A pointer
  // to the modified item is returned. Asserts that the value is object.
  NotNull<Value*> SetKey(std::string&& key, Value&& value);

  // Attempts to remove the value associated with |key|. In case of failure,
  // e.g. the |key| does not exist, false is returned and the underlying
  // dictionary is not changed. In case of success, |key| is deleted from the
  // dictionary and the method returns true. Asserts that the value is object.
  bool RemoveKey(std::string_view key);

  // Sets the |value| associated with the given |path| starting from this
  // object. A |path| has the form "<key>" or "<key>.<key>.[...]", where "."
  // indexes into the next Value down. Obviously, "." can't be used within a
  // key, but there are no other restrictions on keys. If the key at any step
  // of the way doesn't exist, or exists but isn't an Object, a new Value will
  // be created and attached to the path in that location. A pointer to the
  // modified item is returned. Asserts that the value is object.
  NotNull<Value*> SetPath(std::string_view path, Value&& value);

  // Finds the value associated with the given |path| starting from this
  // object. A |path| has the form "<key>" or "<key>.<key>.[...]", where "."
  // indexes into the next Value down. Asserts that the value is object.
  Nullable<const Value*> FindPath(std::string_view path) const;
  Nullable<Value*> FindPath(std::string_view path);

 private:
  friend bool operator==(const Value& lhs, const Value& rhs);
  friend bool operator<(const Value& lhs, const Value& rhs);

  void MoveConstruct(Value&& other);
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
