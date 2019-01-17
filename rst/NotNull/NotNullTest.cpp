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

#include "rst/NotNull/NotNull.h"

#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <gtest/gtest.h>

namespace rst {
namespace {

class Base {
 public:
  virtual ~Base() = default;

  virtual std::string_view GetName() const { return "Base"; }
};

class Derived : public Base {
 public:
  std::string_view GetName() const override { return "Derived"; }
};

}  // namespace

TEST(NotNull, VoidPtr) {
  std::string str;
  NotNull<void*> void_ptr(&str);
  EXPECT_EQ(void_ptr.get(), &str);

  NotNull<void*> copy_ptr(void_ptr);
  EXPECT_EQ(copy_ptr.get(), void_ptr.get());

  NotNull<void*> move_ptr(std::move(copy_ptr));
  EXPECT_EQ(move_ptr.get(), void_ptr.get());

  std::string str2;
  NotNull<void*> void_ptr2(&str2);
  EXPECT_NE(void_ptr2.get(), move_ptr.get());

  void_ptr2 = move_ptr;
  EXPECT_EQ(void_ptr2.get(), move_ptr.get());

  NotNull<void*> void_ptr3(&str2);
  EXPECT_NE(void_ptr3.get(), void_ptr.get());

  void_ptr3 = std::move(move_ptr);
  EXPECT_EQ(void_ptr3.get(), void_ptr.get());

  NotNull<void*> void_ptr4(&str);
  EXPECT_NE(void_ptr4.get(), &str2);

  void_ptr4 = &str2;
  EXPECT_EQ(void_ptr4.get(), &str2);
}

TEST(NotNull, PlainPtr) {
  std::string str;
  NotNull<std::string*> str_ptr(&str);
  str_ptr->append("PlainPtr");
  EXPECT_EQ(*str_ptr, "PlainPtr");

  NotNull<std::string*> copy_ptr(str_ptr);
  EXPECT_EQ(*copy_ptr, "PlainPtr");

  NotNull<std::string*> move_ptr(std::move(copy_ptr));
  EXPECT_EQ(*move_ptr, "PlainPtr");

  Derived d;
  NotNull<Base*> base_ptr(&d);
  EXPECT_EQ(base_ptr->GetName(), "Derived");

  NotNull<Base*> base_copy_ptr(base_ptr);
  EXPECT_EQ(base_copy_ptr->GetName(), "Derived");

  Base b;
  NotNull<Base*> base_ptr2(&b);
  EXPECT_EQ(base_ptr2->GetName(), "Base");

  base_ptr2 = base_ptr;
  EXPECT_EQ(base_ptr2->GetName(), "Derived");

  NotNull<Base*> base_ptr3(&b);
  EXPECT_EQ(base_ptr3->GetName(), "Base");

  base_ptr3 = std::move(base_ptr);
  EXPECT_EQ(base_ptr3->GetName(), "Derived");
}

TEST(NotNull, PlainPtrBaseDerived) {
  const auto foo_base = [](NotNull<Base*> b) -> void {
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](NotNull<Base*> b) -> void {
    EXPECT_EQ(b->GetName(), "Derived");
  };
  class C {
   public:
    C(NotNull<Base*>) {}
  };

  Base b;
  foo_base(&b);
  { C c(&b); }

  Derived d;
  foo_derived(&d);
  { C c(&d); }

  {
    NotNull<Derived*> derived_ptr(&d);
    NotNull<Base*> base_ptr(derived_ptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    NotNull<Derived*> derived_ptr(&d);
    NotNull<Base*> base_ptr(&b);
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = derived_ptr;
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }
}

TEST(NotNull, Crash) {
  {
    std::string str;
    std::string* p = nullptr;

    EXPECT_DEATH((NotNull<std::string*>(p)), "");

    NotNull<std::string*> str_ptr(&str);
    EXPECT_DEATH(str_ptr = p, "");
  }

  {
    auto str = std::make_unique<std::string>();
    std::unique_ptr<std::string> p;

    EXPECT_DEATH((NotNull<std::unique_ptr<std::string>>(std::move(p))), "");

    NotNull<std::unique_ptr<std::string>> str_ptr(std::move(str));
    EXPECT_DEATH(str_ptr = std::move(p), "");
  }

  {
    auto str = std::make_shared<std::string>();
    std::shared_ptr<std::string> p;

    EXPECT_DEATH((NotNull<std::shared_ptr<std::string>>(std::move(p))), "");

    NotNull<std::shared_ptr<std::string>> str_ptr(std::move(str));
    EXPECT_DEATH(str_ptr = std::move(p), "");
  }
}

TEST(NotNull, Take) {
  {
    NotNull<std::unique_ptr<std::string>> str_ptr(
        std::make_unique<std::string>("UniquePtr"));
    const std::unique_ptr<std::string> str = str_ptr.Take();
    EXPECT_EQ(*str, "UniquePtr");
  }

  {
    NotNull<std::shared_ptr<std::string>> str_ptr(
        std::make_shared<std::string>("SharedPtr"));
    const std::shared_ptr<std::string> str = str_ptr.Take();
    EXPECT_EQ(*str, "SharedPtr");
  }
}

TEST(NotNull, Operators) {
  {
    std::string str;
    NotNull<std::string*> str_ptr(&str);
    str_ptr->append("1");
    EXPECT_EQ(*str_ptr, "1");

    *str_ptr = "2";
    EXPECT_EQ(*str_ptr, "2");

    int array[] = {0, 1, 2};
    NotNull<int*> int_ptr(array);
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);

    int_ptr[0] = 3;
    int_ptr[1] = 4;
    int_ptr[2] = 5;
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);
  }

