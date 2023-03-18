cmake_minimum_required(VERSION 3.14)
project(imu-server VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(lib)
add_subdirectory(src)
