cmake_minimum_required(VERSION 3.16)
project(crypto330)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,100000000")

set(
        HEADERS
        include/crypto330/aes.hpp
        include/crypto330/utils.hpp
        include/crypto330/kalyna.hpp include/crypto330/base.hpp)

set(
        SOURCES
        src/aes.cpp
        src/utils.cpp
        src/kalyna.cpp src/base.cpp)

add_library(crypto330 ${HEADERS} ${SOURCES})
target_include_directories(crypto330 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(runnable main.cpp)
target_link_libraries(runnable PUBLIC crypto330)

# Tests
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(unittest test/test.cpp)
target_link_libraries(unittest PUBLIC crypto330 gtest_main)