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

#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

namespace rst {
namespace {

constexpr int64_t kMaxSafeInteger =
    (int64_t{1} << std::numeric_limits<double>::digits) - 1;

}  // namespace

TEST(ValueTest, ConstructFromType) {
  Value null(Value::Type::kNull);
  EXPECT_EQ(null.type(), Value::Type::kNull);
  EXPECT_TRUE(null.IsNull());
  EXPECT_DEATH(null.GetBool(), "");
  EXPECT_DEATH(null.GetInt64(), "");
  EXPECT_DEATH(null.GetInt(), "");
  EXPECT_DEATH(null.GetDouble(), "");
  EXPECT_DEATH(null.GetString(), "");
  EXPECT_DEATH(const_cast<const Value&>(null).GetString(), "");
  EXPECT_DEATH(null.GetArray(), "");
  EXPECT_DEATH(const_cast<const Value&>(null).GetArray(), "");
  EXPECT_DEATH(null.GetObject(), "");
  EXPECT_DEATH(const_cast<const Value&>(null).GetObject(), "");

  Value bool_value(Value::Type::kBool);
  ASSERT_EQ(bool_value.type(), Value::Type::kBool);
  EXPECT_TRUE(bool_value.IsBool());
  EXPECT_EQ(bool_value.GetBool(), false);
  EXPECT_NO_FATAL_FAILURE(bool_value.GetBool());
  EXPECT_DEATH(bool_value.GetInt64(), "");
  EXPECT_DEATH(bool_value.GetInt(), "");
  EXPECT_DEATH(bool_value.GetDouble(), "");
  EXPECT_DEATH(bool_value.GetString(), "");
  EXPECT_DEATH(const_cast<const Value&>(bool_value).GetString(), "");
  EXPECT_DEATH(bool_value.GetArray(), "");
  EXPECT_DEATH(const_cast<const Value&>(bool_value).GetArray(), "");
  EXPECT_DEATH(bool_value.GetObject(), "");
  EXPECT_DEATH(const_cast<const Value&>(bool_value).GetObject(), "");

  Value number(Value::Type::kNumber);
  ASSERT_EQ(number.type(), Value::Type::kNumber);
  EXPECT_TRUE(number.IsNumber());
  EXPECT_EQ(number.GetDouble(), 0.0);
  EXPECT_DEATH(number.GetBool(), "");
  EXPECT_NO_FATAL_FAILURE(number.GetInt64());
  EXPECT_NO_FATAL_FAILURE(number.GetInt());
  EXPECT_NO_FATAL_FAILURE(number.GetDouble());
  EXPECT_DEATH(number.GetString(), "");
  EXPECT_DEATH(const_cast<const Value&>(number).GetString(), "");
  EXPECT_DEATH(number.GetArray(), "");
  EXPECT_DEATH(const_cast<const Value&>(number).GetArray(), "");
  EXPECT_DEATH(number.GetObject(), "");
  EXPECT_DEATH(const_cast<const Value&>(number).GetObject(), "");

  Value string(Value::Type::kString);
  ASSERT_EQ(string.type(), Value::Type::kString);
  EXPECT_TRUE(string.IsString());
  EXPECT_EQ(string.GetString(), Value::String());
  EXPECT_DEATH(string.GetBool(), "");
  EXPECT_DEATH(string.GetInt64(), "");
  EXPECT_DEATH(string.GetInt(), "");
  EXPECT_DEATH(string.GetDouble(), "");
  EXPECT_NO_FATAL_FAILURE(string.GetString());
  EXPECT_NO_FATAL_FAILURE(const_cast<const Value&>(string).GetString());
  EXPECT_DEATH(string.GetArray(), "");
  EXPECT_DEATH(const_cast<const Value&>(string).GetArray(), "");
  EXPECT_DEATH(string.GetObject(), "");
  EXPECT_DEATH(const_cast<const Value&>(string).GetObject(), "");

  Value array(Value::Type::kArray);
  ASSERT_EQ(array.type(), Value::Type::kArray);
  EXPECT_TRUE(array.IsArray());
  EXPECT_EQ(array.GetArray(), Value::Array());
  EXPECT_DEATH(array.GetBool(), "");
  EXPECT_DEATH(array.GetInt64(), "");
  EXPECT_DEATH(array.GetInt(), "");
  EXPECT_DEATH(array.GetDouble(), "");
  EXPECT_DEATH(array.GetString(), "");
  EXPECT_DEATH(const_cast<const Value&>(array).GetString(), "");
  EXPECT_NO_FATAL_FAILURE(array.GetArray());
  EXPECT_NO_FATAL_FAILURE(const_cast<const Value&>(array).GetArray());
  EXPECT_DEATH(array.GetObject(), "");
  EXPECT_DEATH(const_cast<const Value&>(array).GetObject(), "");

  Value object(Value::Type::kObject);
  ASSERT_EQ(object.type(), Value::Type::kObject);
  EXPECT_TRUE(object.IsObject());
  EXPECT_EQ(object.GetObject(), Value::Object());
  EXPECT_DEATH(object.GetBool(), "");
  EXPECT_DEATH(object.GetInt64(), "");
  EXPECT_DEATH(object.GetInt(), "");
  EXPECT_DEATH(object.GetDouble(), "");
  EXPECT_DEATH(object.GetString(), "");
  EXPECT_DEATH(const_cast<const Value&>(object).GetString(), "");
  EXPECT_DEATH(object.GetArray(), "");
  EXPECT_DEATH(const_cast<const Value&>(object).GetArray(), "");
  EXPECT_NO_FATAL_FAILURE(object.GetObject());
  EXPECT_NO_FATAL_FAILURE(const_cast<const Value&>(object).GetObject());
}

TEST(ValueTest, ConstructBool) {
  Value true_value(true);
  ASSERT_EQ(true_value.type(), Value::Type::kBool);
  EXPECT_TRUE(true_value.GetBool());

  Value false_value(false);
  ASSERT_EQ(false_value.type(), Value::Type::kBool);
  EXPECT_FALSE(false_value.GetBool());
}

TEST(ValueTest, ConstructInt) {
  Value value(-37);
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  EXPECT_EQ(value.GetInt(), -37);

  Value value_int32(int32_t{-37});
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  EXPECT_EQ(value.GetInt64(), -37);

  Value value_int64(int64_t{-37});
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  EXPECT_EQ(value.GetInt64(), -37);
}

TEST(ValueTest, ConstructBigInt64) {
  Value value(kMaxSafeInteger);
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  ASSERT_TRUE(value.IsInt64());
  EXPECT_EQ(value.GetInt64(), kMaxSafeInteger);

  value = Value(-kMaxSafeInteger);
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  ASSERT_TRUE(value.IsInt64());
  EXPECT_EQ(value.GetInt64(), -kMaxSafeInteger);

  EXPECT_DEATH(Value(kMaxSafeInteger + 1), "");
  EXPECT_DEATH(Value(-kMaxSafeInteger - 1), "");
}

TEST(ValueTest, ConstructBigInt) {
  Value value(int64_t{std::numeric_limits<int>::max()});
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  ASSERT_TRUE(value.IsInt());
  EXPECT_EQ(value.GetInt(), std::numeric_limits<int>::max());

  value = Value(int64_t{std::numeric_limits<int>::min()});
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  ASSERT_TRUE(value.IsInt());
  EXPECT_EQ(value.GetInt(), std::numeric_limits<int>::min());

  value = Value(int64_t{std::numeric_limits<int>::max()} + 1);
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  ASSERT_FALSE(value.IsInt());
  ASSERT_TRUE(value.IsInt64());
  EXPECT_EQ(value.GetInt64(), int64_t{std::numeric_limits<int>::max()} + 1);

  value = Value(int64_t{std::numeric_limits<int>::min()} - 1);
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  ASSERT_FALSE(value.IsInt());
  ASSERT_TRUE(value.IsInt64());
  EXPECT_EQ(value.GetInt64(), int64_t{std::numeric_limits<int>::min()} - 1);
}

TEST(ValueTest, ConstructDouble) {
  Value value(-4.655);
  ASSERT_EQ(value.type(), Value::Type::kNumber);
  EXPECT_EQ(value.GetDouble(), -4.655);
}

TEST(ValueTest, ConstructWrongDouble) {
  EXPECT_DEATH(Value value(std::numeric_limits<double>::infinity()), "");
  EXPECT_DEATH(Value value(std::numeric_limits<double>::quiet_NaN()), "");
}

TEST(ValueTest, ConstructStringFromConstCharPtr) {
  Value value("foobar");
  ASSERT_EQ(value.type(), Value::Type::kString);
  EXPECT_EQ(value.GetString(), "foobar");

  const char* null = nullptr;
  EXPECT_DEATH((Value(null)), "");
}

TEST(ValueTest, ConstructStringFromStringView) {
  Value value(Value::String(std::string_view("foobar")));
  ASSERT_EQ(value.type(), Value::Type::kString);
  EXPECT_EQ(value.GetString(), "foobar");
}

TEST(ValueTest, ConstructStringFromStdStringRRef) {
  Value value(Value::String("foobar"));
  ASSERT_EQ(value.type(), Value::Type::kString);
  EXPECT_EQ(value.GetString(), "foobar");
}

TEST(ValueTest, ConstructArray) {
  Value::Array storage;
  storage.emplace_back("foo");
  {
    Value value(Value::Clone(storage));
    ASSERT_EQ(value.type(), Value::Type::kArray);
    ASSERT_EQ(value.GetArray().size(), 1);
    ASSERT_EQ(value.GetArray()[0].type(), Value::Type::kString);
    EXPECT_EQ(value.GetArray()[0].GetString(), "foo");
  }

  storage.back() = Value("bar");
  {
    Value value(std::move(storage));
    ASSERT_EQ(value.type(), Value::Type::kArray);
    ASSERT_EQ(value.GetArray().size(), 1);
    ASSERT_EQ(value.GetArray()[0].type(), Value::Type::kString);
    EXPECT_EQ(value.GetArray()[0].GetString(), "bar");
  }
}

TEST(ValueTest, ConstructObject) {
  Value::Object storage;
  storage.emplace("foo", "bar");
  {
    Value value(Value::Clone(storage));
    ASSERT_EQ(value.type(), Value::Type::kObject);
    const auto key = value.FindKey("foo");
    ASSERT_NE(key, nullptr);
    ASSERT_EQ(key->type(), Value::Type::kString);
    EXPECT_EQ(key->GetString(), "bar");
  }

  storage["foo"] = Value("baz");
  {
    Value value(std::move(storage));
    ASSERT_EQ(value.type(), Value::Type::kObject);
    const auto key = value.FindKey("foo");
    ASSERT_NE(key, nullptr);
    ASSERT_EQ(key->type(), Value::Type::kString);
    EXPECT_EQ(key->GetString(), "baz");
  }
}

TEST(ValueTest, CopyBool) {
  Value true_value(true);
  Value copied_true_value(true_value.Clone());
  ASSERT_EQ(copied_true_value.type(), true_value.type());
  EXPECT_EQ(copied_true_value.GetBool(), true_value.GetBool());

  Value false_value(false);
  Value copied_false_value(false_value.Clone());
  ASSERT_EQ(copied_false_value.type(), false_value.type());
  EXPECT_EQ(copied_false_value.GetBool(), false_value.GetBool());

  Value blank;

  blank = true_value.Clone();
  ASSERT_EQ(blank.type(), true_value.type());
  EXPECT_EQ(blank.GetBool(), true_value.GetBool());

  blank = false_value.Clone();
  ASSERT_EQ(blank.type(), false_value.type());
  EXPECT_EQ(blank.GetBool(), false_value.GetBool());
}

TEST(ValueTest, CopyInt) {
  Value value(74);
  Value copied_value(value.Clone());
  ASSERT_EQ(copied_value.type(), value.type());
  EXPECT_EQ(copied_value.GetInt(), value.GetInt());

  Value blank;

  blank = value.Clone();
  ASSERT_EQ(blank.type(), value.type());
  EXPECT_EQ(blank.GetInt(), value.GetInt());
}

TEST(ValueTest, CopyInt32) {
  Value value(int32_t{74});
  Value copied_value(value.Clone());
  ASSERT_EQ(copied_value.type(), value.type());
  EXPECT_EQ(copied_value.GetInt64(), value.GetInt64());

  Value blank;

  blank = value.Clone();
  ASSERT_EQ(blank.type(), value.type());
  EXPECT_EQ(blank.GetInt64(), value.GetInt64());
}

TEST(ValueTest, CopyInt64) {
  Value value(int64_t{74});
  Value copied_value(value.Clone());
  ASSERT_EQ(copied_value.type(), value.type());
  EXPECT_EQ(copied_value.GetInt64(), value.GetInt64());

  Value blank;

  blank = value.Clone();
  ASSERT_EQ(blank.type(), value.type());
  EXPECT_EQ(blank.GetInt64(), value.GetInt64());
}

TEST(ValueTest, CopyDouble) {
  Value value(74.896);
  Value copied_value(value.Clone());
  ASSERT_EQ(copied_value.type(), value.type());
  EXPECT_EQ(copied_value.GetDouble(), value.GetDouble());

  Value blank;

  blank = value.Clone();
  ASSERT_EQ(blank.type(), value.type());
  EXPECT_EQ(blank.GetDouble(), value.GetDouble());
}

TEST(ValueTest, CopyString) {
  Value value("foobar");
  Value copied_value(value.Clone());
  ASSERT_EQ(copied_value.type(), value.type());
  EXPECT_EQ(copied_value.GetString(), value.GetString());

  Value blank;

  blank = value.Clone();
  ASSERT_EQ(blank.type(), value.type());
  EXPECT_EQ(blank.GetString(), value.GetString());
}

TEST(ValueTest, CopyArray) {
  Value::Array storage;
  storage.emplace_back(123);
  Value value(std::move(storage));

  Value copied_value(value.Clone());
  EXPECT_EQ(copied_value, value);

  Value blank;
  blank = value.Clone();
  EXPECT_EQ(blank, value);
}

TEST(ValueTest, CopyObject) {
  Value::Object storage;
  storage.emplace("Int", 123);
  Value value(std::move(storage));

  Value copied_value(value.Clone());
  EXPECT_EQ(copied_value, value);

  Value blank;
  blank = value.Clone();
  EXPECT_EQ(blank, value);
}

TEST(ValueTest, MoveBool) {
  Value true_value(true);
  Value moved_true_value(std::move(true_value));
  ASSERT_EQ(moved_true_value.type(), Value::Type::kBool);
  EXPECT_TRUE(moved_true_value.GetBool());

  Value false_value(false);
  Value moved_false_value(std::move(false_value));
  ASSERT_EQ(moved_false_value.type(), Value::Type::kBool);
  EXPECT_FALSE(moved_false_value.GetBool());

  Value blank;

  blank = Value(true);
  ASSERT_EQ(blank.type(), Value::Type::kBool);
  EXPECT_TRUE(blank.GetBool());

  blank = Value(false);
  ASSERT_EQ(blank.type(), Value::Type::kBool);
  EXPECT_FALSE(blank.GetBool());
}

TEST(ValueTest, MoveInt) {
  Value value(74);
  Value moved_value(std::move(value));
  ASSERT_EQ(moved_value.type(), Value::Type::kNumber);
  EXPECT_EQ(moved_value.GetInt(), 74);

  Value blank;

  blank = Value(47);
  ASSERT_EQ(blank.type(), Value::Type::kNumber);
  EXPECT_EQ(blank.GetInt(), 47);
}

TEST(ValueTest, MoveInt32) {
  Value value(int32_t{74});
  Value moved_value(std::move(value));
  ASSERT_EQ(moved_value.type(), Value::Type::kNumber);
  EXPECT_EQ(moved_value.GetInt64(), 74);

  Value blank;

  blank = Value(47);
  ASSERT_EQ(blank.type(), Value::Type::kNumber);
  EXPECT_EQ(blank.GetInt64(), 47);
}

TEST(ValueTest, MoveInt64) {
  Value value(int64_t{74});
  Value moved_value(std::move(value));
  ASSERT_EQ(moved_value.type(), Value::Type::kNumber);
  EXPECT_EQ(moved_value.GetInt64(), 74);

  Value blank;

  blank = Value(47);
  ASSERT_EQ(blank.type(), Value::Type::kNumber);
  EXPECT_EQ(blank.GetInt64(), 47);
}

TEST(ValueTest, MoveDouble) {
  Value value(74.896);
  Value moved_value(std::move(value));
  ASSERT_EQ(moved_value.type(), Value::Type::kNumber);
  EXPECT_EQ(moved_value.GetDouble(), 74.896);

  Value blank;

  blank = Value(654.38);
  ASSERT_EQ(blank.type(), Value::Type::kNumber);
  EXPECT_EQ(blank.GetDouble(), 654.38);
}

TEST(ValueTest, MoveString) {
  Value value("foobar");
  Value moved_value(std::move(value));
  ASSERT_EQ(moved_value.type(), Value::Type::kString);
  EXPECT_EQ(moved_value.GetString(), "foobar");

  Value blank;

  blank = Value("foobar");
  ASSERT_EQ(blank.type(), Value::Type::kString);
  EXPECT_EQ(blank.GetString(), "foobar");
}

TEST(ValueTest, MoveArray) {
  Value::Array storage;
  storage.emplace_back(123);
  Value value(Value::Clone(storage));
  Value moved_value(std::move(value));
  ASSERT_EQ(moved_value.type(), Value::Type::kArray);
  EXPECT_EQ(moved_value.GetArray().back().GetInt64(), 123);

  Value blank;
  blank = Value(std::move(storage));
  ASSERT_EQ(blank.type(), Value::Type::kArray);
  EXPECT_EQ(blank.GetArray().back().GetInt64(), 123);
}

TEST(ValueTest, MoveConstructObject) {
  Value::Object storage;
  storage.emplace("Int", 123);

  Value value(std::move(storage));
  Value moved_value(std::move(value));
  ASSERT_EQ(moved_value.type(), Value::Type::kObject);
  const auto key = moved_value.FindKey("Int");
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->GetInt64(), 123);
}

