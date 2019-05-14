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

#include <map>
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

TEST(NotNull, PlainPtrNullable) {
  std::string str;
  std::string str2;

  Nullable<std::string*> nullable(&str);
  EXPECT_NE(nullable, nullptr);

  Nullable<std::string*> nullable2(&str2);
  EXPECT_NE(nullable2, nullptr);

  {
    NotNull<std::string*> ptr(nullable);
    EXPECT_EQ(ptr.get(), nullable.get());

    ptr = nullable2;
    EXPECT_EQ(ptr.get(), nullable2.get());
  }
  {
    const auto prev_ptr = nullable.get();
    NotNull<std::string*> ptr(std::move(nullable));
    EXPECT_EQ(ptr.get(), prev_ptr);

    const auto prev_ptr2 = nullable2.get();
    ptr = std::move(nullable2);
    EXPECT_EQ(ptr.get(), prev_ptr2);
  }
}

TEST(NotNull, PlainPtrBaseDerived) {
  const auto foo_base = [](NotNull<Base*> b) {
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](NotNull<Base*> b) {
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

    Nullable<std::string*> nullable;
    EXPECT_EQ(nullable, nullptr);
    EXPECT_DEATH((NotNull<std::string*>(nullable)), "");
    EXPECT_DEATH((str_ptr = nullable), "");
    EXPECT_DEATH((NotNull<std::string*>(std::move(nullable))), "");
    Nullable<std::string*> nullable2;
    EXPECT_EQ(nullable2, nullptr);
    EXPECT_DEATH((str_ptr = std::move(nullable2)), "");
  }

  {
    auto str = std::make_unique<std::string>();
    std::unique_ptr<std::string> p;

    EXPECT_DEATH((NotNull<std::unique_ptr<std::string>>(std::move(p))), "");

    NotNull<std::unique_ptr<std::string>> str_ptr(std::move(str));
    EXPECT_DEATH(str_ptr = std::move(p), "");

    Nullable<std::unique_ptr<std::string>> nullable;
    EXPECT_EQ(nullable, nullptr);
    EXPECT_DEATH((NotNull<std::unique_ptr<std::string>>(std::move(nullable))),
                 "");

    Nullable<std::unique_ptr<std::string>> nullable2;
    EXPECT_EQ(nullable2, nullptr);
    EXPECT_DEATH((str_ptr = std::move(nullable2)), "");
  }

  {
    auto str = std::make_shared<std::string>();
    std::shared_ptr<std::string> p;

    EXPECT_DEATH((NotNull<std::shared_ptr<std::string>>(std::move(p))), "");

    NotNull<std::shared_ptr<std::string>> str_ptr(std::move(str));
    EXPECT_DEATH(str_ptr = std::move(p), "");

    Nullable<std::shared_ptr<std::string>> nullable;
    EXPECT_EQ(nullable, nullptr);
    EXPECT_DEATH((NotNull<std::shared_ptr<std::string>>(nullable.Clone())), "");
    EXPECT_DEATH((str_ptr = nullable.Clone()), "");
    EXPECT_DEATH((NotNull<std::shared_ptr<std::string>>(std::move(nullable))),
                 "");
    Nullable<std::shared_ptr<std::string>> nullable2;
    EXPECT_EQ(nullable2, nullptr);
    EXPECT_DEATH((str_ptr = std::move(nullable2)), "");
  }
}