  {
    std::string str;
    const NotNull<std::string*> str_ptr(&str);
    str_ptr->append("1");
    EXPECT_EQ(*str_ptr, "1");

    *str_ptr = "2";
    EXPECT_EQ(*str_ptr, "2");

    int array[] = {0, 1, 2};
    const NotNull<int*> int_ptr(array);
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);

    int_ptr[0] = 3;
    int_ptr[1] = 4;
    int_ptr[2] = 5;
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);
  }
}

TEST(NotNull, UniquePtr) {
  auto str = std::make_unique<std::string>();
  NotNull<std::unique_ptr<std::string>> str_ptr(std::move(str));
  str_ptr->append("UniquePtr");
  EXPECT_EQ(*str_ptr, "UniquePtr");

  NotNull<std::unique_ptr<std::string>> move_ptr(std::move(str_ptr));
  EXPECT_EQ(*move_ptr, "UniquePtr");

  auto d = std::make_unique<Derived>();
  NotNull<std::unique_ptr<Base>> base_ptr(std::move(d));
  EXPECT_EQ(base_ptr->GetName(), "Derived");

  auto b = std::make_unique<Base>();
  NotNull<std::unique_ptr<Base>> base_ptr3(std::move(b));
  EXPECT_EQ(base_ptr3->GetName(), "Base");

  base_ptr3 = std::move(base_ptr);
  EXPECT_EQ(base_ptr3->GetName(), "Derived");
}

TEST(NotNull, UniquePtrBaseDerived) {
  const auto foo_base = [](NotNull<std::unique_ptr<Base>> b) -> void {
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](NotNull<std::unique_ptr<Base>> b) -> void {
    EXPECT_EQ(b->GetName(), "Derived");
  };
  class C {
   public:
    C(NotNull<std::unique_ptr<Base>>) {}
  };

  foo_base(std::make_unique<Base>());
  { C c(std::make_unique<Base>()); }

  foo_derived(std::make_unique<Derived>());
  { C c(std::make_unique<Derived>()); }

  {
    NotNull<std::unique_ptr<Derived>> derived_ptr(std::make_unique<Derived>());
    NotNull<std::unique_ptr<Base>> base_ptr(std::move(derived_ptr));
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    NotNull<std::unique_ptr<Derived>> derived_ptr(std::make_unique<Derived>());
    NotNull<std::unique_ptr<Base>> base_ptr(std::make_unique<Base>());
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = std::move(derived_ptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }
}

TEST(NotNull, SharedPtr) {
  auto str = std::make_shared<std::string>();
  NotNull<std::shared_ptr<std::string>> str_ptr(std::move(str));
  str_ptr->append("SharedPtr");
  EXPECT_EQ(*str_ptr, "SharedPtr");

  NotNull<std::shared_ptr<std::string>> copy_ptr(str_ptr);
  EXPECT_EQ(*copy_ptr, "SharedPtr");

  NotNull<std::shared_ptr<std::string>> move_ptr(std::move(copy_ptr));
  EXPECT_EQ(*move_ptr, "SharedPtr");

  auto d = std::make_shared<Derived>();
  NotNull<std::shared_ptr<Base>> base_ptr(std::move(d));
  EXPECT_EQ(base_ptr->GetName(), "Derived");

  NotNull<std::shared_ptr<Base>> base_copy_ptr(base_ptr);
  EXPECT_EQ(base_copy_ptr->GetName(), "Derived");

  auto b = std::make_shared<Base>();
  NotNull<std::shared_ptr<Base>> base_ptr2(b);
  EXPECT_EQ(base_ptr2->GetName(), "Base");

  base_ptr2 = base_ptr;
  EXPECT_EQ(base_ptr2->GetName(), "Derived");

  NotNull<std::shared_ptr<Base>> base_ptr3(b);
  EXPECT_EQ(base_ptr3->GetName(), "Base");

  base_ptr3 = std::move(base_ptr);
  EXPECT_EQ(base_ptr3->GetName(), "Derived");
}

TEST(NotNull, SharedPtrBaseDerived) {
  const auto foo_base = [](NotNull<std::shared_ptr<Base>> b) -> void {
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](NotNull<std::shared_ptr<Base>> b) -> void {
    EXPECT_EQ(b->GetName(), "Derived");
  };
  class C {
   public:
    C(NotNull<std::shared_ptr<Base>>) {}
  };

  foo_base(std::make_shared<Base>());
  { C c(std::make_shared<Base>()); }

  foo_derived(std::make_shared<Derived>());
  { C c(std::make_shared<Derived>()); }

  {
    NotNull<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    NotNull<std::shared_ptr<Base>> base_ptr(derived_ptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    NotNull<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    NotNull<std::shared_ptr<Base>> base_ptr(std::make_shared<Base>());
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = derived_ptr;
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    NotNull<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    NotNull<std::shared_ptr<Base>> base_ptr(std::move(derived_ptr));
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    NotNull<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    NotNull<std::shared_ptr<Base>> base_ptr(std::make_shared<Base>());
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = std::move(derived_ptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }
}

TEST(Nullable, Test) {
  static_assert(std::is_same<std::string*, Nullable<std::string*>>::value);
  static_assert(
      std::is_same<const std::string*, Nullable<const std::string*>>::value);
}

}  // namespace rst
