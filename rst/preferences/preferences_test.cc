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

#include <utility>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "rst/memory/memory.h"
#include "rst/not_null/not_null.h"
#include "rst/preferences/memory_preferences_store.h"
#include "rst/preferences/preferences.h"
#include "rst/preferences/preferences_store.h"

using testing::_;
using testing::Return;

namespace rst {
namespace {

class MockPreferencesStore : public PreferencesStore {
 public:
  MOCK_CONST_METHOD1(GetValue, Nullable<const Value*>(std::string_view path));

  MOCK_METHOD2(SetValue, void(std::string_view path, Value&& value));
};

}  // namespace

class PreferencesTest : public testing::Test {
 public:
  PreferencesTest() {
    prefs_.RegisterBoolPreference("bool", true);
    prefs_.RegisterIntPreference("int", 10);
    prefs_.RegisterDoublePreference("double", 50.0);
    prefs_.RegisterStringPreference("string", "Hello");

    Value::Array array;
    array.emplace_back("a");
    array.emplace_back(1);
    prefs_.RegisterArrayPreference("array", std::move(array));

    Value::Object object;
    object.emplace("first", "first");
    object.emplace("second", "second");
    prefs_.RegisterObjectPreference("object", std::move(object));
  }

  ~PreferencesTest();

 protected:
  NotNull<MockPreferencesStore*> pref_store_{new MockPreferencesStore()};
  Preferences prefs_{WrapUnique(pref_store_)};
};

PreferencesTest::~PreferencesTest() = default;

TEST_F(PreferencesTest, GetDefaultValues) {
  EXPECT_CALL(*pref_store_, GetValue(std::string_view("bool")))
      .WillOnce(Return(nullptr));
  EXPECT_EQ(prefs_.GetBool("bool"), true);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("int")))
      .WillOnce(Return(nullptr));
  EXPECT_EQ(prefs_.GetInt("int"), 10);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("double")))
      .WillOnce(Return(nullptr));
  EXPECT_EQ(prefs_.GetDouble("double"), 50.0);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("string")))
      .WillOnce(Return(nullptr));
  EXPECT_EQ(prefs_.GetString("string"), "Hello");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("array")))
      .WillOnce(Return(nullptr));
  Value::Array array;
  array.emplace_back("a");
  array.emplace_back(1);
  EXPECT_EQ(prefs_.GetArray("array"), array);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("object")))
      .WillOnce(Return(nullptr));
  Value::Object object;
  object.emplace("first", "first");
  object.emplace("second", "second");
  EXPECT_EQ(prefs_.GetObject("object"), object);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());
}

TEST_F(PreferencesTest, GetStoredValues) {
  Value result;

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("bool")))
      .WillOnce(Return(&result));
  result = Value(false);
  EXPECT_EQ(prefs_.GetBool("bool"), false);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("int")))
      .WillOnce(Return(&result));
  result = Value(20);
  EXPECT_EQ(prefs_.GetInt("int"), 20);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("double")))
      .WillOnce(Return(&result));
  result = Value(60.0);
  EXPECT_EQ(prefs_.GetDouble("double"), 60.0);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("string")))
      .WillOnce(Return(&result));
  result = Value("World");
  EXPECT_EQ(prefs_.GetString("string"), "World");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("array")))
      .WillOnce(Return(&result));
  Value::Array array;
  array.emplace_back("b");
  array.emplace_back(2);
  result = Value(Value::Clone(array));
  EXPECT_EQ(prefs_.GetArray("array"), array);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("object")))
      .WillOnce(Return(&result));
  Value::Object object;
  object.emplace("second", "second");
  object.emplace("third", "third");
  result = Value(Value::Clone(object));
  EXPECT_EQ(prefs_.GetObject("object"), object);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());
}

TEST_F(PreferencesTest, SetValues) {
  EXPECT_CALL(*pref_store_, SetValue(_, _))
      .WillOnce([](const std::string_view path, Value&& value) {
        EXPECT_EQ(path, "bool");
        EXPECT_EQ(value, Value(false));
      });
  prefs_.SetBool("bool", false);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, SetValue(_, _))
      .WillOnce([](const std::string_view path, Value&& value) {
        EXPECT_EQ(path, "int");
        EXPECT_EQ(value, Value(20));
      });
  prefs_.SetInt("int", 20);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, SetValue(_, _))
      .WillOnce([](const std::string_view path, Value&& value) {
        EXPECT_EQ(path, "double");
        EXPECT_EQ(value, Value(60.0));
      });
  prefs_.SetDouble("double", 60.0);
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, SetValue(_, _))
      .WillOnce([](const std::string_view path, Value&& value) {
        EXPECT_EQ(path, "string");
        EXPECT_EQ(value, Value("World"));
      });
  prefs_.SetString("string", "World");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  Value::Array array;
  array.emplace_back("b");
  array.emplace_back(2);
  EXPECT_CALL(*pref_store_, SetValue(_, _))
      .WillOnce([&array](const std::string_view path, Value&& value) {
        EXPECT_EQ(path, "array");
        EXPECT_EQ(value, Value(std::move(array)));
      });
  prefs_.SetArray("array", Value::Clone(array));
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  Value::Object object;
  object.emplace("second", "second");
  object.emplace("third", "third");
  EXPECT_CALL(*pref_store_, SetValue(_, _))
      .WillOnce([&object](const std::string_view path, Value&& value) {
        EXPECT_EQ(path, "object");
        EXPECT_EQ(value, Value(std::move(object)));
      });
  prefs_.SetObject("object", Value::Clone(object));
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());
}

