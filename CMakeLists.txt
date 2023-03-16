cmake_minimum_required(VERSION 3.23)
project(bf)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 20)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using LLVM Include: ${LLVM_INCLUDE_DIRS}")
# Add LLVM Libraries to the path
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
message(STATUS ${LLVM_LIBRARY_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})


add_executable(bf
        src/main.cpp
        src/TokenType.cpp
        src/Token.cpp
        src/AstVisitors.cpp src/NullOstream.cpp src/LLVM.cpp)

target_link_libraries(bf LLVM)