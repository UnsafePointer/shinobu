cmake_minimum_required(VERSION 3.16.0)
project(shinobu)

file(GLOB_RECURSE SHINOBU_SOURCES src/*.cpp)

include_directories(include)

add_subdirectory(imgui)
add_subdirectory(mini-yaml)
add_subdirectory(Gb_Snd_Emu)

add_executable(shinobu ${SHINOBU_SOURCES})
target_link_libraries(shinobu imgui)
target_link_libraries(shinobu yaml)
target_link_libraries(shinobu gb_snd_emu)

set_property(TARGET shinobu PROPERTY CXX_STANDARD 17)
target_compile_options(shinobu PRIVATE -Werror -Wall -Wextra)
