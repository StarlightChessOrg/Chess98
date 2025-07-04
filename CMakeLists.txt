cmake_minimum_required(VERSION 3.31)
project(Chess98)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS -O3)

include_directories(Chess98)

add_executable(Chess98
        Chess98/base.hpp
        Chess98/bitboard.hpp
        Chess98/board.hpp
        Chess98/book.hpp
        Chess98/evaluate.hpp
        Chess98/hash.hpp
        Chess98/heuristic.hpp
        Chess98/main.cpp
        Chess98/moves.hpp
        Chess98/search.hpp
        Chess98/test.hpp
        Chess98/ui.hpp
        Chess98/utils.hpp)
