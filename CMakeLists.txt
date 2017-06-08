cmake_minimum_required(VERSION 2.6)

include_directories(.)

add_library(rst

rst/Check/Check.h

rst/Cpp14/Memory.h

rst/Defer/Defer.h

rst/Format/Format.cpp
rst/Format/Format.h
rst/Format/FormatError.cpp
rst/Format/FormatError.h
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

rst/Result/Result.h

rst/Status/Status.cpp
rst/Status/Status.h

)

add_executable(rst_tests

rst/Check/Test.cpp
rst/Check/NDebugTest.cpp

rst/Test.cpp

rst/Cpp14/Test.cpp

rst/Defer/Test.cpp

rst/Format/Test.cpp

rst/HiddenString/Test.cpp

rst/Logger/Test.cpp

rst/Noncopyable/Test.cpp

rst/Optional/Test.cpp

rst/Result/Test.cpp

rst/Status/Test.cpp
)

TARGET_LINK_LIBRARIES(rst_tests rst pthread gtest gmock)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++11 -g -fsanitize=address -pthread")
