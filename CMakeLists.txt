cmake_minimum_required(VERSION 2.6)

include_directories(.)

add_library(rst

rst/Check/Check.h

rst/Defer/Defer.h

rst/Format/Format.cpp
rst/Format/Format.h
rst/Format/Writer.cpp
rst/Format/Writer.h

rst/GUID/GUID.cpp
rst/GUID/GUID.h

rst/HiddenString/HiddenString.h

rst/Legacy/Memory.h
rst/Legacy/Optional.h

rst/Logger/FileNameSink.cpp
rst/Logger/FileNameSink.h
rst/Logger/FilePtrSink.cpp
rst/Logger/FilePtrSink.h
rst/Logger/ISink.cpp
rst/Logger/ISink.h
rst/Logger/LogError.cpp
rst/Logger/LogError.h
rst/Logger/Logger.cpp
rst/Logger/Logger.h

rst/Macros/Macros.h

rst/NoDestructor/NoDestructor.h

rst/NotNull/NotNull.h

rst/Preferences/IPreferencesStore.cpp
rst/Preferences/IPreferencesStore.h
rst/Preferences/Preferences.cpp
rst/Preferences/Preferences.h

rst/Memory/Memory.h
rst/Memory/WeakPtr.h

rst/RTTI/RTTI.h

rst/Status/Status.cpp
rst/Status/Status.h
rst/Status/StatusOr.h

rst/STL/Adapters.h
rst/STL/Algorithm.h

rst/Value/Value.h
rst/Value/Value.cpp
)

add_executable(rst_tests

rst/Check/CheckTest.cpp
rst/Check/CheckNDebugTest.cpp

rst/Test.cpp

rst/Defer/DeferTest.cpp

rst/Format/FormatTest.cpp

rst/GUID/GUIDTest.cpp

rst/HiddenString/HiddenStringTest.cpp

rst/Legacy/MemoryTest.cpp
rst/Legacy/OptionalTest.cpp

rst/Logger/LoggerNDebugTest.cpp
rst/Logger/LoggerTest.cpp

rst/Macros/MacrosTest.cpp

rst/NoDestructor/NoDestructorTest.cpp

rst/NotNull/NotNullTest.cpp

rst/Preferences/PreferencesTest.cpp

rst/Memory/MemoryTest.cpp
rst/Memory/WeakPtrTest.cpp

rst/RTTI/RTTITest.cpp

rst/Status/StatusOrTest.cpp
rst/Status/StatusTest.cpp

rst/STL/AdaptersTest.cpp
rst/STL/AlgorithmTest.cpp

rst/Value/ValueTest.cpp
)

TARGET_LINK_LIBRARIES(rst_tests rst pthread gtest gmock)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++17 -O2 -g -fsanitize=address -pthread -fno-exceptions -fno-rtti -Wexit-time-destructors")
