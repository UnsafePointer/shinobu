cmake_minimum_required(VERSION 3.10.2)
project(shinobu)

option(SENTRY "Compile with GDB support")

file(GLOB_RECURSE SHINOBU_SOURCES src/*.cpp)

include_directories(include)

add_subdirectory(third_party/imgui)
add_subdirectory(third_party/mini-yaml)
add_subdirectory(third_party/Gb_Snd_Emu)

add_executable(shinobu ${SHINOBU_SOURCES})
target_link_libraries(shinobu imgui)
target_link_libraries(shinobu yaml)
target_link_libraries(shinobu gb_snd_emu)

if(SENTRY)
    add_definitions(-DSENTRY)
    add_subdirectory(third_party/sentry-native)
    target_link_libraries(shinobu sentry)
    if(MINGW)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_link_options(shinobu PRIVATE -Wl,-pdb=)
            target_compile_options(shinobu PRIVATE -gcodeview)
        else()
            message(FATAL_ERROR "Only Clang is supported")
        endif()
    endif()
else()
    target_compile_options(shinobu PRIVATE -Werror -Wall -Wextra)
endif(SENTRY)

set_property(TARGET shinobu PROPERTY CXX_STANDARD 17)