TEST(NotNull, Take) {
  {
    NotNull<std::unique_ptr<std::string>> str_ptr(
        std::make_unique<std::string>("UniquePtr"));
    const std::unique_ptr<std::string> str = str_ptr.Take();
    EXPECT_EQ(*str, "UniquePtr");
    EXPECT_DEATH(NotNull<std::unique_ptr<std::string>>(std::move(str_ptr)), "");
    NotNull<std::unique_ptr<std::string>> str_ptr2(
        std::make_unique<std::string>("UniquePtr"));
    EXPECT_DEATH(str_ptr2 = std::move(str_ptr), "");
  }

  {
    NotNull<std::shared_ptr<std::string>> str_ptr(
        std::make_shared<std::string>("SharedPtr"));
    const std::shared_ptr<std::string> str = str_ptr.Take();
    EXPECT_EQ(*str, "SharedPtr");
    EXPECT_DEATH(NotNull<std::shared_ptr<std::string>>(std::move(str_ptr)), "");
    NotNull<std::shared_ptr<std::string>> str_ptr2(
        std::make_shared<std::string>("UniquePtr"));
    EXPECT_DEATH(str_ptr2 = std::move(str_ptr), "");
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

    std::string str2;
    NotNull<std::string*> str_ptr2(&str2);
    EXPECT_NE(str_ptr, str_ptr2);
    EXPECT_NE(str_ptr2, str_ptr);

    NotNull<std::string*> str_ptr3(&str);
    EXPECT_EQ(str_ptr, str_ptr3);
    EXPECT_EQ(str_ptr3, str_ptr);
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

    std::string str2;
    const NotNull<std::string*> str_ptr2(&str2);
    EXPECT_NE(str_ptr, str_ptr2);
    EXPECT_NE(str_ptr2, str_ptr);

    const NotNull<std::string*> str_ptr3(&str);
    EXPECT_EQ(str_ptr, str_ptr3);
    EXPECT_EQ(str_ptr3, str_ptr);
  }

  {
    std::map<NotNull<std::string*>, bool> map;
    std::string s1, s2;
    map.emplace(&s1, true);
    map.emplace(&s2, true);
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

TEST(NotNull, UniquePtrNullable) {
  Nullable<std::unique_ptr<std::string>> nullable(
      std::make_unique<std::string>());
  EXPECT_NE(nullable, nullptr);

  Nullable<std::unique_ptr<std::string>> nullable2(
      std::make_unique<std::string>());
  EXPECT_NE(nullable2, nullptr);

  {
    const auto prev_ptr = nullable.get();
    NotNull<std::unique_ptr<std::string>> ptr(std::move(nullable));
    EXPECT_EQ(ptr.get(), prev_ptr);

    const auto prev_ptr2 = nullable2.get();
    ptr = std::move(nullable2);
    EXPECT_EQ(ptr.get(), prev_ptr2);
  }
}

TEST(NotNull, UniquePtrBaseDerived) {
  const auto foo_base = [](NotNull<std::unique_ptr<Base>> b) {
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](NotNull<std::unique_ptr<Base>> b) {
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

  NotNull<std::shared_ptr<std::string>> copy_ptr(str_ptr.Clone());
  EXPECT_EQ(*copy_ptr, "SharedPtr");

  NotNull<std::shared_ptr<std::string>> move_ptr(std::move(copy_ptr));
  EXPECT_EQ(*move_ptr, "SharedPtr");

  auto d = std::make_shared<Derived>();
  NotNull<std::shared_ptr<Base>> base_ptr(std::move(d));
  EXPECT_EQ(base_ptr->GetName(), "Derived");

  NotNull<std::shared_ptr<Base>> base_copy_ptr(base_ptr.Clone());
  EXPECT_EQ(base_copy_ptr->GetName(), "Derived");

  auto b = std::make_shared<Base>();
  NotNull<std::shared_ptr<Base>> base_ptr2((std::shared_ptr(b)));
  EXPECT_EQ(base_ptr2->GetName(), "Base");

  base_ptr2 = base_ptr.Clone();
  EXPECT_EQ(base_ptr2->GetName(), "Derived");

  NotNull<std::shared_ptr<Base>> base_ptr3((std::shared_ptr(b)));
  EXPECT_EQ(base_ptr3->GetName(), "Base");

  base_ptr3 = std::move(base_ptr);
  EXPECT_EQ(base_ptr3->GetName(), "Derived");
}

TEST(NotNull, SharedPtrNullable) {
  Nullable<std::shared_ptr<std::string>> nullable(
      std::make_shared<std::string>());
  EXPECT_NE(nullable, nullptr);

  Nullable<std::shared_ptr<std::string>> nullable2(
      std::make_shared<std::string>());
  EXPECT_NE(nullable2, nullptr);

  {
    NotNull<std::shared_ptr<std::string>> ptr(nullable.Clone());
    EXPECT_EQ(ptr.get(), nullable.get());

    ptr = nullable2.Clone();
    EXPECT_EQ(ptr.get(), nullable2.get());
  }
  {
    const auto prev_ptr = nullable.get();
    NotNull<std::shared_ptr<std::string>> ptr(std::move(nullable));
    EXPECT_EQ(ptr.get(), prev_ptr);

    const auto prev_ptr2 = nullable2.get();
    ptr = std::move(nullable2);
    EXPECT_EQ(ptr.get(), prev_ptr2);
  }
}

TEST(NotNull, SharedPtrBaseDerived) {
  const auto foo_base = [](NotNull<std::shared_ptr<Base>> b) {
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](NotNull<std::shared_ptr<Base>> b) {
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
    NotNull<std::shared_ptr<Base>> base_ptr(derived_ptr.Clone());
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    NotNull<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    NotNull<std::shared_ptr<Base>> base_ptr(std::make_shared<Base>());
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = derived_ptr.Clone();
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

TEST(Nullable, VoidPtr) {
  std::string str;
  Nullable<void*> void_ptr(&str);
  EXPECT_EQ(void_ptr.get(), &str);

  Nullable<void*> copy_ptr(void_ptr);
  EXPECT_EQ(copy_ptr.get(), void_ptr.get());

  Nullable<void*> move_ptr(std::move(copy_ptr));
  EXPECT_EQ(move_ptr.get(), void_ptr.get());

  std::string str2;
  Nullable<void*> void_ptr2(&str2);
  EXPECT_NE(void_ptr2.get(), move_ptr.get());

  void_ptr2 = move_ptr;
  EXPECT_EQ(void_ptr2.get(), move_ptr.get());

  Nullable<void*> void_ptr3(&str2);
  EXPECT_NE(void_ptr3.get(), void_ptr.get());

  void_ptr3 = std::move(move_ptr);
  EXPECT_EQ(void_ptr3.get(), void_ptr.get());

  Nullable<void*> void_ptr4(&str);
  EXPECT_NE(void_ptr4.get(), &str2);

  void_ptr4 = &str2;
  EXPECT_EQ(void_ptr4.get(), &str2);
}

TEST(Nullable, PlainPtr) {
  std::string str;
  Nullable<std::string*> str_ptr(&str);
  ASSERT_NE(str_ptr, nullptr);
  str_ptr->append("PlainPtr");
  EXPECT_EQ(*str_ptr, "PlainPtr");

  Nullable<std::string*> copy_ptr(str_ptr);
  ASSERT_NE(copy_ptr, nullptr);
  EXPECT_EQ(*copy_ptr, "PlainPtr");

  Nullable<std::string*> move_ptr(std::move(copy_ptr));
  ASSERT_NE(move_ptr, nullptr);
  EXPECT_EQ(*move_ptr, "PlainPtr");

  Derived d;
  Nullable<Base*> base_ptr(&d);
  ASSERT_NE(base_ptr, nullptr);
  EXPECT_EQ(base_ptr->GetName(), "Derived");

  Nullable<Base*> base_copy_ptr(base_ptr);
  ASSERT_NE(base_copy_ptr, nullptr);
  EXPECT_EQ(base_copy_ptr->GetName(), "Derived");

  Base b;
  Nullable<Base*> base_ptr2(&b);
  ASSERT_NE(base_ptr2, nullptr);
  EXPECT_EQ(base_ptr2->GetName(), "Base");

  base_ptr2 = base_ptr;
  ASSERT_NE(base_ptr2, nullptr);
  EXPECT_EQ(base_ptr2->GetName(), "Derived");

  Nullable<Base*> base_ptr3(&b);
  ASSERT_NE(base_ptr3, nullptr);
  EXPECT_EQ(base_ptr3->GetName(), "Base");

  base_ptr3 = std::move(base_ptr);
  ASSERT_NE(base_ptr3, nullptr);
  EXPECT_EQ(base_ptr3->GetName(), "Derived");

  Nullable<void*> null_ptr;
  EXPECT_EQ(null_ptr, nullptr);

  Nullable<void*> null_ptr2(nullptr);
  EXPECT_EQ(null_ptr2, nullptr);

  null_ptr2 = nullptr;
  EXPECT_EQ(null_ptr2, nullptr);
}

TEST(Nullable, PlainPtrNotNull) {
  std::string str;
  std::string str2;

  NotNull<std::string*> not_null(&str);
  NotNull<std::string*> not_null2(&str2);

  {
    Nullable<std::string*> ptr(not_null);
    EXPECT_EQ(ptr.get(), not_null.get());

    ptr = not_null2;
    EXPECT_EQ(ptr.get(), not_null2.get());
  }
  {
    const auto prev_ptr = not_null.get();
    Nullable<std::string*> ptr(std::move(not_null));
    EXPECT_EQ(ptr.get(), prev_ptr);

    const auto prev_ptr2 = not_null2.get();
    ptr = std::move(not_null2);
    EXPECT_EQ(ptr.get(), prev_ptr2);
  }
}

TEST(Nullable, PlainPtrBaseDerived) {
  const auto foo_base = [](Nullable<Base*> b) {
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](Nullable<Base*> b) {
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->GetName(), "Derived");
  };
  class C {
   public:
    C(Nullable<Base*>) {}
  };

  Base b;
  foo_base(&b);
  { C c(&b); }

  Derived d;
  foo_derived(&d);
  { C c(&d); }

  {
    Nullable<Derived*> derived_ptr(&d);
    Nullable<Base*> base_ptr(derived_ptr);
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    Nullable<Derived*> derived_ptr(&d);
    Nullable<Base*> base_ptr(&b);
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = derived_ptr;
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }
}

TEST(Nullable, Crash) {
  {
    std::string array[] = {"a", "b", "c"};
    Nullable<std::string*> ptr(array);

    EXPECT_NO_FATAL_FAILURE(ptr.get());
    EXPECT_DEATH((ptr->size()), "");
    EXPECT_DEATH((*ptr), "");
    EXPECT_DEATH((ptr[0]), "");
    EXPECT_DEATH((ptr[1]), "");

    ptr = nullptr;
    EXPECT_EQ(ptr, nullptr);

    EXPECT_NO_FATAL_FAILURE(ptr.get());
    EXPECT_DEATH((ptr->size()), "");
    EXPECT_DEATH((*ptr), "");
    EXPECT_DEATH((ptr[0]), "");
    EXPECT_DEATH((ptr[1]), "");
  }

  {
    auto str = std::make_unique<std::string>("a");
    Nullable<std::unique_ptr<std::string>> ptr(std::move(str));

    EXPECT_NO_FATAL_FAILURE(ptr.get());
    EXPECT_DEATH((ptr->size()), "");
    EXPECT_DEATH((*ptr), "");

    ptr = nullptr;
    EXPECT_EQ(ptr, nullptr);

    EXPECT_NO_FATAL_FAILURE(ptr.get());
    EXPECT_DEATH((ptr->size()), "");
    EXPECT_DEATH((*ptr), "");
  }

  {
    auto str = std::make_shared<std::string>("a");
    Nullable<std::shared_ptr<std::string>> ptr(std::move(str));

    EXPECT_NO_FATAL_FAILURE(ptr.get());
    EXPECT_DEATH((ptr->size()), "");
    EXPECT_DEATH((*ptr), "");

    ptr = nullptr;
    EXPECT_EQ(ptr, nullptr);

    EXPECT_NO_FATAL_FAILURE(ptr.get());
    EXPECT_DEATH((ptr->size()), "");
    EXPECT_DEATH((*ptr), "");
  }
}

TEST(Nullable, Take) {
  {
    Nullable<std::unique_ptr<std::string>> str_ptr(
        std::make_unique<std::string>("UniquePtr"));
    const std::unique_ptr<std::string> str = str_ptr.Take();
    EXPECT_EQ(*str, "UniquePtr");
  }

  {
    Nullable<std::shared_ptr<std::string>> str_ptr(
        std::make_shared<std::string>("SharedPtr"));
    const std::shared_ptr<std::string> str = str_ptr.Take();
    EXPECT_EQ(*str, "SharedPtr");
  }
}

TEST(Nullable, Operators) {
  {
    std::string str;
    Nullable<std::string*> str_ptr(&str);
    ASSERT_NE(str_ptr, nullptr);
    str_ptr->append("1");
    EXPECT_EQ(*str_ptr, "1");

    *str_ptr = "2";
    EXPECT_EQ(*str_ptr, "2");

    int array[] = {0, 1, 2};
    Nullable<int*> int_ptr(array);
    ASSERT_NE(int_ptr, nullptr);
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);

    int_ptr[0] = 3;
    int_ptr[1] = 4;
    int_ptr[2] = 5;
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);

    EXPECT_NE(str_ptr, nullptr);
    EXPECT_NE(nullptr, str_ptr);

    Nullable<std::string*> str_ptr2;
    EXPECT_EQ(str_ptr2, nullptr);
    EXPECT_EQ(nullptr, str_ptr2);

    Nullable<std::string*> str_ptr3(&str);
    EXPECT_EQ(str_ptr, str_ptr3);
    EXPECT_EQ(str_ptr3, str_ptr);

    EXPECT_NE(str_ptr2, str_ptr3);
    EXPECT_NE(str_ptr3, str_ptr2);

    std::string str2;
    EXPECT_EQ(str_ptr, &str);
    EXPECT_EQ(&str, str_ptr);

    EXPECT_NE(str_ptr, &str2);
    EXPECT_NE(&str2, str_ptr);
  }

  {
    std::string str;
    const Nullable<std::string*> str_ptr(&str);
    ASSERT_NE(str_ptr, nullptr);
    str_ptr->append("1");
    EXPECT_EQ(*str_ptr, "1");

    *str_ptr = "2";
    EXPECT_EQ(*str_ptr, "2");

    int array[] = {0, 1, 2};
    const Nullable<int*> int_ptr(array);
    ASSERT_NE(int_ptr, nullptr);
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);

    int_ptr[0] = 3;
    int_ptr[1] = 4;
    int_ptr[2] = 5;
    EXPECT_EQ(int_ptr[0], array[0]);
    EXPECT_EQ(int_ptr[1], array[1]);
    EXPECT_EQ(int_ptr[2], array[2]);

    EXPECT_NE(str_ptr, nullptr);
    EXPECT_NE(nullptr, str_ptr);

    const Nullable<std::string*> str_ptr2;
    EXPECT_EQ(str_ptr2, nullptr);
    EXPECT_EQ(nullptr, str_ptr2);

    const Nullable<std::string*> str_ptr3(&str);
    EXPECT_EQ(str_ptr, str_ptr3);
    EXPECT_EQ(str_ptr3, str_ptr);

    EXPECT_NE(str_ptr2, str_ptr3);
    EXPECT_NE(str_ptr3, str_ptr2);

    std::string str2;
    EXPECT_EQ(str_ptr, &str);
    EXPECT_EQ(&str, str_ptr);

    EXPECT_NE(str_ptr, &str2);
    EXPECT_NE(&str2, str_ptr);
  }

  {
    std::map<Nullable<std::string*>, bool> map;
    std::string s1, s2;
    map.emplace(&s1, true);
    map.emplace(&s2, true);
  }
}

TEST(Nullable, UniquePtr) {
  auto str = std::make_unique<std::string>();
  Nullable<std::unique_ptr<std::string>> str_ptr(std::move(str));
  ASSERT_NE(str_ptr, nullptr);
  str_ptr->append("UniquePtr");
  EXPECT_EQ(*str_ptr, "UniquePtr");

  Nullable<std::unique_ptr<std::string>> move_ptr(std::move(str_ptr));
  ASSERT_NE(move_ptr, nullptr);
  EXPECT_EQ(*move_ptr, "UniquePtr");

  auto d = std::make_unique<Derived>();
  Nullable<std::unique_ptr<Base>> base_ptr(std::move(d));
  ASSERT_NE(base_ptr, nullptr);
  EXPECT_EQ(base_ptr->GetName(), "Derived");

  auto b = std::make_unique<Base>();
  Nullable<std::unique_ptr<Base>> base_ptr3(std::move(b));
  ASSERT_NE(base_ptr3, nullptr);
  EXPECT_EQ(base_ptr3->GetName(), "Base");

  base_ptr3 = std::move(base_ptr);
  ASSERT_NE(base_ptr3, nullptr);
  EXPECT_EQ(base_ptr3->GetName(), "Derived");

  Nullable<std::unique_ptr<std::string>> null_ptr;
  EXPECT_EQ(null_ptr, nullptr);

  Nullable<std::unique_ptr<std::string>> null_ptr2(nullptr);
  EXPECT_EQ(null_ptr2, nullptr);

  null_ptr2 = nullptr;
  EXPECT_EQ(null_ptr2, nullptr);
}

TEST(Nullable, UniquePtrNotNull) {
  NotNull<std::unique_ptr<std::string>> not_null(
      std::make_unique<std::string>());
  NotNull<std::unique_ptr<std::string>> not_null2(
      std::make_unique<std::string>());

  {
    const auto prev_ptr = not_null.get();
    Nullable<std::unique_ptr<std::string>> ptr(std::move(not_null));
    EXPECT_EQ(ptr.get(), prev_ptr);

    const auto prev_ptr2 = not_null2.get();
    ptr = std::move(not_null2);
    EXPECT_EQ(ptr.get(), prev_ptr2);
  }
}

TEST(Nullable, UniquePtrBaseDerived) {
  const auto foo_base = [](Nullable<std::unique_ptr<Base>> b) {
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](Nullable<std::unique_ptr<Base>> b) {
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->GetName(), "Derived");
  };
  class C {
   public:
    C(Nullable<std::unique_ptr<Base>>) {}
  };

  foo_base(std::make_unique<Base>());
  { C c(std::make_unique<Base>()); }

  foo_derived(std::make_unique<Derived>());
  { C c(std::make_unique<Derived>()); }

  {
    Nullable<std::unique_ptr<Derived>> derived_ptr(std::make_unique<Derived>());
    Nullable<std::unique_ptr<Base>> base_ptr(std::move(derived_ptr));
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    Nullable<std::unique_ptr<Derived>> derived_ptr(std::make_unique<Derived>());
    Nullable<std::unique_ptr<Base>> base_ptr(std::make_unique<Base>());
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = std::move(derived_ptr);
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }
}

