cmake_minimum_required(VERSION 2.6)

include_directories(.)

add_library(rst

rst/bind/bind.h
rst/bind/bind_helpers.h

rst/check/check.h

rst/defer/defer.h

rst/files/file_utils.cc
rst/files/file_utils.h

rst/format/format.cc
rst/format/format.h

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

rst/no_destructor/no_destructor.h

rst/not_null/not_null.h

rst/preferences/memory_preferences_store.cc
rst/preferences/memory_preferences_store.h
rst/preferences/preferences.cc
rst/preferences/preferences.h
rst/preferences/preferences_store.cc
rst/preferences/preferences_store.h

rst/memory/memory.h
rst/memory/weak_ptr.h

rst/random/random_device.cc
rst/random/random_device.h

rst/rtti/rtti.h

rst/status/status.cc
rst/status/status.h
rst/status/status_or.h

rst/stl/adapters.h
rst/stl/algorithm.h

rst/threading/barrier.h
rst/threading/barrier.cc

rst/type/type.h

rst/task_runner/task_runner.cc
rst/task_runner/task_runner.h
rst/task_runner/item.cc
rst/task_runner/item.h
rst/task_runner/polling_task_runner.cc
rst/task_runner/polling_task_runner.h
rst/task_runner/thread_task_runner.cc
rst/task_runner/thread_task_runner.h

rst/value/value.h
rst/value/value.cc
)

add_executable(rst_tests

rst/bind/bind_test.cc
rst/bind/bind_helpers_test.cc

rst/check/check_test.cc
rst/check/check_ndebug_test.cc

rst/test.cc

rst/defer/defer_test.cc

rst/files/file_utils_test.cc

rst/format/format_test.cc

rst/guid/guid_test.cc

rst/hidden_string/hidden_string_test.cc

rst/legacy/memory_test.cc
rst/legacy/optional_test.cc

rst/logger/logger_ndebug_test.cc
rst/logger/logger_test.cc

rst/macros/macros_test.cc

rst/no_destructor/no_destructor_test.cc

rst/not_null/not_null_test.cc

rst/preferences/preferences_test.cc

rst/memory/memory_test.cc
rst/memory/weak_ptr_test.cc

rst/rtti/rtti_test.cc

rst/status/status_or_test.cc
rst/status/status_test.cc

rst/stl/adapters_test.cc
rst/stl/algorithm_test.cc

rst/task_runner/polling_task_runner_test.cc
rst/task_runner/thread_task_runner_test.cc

rst/threading/barrier_test.cc

rst/type/type_test.cc

rst/value/value_test.cc
)

TARGET_LINK_LIBRARIES(rst_tests rst pthread gtest gmock)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -pedantic -std=c++17 -O2 -g -fsanitize=address -pthread -fno-exceptions -fno-rtti -Wexit-time-destructors")
