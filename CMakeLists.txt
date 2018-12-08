cmake_minimum_required (VERSION 3.7)
project (cppsim)

set(CMAKE_CXX_STANDARD 11)

add_subdirectory (src)
add_subdirectory (test)

enable_testing ()

add_test (NAME MyTest COMMAND Test)