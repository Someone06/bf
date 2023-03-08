cmake_minimum_required(VERSION 3.23)
project(bf)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 23)

add_executable(bf
        src/main.cpp
        src/TokenType.cpp
        src/Token.cpp
        )

