[![Build Status](https://travis-ci.com/sabbakumov/rst.svg?branch=master)](https://travis-ci.com/sabbakumov/rst)

# RST C++ library

This is the RST C++17 library.
It is licensed under the Simplified BSD License.

# Table of Contents
* [Getting the Code](#GettingTheCode)
* [Codemap](#Codemap)
  * [Bind](#Bind)
    * [Bind](#Bind2)
    * [DoNothing](#DoNothing)
    * [NullFunction](#NullFunction)
  * [Check](#Check)
  * [Clone](#Clone)
  * [Defer](#Defer)
  * [Files](#Files)
    * [WriteFile](#WriteFile)
    * [WriteImportantFile](#WriteImportantFile)
    * [ReadFile](#ReadFile)
  * [GUID](#GUID)
  * [Hidden String](#HiddenString)
  * [Logger](#Logger)
  * [Macros](#Macros)
    * [Macros](#Macros2)
      * [RST_DISALLOW_COPY](#RST_DISALLOW_COPY)
      * [RST_DISALLOW_ASSIGN](#RST_DISALLOW_ASSIGN)
      * [RST_DISALLOW_COPY_AND_ASSIGN](#RST_DISALLOW_COPY_AND_ASSIGN)
      * [RST_DISALLOW_IMPLICIT_CONSTRUCTORS](#RST_DISALLOW_IMPLICIT_CONSTRUCTORS)
      * [RST_CAT](#RST_CAT)
      * [RST_BUILDFLAG](#RST_BUILDFLAG)
    * [OS](#OS)
    * [Optimization](#Optimization)
    * [Thread Annotations](#ThreadAnnotations)
      * [RST_GUARDED_BY](#RST_GUARDED_BY)
      * [RST_PT_GUARDED_BY](#RST_PT_GUARDED_BY)
  * [Memory](#Memory)
    * [WrapUnique](#WrapUnique)
    * [WeakPtr](#WeakPtr)
  * [NoDestructor](#NoDestructor)
  * [NotNull](#NotNull)
    * [NotNull](#NotNull2)
    * [Nullable](#Nullable)
  * [Preferences](#Preferences)
  * [RandomDevice](#RandomDevice)
  * [RTTI](#RTTI)
  * [STL](#STL)
    * [Algorithm](#Algorithm)
    * [HashCombine](#HashCombine)
    * [Reversed](#Reversed)
    * [StringResizeUninitialized](#StringResizeUninitialized)
    * [TakeFunction](#TakeFunction)
    * [VectorBuilder](#VectorBuilder)
  * [Status](#Status)
    * [Status Macros](#StatusMacros)
      * [RST_TRY](#RST_TRY)
      * [RST_TRY_ASSIGN](#RST_TRY_ASSIGN)
      * [RST_TRY_ASSIGN_UNWRAP](#RST_TRY_ASSIGN_UNWRAP)
      * [RST_TRY_CREATE](#RST_TRY_CREATE)
    * [StatusOr](#StatusOr)
    * [Status](#Status2)
  * [Strings](#Strings)
    * [Format](#Format)
    * [StrCat](#StrCat)
  * [TaskRunner](#TaskRunner)
    * [PollingTaskRunner](#PollingTaskRunner)
    * [ThreadPoolTaskRunner](#ThreadPoolTaskRunner)
  * [Threading](#Threading)
    * [Barrier](#Barrier)
  * [Timer](#Timer)
    * [OneShotTimer](#OneShotTimer)
  * [Type](#Type)
  * [Value](#Value)

<a name="GettingTheCode"></a>
# Getting the Code
```bash
git clone https://github.com/sabbakumov/rst.git
cd rst
mkdir build && cd build
cmake ..
cmake --build . && ./rst_tests
```

The library doesn't use exceptions and RTTI, and they are disabled by default.
But you can enable compile support for them:
```bash
cmake .. -DRST_ENABLE_CXX_EXCEPTIONS=ON -DRST_ENABLE_CXX_RTTI=ON
```

You can enable ASAN build:
```bash
cmake .. -DRST_ENABLE_ASAN=ON
```

You can enable TSAN build:
```bash
cmake .. -DRST_ENABLE_TSAN=ON
```

You can enable UBSAN build:
```bash
cmake .. -DRST_ENABLE_UBSAN=ON
```

<a name="Codemap"></a>
# Codemap
<a name="Bind"></a>
## Bind
<a name="Bind2"></a>
### Bind
Like `std::bind()` but the returned function object doesn't invoke `rst::Bind`'s
first callable object argument when `rst::Bind`'s second argument
(`rst::WeakPtr` or `std::weak_ptr`) was invalidated.
  
```cpp
#include "rst/bind/bind.h"

class Controller : public rst::SupportsWeakPtr<Controller>,
                   public std::enable_shared_from_this<Controller> {
 public:
  void SpawnWorker() {
    Worker::StartNew(rst::Bind(&Controller::WorkComplete, AsWeakPtr()));
    Worker::StartNew(rst::Bind(&Controller::WorkComplete, weak_from_this()));
  }
  void WorkComplete(const Result& result) {}
};

class Worker {
 public:
  static void StartNew(std::function<void(const Result&)>&& callback) {
    new Worker(std::move(callback));
    // Asynchronous processing...
  }

 private:
  Worker(std::function<void(const Result&)>&& callback)
      : callback_(std::move(callback)) {}

  void DidCompleteAsynchronousProcessing(const Result& result) {
    callback_(result);  // Does nothing if the controller has been deleted.
    delete this;
  }

  const std::function<void(const Result&)> callback_;
};
```

<a name="DoNothing"></a>
### DoNothing
Creates a placeholder function object that will implicitly convert into any
`std::function` type, and does nothing when called.

```cpp
#include "rst/bind/bind_helpers.h"

using MyCallback = std::function<void(bool arg)>;
void MyFunction(const MyCallback& callback) {
  callback(true);  // Uh oh...
}

MyFunction(MyCallback());  // ... this will crash!

// Instead, use rst::DoNothing():
MyFunction(rst::DoNothing());  // Can be run, will no-op.
```

<a name="NullFunction"></a>
### NullFunction
Creates a null function object that will implicitly convert into any
`std::function` type.

```cpp
#include "rst/bind/bind_helpers.h"

using MyCallback = std::function<void(bool arg)>;
void MyFunction(const MyCallback& callback) {
  if (callback != nullptr)
    callback(true);
}

MyFunction(rst::NullFunction());
```

<a name="Check"></a>
## Check
Chromium-like checking macros for better programming error handling.

The `RST_CHECK()` macro will cause an immediate crash if its condition is not
met. `RST_DCHECK()` is like `RST_CHECK()` but is only compiled in debug build.
`RST_NOTREACHED()` is equivalent to `RST_DCHECK(false)`. Here are some rules for
using these:

Use `RST_DCHECK()` or `RST_NOTREACHED()` as assertions, e.g. to document pre-
and post-conditions. A `RST_DCHECK()` means "this condition must always be
true", not "this condition is normally true, but perhaps not in exceptional
cases." Things like disk corruption or strange network errors are examples
of exceptional circumstances that nevertheless should not result in
`RST_DCHECK()` failure.
A consequence of this is that you should not handle `RST_DCHECK()` failures,
even if failure would result in a crash. Attempting to handle a `RST_DCHECK()`
failure is a statement that the `RST_DCHECK()` can fail, which contradicts the
point of writing the `RST_DCHECK()`. In particular, do not write code like the
following:

```cpp
#include "rst/check/check.h"

RST_DCHECK(foo);
if (!foo)  // Eliminate this code.
  ...

if (!bar) {  // Replace this whole conditional with "RST_DCHECK(bar);".
  RST_NOTREACHED();
  return;
}
```

Use `RST_CHECK()` if the consequence of a failed assertion would be a security
vulnerability or a contract violation, where crashing is preferable.

If you want to do more complex logic in a debug build write the following:

```cpp
#include "rst/check/check.h"
#include "rst/macros/macros.h"

#if RST_BUILDFLAG(DCHECK_IS_ON)
// Some complex logic in a debug build.
#endif  // RST_BUILDFLAG(DCHECK_IS_ON)
```

<a name="Clone"></a>
## Clone
Used to explicitly copy the value and make it visible to the reader:

```cpp
#include "rst/clone/clone.h"

void ConsumeString(std::string&& val);

const std::string s = ...;
ConsumeString(rst::Clone(s));
```

<a name="Defer"></a>
## Defer
Executes a function object on scope exit.

```cpp
#include "rst/defer/defer.h"

void Foo() {
  std::FILE* f = std::fopen(...);
  RST_DEFER([f]() { std::fclose(f); });
}
```

<a name="Files"></a>
## Files
<a name="WriteFile"></a>
### WriteFile
Writes data to a file. Returns `rst::FileError` on error.

```cpp
#include "rst/files/file_utils.h"

const std::string_view data = ...;
rst::Status status = rst::WriteFile("filename.txt", data);
```

<a name="WriteImportantFile"></a>
### WriteImportantFile
Like `rst::WriteFile()` but ensures that the file won't be corrupted by
application crash during write. This is done by writing data to a file near the
destination file, and then renaming the temporary file to the destination one.

```cpp
#include "rst/files/file_utils.h"

const std::string_view data = ...;
rst::Status status = rst::WriteImportantFile("filename.txt", data);
```

<a name="ReadFile"></a>
### ReadFile
Reads content from a file to a string. Returns `rst::FileOpenError` if the file
can not be opened, `rst::FileError` on other error.

```cpp
#include "rst/files/file_utils.h"

rst::StatusOr<std::string> content = rst::ReadFile("filename.txt");
```

<a name="GUID"></a>
## GUID
```cpp
#include "rst/guid/guid.h"

// Generates a 128-bit random GUID in the form of version 4 as described in RFC
// 4122, section 4.4. The format of GUID version 4 must be
// xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx, where y is one of [8, 9, A, B]. The
// hexadecimal values "a" through "f" are output as lower case characters.
const rst::Guid guid;
const std::string guid_string = guid.AsString();

void TakeGuid(std::string_view guid);
TakeGuid(guid.AsStringView().value());

// Returns true if the input string conforms to the version 4 GUID format.
// Note that this does not check if the hexadecimal values "a" through "f" are
// in lower case characters, as Version 4 RFC says they're case insensitive.
// (Use IsValidGuidOutputString() for checking if the given string is valid
// output string).
RST_DCHECK(rst::Guid::IsValid(guid.AsStringView().value()));

// Returns true if the input string is valid version 4 GUID output string.
// This also checks if the hexadecimal values "a" through "f" are in lower case
// characters.
RST_DCHECK(rst::Guid::IsValidOutputString(guid.AsStringView().value()));
```

<a name="HiddenString"></a>
## Hidden String
Compile time encrypted string modified implementation originally taken from
https://stackoverflow.com/questions/7270473/compile-time-string-encryption.

Strings encrypted with this method are not visible directly in the binary.

```cpp
#include "rst/hidden_string/hidden_string.h"

RST_HIDDEN_STRING(kHidden, "Not visible");
RST_DCHECK(kHidden.Decrypt() == "Not visible");
```

<a name="Logger"></a>
## Logger
General logger component. Note that fatal logs abort the program.

```cpp
#include "rst/logger/file_name_sink.h"
#include "rst/logger/file_ptr_sink.h"
#include "rst/logger/logger.h"

// Constructs a logger with a custom sink.
auto sink = rst::FileNameSink::Create("log.txt");
RST_CHECK(!sink.err());
rst::Logger logger(std::move(*sink));

auto sink = std::make_unique<rst::FilePtrSink>(stderr);
rst::Logger logger(std::move(sink));

// To get logger macros working.
rst::Logger::SetGlobalLogger(&logger);

RST_LOG_INFO("Init subsystem A");
// DLOG versions log only in a debug build.
RST_DLOG_WARNING("Init subsystem A.B");

RST_LOG_DEBUG("message");
RST_LOG_INFO("message");
RST_LOG_WARNING("message");
RST_LOG_ERROR("message");
RST_LOG_FATAL("message");

RST_DLOG_DEBUG("message");
RST_DLOG_INFO("message");
RST_DLOG_WARNING("message");
RST_DLOG_ERROR("message");
RST_DLOG_FATAL("message");
```

<a name="Macros"></a>
## Macros
<a name="Macros2"></a>
### Macros
<a name="RST_DISALLOW_COPY"></a>
### RST_DISALLOW_COPY
Put this in the declarations for a class to be uncopyable.
```cpp
#include "rst/macros/macros.h"

class NonCopyable {
 private:
  RST_DISALLOW_COPY(NonCopyable);
};
```

<a name="RST_DISALLOW_ASSIGN"></a>
### RST_DISALLOW_ASSIGN
Put this in the declarations for a class to be unassignable.
```cpp
#include "rst/macros/macros.h"

class NonAssignable {
 private:
  RST_DISALLOW_ASSIGN(NonAssignable);
};
```

<a name="RST_DISALLOW_COPY_AND_ASSIGN"></a>
### RST_DISALLOW_COPY_AND_ASSIGN
Put this in the declarations for a class to be uncopyable and unassignable.
```cpp
#include "rst/macros/macros.h"

class NonCopyAssignable {
 private:
  RST_DISALLOW_COPY_AND_ASSIGN(NonCopyAssignable);
};
```

<a name="RST_DISALLOW_IMPLICIT_CONSTRUCTORS"></a>
### RST_DISALLOW_IMPLICIT_CONSTRUCTORS
A macro to disallow all the implicit constructors, namely the default
constructor, copy constructor and operator=() functions. This is especially
useful for classes containing only static methods.
```cpp
#include "rst/macros/macros.h"

class NonConstructible {
 private:
  RST_DISALLOW_IMPLICIT_CONSTRUCTORS(NonConstructible);
};
```

<a name="RST_CAT"></a>
### RST_CAT
This does a concatenation of two preprocessor arguments.
```cpp
#include "rst/macros/macros.h"

static constexpr auto kAb = "cd";
const std::string s = RST_CAT(kA, b);
RST_DCHECK(s == kAb);
```

<a name="RST_BUILDFLAG"></a>
### RST_BUILDFLAG
`RST_BUILDFLAG()` macro unmangles the names of the build flags in a way that
looks natural, and gives errors if the flag is not defined. Normally in the
preprocessor it's easy to make mistakes that interpret "you haven't done the
setup to know what the flag is" as "flag is off".
```cpp
#include "rst/macros/macros.h"

#define RST_BUILDFLAG_ENABLE_FOO() (true)

#if RST_BUILDFLAG(ENABLE_FOO)
// ...
#endif  // RST_BUILDFLAG(ENABLE_FOO)
```

<a name="OS"></a>
### OS
Macros to test the current OS.

```cpp
#include "rst/macros/macros.h"
#include "rst/macros/os.h"

#if RST_BUILDFLAG(OS_WIN)
// Windows code.
#endif  // RST_BUILDFLAG(OS_WIN)

#if RST_BUILDFLAG(OS_ANDROID)
// Android code.
#endif  // RST_BUILDFLAG(OS_ANDROID)
```

<a name="Optimization"></a>
### Optimization
Enables the compiler to prioritize compilation using static analysis for
likely paths within a boolean or switch branches.

```cpp
#include "rst/macros/optimization.h"

if (RST_LIKELY(expression))
  return result;  // Faster if more likely.
else
  return 0;

if (RST_UNLIKELY(expression))
  return result;
else
  return 0;  // Faster if more likely.

switch (RST_LIKELY_EQ(x, 5)) {
  default:
  case 0:
  case 3:
  case 5:  // Faster if more likely.
}
```

Compilers can use the information that a certain branch is not likely to be
taken (for instance, a `RST_CHECK()` failure) to optimize for the common case
in the absence of better information.

Recommendation: modern CPUs dynamically predict branch execution paths,
typically with accuracy greater than 97%. As a result, annotating every
branch in a codebase is likely counterproductive; however, annotating
specific branches that are both hot and consistently mispredicted is likely
to yield performance improvements.

<a name="ThreadAnnotations"></a>
### Thread Annotations
<a name="RST_GUARDED_BY"></a>
#### RST_GUARDED_BY
Documents if a shared field or global variable needs to be protected by a
mutex. Allows the user to specify a particular mutex that should be held
when accessing the annotated variable.

```cpp
#include "rst/macros/thread_annotations.h"

std::mutex mtx;
int i RST_GUARDED_BY(mtx);
```

<a name="RST_PT_GUARDED_BY"></a>
#### RST_PT_GUARDED_BY
Documents if the memory location pointed to by a pointer should be guarded
by a mutex when dereferencing the pointer.

```cpp
#include "rst/macros/thread_annotations.h"

std::mutex mtx;
int* p RST_PT_GUARDED_BY(mtx);
std::unique_ptr<int> p2 RST_PT_GUARDED_BY(mtx);
```

Note that a pointer variable to a shared memory location could itself be a
shared variable.

```cpp
#include "rst/macros/thread_annotations.h"

// |q|, guarded by |mtx1|, points to a shared memory location that is
// guarded by |mtx2|:
int* q RST_GUARDED_BY(mtx1) RST_PT_GUARDED_BY(mtx2);
```

<a name="Memory"></a>
## Memory
<a name="WrapUnique"></a>
### WrapUnique
Chromium-like `WrapUnique()`.

A helper to transfer ownership of a raw pointer to a `std::unique_ptr<T>`. It is
usually used inside factory methods.

```cpp
#include "rst/memory/memory.h"

class Foo {
 public:
  rst::NotNull<std::unique_ptr<Foo>> Create() {
    return rst::WrapUnique(new Foo());
  }

  rst::NotNull<std::unique_ptr<Foo>> CreateFromNotNull() {
    return rst::WrapUnique(rst::NotNull(new Foo()));
  }

 private:
  Foo() = default;
};
```

<a name="WeakPtr"></a>
### WeakPtr
Chromium-based `WeakPtr`.

Weak pointers are pointers to an object that do not affect its lifetime, and
which may be invalidated (i.e. reset to `nullptr`) by the object, or its
owner, at any time, most commonly when the object is about to be deleted.

Weak pointers are useful when an object needs to be accessed safely by one
or more objects other than its owner, and those callers can cope with the
object vanishing and e.g. tasks posted to it being silently dropped.
Reference-counting such an object would complicate the ownership graph and
make it harder to reason about the object's lifetime.

```cpp
#include "rst/memory/weak_ptr.h"

class Controller : public rst::SupportsWeakPtr<Controller> {
 public:
  void SpawnWorker() { Worker::StartNew(AsWeakPtr()); }
  void WorkComplete(const Result& result) {}
};

class Worker {
 public:
  static void StartNew(rst::WeakPtr<Controller>&& controller) {
    new Worker(std::move(controller));
    // Asynchronous processing...
  }

 private:
  Worker(rst::WeakPtr<Controller>&& controller)
      : controller_(std::move(controller)) {}

  void DidCompleteAsynchronousProcessing(const Result& result) {
    rst::Nullable<Controller*> controller = controller_.GetNullable();
    if (controller != nullptr)
      controller->WorkComplete(result);
  }

  const rst::WeakPtr<Controller> controller_;
};
```

With this implementation a caller may use `SpawnWorker()` to dispatch multiple
Workers and subsequently delete the Controller, without waiting for all
Workers to have completed.

<a name="NoDestructor"></a>
## NoDestructor
Chromium-like `NoDestructor` class.

A wrapper that makes it easy to create an object of type T with static
storage duration that:
* is only constructed on first access
* never invokes the destructor

Runtime constant example:
```cpp
#include "rst/no_destructor/no_destructor.h"

const std::string& GetLineSeparator() {
  static const rst::NoDestructor<std::string> s(5, '-');
  return *s;
}
```

More complex initialization with a lambda:
```cpp
#include "rst/no_destructor/no_destructor.h"

const std::string& GetSession() {
  static const rst::NoDestructor<std::string> session([] {
    std::string s(16);
    // ...
    return s;
  }());
  return *session;
}
```

`NoDestructor<T>` stores the object inline, so it also avoids a pointer
indirection and memory allocation.

Note that since the destructor is never run, this will leak memory if used
as a stack or member variable. Furthermore, a `NoDestructor<T>` should never
have global scope as that may require a static initializer.

<a name="NotNull"></a>
## NotNull
<a name="NotNull2"></a>
### NotNull
`NotNull` is a Microsoft GSL-like class that restricts a pointer or a smart
pointer to only hold non-null values. It doesn't support constructing and
assignment from `nullptr_t`. Also it asserts that the passed pointer is not
`nullptr`.

```cpp
#include "rst/not_null/not_null.h"

void Foo(rst::NotNul<int*>) {}

int i = 0;
Foo(&i);  // OK.
Foo(nullptr);  // Compilation error.
int* ptr = nullptr;
Foo(ptr);  // Asserts.
```

There are specializations for `std::unique_ptr` and `std::shared_ptr`. In order
to take the inner smart pointer use `Take()` method:
```cpp
rst::NotNull<std::unique_ptr<T>> p = ...;
std::unique_ptr<T> inner = std::move(p).Take();
```

Note `std::move(p)` is used to call `Take()`. It is a sign that `p` is in valid
but unspecified state. No method other than destructor can be called.

<a name="Nullable"></a>
### Nullable
`Nullable` is a class that explicitly states that a pointer or a smart pointer
can hold non-null values. It asserts that the object is checked for `nullptr`
after construction.

```cpp
#include "rst/not_null/not_null.h"

void Foo(rst::Nullable<int*> ptr) {
  if (ptr != nullptr)
    *ptr = 0;  // OK.
}

void Bar(rst::Nullable<int*> ptr) {
  *ptr = 0;  // Assert.
}
```

There are specializations for `std::unique_ptr` and `std::shared_ptr`. In order
to take the inner smart pointer use `Take()` method:
```cpp
rst::Nullable<std::unique_ptr<T>> p = ...;
std::unique_ptr<T> inner = std::move(p).Take();
```

Note `std::move(p)` is used to call `Take()`. It is a sign that `p` is in valid
but unspecified state. No method other than destructor can be called.

<a name="Preferences"></a>
## Preferences
A set of preferences stored in a `rst:PreferencesStore`.

`rst::Preferences` need to be registered with a type, dotted path, and a default
value before they can be used. A type can be any of the `rst::Value` types.

```cpp
#include "rst/preferences/memory_preferences_store.h"
#include "rst/preferences/preferences.h"

rst::Preferences preferences(std::make_unique<rst::MemoryPreferencesStore>());

preferences.RegisterIntPreference("int.preference", 10);
RST_DCHECK(preferences.GetInt("int.preference") == 10);

preferences.SetInt("int.preference", 20);
RST_DCHECK(preferences.GetInt("int.preference") == 20);

// etc.
```

<a name="RandomDevice"></a>
## RandomDevice
Returns a reference to a singleton in order to have only one random device per
thread.
```cpp
#include "rst/random/random_device.h"

std::random_device& GetRandomDevice();
```

<a name="RTTI"></a>
## RTTI
LLVM-based RTTI.

```cpp
#include "rst/rtti/rtti.h"

class FileError : public rst::ErrorInfo<FileError> {
 public:
  explicit FileError(std::string&& message);
  ~FileError();

  const std::string& AsString() const override;

  // Important to have this non-const field!
  static char id_;

 private:
  const std::string message_;
};

rst::Status status = Bar();
if (status.err() &&
    rst::dyn_cast<FileError>(status.GetError()) != nullptr) {
  // File doesn't exist.
}
```

<a name="STL"></a>
## STL
<a name="Algorithm"></a>
### Algorithm
Container-based versions of algorithmic functions within the C++ standard
library.

```cpp
#include "rst/stl/algorithm.h"

template <class C>
void c_sort(C& c);

template <class C, class Compare>
void c_sort(C& c, Compare&& comp);

template <class C>
void c_stable_sort(C& c);

template <class C, class Compare>
void c_stable_sort(C& c, Compare&& comp);

template <class C, class UnaryPredicate>
auto c_find_if(C& c, UnaryPredicate&& pred);

template <class C, class Compare>
void c_push_heap(C& c, Compare&& comp);

template <class C, class Compare>
void c_pop_heap(C& c, Compare&& comp);

template <class C, class Compare>
bool c_is_sorted(const C& c, Compare&& comp);

template <class C, class URBG>
void c_shuffle(C& c, URBG&& g);
```

<a name="HashCombine"></a>
### HashCombine
Boost-like function to create a hash value from several variables. It can take
any type that `std::hash` does.

```cpp
#include "rst/stl/hash.h"

struct Point {
  Point(const int x, const int y) : x(x), y(y) {}

  int x = 0;
  int y = 0;
};

bool operator==(const Point lhs, const Point rhs) {
  return (lhs.x == rhs.x) && (lhs.y == rhs.y);
}

namespace std {

template <>
struct hash<Point> {
  size_t operator()(const Point point) const {
    return rst::HashCombine({point.x, point.y});
  }
};

}  // namespace std
```

<a name="Reversed"></a>
### Reversed
Returns a Chromium-like container adapter usable in a range-based for
statement for iterating a reversible container in reverse order.

```cpp
#include "rst/stl/reversed.h"

std::vector<int> v = ...;
for (auto i : rst::Reversed(v)) {
  // Iterates through v from back to front.
}
```

<a name="StringResizeUninitialized"></a>
### StringResizeUninitialized
Like `std::string::resize()`, except any new characters added to a string as a
result of resizing may be left uninitialized, rather than being filled with '\0'
bytes. Typically used when code is then going to overwrite the backing store of
the `std::string` with known data.
```cpp
#include "rst/stl/resize_uninitialized.h"

std::string s;
rst::StringResizeUninitialized(&s, 10);
void* ptr = ...
std::memcpy(s.data(), ptr, 10);
```

<a name="TakeFunction"></a>
### TakeFunction
Like `std::move()` for `std::function` except that it also assigns `nullptr` to
a moved argument.
```cpp
#include "rst/stl/function.h"

std::function<void()> f = ...;
auto moved_f = std::move(f);  // f is in a valid but unspecified state
                              // after the call.
std::function<void()> f = ...;
auto moved_f = rst::TakeFunction(std::move(f));  // f is nullptr.
```

<a name="VectorBuilder"></a>
### VectorBuilder
Allows in-place initialization of a `std::vector` of move-only objects.

```cpp
#include "rst/stl/vector_builder.h"

const std::vector<std::unique_ptr<int>> vec =
    rst::VectorBuilder<std::unique_ptr<int>>()
        .emplace_back(std::make_unique<int>(1))
        .emplace_back(std::make_unique<int>(-1))
        .Build();
```

<a name="Status"></a>
## Status
<a name="StatusMacros"></a>
### Status Macros
<a name="RST_TRY"></a>
#### RST_TRY
Macro to allow exception-like handling of `rst::Status` return values.

If the evaluation of statement results in an error, returns that error from the
current function.

```cpp
#include "rst/status/status_macros.h"

rst::Status Foo();

rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  RST_TRY(Foo());
  ...
}
```

<a name="RST_TRY_ASSIGN"></a>
#### RST_TRY_ASSIGN
Macro to allow exception-like handling of `rst::StatusOr` return values.

Assigns the result of evaluation of statement to lvalue and if it results in an
error, returns that error from the current function.

lvalue should be an existing non-const variable accessible in the current
scope.

`RST_TRY_ASSIGN()` expands into multiple statements; it cannot be used in a
single statement (e.g. as the body of an if statement without {})!

```cpp
#include "rst/status/status_macros.h"

rst::StatusOr<MyType> Foo();

rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  rst::StatusOr<MyType> existing_var = ...;
  RST_TRY_ASSIGN(existing_var, Foo());
  ...
}
```

<a name="RST_TRY_ASSIGN_UNWRAP"></a>
#### RST_TRY_ASSIGN_UNWRAP
Macro to allow exception-like handling of `rst::StatusOr` return values.

Assigns the unwrapped result of evaluation of statement to lvalue and if it
results in an error, returns that error from the current function.

lvalue should be an existing non-const variable accessible in the current
scope.

```cpp
#include "rst/status/status_macros.h"

rst::StatusOr<MyType> Foo();

rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  MyType existing_var = ...;
  RST_TRY_ASSIGN_UNWRAP(existing_var, Foo());
  ...
}
```

<a name="RST_TRY_CREATE"></a>
#### RST_TRY_CREATE
Macro to allow exception-like handling of `rst::StatusOr` return values.

Creates lvalue and assigns the result of evaluation of statement to it and
if it results in an error, returns that error from the current function.

lvalue should be a new variable.

`RST_TRY_CREATE()` expands into multiple statements; it cannot be used in a
single statement (e.g. as the body of an if statement without {})!

```cpp
#include "rst/status/status_macros.h"

rst::StatusOr<MyType> Foo();

rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  RST_TRY_CREATE(rst::StatusOr<MyType>, var1, Foo());
  RST_TRY_CREATE(auto, var2, Foo());
  ...
}
```

<a name="StatusOr"></a>
### StatusOr
A Google-like `StatusOr` class for recoverable error handling. It's impossible
to ignore an error, since error checking is mandatory. `rst::StatusOr`
destructor examines if the checked flag is set.
All instances must be checked before destruction, even if they're
moved-assigned or constructed from success values that have already been
checked. This enforces checking through all levels of the call stack.

```cpp
#include "rst/status/status_or.h"

rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  rst::StatusOr<std::string> foo = Foo();
  if (foo.err())
    return std::move(foo).TakeStatus();
  ...
}

// Or:
rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  RST_TRY_CREATE(auto, foo, Foo());
  RST_TRY_CREATE(rst::StatusOr<std::string>, foo, Foo());
  RST_TRY_ASSIGN(foo, Foo());
  std::cout << *foo << ", " << foo->size() << std::endl;
  ...
}

// Check specific error:
rst::StatusOr<std::string> status = Foo();
if (status.err() &&
    rst::dyn_cast<FileOpenError>(status.GetError()) != nullptr) {
  // File doesn't exist.
}

rst::Status Bad() {  // Or can be rst::StatusOr<T> Bar().
  Foo();  // Doesn't compile, since the class is marked as [[nodiscard]].
  {
    rst::StatusOr<std::string> foo = Foo();
    // DCHECK's, since error handling is ignored.
  }
  ...
}
```

<a name="Status2"></a>
### Status
A Google-like `Status` class for recoverable error handling. It's impossible to
ignore an error, since error checking is mandatory. `rst::Status` destructor
examines if the checked flag is set.
All instances must be checked before destruction, even if they're
moved-assigned or constructed from success values that have already been
checked. This enforces checking through all levels of the call stack.

```cpp
#include "rst/status/status.h"

rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  rst::Status status = Foo();
  if (status.err())
    return status;
  ...
}

// Or:
rst::Status Bar() {  // Or can be rst::StatusOr<T> Bar().
  RST_TRY(Foo());
  ...
}

// Check specific error:
rst::Status status = Foo();
if (status.err() &&
    rst::dyn_cast<FileOpenError>(status.GetError()) != nullptr) {
  // File doesn't exist.
}

rst::Status Bad() {  // Or can be rst::StatusOr<T> Bar().
  Foo();  // Doesn't compile, since the class is marked as [[nodiscard]].
  {
    rst::Status status = Foo();
    // DCHECK's, since error handling is ignored.
  }
  ...
}
```

<a name="Strings"></a>
## Strings
<a name="Format"></a>
### Format
This function is for efficiently performing string formatting.

Unlike `printf`-style format specifiers, `rst::Format()` functions do not need
to specify the type of the arguments. Supported arguments following the format
string, such as `string`s, `string_view`s, `int`s, `float`s, and `bool`s, are
automatically converted to `string`s during the formatting process. See below
for a full list of supported types.

`rst::Format()` does not allow you to specify how to format a value, beyond the
default conversion to string. For example, you cannot format an integer in hex.

The format string uses identifiers indicated by a {} like in Python.

A '{{' or '}}' sequence in the format string causes a literal '{' or '}' to
be output.

```cpp
#include "rst/strings/format.h"

std::string s = rst::Format("{} purchased {} {}", {"Bob", 5, "Apples"});
RST_DCHECK(s == "Bob purchased 5 Apples");
```

Supported types:
  * `std::string_view`, `std::string`, `const char*`
  * `short`, `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`,
    `long long`, `unsigned long long`
  * `float`, `double`, `long double` (printed as if %g is specified for
    `printf()`)
  * `bool` (printed as "true" or "false")
  * `char`
  * `enum`s (printed as underlying integer type)

If an invalid format string is provided, `rst::Format()` asserts in a debug
build.

<a name="StrCat"></a>
### StrCat
This function is for efficiently performing merging an arbitrary number of
strings or numbers into one string, and is designed to be the fastest
possible way to construct a string out of a mix of raw C strings,
`string_view` elements, `std::string` value, boolean and numeric values.
`rst::StrCat()` is generally more efficient on string concatenations involving
more than one unary operator, such as a + b + c or a += b + c, since it avoids
the creation of temporary string objects during string construction.

Supported arguments such as `string`s, `string_view`s, `int`s, `float`s, and
`bool`s, are automatically converted to `string`s during the concatenation
process. See below for a full list of supported types.

```cpp
#include "rst/strings/str_cat.h"

std::string s = rst::StrCat({"Bob", " purchased ", 5, " ", "Apples"});
RST_DCHECK(s == "Bob purchased 5 Apples");
```

Supported types:
  * `std::string_view`, `std::string`, `const char*`
  * `short`, `unsigned short`, `int`, `unsigned int`, `long`, `unsigned long`,
    `long long`, `unsigned long long`
  * `float`, `double`, `long double` (printed as if %g is specified for
    `printf()`)
  * `bool` (printed as "true" or "false")
  * `char`
  * `enum`s (printed as underlying integer type)

<a name="TaskRunner"></a>
## TaskRunner
<a name="PollingTaskRunner"></a>
### PollingTaskRunner
Task runner that is supposed to run tasks on the same thread.

```cpp
#include "rst/task_runner/polling_task_runner.h"

std::function<std::chrono::nanoseconds()> time_function = ...;
rst::PollingTaskRunner task_runner(std::move(time_function));
for (;; task_runner.RunPendingTasks()) {
  // ...
  std::function<void()> task = ...;
  task_runner.PostTask(std::move(task));
  task = ...;
  task_runner.PostDelayedTask(std::move(task), std::chrono::seconds(1));
  // ...
}
```

<a name="ThreadPoolTaskRunner"></a>
### ThreadPoolTaskRunner
Task runner that is supposed to run tasks on dedicated threads that have their
keep alive time. After that time of inactivity the threads stop.

```cpp
#include "rst/task_runner/thread_pool_task_runner.h"

std::function<std::chrono::nanoseconds()> time_function = ...;
size_t max_threads_num = ...;
std::chrono::nanoseconds keep_alive_time = ...;
rst::ThreadPoolTaskRunner task_runner(max_threads_num,
                                      std::move(time_function),
                                      keep_alive_time);
...
std::function<void()> task = ...;
task_runner.PostTask(std::move(task));
task = ...;
task_runner.PostDelayedTask(std::move(task), std::chrono::seconds(1));
...

// Posts a single |task| and waits for all |iterations| to complete before
// returning. The current index of iteration is passed to each invocation.
std::function<void(size_t)> task = ...;
constexpr size_t iterations = 100;
task_runner.ApplyTaskSync(std::move(task), iterations);
```

<a name="Threading"></a>
## Threading
<a name="Barrier"></a>
### Barrier
Provides a thread-coordination mechanism that allows a set of participating
threads to block until an operation is completed. The value of the counter
is initialized on creation. Threads block until the counter is decremented
to zero.

```cpp
#include "rst/threading/barrier.h"

rst::Barrier barrier(5);

std::vector<std::thread> threads;
for (auto i = 0; i < 5; i++)
  threads.emplace_back([&barrier]() { barrier.CountDown(); });

barrier.Wait();
// Synchronization point.
```

<a name="Timer"></a>
## Timer
<a name="OneShotTimer"></a>
### OneShotTimer
`rst::OneShotTimer` provides a simple timer API. As the name suggests,
`rst::OneShotTimer` calls back once after a time delay expires.

`rst::OneShotTimer` cancels the timer when it goes out of scope, which makes it
easy to ensure that you do not get called when your object has gone out of
scope. Just instantiate a timer as a member variable of the class for which
you wish to receive timer events.

```cpp
#include "rst/timer/one_shot_timer.h"

class MyClass {
 public:
  void DelayDoingStuff() {
    timer_.Start(std::bind(&MyClass::DoStuff, this),
                 std::chrono::seconds(1));
  }

 private:
  void DoStuff() {
    // This method is called after 1 second.
  }

  rst::OneShotTimer timer_{&GetTaskRunner};
};
```

<a name="Type"></a>
## Type
A Chromium-like `StrongAlias` type.

A type-safe alternative for a typedef or a using directive like in Golang.

The motivation is to disallow several classes of errors:

```cpp
using Orange = int;
using Apple = int;
Apple apple(2);
Orange orange = apple;  // Orange should not be able to become an Apple.
Orange x = orange + apple;  // Shouldn't add Oranges and Apples.
if (orange > apple);  // Shouldn't compare Apples to Oranges.
void foo(Orange);
void foo(Apple);  // Redefinition.
```

Type may instead be used as follows:

```cpp
#include "rst/type/type.h"

using Orange = rst::Type<class OrangeTag, int>;
using Apple = rst::Type<class AppleTag, int>;
Apple apple(2);
Orange orange = apple;  // Does not compile.
Orange other_orange = orange;  // Compiles, types match.
Orange x = orange + apple;  // Does not compile.
Orange y = Orange(orange.value() + apple.value());  // Compiles.
if (orange > apple);  // Does not compile.
if (orange > other_orange);  // Compiles.
void foo(Orange);
void foo(Apple);  // Compiles into separate overload.
```

TagType is an empty tag class (also called "phantom type") that only serves
the type system to differentiate between different instantiations of the
template.

<a name="Value"></a>
## Value
A Chromium-like JSON `Value` class.

This is a recursive data storage class intended for storing settings and
other persistable data.

A `rst::Value` represents something that can be stored in JSON or passed to/from
JavaScript. As such, it is not a generalized variant type, since only the
types supported by JavaScript/JSON are supported.

In particular this means that there is no support for `int64_t` or unsigned
numbers. Writing JSON with such types would violate the spec. If you need
something like this, either use a `double` or make a `string` value containing
the number you want.

Construction:

`rst::Value` is directly constructible from `bool`, `int32_t`, `int64_t`
(checking that the modulo of the value is <= 2^53 - 1, `double` (excluding NaN
and inf), `const char*`, `std::string`, `rst::Value::Array`, and
`rst::Value::Object`.

Copying:

`rst::Value` does not support C++ copy semantics to make it harder to
accidentally copy large values. Instead, use `Clone()` to manually create a deep
copy.

Reading:

`GetBool()`, `GetInt()`, etc. assert that the `rst::Value` has the correct
`type()` before returning the contained value. `bool`, `int`, `double` are
returned by value. `std::string`, `rst::Value::Object`, `rst::Value::Array` are
returned by reference.

Assignment:

It is not possible to directly assign `bool`, `int`, etc. to a `rst::Value`.
Instead, wrap the underlying type in `rst::Value` before assigning.

Objects support:

`FindKey()`: Finds a value by `std::string_view` key, returning `nullptr` if the
key is not present.
`FindKeyOfType()`: Finds a value by `std::string_view` key, returning `nullptr`
if the key is not present or the `type()` of the value is not what is needed.
`FindBoolKey()`, `FindIntKey()`, ...: Similar to `FindKey()`, but ensures that
the `rst::Value` also has the correct `type()`. `bool`, `int`, `double` are
returned by `std::optional<>`. `std::string`, `rst::Value::Object`,
`rst::Value::Array` are returned by `rst::Nullable` pointer.
`SetKey()`: Associates a `rst::Value` with a `std::string` key.
`RemoveKey()`: Removes the key from this object, if present.

Objects also support an additional set of helper methods that operate on
"paths": `FindPath()`, and `SetPath()`.
Dotted paths are a convenience method of naming intermediate nested objects,
separating the components of the path using '.' characters. For example, finding
a path on a `rst::Value::Object` using the dotted path:

  "aaa.bbb.ccc"

will first look for a `rst::Value::Object` associated with the key "aaa", then
another `rst::Value::Object` under the "aaa" object associated with the
key "bbb", and then a `rst::Value` under the "bbb" object associated
with the key "ccc".

If a path only has one component (i.e. has no dots), please use the regular,
non-path APIs.
