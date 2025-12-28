# 基本信息
cmake_minimum_required(VERSION 3.14)
project(Chess98 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Relesse构建
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_compile_options(-O3)
endif()

# 文件和库
file(GLOB SOURCE_CPP_FILES "Chess98/*.cpp")
file(GLOB SOURCE_HPP_FILES "Chess98/*.hpp")
add_executable(Chess98 ${SOURCE_CPP_FILES} ${SOURCE_HPP_FILES})
target_include_directories(Chess98 PRIVATE Chess98)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
