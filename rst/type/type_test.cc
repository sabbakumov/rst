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

#include "rst/type/type.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>

#include <gtest/gtest.h>

#include "rst/check/check.h"

namespace rst {
namespace {

template <class T>
T GetExampleValue(int index);

template <>
int GetExampleValue<int>(const int index) {
  return 5 + index;
}

template <>
uint64_t GetExampleValue<uint64_t>(const int index) {
  return uint64_t{500} + static_cast<uint64_t>(index);
}

template <>
std::string GetExampleValue<std::string>(const int index) {
  return std::string(static_cast<size_t>(index), 'a');
}

class TypeNames {
 public:
  template <class T>
  static std::string GetName(int) {
    if constexpr (std::is_same_v<T, int>)
      return "int";
    if constexpr (std::is_same_v<T, uint64_t>)
      return "uint64";
    if constexpr (std::is_same_v<T, std::string>)
      return "string";

    RST_NOTREACHED();
    return std::string();
  }
};

}  // namespace

template <class T>
class TypeTest : public testing::Test {};

using TestedTypes = testing::Types<int, uint64_t, std::string>;
TYPED_TEST_SUITE(TypeTest, TestedTypes, TypeNames);

TYPED_TEST(TypeTest, ValueAccessesUnderlyingValue) {
  using FooType = Type<class FooTag, TypeParam>;

  const FooType const_type(GetExampleValue<TypeParam>(1));
  EXPECT_EQ(const_type.value(), GetExampleValue<TypeParam>(1));
  static_assert(
      std::is_const_v<
          typename std::remove_reference_t<decltype(const_type.value())>>,
      "Reference returned by const value getter should be const.");
}

TYPED_TEST(TypeTest, ExplicitConversionToUnderlyingValue) {
  using FooType = Type<class FooTag, TypeParam>;

  const FooType const_type(GetExampleValue<TypeParam>(1));
  EXPECT_EQ(static_cast<TypeParam>(const_type), GetExampleValue<TypeParam>(1));
}

TYPED_TEST(TypeTest, CanBeCopyConstructed) {
  using FooType = Type<class FooTag, TypeParam>;
  FooType type(GetExampleValue<TypeParam>(0));
  FooType copy_constructed = type;
  EXPECT_EQ(copy_constructed, type);

  FooType copy_assigned;
  copy_assigned = type;
  EXPECT_EQ(copy_assigned, type);
}

TYPED_TEST(TypeTest, CanBeMoveConstructed) {
  using FooType = Type<class FooTag, TypeParam>;
  FooType type(GetExampleValue<TypeParam>(0));
  FooType move_constructed = std::move(type);
  EXPECT_EQ(move_constructed, FooType(GetExampleValue<TypeParam>(0)));

  FooType type2(GetExampleValue<TypeParam>(2));
  FooType move_assigned;
  move_assigned = std::move(type2);
  EXPECT_EQ(move_assigned, FooType(GetExampleValue<TypeParam>(2)));
}

TYPED_TEST(TypeTest, CanBeConstructedFromMoveOnlyType) {
  using FooType = Type<class FooTag, std::unique_ptr<TypeParam>>;

  FooType a(std::make_unique<TypeParam>(GetExampleValue<TypeParam>(0)));
  EXPECT_EQ(*a.value(), GetExampleValue<TypeParam>(0));

  auto bare_value = std::make_unique<TypeParam>(GetExampleValue<TypeParam>(1));
  FooType b(std::move(bare_value));
  EXPECT_EQ(*b.value(), GetExampleValue<TypeParam>(1));
}

TYPED_TEST(TypeTest, SizeSameAsUnderlyingType) {
  using FooType = Type<class FooTag, TypeParam>;
  static_assert(sizeof(FooType) == sizeof(TypeParam),
                "Type should be as large as the underlying type.");
}

TYPED_TEST(TypeTest, IsDefaultConstructible) {
  using FooType = Type<class FooTag, TypeParam>;
  static_assert(std::is_default_constructible_v<FooType>,
                "Should be possible to default-construct a Type.");
}

TEST(TypeTest, TrivialTypeIsStandardLayout) {
  using FooType = Type<class FooTag, int>;
  static_assert(std::is_standard_layout_v<FooType>,
                "int-based type should have standard layout. ");
  static_assert(std::is_trivially_copyable_v<FooType>,
                "int-based type should be trivially copyable. ");
}

TYPED_TEST(TypeTest, CannotBeCreatedFromDifferentType) {
  using FooType = Type<class FooTag, TypeParam>;
  using BarType = Type<class BarTag, TypeParam>;
  static_assert(!std::is_constructible_v<FooType, BarType>,
                "Should be impossible to construct FooType from a BarType.");
  static_assert(!std::is_convertible_v<BarType, FooType>,
                "Should be impossible to convert a BarType into FooType.");
}

TYPED_TEST(TypeTest, CannotBeImplicitlyConverterToUnderlyingValue) {
  using FooType = Type<class FooTag, TypeParam>;
  static_assert(!std::is_convertible_v<FooType, TypeParam>,
                "Should be impossible to implicitly convert a Type into "
                "an underlying type.");
}

TYPED_TEST(TypeTest, ComparesEqualToSameValue) {
  using FooType = Type<class FooTag, TypeParam>;

  const FooType a = FooType(GetExampleValue<TypeParam>(0));
  EXPECT_EQ(a, a);
  EXPECT_FALSE(a != a);
  EXPECT_TRUE(a >= a);
  EXPECT_TRUE(a <= a);
  EXPECT_FALSE(a > a);
  EXPECT_FALSE(a < a);

  const FooType b = FooType(GetExampleValue<TypeParam>(0));
  EXPECT_EQ(a, b);
  EXPECT_FALSE(a != b);
  EXPECT_TRUE(a >= b);
  EXPECT_TRUE(a <= b);
  EXPECT_FALSE(a > b);
  EXPECT_FALSE(a < b);
}

TYPED_TEST(TypeTest, ComparesCorrectlyToDifferentValue) {
  using FooType = Type<class FooTag, TypeParam>;
  const FooType a = FooType(GetExampleValue<TypeParam>(0));
  const FooType b = FooType(GetExampleValue<TypeParam>(1));
  EXPECT_NE(a, b);
  EXPECT_FALSE(a == b);
  EXPECT_TRUE(b >= a);
  EXPECT_TRUE(a <= b);
  EXPECT_TRUE(b > a);
  EXPECT_TRUE(a < b);
}

TEST(TypeTest, CanBeDerivedFrom) {
  class CountryCode : public Type<CountryCode, std::string> {
   public:
    explicit CountryCode(std::string&& value)
        : Type<CountryCode, std::string>::Type(std::move(value)) {
      if (value_.size() != 2)
        value_.clear();
    }

    bool IsNull() const { return value_.empty(); }
  };

  CountryCode valid("US");
  EXPECT_FALSE(valid.IsNull());

  CountryCode invalid("United States");
  EXPECT_TRUE(invalid.IsNull());
}

TEST(TypeTest, CanWrapComplexStructures) {
  using PairOfStrings = std::pair<std::string, std::string>;
  using ComplexType = Type<class FooTag, PairOfStrings>;

  ComplexType a1(std::make_pair("aaa", "bbb"));
  ComplexType a2(std::make_pair("ccc", "ddd"));
  EXPECT_TRUE(a1 < a2);

  EXPECT_TRUE(a1.value() == PairOfStrings("aaa", "bbb"));
}

TYPED_TEST(TypeTest, CanBeKeysInStdUnorderedMap) {
  using FooType = Type<class FooTag, TypeParam>;
  std::unordered_map<FooType, std::string> map;

  FooType k1(GetExampleValue<TypeParam>(0));
  FooType k2(GetExampleValue<TypeParam>(1));

  map[k1] = "value1";
  map[k2] = "value2";

  EXPECT_EQ(map[k1], "value1");
  EXPECT_EQ(map[k2], "value2");
}

TYPED_TEST(TypeTest, CanBeKeysInStdMap) {
  using FooType = Type<class FooTag, TypeParam>;
  std::map<FooType, std::string> map;

  FooType k1(GetExampleValue<TypeParam>(0));
  FooType k2(GetExampleValue<TypeParam>(1));

  map[k1] = "value1";
  map[k2] = "value2";

  EXPECT_EQ(map[k1], "value1");
  EXPECT_EQ(map[k2], "value2");
}

TYPED_TEST(TypeTest, CanDifferentiateOverloads) {
  using FooType = Type<class FooTag, TypeParam>;
  using BarType = Type<class BarTag, TypeParam>;
  class Scope {
   public:
    static std::string Overload(FooType) { return "FooType"; }
    static std::string Overload(BarType) { return "BarType"; }
  };
  EXPECT_EQ("FooType", Scope::Overload(FooType()));
  EXPECT_EQ("BarType", Scope::Overload(BarType()));
}

}  // namespace rst