TEST(Nullable, SharedPtr) {
  auto str = std::make_shared<std::string>();
  Nullable<std::shared_ptr<std::string>> str_ptr(std::move(str));
  ASSERT_NE(str_ptr, nullptr);
  str_ptr->append("SharedPtr");
  EXPECT_EQ(*str_ptr, "SharedPtr");

  Nullable<std::shared_ptr<std::string>> copy_ptr(str_ptr.Clone());
  ASSERT_NE(copy_ptr, nullptr);
  EXPECT_EQ(*copy_ptr, "SharedPtr");

  Nullable<std::shared_ptr<std::string>> move_ptr(std::move(copy_ptr));
  ASSERT_NE(move_ptr, nullptr);
  EXPECT_EQ(*move_ptr, "SharedPtr");

  auto d = std::make_shared<Derived>();
  Nullable<std::shared_ptr<Base>> base_ptr(std::move(d));
  ASSERT_NE(base_ptr, nullptr);
  EXPECT_EQ(base_ptr->GetName(), "Derived");

  Nullable<std::shared_ptr<Base>> base_copy_ptr(base_ptr.Clone());
  ASSERT_NE(base_copy_ptr, nullptr);
  EXPECT_EQ(base_copy_ptr->GetName(), "Derived");

  auto b = std::make_shared<Base>();
  Nullable<std::shared_ptr<Base>> base_ptr2((std::shared_ptr(b)));
  ASSERT_NE(base_ptr2, nullptr);
  EXPECT_EQ(base_ptr2->GetName(), "Base");

  base_ptr2 = base_ptr.Clone();
  ASSERT_NE(base_ptr2, nullptr);
  EXPECT_EQ(base_ptr2->GetName(), "Derived");

  Nullable<std::shared_ptr<Base>> base_ptr3((std::shared_ptr(b)));
  ASSERT_NE(base_ptr3, nullptr);
  EXPECT_EQ(base_ptr3->GetName(), "Base");

  base_ptr3 = std::move(base_ptr);
  ASSERT_NE(base_ptr3, nullptr);
  EXPECT_EQ(base_ptr3->GetName(), "Derived");

  Nullable<std::shared_ptr<std::string>> null_ptr;
  EXPECT_EQ(null_ptr, nullptr);

  Nullable<std::shared_ptr<std::string>> null_ptr2(nullptr);
  EXPECT_EQ(null_ptr2, nullptr);

  null_ptr2 = nullptr;
  EXPECT_EQ(null_ptr2, nullptr);
}

