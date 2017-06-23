cmake_minimum_required(VERSION 3.8.1)
project(directory_test C CXX)

include_directories(include)
include_directories(include/asio-1.10.6/include)

set(SOURCES src/Main.cpp src/HTTPEquation.cpp src/json11.cpp)


add_definitions(-DASIO_STANDALONE -DASIO_STANDALONE -DASIO_HAS_STD_SHARED_PTR -DASIO_HAS_STD_ADDRESSOF -DASIO_HAS_STD_TYPE_TRAITS -DASIO_HAS_STD_ARRAY -DASIO_HAS_CSTDINT)
add_executable(testFormaPol ${SOURCES})