TEST(ValueTest, MoveAssignObject) {
  Value::Object storage;
  storage.emplace("Int", 123);

  Value blank;
  blank = Value(std::move(storage));
  ASSERT_EQ(blank.type(), Value::Type::kObject);
  const auto key = blank.FindKey("Int");
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->GetInt64(), 123);
}

TEST(ValueTest, FindKey) {
  Value::Object storage;
  storage.emplace("foo", "bar");
  Value dict(std::move(storage));
  EXPECT_NE(dict.FindKey("foo"), nullptr);
  EXPECT_EQ(dict.FindKey("baz"), nullptr);

  const auto found = dict.FindKey("notfound");
  EXPECT_EQ(found, nullptr);

  Value null;
  EXPECT_DEATH(null.FindKey("foo"), "");
}

TEST(ValueTest, FindKeyChangeValue) {
  Value::Object storage;
  storage.emplace("foo", "bar");
  Value dict(std::move(storage));
  auto found = dict.FindKey("foo");
  ASSERT_NE(found, nullptr);
  EXPECT_EQ(found->GetString(), "bar");

  *found = Value(123);
  const auto key = dict.FindKey("foo");
  ASSERT_NE(key, nullptr);
  EXPECT_EQ(key->GetInt64(), 123);
}

TEST(ValueTest, FindKeyConst) {
  Value::Object storage;
  storage.emplace("foo", "bar");
  const Value dict(std::move(storage));
  EXPECT_NE(dict.FindKey("foo"), nullptr);
  EXPECT_EQ(dict.FindKey("baz"), nullptr);

  const Value null;
  EXPECT_DEATH(null.FindKey("foo"), "");
}

