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

#ifndef RST_PREFERENCES_PREFERENCES_H_
#define RST_PREFERENCES_PREFERENCES_H_

#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <string_view>

#include "rst/macros/macros.h"
#include "rst/not_null/not_null.h"
#include "rst/preferences/preferences_store.h"
#include "rst/value/value.h"

namespace rst {

// A set of preferences stored in a PreferencesStore.
//
// Preferences need to be registered with a type and a default value before they
// are used.
//
// Example:
//   Preferences preferences(...);
//
//   preferences.RegisterIntPreference("int.preference", 10);
//   assert(preferences.GetInt("int.preference") == 10);
//
//   preferences.SetInt("int.preference", 20);
//   assert(preferences.GetInt("int.preference") == 20);
//
class Preferences {
 public:
  explicit Preferences(
      NotNull<std::unique_ptr<PreferencesStore>> preferences_store);
  ~Preferences();

  // These will all assert that the preference is not registered more than once.
  void RegisterBoolPreference(std::string&& path, bool default_value);
  void RegisterIntPreference(std::string&& path, int default_value);
  void RegisterDoublePreference(std::string&& path, double default_value);
  void RegisterStringPreference(std::string&& path,
                                Value::String&& default_value);
  void RegisterArrayPreference(std::string&& path,
                               Value::Array&& default_value);
  void RegisterObjectPreference(std::string&& path,
                                Value::Object&& default_value);

  // These will all assert that the preference is registered with the
  // corresponding type.
  bool GetBool(std::string_view path) const;
  int GetInt(std::string_view path) const;
  double GetDouble(std::string_view path) const;
  const Value::String& GetString(std::string_view path) const;
  const Value::Array& GetArray(std::string_view path) const;
  const Value::Object& GetObject(std::string_view path) const;

  // These will all assert that the preference is registered with the
  // corresponding type.
  void SetBool(std::string_view path, bool value);
  void SetInt(std::string_view path, int value);
  void SetDouble(std::string_view path, double value);
  void SetString(std::string_view path, Value::String&& value);
  void SetArray(std::string_view path, Value::Array&& value);
  void SetObject(std::string_view path, Value::Object&& value);

 private:
  void RegisterPreference(std::string&& path, Value&& default_value);

  void SetValue(std::string_view path, Value&& value);

  // Default values for each path.
  std::map<std::string, Value, std::less<>> defaults_;
  const NotNull<std::unique_ptr<PreferencesStore>> preferences_store_;

  RST_DISALLOW_COPY_AND_ASSIGN(Preferences);
};

}  // namespace rst

#endif  // RST_PREFERENCES_PREFERENCES_H_
