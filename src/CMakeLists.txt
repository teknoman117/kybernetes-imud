cmake_minimum_required(VERSION 3.14)

add_executable(imu-server "${CMAKE_CURRENT_LIST_DIR}/imu-server.cpp")

target_link_libraries(imu-server
    PRIVATE
        sparkfun-icm-20948
)