TEST(ValueTest, FindKeyOfType) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  Value dict(std::move(storage));
  EXPECT_NE(dict.FindKeyOfType("null", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kNull), nullptr);
  EXPECT_NE(dict.FindKeyOfType("bool", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kBool), nullptr);
  EXPECT_NE(dict.FindKeyOfType("number", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kNumber), nullptr);
  EXPECT_NE(dict.FindKeyOfType("string", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kString), nullptr);
  EXPECT_NE(dict.FindKeyOfType("array", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kArray), nullptr);
  EXPECT_NE(dict.FindKeyOfType("dict", Value::Type::kObject), nullptr);

  Value null;
  EXPECT_DEATH(null.FindKeyOfType("dict", Value::Type::kObject), "");
}

TEST(ValueTest, FindKeyOfTypeConst) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_NE(dict.FindKeyOfType("null", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("null", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kNull), nullptr);
  EXPECT_NE(dict.FindKeyOfType("bool", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("bool", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kBool), nullptr);
  EXPECT_NE(dict.FindKeyOfType("number", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("number", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kNumber), nullptr);
  EXPECT_NE(dict.FindKeyOfType("string", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("string", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kString), nullptr);
  EXPECT_NE(dict.FindKeyOfType("array", Value::Type::kArray), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("array", Value::Type::kObject), nullptr);

  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kNull), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kBool), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kNumber), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kString), nullptr);
  EXPECT_EQ(dict.FindKeyOfType("dict", Value::Type::kArray), nullptr);
  EXPECT_NE(dict.FindKeyOfType("dict", Value::Type::kObject), nullptr);

  const Value null;
  EXPECT_DEATH(null.FindKeyOfType("dict", Value::Type::kObject), "");
}

