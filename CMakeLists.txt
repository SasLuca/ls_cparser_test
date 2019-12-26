cmake_minimum_required(VERSION 3.15)
project(ls_cparser CXX)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 17)
set(CXX_EXTENSIONS OFF)

add_subdirectory(external/Catch2)

include_directories(source)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS source/*)
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS tests/*)

add_executable(test_tokenizer tests/catch2_main.cpp tests/tokenizer_tests.cpp source/ls_cparser.h)
target_link_libraries(test_tokenizer PRIVATE Catch2)