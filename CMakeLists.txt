cmake_minimum_required(VERSION 3.15.3)
project(FooBug)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 11)

include_directories(include)
file(GLOB SOURCES "src/*.cpp" "include/*.hpp")
add_library(FooBug STATIC ${SOURCES})

# Tests
#
configure_file(DownloadGoogleTest.txt.in googletest-download/CMakeLists.txt)
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
# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

file(GLOB TESTS "test/*.cpp")
include(GoogleTest)
add_executable(tests ${TESTS})

# Enable ASAN
target_compile_options(FooBug PRIVATE -fsanitize=address)

# If you comment out the line below then the buffer overflow will not occur
target_compile_options(tests PRIVATE -fsanitize=address)
target_link_options(tests PRIVATE -fsanitize=address)

target_link_libraries(tests
  gtest_main
  FooBug
)

target_include_directories(tests PUBLIC build/googletest-src/googletest/include)
target_include_directories(tests PUBLIC test/include)
enable_testing()
gtest_discover_tests(tests)