TEST(ValueTest, FindBoolKey) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_EQ(dict.FindBoolKey("null"), std::nullopt);
  EXPECT_NE(dict.FindBoolKey("bool"), std::nullopt);
  EXPECT_EQ(dict.FindBoolKey("number"), std::nullopt);
  EXPECT_EQ(dict.FindBoolKey("string"), std::nullopt);
  EXPECT_EQ(dict.FindBoolKey("array"), std::nullopt);
  EXPECT_EQ(dict.FindBoolKey("dict"), std::nullopt);

  const Value null;
  EXPECT_DEATH(null.FindBoolKey("dict"), "");
}

TEST(ValueTest, FindIntKey) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_EQ(dict.FindIntKey("null"), std::nullopt);
  EXPECT_EQ(dict.FindIntKey("bool"), std::nullopt);
  EXPECT_NE(dict.FindIntKey("number"), std::nullopt);
  EXPECT_EQ(dict.FindIntKey("string"), std::nullopt);
  EXPECT_EQ(dict.FindIntKey("array"), std::nullopt);
  EXPECT_EQ(dict.FindIntKey("dict"), std::nullopt);

  const Value null;
  EXPECT_DEATH(null.FindIntKey("dict"), "");
}

TEST(ValueTest, FindInt64Key) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_EQ(dict.FindInt64Key("null"), std::nullopt);
  EXPECT_EQ(dict.FindInt64Key("bool"), std::nullopt);
  EXPECT_NE(dict.FindInt64Key("number"), std::nullopt);
  EXPECT_EQ(dict.FindInt64Key("string"), std::nullopt);
  EXPECT_EQ(dict.FindInt64Key("array"), std::nullopt);
  EXPECT_EQ(dict.FindInt64Key("dict"), std::nullopt);

  const Value null;
  EXPECT_DEATH(null.FindInt64Key("dict"), "");
}

