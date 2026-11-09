cmake_minimum_required(VERSION 3.20)
project(lab3_variant1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(LAB3_COVERAGE "Enable GCC/Clang coverage instrumentation" OFF)

add_library(lab3_structures
    src/container.cpp
    src/structures.cpp
    src/hash_table.cpp
    src/database.cpp
)
target_include_directories(lab3_structures PUBLIC include)

if(MSVC)
    target_compile_options(lab3_structures PRIVATE /W4 /permissive- /utf-8)
else()
    target_compile_options(lab3_structures PRIVATE -Wall -Wextra -Wpedantic)
endif()

if(LAB3_COVERAGE AND CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(lab3_structures PUBLIC --coverage -O0 -g)
    target_link_options(lab3_structures PUBLIC --coverage)
endif()

add_executable(lab3_demo src/main.cpp)
target_link_libraries(lab3_demo PRIVATE lab3_structures)

add_executable(lab3_tests tests/tests.cpp)
target_include_directories(lab3_tests PRIVATE third_party)
target_link_libraries(lab3_tests PRIVATE lab3_structures)

add_executable(lab3_benchmark benchmarks/benchmark.cpp)
target_link_libraries(lab3_benchmark PRIVATE lab3_structures)

enable_testing()
add_test(NAME lab3_tests COMMAND lab3_tests)

option(LAB3_CORE_ONLY "Build without external test frameworks for limited environments" OFF)
if(NOT LAB3_CORE_ONLY)
    find_package(Boost REQUIRED)
    find_package(GTest REQUIRED)
    add_executable(lab3_boost_tests tests/boost_tests.cpp)
    target_link_libraries(lab3_boost_tests PRIVATE lab3_structures Boost::headers)
    add_executable(lab3_gtests tests/gtest_tests.cpp)
    target_link_libraries(lab3_gtests PRIVATE lab3_structures GTest::gtest_main)
    add_test(NAME lab3_boost_tests COMMAND lab3_boost_tests)
    add_test(NAME lab3_gtests COMMAND lab3_gtests)
endif()
