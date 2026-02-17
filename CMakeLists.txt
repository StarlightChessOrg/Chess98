# 基本信息
cmake_minimum_required(VERSION 3.14)
project(Chess98 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 严格模式
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
elseif (MSVC)
    add_compile_options(/W4 /WX)
endif()

# 文件
file(GLOB SOURCE_CPP_FILES "src/*.cpp")
file(GLOB SOURCE_HPP_FILES "src/*.hpp")
add_executable(Chess98 ${SOURCE_CPP_FILES} ${SOURCE_HPP_FILES})

# 测试
enable_testing()
add_test(NAME Chess98 COMMAND Chess98)
