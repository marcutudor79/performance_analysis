cmake_minimum_required(VERSION 3.30)
project(keypoint_matching)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-mavx -mavx2")

add_executable(keypoint_matching main.cpp)
