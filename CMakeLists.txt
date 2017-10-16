cmake_minimum_required(VERSION 2.6)

include_directories(.)

add_library(rst

rst/BuildFlag/BuildFlag.h

rst/Check/Check.h

rst/Cpp14/Memory.h

rst/Defer/Defer.h

rst/Format/Format.cpp
rst/Format/Format.h
rst/Format/Writer.cpp
rst/Format/Writer.h

rst/HiddenString/HiddenString.h

rst/Logger/FileNameSink.cpp
rst/Logger/FileNameSink.h
rst/Logger/FilePtrSink.cpp
rst/Logger/FilePtrSink.h
rst/Logger/ISink.h
rst/Logger/LogError.cpp
rst/Logger/LogError.h
rst/Logger/Logger.cpp
rst/Logger/Logger.h

rst/Noncopyable/Noncopyable.h

rst/Optional/Optional.h

rst/Status/Status.cpp
rst/Status/Status.h
rst/Status/StatusOr.h

)

add_executable(rst_tests

rst/BuildFlag/BuildFlagTest.cpp

rst/Check/CheckTest.cpp
rst/Check/CheckNDebugTest.cpp

rst/Test.cpp

rst/Cpp14/MemoryTest.cpp

rst/Defer/DeferTest.cpp

rst/Format/FormatTest.cpp

rst/HiddenString/HiddenStringTest.cpp

rst/Logger/LoggerNDebugTest.cpp
rst/Logger/LoggerTest.cpp

rst/Noncopyable/NoncopyableTest.cpp

rst/Optional/OptionalTest.cpp

rst/Status/StatusOrTest.cpp
rst/Status/StatusTest.cpp
)

TARGET_LINK_LIBRARIES(rst_tests rst pthread gtest gmock)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++14 -g -fsanitize=address -pthread -fno-exceptions -fno-rtti")