TEST(ValueTest, FindDoubleKey) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_EQ(dict.FindDoubleKey("null"), std::nullopt);
  EXPECT_EQ(dict.FindDoubleKey("bool"), std::nullopt);
  EXPECT_NE(dict.FindDoubleKey("number"), std::nullopt);
  EXPECT_EQ(dict.FindDoubleKey("string"), std::nullopt);
  EXPECT_EQ(dict.FindDoubleKey("array"), std::nullopt);
  EXPECT_EQ(dict.FindDoubleKey("dict"), std::nullopt);

  const Value null;
  EXPECT_DEATH(null.FindDoubleKey("dict"), "");
}

TEST(ValueTest, FindStringKey) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_EQ(dict.FindStringKey("null"), nullptr);
  EXPECT_EQ(dict.FindStringKey("bool"), nullptr);
  EXPECT_EQ(dict.FindStringKey("number"), nullptr);
  EXPECT_NE(dict.FindStringKey("string"), nullptr);
  EXPECT_EQ(dict.FindStringKey("array"), nullptr);
  EXPECT_EQ(dict.FindStringKey("dict"), nullptr);

  const Value null;
  EXPECT_DEATH(null.FindStringKey("dict"), "");
}

TEST(ValueTest, FindArrayKey) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_EQ(dict.FindArrayKey("null"), nullptr);
  EXPECT_EQ(dict.FindArrayKey("bool"), nullptr);
  EXPECT_EQ(dict.FindArrayKey("number"), nullptr);
  EXPECT_EQ(dict.FindArrayKey("string"), nullptr);
  EXPECT_NE(dict.FindArrayKey("array"), nullptr);
  EXPECT_EQ(dict.FindArrayKey("dict"), nullptr);

  const Value null;
  EXPECT_DEATH(null.FindArrayKey("dict"), "");
}