TEST_F(PreferencesTest, DoubleRegistration) {
  EXPECT_DEATH(prefs_.RegisterBoolPreference("bool", true), "");
  EXPECT_DEATH(prefs_.RegisterIntPreference("int", 10), "");
  EXPECT_DEATH(prefs_.RegisterDoublePreference("double", 50.0), "");
  EXPECT_DEATH(prefs_.RegisterStringPreference("string", "Hello"), "");

  Value::Array array;
  array.emplace_back("a");
  array.emplace_back(1);
  EXPECT_DEATH(prefs_.RegisterArrayPreference("array", std::move(array)), "");

  Value::Object object;
  object.emplace("first", "first");
  object.emplace("second", "second");
  EXPECT_DEATH(prefs_.RegisterObjectPreference("object", std::move(object)),
               "");
}

TEST_F(PreferencesTest, GetUnregisteredValues) {
  EXPECT_DEATH(prefs_.GetBool("unregistered_bool"), "");
  EXPECT_DEATH(prefs_.GetInt("unregistered_int"), "");
  EXPECT_DEATH(prefs_.GetDouble("unregistered_double"), "");
  EXPECT_DEATH(prefs_.GetString("unregistered_string"), "");
  EXPECT_DEATH(prefs_.GetArray("unregistered_array"), "");
  EXPECT_DEATH(prefs_.GetObject("unregistered_object"), "");
}

TEST_F(PreferencesTest, GetValuesOfDifferentType) {
  EXPECT_CALL(*pref_store_, GetValue(std::string_view("bool")))
      .WillOnce(Return(nullptr));
  EXPECT_NO_FATAL_FAILURE(prefs_.GetBool("bool"));
  EXPECT_DEATH(prefs_.GetInt("bool"), "");
  EXPECT_DEATH(prefs_.GetDouble("bool"), "");
  EXPECT_DEATH(prefs_.GetString("bool"), "");
  EXPECT_DEATH(prefs_.GetArray("bool"), "");
  EXPECT_DEATH(prefs_.GetObject("bool"), "");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("int")))
      .WillOnce(Return(nullptr))
      .WillOnce(Return(nullptr));
  EXPECT_DEATH(prefs_.GetBool("int"), "");
  EXPECT_NO_FATAL_FAILURE(prefs_.GetInt("int"));
  EXPECT_NO_FATAL_FAILURE(prefs_.GetDouble("int"));
  EXPECT_DEATH(prefs_.GetString("int"), "");
  EXPECT_DEATH(prefs_.GetArray("int"), "");
  EXPECT_DEATH(prefs_.GetObject("int"), "");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("double")))
      .WillOnce(Return(nullptr))
      .WillOnce(Return(nullptr));
  EXPECT_DEATH(prefs_.GetBool("double"), "");
  EXPECT_NO_FATAL_FAILURE(prefs_.GetInt("double"));
  EXPECT_NO_FATAL_FAILURE(prefs_.GetDouble("double"));
  EXPECT_DEATH(prefs_.GetString("double"), "");
  EXPECT_DEATH(prefs_.GetArray("double"), "");
  EXPECT_DEATH(prefs_.GetObject("double"), "");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("string")))
      .WillOnce(Return(nullptr));
  EXPECT_DEATH(prefs_.GetBool("string"), "");
  EXPECT_DEATH(prefs_.GetInt("string"), "");
  EXPECT_DEATH(prefs_.GetDouble("string"), "");
  EXPECT_NO_FATAL_FAILURE(prefs_.GetString("string"));
  EXPECT_DEATH(prefs_.GetArray("string"), "");
  EXPECT_DEATH(prefs_.GetObject("string"), "");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("array")))
      .WillOnce(Return(nullptr));
  EXPECT_DEATH(prefs_.GetBool("array"), "");
  EXPECT_DEATH(prefs_.GetInt("array"), "");
  EXPECT_DEATH(prefs_.GetDouble("array"), "");
  EXPECT_DEATH(prefs_.GetString("array"), "");
  EXPECT_NO_FATAL_FAILURE(prefs_.GetArray("array"));
  EXPECT_DEATH(prefs_.GetObject("array"), "");
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());

  EXPECT_CALL(*pref_store_, GetValue(std::string_view("object")))
      .WillOnce(Return(nullptr));
  EXPECT_DEATH(prefs_.GetBool("object"), "");
  EXPECT_DEATH(prefs_.GetInt("object"), "");
  EXPECT_DEATH(prefs_.GetDouble("object"), "");
  EXPECT_DEATH(prefs_.GetString("object"), "");
  EXPECT_DEATH(prefs_.GetArray("object"), "");
  EXPECT_NO_FATAL_FAILURE(prefs_.GetObject("object"));
  testing::Mock::VerifyAndClearExpectations(pref_store_.get());
}

class MemoryPreferencesStoreTest : public testing::Test {
 public:
  ~MemoryPreferencesStoreTest();
};

MemoryPreferencesStoreTest::~MemoryPreferencesStoreTest() = default;

TEST_F(MemoryPreferencesStoreTest, MemoryPreferencesStore) {
  MemoryPreferencesStore pref_store;

  auto value = pref_store.GetValue("path");
  EXPECT_EQ(value, nullptr);

  pref_store.SetValue("path", Value(10));
  value = pref_store.GetValue("path");
  ASSERT_NE(value, nullptr);
  EXPECT_EQ(*value, Value(10));

  value = pref_store.GetValue("path.path2");
  EXPECT_EQ(value, nullptr);

  pref_store.SetValue("path.path2", Value(20));
  value = pref_store.GetValue("path.path2");
  ASSERT_NE(value, nullptr);
  EXPECT_EQ(*value, Value(20));
}

}  // namespace rst