TEST(Nullable, SharedPtrNotNull) {
  NotNull<std::shared_ptr<std::string>> not_null(
      std::make_shared<std::string>());
  NotNull<std::shared_ptr<std::string>> not_null2(
      std::make_shared<std::string>());

  {
    Nullable<std::shared_ptr<std::string>> ptr(not_null.Clone());
    EXPECT_EQ(ptr.get(), not_null.get());

    ptr = not_null2.Clone();
    EXPECT_EQ(ptr.get(), not_null2.get());
  }
  {
    const auto prev_ptr = not_null.get();
    Nullable<std::shared_ptr<std::string>> ptr(std::move(not_null));
    EXPECT_EQ(ptr.get(), prev_ptr);

    const auto prev_ptr2 = not_null2.get();
    ptr = std::move(not_null2);
    EXPECT_EQ(ptr.get(), prev_ptr2);
  }
}

TEST(Nullable, SharedPtrBaseDerived) {
  const auto foo_base = [](Nullable<std::shared_ptr<Base>> b) {
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->GetName(), "Base");
  };
  const auto foo_derived = [](Nullable<std::shared_ptr<Base>> b) {
    ASSERT_NE(b, nullptr);
    EXPECT_EQ(b->GetName(), "Derived");
  };
  class C {
   public:
    C(Nullable<std::shared_ptr<Base>>) {}
  };

  foo_base(std::make_shared<Base>());
  { C c(std::make_shared<Base>()); }

  foo_derived(std::make_shared<Derived>());
  { C c(std::make_shared<Derived>()); }

  {
    Nullable<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    Nullable<std::shared_ptr<Base>> base_ptr(derived_ptr.Clone());
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    Nullable<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    Nullable<std::shared_ptr<Base>> base_ptr(std::make_shared<Base>());
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = derived_ptr.Clone();
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    Nullable<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    Nullable<std::shared_ptr<Base>> base_ptr(std::move(derived_ptr));
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }

  {
    Nullable<std::shared_ptr<Derived>> derived_ptr(std::make_shared<Derived>());
    Nullable<std::shared_ptr<Base>> base_ptr(std::make_shared<Base>());
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Base");
    base_ptr = std::move(derived_ptr);
    ASSERT_NE(base_ptr, nullptr);
    EXPECT_EQ(base_ptr->GetName(), "Derived");
  }
}

}  // namespace rst
