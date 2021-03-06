cmake_minimum_required(VERSION 3.16)
project(crypto330)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -fopenmp")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--stack,100000000")

set(COMPILE_FLAGS "--coverage")
set(CMAKE_EXE_LINKER_FLAGS "--coverage")

set(
        HEADERS
        include/crypto330/block/aes.hpp
        include/crypto330/utils.hpp
        include/crypto330/block/kalyna.hpp
        include/crypto330/block/block.hpp
        include/crypto330/stream/block_stream.hpp
        include/crypto330/stream/rc4.hpp
        include/crypto330/stream/salsa.hpp
        include/crypto330/hash/hash.hpp
        include/crypto330/hash/sha256.hpp
        include/crypto330/hash/kupyna.hpp
        include/crypto330/hugeint/hugeint.hpp
        include/crypto330/hugeint/math.hpp
        include/crypto330/symmetric/rsa.hpp
        include/crypto330/symmetric/elliptic.hpp)


set(
        SOURCES
        src/aes.cpp
        src/utils.cpp
        src/kalyna.cpp
        src/block.cpp
        src/block_stream.cpp
        src/rc4.cpp
        src/salsa.cpp
        src/sha256.cpp
        src/kupyna.cpp
        src/hugeint.cpp
        src/math.cpp
        src/rsa.cpp
        src/elliptic.cpp)

add_library(crypto330 ${HEADERS} ${SOURCES})
target_include_directories(crypto330 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(runnable main.cpp)
target_link_libraries(runnable PUBLIC crypto330)

# Tests
add_subdirectory(googletest)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

add_executable(unittest test/test.cpp test/hugeint_test.cpp test/rsa_test.cpp test/elliptic_test.cpp)
target_link_libraries(unittest PUBLIC crypto330 gtest_main)