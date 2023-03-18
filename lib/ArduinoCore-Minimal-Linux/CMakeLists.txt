cmake_minimum_required(VERSION 3.14)

add_library(arduino-core-linux STATIC
    "${CMAKE_CURRENT_LIST_DIR}/src/Arduino.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/src/Wire.cpp"
)

target_include_directories(arduino-core-linux
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
)

target_link_libraries(arduino-core-linux
    PUBLIC
        arduino-core
)

target_compile_features(arduino-core-linux PUBLIC cxx_std_17)
