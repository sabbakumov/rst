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

#include "rst/Preferences/Preferences.h"

#include <utility>

#include "rst/Check/Check.h"

namespace rst {

Preferences::Preferences(
    NotNull<std::unique_ptr<IPreferencesStore>> preferences_store)
    : preferences_store_(std::move(preferences_store)) {}

Preferences::~Preferences() = default;

void Preferences::RegisterBoolPreference(std::string&& key,
                                         const bool default_value) {
  RegisterPreference(std::move(key), Value(default_value));
}

void Preferences::RegisterIntPreference(std::string&& key,
                                        const int default_value) {
  RegisterPreference(std::move(key), Value(default_value));
}

void Preferences::RegisterDoublePreference(std::string&& key,
                                           const double default_value) {
  RegisterPreference(std::move(key), Value(default_value));
}

void Preferences::RegisterStringPreference(std::string&& key,
                                           Value::String&& default_value) {
  RegisterPreference(std::move(key), Value(std::move(default_value)));
}

void Preferences::RegisterArrayPreference(std::string&& key,
                                          Value::Array&& default_value) {
  RegisterPreference(std::move(key), Value(std::move(default_value)));
}

void Preferences::RegisterObjectPreference(std::string&& key,
                                           Value::Object&& default_value) {
  RegisterPreference(std::move(key), Value(std::move(default_value)));
}

bool Preferences::GetBool(const std::string_view key) const {
  RST_DCHECK((defaults_.find(key) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(key)->second.IsBool()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(key);
  if (stored_pref == nullptr)
    return defaults_.find(key)->second.GetBool();

  return stored_pref->GetBool();
}

int Preferences::GetInt(const std::string_view key) const {
  RST_DCHECK((defaults_.find(key) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(key)->second.IsInt()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(key);
  if (stored_pref == nullptr)
    return defaults_.find(key)->second.GetInt();

  return stored_pref->GetInt();
}

double Preferences::GetDouble(const std::string_view key) const {
  RST_DCHECK((defaults_.find(key) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(key)->second.IsNumber()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(key);
  if (stored_pref == nullptr)
    return defaults_.find(key)->second.GetDouble();

  return stored_pref->GetDouble();
}

const Value::String& Preferences::GetString(const std::string_view key) const {
  RST_DCHECK((defaults_.find(key) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(key)->second.IsString()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(key);
  if (stored_pref == nullptr)
    return defaults_.find(key)->second.GetString();

  return stored_pref->GetString();
}

const Value::Array& Preferences::GetArray(const std::string_view key) const {
  RST_DCHECK((defaults_.find(key) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(key)->second.IsArray()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(key);
  if (stored_pref == nullptr)
    return defaults_.find(key)->second.GetArray();

  return stored_pref->GetArray();
}

const Value::Object& Preferences::GetObject(const std::string_view key) const {
  RST_DCHECK((defaults_.find(key) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(key)->second.IsObject()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(key);
  if (stored_pref == nullptr)
    return defaults_.find(key)->second.GetObject();

  return stored_pref->GetObject();
}

void Preferences::SetBool(const std::string_view key, const bool value) {
  SetValue(key, Value(value));
}

void Preferences::SetInt(const std::string_view key, const int value) {
  SetValue(key, Value(value));
}

void Preferences::SetDouble(const std::string_view key, const double value) {
  SetValue(key, Value(value));
}

void Preferences::SetString(const std::string_view key, Value::String&& value) {
  SetValue(key, Value(std::move(value)));
}

void Preferences::SetArray(const std::string_view key, Value::Array&& value) {
  SetValue(key, Value(std::move(value)));
}

void Preferences::SetObject(const std::string_view key, Value::Object&& value) {
  SetValue(key, Value(std::move(value)));
}

void Preferences::RegisterPreference(std::string&& key, Value&& default_value) {
  RST_DCHECK(default_value.type() != Value::Type::kNull);
  RST_DCHECK((defaults_.find(key) == defaults_.cend()) &&
             "Trying to register a previously registered preference");

  defaults_.emplace(std::move(key), std::move(default_value));
}

void Preferences::SetValue(const std::string_view key, Value&& value) {
  RST_DCHECK(value.type() != Value::Type::kNull);
  RST_DCHECK((defaults_.find(key) != defaults_.cend()) &&
             "Trying to write an unregistered preference");
  RST_DCHECK((defaults_.find(key)->second.type() == value.type()) &&
             "Trying to write a preference of different type");

  preferences_store_->SetValue(key, std::move(value));
}

}  // namespace rst
