cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_COMPILER g++-10)
project(matrix)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_definitions(-DDEBUG)

add_executable(
  matrix
  main.cpp
)

add_executable(
  test_matrix
  tests/test_matrix.cpp
)
add_executable(
  test_shell_matrix
  tests/test_shell_matrix.cpp
)
add_executable(
  test_vector
  tests/test_vector.cpp
)

target_include_directories(matrix PUBLIC include)
