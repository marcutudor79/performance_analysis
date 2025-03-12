cmake_minimum_required(VERSION 3.30)
project(invertBits)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-fopenmp")

find_package(threads)
add_executable(invertBits main.cpp)
target_link_libraries(invertBits ${CMAKE_THREAD_LIBS_INIT} gomp)