TEST(ValueTest, FindObjectKey) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  const Value dict(std::move(storage));
  EXPECT_EQ(dict.FindObjectKey("null"), nullptr);
  EXPECT_EQ(dict.FindObjectKey("bool"), nullptr);
  EXPECT_EQ(dict.FindObjectKey("number"), nullptr);
  EXPECT_EQ(dict.FindObjectKey("string"), nullptr);
  EXPECT_EQ(dict.FindObjectKey("array"), nullptr);
  EXPECT_NE(dict.FindObjectKey("dict"), nullptr);

  const Value null;
  EXPECT_DEATH(null.FindObjectKey("dict"), "");
}

TEST(ValueTest, SetKey) {
  Value::Object storage;
  storage.emplace("null", Value::Type::kNull);
  storage.emplace("bool", Value::Type::kBool);
  storage.emplace("number", Value::Type::kNumber);
  storage.emplace("string", Value::Type::kString);
  storage.emplace("array", Value::Type::kArray);
  storage.emplace("dict", Value::Type::kObject);

  Value dict(Value::Type::kObject);
  dict.SetKey(std::string(std::string_view("null")), Value(Value::Type::kNull));
  dict.SetKey(std::string(std::string_view("bool")), Value(Value::Type::kBool));
  dict.SetKey(std::string("number"), Value(Value::Type::kNumber));
  dict.SetKey(std::string("string"), Value(Value::Type::kString));
  dict.SetKey("array", Value(Value::Type::kArray));
  dict.SetKey("dict", Value(Value::Type::kObject));

  EXPECT_EQ(dict, Value(std::move(storage)));

  Value null;
  EXPECT_DEATH(null.SetKey(std::string("number"), Value()), "");
  EXPECT_DEATH(null.SetKey(std::string(std::string_view("number")), Value()),
               "");
  EXPECT_DEATH(null.SetKey("number", Value()), "");
}

TEST(ValueTest, SetKeyReturns) {
  Value root(Value::Type::kObject);

  const auto null_weak = root.SetKey("null", Value());
  EXPECT_EQ(*null_weak, Value());

  const auto bool_weak = root.SetKey("bool", Value(true));
  EXPECT_EQ(*bool_weak, Value(true));

  const auto int_weak = root.SetKey("int", Value(42));
  EXPECT_EQ(*int_weak, Value(42));

  const auto double_weak = root.SetKey("double", Value(3.14));
  EXPECT_EQ(*double_weak, Value(3.14));

  const auto string_weak = root.SetKey("string", Value("hello"));
  EXPECT_EQ(*string_weak, Value("hello"));

  Value::Array array;
  array.emplace_back(0);
  array.emplace_back(1);
  const auto array_weak = root.SetKey("array", Value(Value::Clone(array)));
  EXPECT_EQ(*array_weak, Value(std::move(array)));

  Value::Object object;
  object.emplace("first", "first");
  object.emplace("second", "second");
  const auto object_weak = root.SetKey("object", Value(Value::Clone(object)));
  EXPECT_EQ(*object_weak, Value(std::move(object)));
}

TEST(ValueTest, RemoveKey) {
  Value root(Value::Type::kObject);
  root.SetKey("one", Value(123));

  EXPECT_FALSE(root.RemoveKey("two"));
  EXPECT_TRUE(root.RemoveKey("one"));
  EXPECT_FALSE(root.RemoveKey("one"));

  Value null;
  EXPECT_DEATH(null.RemoveKey("one"), "");
}

