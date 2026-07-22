cmake_minimum_required(VERSION 3.20)
project(Chess98 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR "Use: cmake -B .build/cmake/<preset>")
endif()

set(CHESS98_SOURCES
    src/base.cpp
    src/evaluate.cpp
    src/heuristic.cpp
    src/main.cpp
    src/moves.cpp
    src/position.cpp
    src/search.cpp
    src/test.cpp
    src/ucci.cpp
)

add_executable(Chess98 ${CHESS98_SOURCES})
target_include_directories(Chess98 PRIVATE ${CMAKE_SOURCE_DIR}/src)

if(MSVC)
    target_compile_options(Chess98 PRIVATE /W4 /permissive- /Zc:__cplusplus)
else()
    target_compile_options(Chess98 PRIVATE -Wall -Wextra -Wpedantic)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "")
        target_compile_options(Chess98 PRIVATE -g -O0)
    elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
        target_compile_options(Chess98 PRIVATE -O3 -DNDEBUG)
    elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        target_compile_options(Chess98 PRIVATE -O2 -g -DNDEBUG)
    endif()
endif()

set_target_properties(Chess98 PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}"
)

enable_testing()
add_test(NAME Chess98 COMMAND Chess98)
