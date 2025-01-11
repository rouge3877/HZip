# CMakeLists.txt
cmake_minimum_required(VERSION 3.10)
project(HuffZip)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 包含头文件目录
include_directories(include)

# 源文件
set(SOURCES
    src/main.cpp
    src/BitIO.cpp
    src/HuffmanTree.cpp
    src/HuffmanEncoder.cpp
    src/HuffmanDecoder.cpp
    src/Utils.cpp
)

add_executable(huff ${SOURCES})
