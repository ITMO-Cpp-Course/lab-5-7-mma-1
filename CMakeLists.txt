cmake_minimum_required(VERSION 3.20)
project(Lab4 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(project_warnings INTERFACE)
if (MSVC)
    target_compile_options(project_warnings INTERFACE /W4 /permissive-)
else ()
    target_compile_options(project_warnings INTERFACE -Wall -Wextra -Wpedantic -Werror)
endif ()

if (NOT WIN32)
    add_library(project_sanitizers INTERFACE)
    target_compile_options(project_sanitizers INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>)
    target_link_options(project_sanitizers INTERFACE $<$<CONFIG:Debug>:-fsanitize=address>)
endif ()

include(CTest)
enable_testing()

add_subdirectory(src)
add_subdirectory(tests)