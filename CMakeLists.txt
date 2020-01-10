cmake_minimum_required(VERSION 3.5)

project(rst CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(rst
  rst/bind/bind.h
  rst/bind/bind_helpers.h
  
  rst/check/check.h
  
  rst/defer/defer.h
  
  rst/files/file_utils.cc
  rst/files/file_utils.h
  
  rst/guid/guid.cc
  rst/guid/guid.h
  
  rst/hidden_string/hidden_string.h
  
  rst/legacy/memory.h
  rst/legacy/optional.h
  
  rst/logger/file_name_sink.cc
  rst/logger/file_name_sink.h
  rst/logger/file_ptr_sink.cc
  rst/logger/file_ptr_sink.h
  rst/logger/log_error.cc
  rst/logger/log_error.h
  rst/logger/logger.cc
  rst/logger/logger.h
  rst/logger/sink.cc
  rst/logger/sink.h
  
  rst/macros/macros.h
  rst/macros/optimization.h
  rst/macros/os.h
  
  rst/memory/memory.h
  rst/memory/weak_ptr.h
  
  rst/no_destructor/no_destructor.h
  
  rst/not_null/not_null.h
  
  rst/preferences/memory_preferences_store.cc
  rst/preferences/memory_preferences_store.h
  rst/preferences/preferences.cc
  rst/preferences/preferences.h
  rst/preferences/preferences_store.cc
  rst/preferences/preferences_store.h
  
  rst/random/random_device.cc
  rst/random/random_device.h
  
  rst/rtti/rtti.h
  
  rst/status/status.cc
  rst/status/status.h
  rst/status/status_macros.h
  rst/status/status_or.h
  
  rst/stl/adapters.h
  rst/stl/algorithm.h
  rst/stl/resize_uninitialized.h
  
  rst/strings/arg.cc
  rst/strings/arg.h
  rst/strings/format.cc
  rst/strings/format.h
  rst/strings/str_cat.cc
  rst/strings/str_cat.h
  
  rst/threading/barrier.h
  rst/threading/barrier.cc
  
  rst/type/type.h
  
  rst/task_runner/task_runner.cc
  rst/task_runner/task_runner.h
  rst/task_runner/item.h
  rst/task_runner/polling_task_runner.cc
  rst/task_runner/polling_task_runner.h
  rst/task_runner/thread_task_runner.cc
  rst/task_runner/thread_task_runner.h
  
  rst/value/value.h
  rst/value/value.cc
)

target_include_directories(rst PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(rst_tests
  rst/bind/bind_test.cc
  rst/bind/bind_helpers_test.cc
  
  rst/check/check_test.cc
  rst/check/check_ndebug_test.cc
  
  rst/defer/defer_test.cc
  
  rst/files/file_utils_test.cc
  
  rst/guid/guid_test.cc
  
  rst/hidden_string/hidden_string_test.cc
  
  rst/legacy/memory_test.cc
  rst/legacy/optional_test.cc
  
  rst/logger/logger_ndebug_test.cc
  rst/logger/logger_test.cc
  
  rst/macros/macros_test.cc
  
  rst/memory/memory_test.cc
  rst/memory/weak_ptr_test.cc
  
  rst/no_destructor/no_destructor_test.cc
  
  rst/not_null/not_null_test.cc
  
  rst/preferences/preferences_test.cc
  
  rst/random/random_device_test.cc
  
  rst/rtti/rtti_test.cc
  
  rst/status/status_macros_test.cc
  rst/status/status_or_test.cc
  rst/status/status_test.cc
  
  rst/stl/adapters_test.cc
  rst/stl/algorithm_test.cc
  rst/stl/resize_uninitialized_test.cc
  
  rst/strings/format_test.cc
  rst/strings/str_cat_test.cc
  
  rst/task_runner/polling_task_runner_test.cc
  rst/task_runner/thread_task_runner_test.cc
  
  rst/threading/barrier_test.cc
  
  rst/type/type_test.cc
  
  rst/value/value_test.cc
)

if (POLICY CMP0054)
  cmake_policy(SET CMP0054 NEW)
endif()

if (POLICY CMP0048)
  cmake_policy(SET CMP0048 NEW)
endif()

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

target_link_libraries(rst_tests PRIVATE rst gtest_main gmock)

option(RST_ENABLE_CXX_EXCEPTIONS "Enable C++ exception support" OFF)
option(RST_ENABLE_CXX_RTTI "Enable C++ RTTI support" OFF)

option(RST_ENABLE_ASAN "Enable Address Sanitizer" OFF)
option(RST_ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(RST_ENABLE_UBSAN "Enable Undefined Behavior Sanitizer" OFF)

set(cxx_rst_public_flags "")
set(cxx_rst_public_link_flags "")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(cxx_rst_public_flags "${cxx_rst_public_flags};-Wno-unknown-pragmas")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(cxx_rst_public_flags "${cxx_rst_public_flags};/wd4068")
endif()

if (NOT RST_ENABLE_CXX_EXCEPTIONS)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(cxx_rst_public_flags "${cxx_rst_public_flags};-fno-exceptions")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(cxx_rst_public_flags "${cxx_rst_public_flags};/D_HAS_EXCEPTIONS=0")
  endif()
endif()

if (NOT RST_ENABLE_CXX_RTTI)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(cxx_rst_public_flags "${cxx_rst_public_flags};-fno-rtti")
  elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(cxx_rst_public_flags "${cxx_rst_public_flags};/GR-")
  endif()
endif()

if (RST_ENABLE_ASAN)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(cxx_rst_public_flags "${cxx_rst_public_flags};-fsanitize=address;"
        "-fno-omit-frame-pointer")
    set(cxx_rst_public_link_flags "${cxx_rst_public_link_flags};"
        "-fsanitize=address")
  endif()
endif()

if (RST_ENABLE_TSAN)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(cxx_rst_public_flags "${cxx_rst_public_flags};-fsanitize=thread;"
        "-fno-omit-frame-pointer")
    set(cxx_rst_public_link_flags "${cxx_rst_public_link_flags};"
        "-fsanitize=thread")
  endif()
endif()

if (RST_ENABLE_UBSAN)
  if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR
      CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(cxx_rst_public_flags "${cxx_rst_public_flags};-fsanitize=undefined;"
        "-fno-omit-frame-pointer")
    set(cxx_rst_public_link_flags "${cxx_rst_public_link_flags};"
        "-fsanitize=undefined")
  endif()
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(cxx_rst_flags "${cxx_rst_flags};-Werror;-Wall;-Wextra;-pedantic;"
      "-Weverything;-Wno-c++98-compat;-Wno-c++98-c++11-c++14-compat;"
      "-Wno-c++98-compat-pedantic;-Wno-padded;-Wno-return-std-move-in-c++11;"
      "-Wno-shadow-field-in-constructor;-Wno-unknown-pragmas;"
      "-Wno-deprecated-declarations")
  set(cxx_rst_tests_flags "${cxx_rst_flags};-Wno-global-constructors;"
      "-Wno-used-but-marked-unused;-Wno-covered-switch-default;"
      "-Wno-unused-member-function")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(cxx_rst_flags "${cxx_rst_flags};-Werror;-Wall;-Wextra;-pedantic;"
      "-Wno-unknown-pragmas;-Wcast-qual;-Wconversion-null;"
      "-Wmissing-declarations;-Woverlength-strings;-Wpointer-arith;"
      "-Wunused-local-typedefs;-Wunused-result;-Wvarargs;-Wvla;-Wwrite-strings")
  set(cxx_rst_tests_flags "${cxx_rst_flags}")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(cxx_rst_flags "${cxx_rst_flags};/Wall;/WX;/wd4996;/wd4018;/wd4820;"
      "/wd4514;/wd4710;/wd4582;/wd4583;/wd5045;/wd4668;/wd4577;/wd4530;/wd5039")
  set(cxx_rst_tests_flags "${cxx_rst_flags};/wd4834;/wd4625;/wd4626;/wd5026;"
      "/wd5027;/wd4571")
endif()

target_compile_options(rst PRIVATE ${cxx_rst_flags})
target_compile_options(rst_tests PRIVATE ${cxx_rst_tests_flags})

target_compile_options(rst PUBLIC ${cxx_rst_public_flags})
target_link_libraries(rst PUBLIC ${cxx_rst_public_link_flags})