TEST(ValueTest, Comparisons) {
  Value null1;
  Value null2;
  EXPECT_EQ(null2, null1);
  EXPECT_FALSE(null1 != null2);
  EXPECT_FALSE(null1 < null2);
  EXPECT_FALSE(null1 > null2);
  EXPECT_LE(null1, null2);
  EXPECT_GE(null1, null2);

  Value bool1(false);
  Value bool2(true);
  EXPECT_FALSE(bool1 == bool2);
  EXPECT_NE(bool2, bool1);
  EXPECT_LT(bool1, bool2);
  EXPECT_FALSE(bool1 > bool2);
  EXPECT_LE(bool1, bool2);
  EXPECT_FALSE(bool1 >= bool2);

  Value int1(1);
  Value int2(2);
  EXPECT_FALSE(int1 == int2);
  EXPECT_NE(int2, int1);
  EXPECT_LT(int1, int2);
  EXPECT_FALSE(int1 > int2);
  EXPECT_LE(int1, int2);
  EXPECT_FALSE(int1 >= int2);

  Value double1(1.0);
  Value double2(2.0);
  EXPECT_FALSE(double1 == double2);
  EXPECT_NE(double2, double1);
  EXPECT_LT(double1, double2);
  EXPECT_FALSE(double1 > double2);
  EXPECT_LE(double1, double2);
  EXPECT_FALSE(double1 >= double2);

  Value string1("1");
  Value string2("2");
  EXPECT_FALSE(string1 == string2);
  EXPECT_NE(string2, string1);
  EXPECT_LT(string1, string2);
  EXPECT_FALSE(string1 > string2);
  EXPECT_LE(string1, string2);
  EXPECT_FALSE(string1 >= string2);

  Value null_array1(Value::Type::kArray);
  Value null_array2(Value::Type::kArray);
  EXPECT_EQ(null_array2, null_array1);
  EXPECT_FALSE(null_array1 != null_array2);
  EXPECT_FALSE(null_array1 < null_array2);
  EXPECT_FALSE(null_array1 > null_array2);
  EXPECT_LE(null_array1, null_array2);
  EXPECT_GE(null_array1, null_array2);

  Value int_array1(Value::Type::kArray);
  Value int_array2(Value::Type::kArray);
  int_array1.GetArray().emplace_back(Value(1));
  int_array2.GetArray().emplace_back(Value(2));
  EXPECT_FALSE(int_array1 == int_array2);
  EXPECT_NE(int_array2, int_array1);
  EXPECT_LT(int_array1, int_array2);
  EXPECT_FALSE(int_array1 > int_array2);
  EXPECT_LE(int_array1, int_array2);
  EXPECT_FALSE(int_array1 >= int_array2);

  Value null_dict1(Value::Type::kObject);
  Value null_dict2(Value::Type::kObject);
  EXPECT_EQ(null_dict2, null_dict1);
  EXPECT_FALSE(null_dict1 != null_dict2);
  EXPECT_FALSE(null_dict1 < null_dict2);
  EXPECT_FALSE(null_dict1 > null_dict2);
  EXPECT_LE(null_dict1, null_dict2);
  EXPECT_GE(null_dict1, null_dict2);

  Value int_dict1(Value::Type::kObject);
  Value int_dict2(Value::Type::kObject);
  int_dict1.SetKey("key", Value(1));
  int_dict2.SetKey("key", Value(2));
  EXPECT_FALSE(int_dict1 == int_dict2);
  EXPECT_NE(int_dict2, int_dict1);
  EXPECT_LT(int_dict1, int_dict2);
  EXPECT_FALSE(int_dict1 > int_dict2);
  EXPECT_LE(int_dict1, int_dict2);
  EXPECT_FALSE(int_dict1 >= int_dict2);

  std::vector<Value> values;
  values.emplace_back(std::move(null1));
  values.emplace_back(std::move(bool1));
  values.emplace_back(std::move(double1));
  values.emplace_back(std::move(string1));
  values.emplace_back(std::move(int_array1));
  values.emplace_back(std::move(int_dict1));
  for (size_t i = 0, size = values.size(); i < size; i++) {
    for (size_t j = i + 1; j < size; j++) {
      EXPECT_FALSE(values[i] == values[j]);
      EXPECT_NE(values[j], values[i]);
      EXPECT_LT(values[i], values[j]);
      EXPECT_FALSE(values[i] > values[j]);
      EXPECT_LE(values[i], values[j]);
      EXPECT_FALSE(values[i] >= values[j]);
    }
  }
}

TEST(ValueTest, SelfSwap) {
  Value test(1);
  std::swap(test, test);
  EXPECT_EQ(test.GetInt64(), 1);
}

TEST(ValueTest, SetPathOnlyForObject) {
  EXPECT_DEATH(Value(Value::Type::kNull).SetPath("key", Value()), "");
  EXPECT_DEATH(Value(Value::Type::kBool).SetPath("key", Value()), "");
  EXPECT_DEATH(Value(Value::Type::kNumber).SetPath("key", Value()), "");
  EXPECT_DEATH(Value(Value::Type::kString).SetPath("key", Value()), "");
  EXPECT_DEATH(Value(Value::Type::kArray).SetPath("key", Value()), "");
  EXPECT_NO_FATAL_FAILURE(Value(Value::Type::kObject).SetPath("key", Value()));
}

TEST(ValueTest, FindPathOnlyForObject) {
  EXPECT_DEATH(Value(Value::Type::kNull).FindPath("key"), "");
  EXPECT_DEATH(Value(Value::Type::kBool).FindPath("key"), "");
  EXPECT_DEATH(Value(Value::Type::kNumber).FindPath("key"), "");
  EXPECT_DEATH(Value(Value::Type::kString).FindPath("key"), "");
  EXPECT_DEATH(Value(Value::Type::kArray).FindPath("key"), "");
  EXPECT_NO_FATAL_FAILURE(Value(Value::Type::kObject).FindPath("key"));

  EXPECT_DEATH(
      static_cast<const Value&>(Value(Value::Type::kNull)).FindPath("key"), "");
  EXPECT_DEATH(
      static_cast<const Value&>(Value(Value::Type::kBool)).FindPath("key"), "");
  EXPECT_DEATH(
      static_cast<const Value&>(Value(Value::Type::kNumber)).FindPath("key"),
      "");
  EXPECT_DEATH(
      static_cast<const Value&>(Value(Value::Type::kString)).FindPath("key"),
      "");
  EXPECT_DEATH(
      static_cast<const Value&>(Value(Value::Type::kArray)).FindPath("key"),
      "");
  EXPECT_NO_FATAL_FAILURE(
      static_cast<const Value&>(Value(Value::Type::kObject)).FindPath("key"));
}

