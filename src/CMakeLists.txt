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

# Install the daemon
install(TARGETS imu-server RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR})

# Install systemd units
configure_file(
    "${PROJECT_SOURCE_DIR}/systemd/imu-server.service"
    "${CMAKE_CURRENT_BINARY_DIR}/imu-server.service"
)

set(SYSTEMD_UNIT_DIR "lib/systemd/system")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/imu-server.service" DESTINATION "${SYSTEMD_UNIT_DIR}")

# TODO: support systemd socket handover
#install(FILES "${PROJECT_SOURCE_DIR}/systemd/imu-server.socket" DESTINATION "${SYSTEMD_UNIT_DIR}")