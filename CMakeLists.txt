cmake_minimum_required(VERSION 3.16.0)
project(shinobu)

file(GLOB_RECURSE SHINOBU_SOURCES src/*.cpp)

include_directories(include)
add_executable(shinobu ${SHINOBU_SOURCES})

set_property(TARGET shinobu PROPERTY CXX_STANDARD 17)
target_compile_options(shinobu PRIVATE -Werror -Wall -Wextra)