TEST(ValueTest, SetPathLevel1) {
  Value object(Value::Type::kObject);

  auto value = object.SetPath("key", Value(1));
  EXPECT_EQ(*value, Value(1));
  EXPECT_EQ(object.FindIntKey("key"), 1);

  auto v = object.FindPath("key");
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(*v, Value(1));

  EXPECT_EQ(object.FindPath("key1"), nullptr);
  EXPECT_EQ(const_cast<const Value&>(object).FindPath("key1"), nullptr);

  auto const_v = const_cast<const Value&>(object).FindPath("key");
  ASSERT_NE(const_v, nullptr);
  EXPECT_EQ(*const_v, Value(1));

  value = object.SetPath("key", Value(2));
  EXPECT_EQ(*value, Value(2));
  EXPECT_EQ(object.FindIntKey("key"), 2);

  v = object.FindPath("key");
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(*v, Value(2));

  const_v = const_cast<const Value&>(object).FindPath("key");
  ASSERT_NE(const_v, nullptr);
  EXPECT_EQ(*const_v, Value(2));
}

TEST(ValueTest, SetPathLevel2) {
  Value object(Value::Type::kObject);

  auto value = object.SetPath("key1.key2", Value(1));
  EXPECT_EQ(*value, Value(1));

  auto key1 = object.FindObjectKey("key1");
  ASSERT_NE(key1, nullptr);
  EXPECT_EQ(key1->FindIntKey("key2"), 1);

  auto v = object.FindPath("key1.key2");
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(*v, Value(1));

  auto const_v = const_cast<const Value&>(object).FindPath("key1.key2");
  ASSERT_NE(const_v, nullptr);
  EXPECT_EQ(*const_v, Value(1));

  EXPECT_EQ(object.FindPath("key1.key3"), nullptr);
  EXPECT_EQ(object.FindPath("key2.key2"), nullptr);
  EXPECT_EQ(const_cast<const Value&>(object).FindPath("key1.key3"), nullptr);
  EXPECT_EQ(const_cast<const Value&>(object).FindPath("key2.key2"), nullptr);

  value = object.SetPath("key1.key2", Value(2));
  EXPECT_EQ(*value, Value(2));

  key1 = object.FindObjectKey("key1");
  ASSERT_NE(key1, nullptr);
  EXPECT_EQ(key1->FindIntKey("key2"), 2);

  v = object.FindPath("key1.key2");
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(*v, Value(2));

  const_v = const_cast<const Value&>(object).FindPath("key1.key2");
  ASSERT_NE(const_v, nullptr);
  EXPECT_EQ(*const_v, Value(2));
}

TEST(ValueTest, SetPathLevel3) {
  Value object(Value::Type::kObject);

  auto value = object.SetPath("key1.key2.key3", Value(1));
  EXPECT_EQ(*value, Value(1));

  auto key1 = object.FindObjectKey("key1");
  ASSERT_NE(key1, nullptr);

  auto key2 = key1->FindObjectKey("key2");
  ASSERT_NE(key2, nullptr);
  EXPECT_EQ(key2->FindIntKey("key3"), 1);

  auto v = object.FindPath("key1.key2.key3");
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(*v, Value(1));

  auto const_v = const_cast<const Value&>(object).FindPath("key1.key2.key3");
  ASSERT_NE(const_v, nullptr);
  EXPECT_EQ(*const_v, Value(1));

  EXPECT_EQ(object.FindPath("key1.key2.key4"), nullptr);
  EXPECT_EQ(object.FindPath("key1.key3.key3"), nullptr);
  EXPECT_EQ(object.FindPath("key2.key2.key3"), nullptr);
  EXPECT_EQ(const_cast<const Value&>(object).FindPath("key1.key2.key4"),
            nullptr);
  EXPECT_EQ(const_cast<const Value&>(object).FindPath("key1.key3.key3"),
            nullptr);
  EXPECT_EQ(const_cast<const Value&>(object).FindPath("key2.key2.key3"),
            nullptr);

  value = object.SetPath("key1.key2.key3", Value(2));
  EXPECT_EQ(*value, Value(2));

  key1 = object.FindObjectKey("key1");
  ASSERT_NE(key1, nullptr);

  key2 = key1->FindObjectKey("key2");
  ASSERT_NE(key2, nullptr);
  EXPECT_EQ(key2->FindIntKey("key3"), 2);

  v = object.FindPath("key1.key2.key3");
  ASSERT_NE(v, nullptr);
  EXPECT_EQ(*v, Value(2));

  const_v = const_cast<const Value&>(object).FindPath("key1.key2.key3");
  ASSERT_NE(const_v, nullptr);
  EXPECT_EQ(*const_v, Value(2));
}

}  // namespace rst
