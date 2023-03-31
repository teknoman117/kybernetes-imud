cmake_minimum_required(VERSION 3.14)

find_package(asio REQUIRED)
find_package(Threads REQUIRED)

add_executable(imu-server "${CMAKE_CURRENT_LIST_DIR}/imu-server.cpp")

target_link_libraries(imu-server
    PRIVATE
        asio::asio
        Threads::Threads
        sparkfun-icm-20948
)