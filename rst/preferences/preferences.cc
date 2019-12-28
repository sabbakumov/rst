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

#include "rst/preferences/preferences.h"

namespace rst {

Preferences::Preferences(
    NotNull<std::unique_ptr<PreferencesStore>> preferences_store)
    : preferences_store_(std::move(preferences_store)) {}

Preferences::~Preferences() = default;

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

}  // namespace rst
