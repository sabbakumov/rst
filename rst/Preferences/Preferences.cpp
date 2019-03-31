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

void Preferences::RegisterBoolPreference(std::string&& path,
                                         const bool default_value) {
  RegisterPreference(std::move(path), Value(default_value));
}

void Preferences::RegisterIntPreference(std::string&& path,
                                        const int default_value) {
  RegisterPreference(std::move(path), Value(default_value));
}

void Preferences::RegisterDoublePreference(std::string&& path,
                                           const double default_value) {
  RegisterPreference(std::move(path), Value(default_value));
}

void Preferences::RegisterStringPreference(std::string&& path,
                                           Value::String&& default_value) {
  RegisterPreference(std::move(path), Value(std::move(default_value)));
}

void Preferences::RegisterArrayPreference(std::string&& path,
                                          Value::Array&& default_value) {
  RegisterPreference(std::move(path), Value(std::move(default_value)));
}

void Preferences::RegisterObjectPreference(std::string&& path,
                                           Value::Object&& default_value) {
  RegisterPreference(std::move(path), Value(std::move(default_value)));
}

bool Preferences::GetBool(const std::string_view path) const {
  RST_DCHECK((defaults_.find(path) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(path)->second.IsBool()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(path);
  if (stored_pref == nullptr)
    return defaults_.find(path)->second.GetBool();

  return stored_pref->GetBool();
}

int Preferences::GetInt(const std::string_view path) const {
  RST_DCHECK((defaults_.find(path) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(path)->second.IsInt()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(path);
  if (stored_pref == nullptr)
    return defaults_.find(path)->second.GetInt();

  return stored_pref->GetInt();
}

double Preferences::GetDouble(const std::string_view path) const {
  RST_DCHECK((defaults_.find(path) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(path)->second.IsNumber()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(path);
  if (stored_pref == nullptr)
    return defaults_.find(path)->second.GetDouble();

  return stored_pref->GetDouble();
}

const Value::String& Preferences::GetString(const std::string_view path) const {
  RST_DCHECK((defaults_.find(path) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(path)->second.IsString()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(path);
  if (stored_pref == nullptr)
    return defaults_.find(path)->second.GetString();

  return stored_pref->GetString();
}

const Value::Array& Preferences::GetArray(const std::string_view path) const {
  RST_DCHECK((defaults_.find(path) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(path)->second.IsArray()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(path);
  if (stored_pref == nullptr)
    return defaults_.find(path)->second.GetArray();

  return stored_pref->GetArray();
}

const Value::Object& Preferences::GetObject(const std::string_view path) const {
  RST_DCHECK((defaults_.find(path) != defaults_.cend()) &&
             "Trying to read an unregistered preference");
  RST_DCHECK((defaults_.find(path)->second.IsObject()) &&
             "Trying to read a preference of different type");

  const auto stored_pref = preferences_store_->GetValue(path);
  if (stored_pref == nullptr)
    return defaults_.find(path)->second.GetObject();

  return stored_pref->GetObject();
}

void Preferences::SetBool(const std::string_view path, const bool value) {
  SetValue(path, Value(value));
}

void Preferences::SetInt(const std::string_view path, const int value) {
  SetValue(path, Value(value));
}

void Preferences::SetDouble(const std::string_view path, const double value) {
  SetValue(path, Value(value));
}

void Preferences::SetString(const std::string_view path,
                            Value::String&& value) {
  SetValue(path, Value(std::move(value)));
}

void Preferences::SetArray(const std::string_view path, Value::Array&& value) {
  SetValue(path, Value(std::move(value)));
}

void Preferences::SetObject(const std::string_view path,
                            Value::Object&& value) {
  SetValue(path, Value(std::move(value)));
}

void Preferences::RegisterPreference(std::string&& path,
                                     Value&& default_value) {
  RST_DCHECK(default_value.type() != Value::Type::kNull);
  RST_DCHECK((defaults_.find(path) == defaults_.cend()) &&
             "Trying to register a previously registered preference");

  defaults_.emplace(std::move(path), std::move(default_value));
}

void Preferences::SetValue(const std::string_view path, Value&& value) {
  RST_DCHECK(value.type() != Value::Type::kNull);
  RST_DCHECK((defaults_.find(path) != defaults_.cend()) &&
             "Trying to write an unregistered preference");
  RST_DCHECK((defaults_.find(path)->second.type() == value.type()) &&
             "Trying to write a preference of different type");

  preferences_store_->SetValue(path, std::move(value));
}

}  // namespace rst
