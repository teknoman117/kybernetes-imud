cmake_minimum_required(VERSION 3.14)

include(GNUInstallDirs)

find_package(asio REQUIRED)
find_package(systemd REQUIRED)
find_package(Threads REQUIRED)

add_executable(imud "${CMAKE_CURRENT_LIST_DIR}/imud.cpp")

target_link_libraries(imud
    PRIVATE
        asio::asio
        Threads::Threads
        sparkfun-icm-20948
)

# we are only using the systemd headers
target_include_directories(imud
    PRIVATE
        "${systemd_INCLUDE_DIRS}"
)

# Install the daemon
install(TARGETS imud RUNTIME DESTINATION ${CMAKE_INSTALL_SBINDIR})

# Install systemd units
configure_file(
    "${PROJECT_SOURCE_DIR}/systemd/imud.service"
    "${CMAKE_CURRENT_BINARY_DIR}/imud.service"
)

set(SYSTEMD_UNIT_DIR "lib/systemd/system")
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/imud.service" DESTINATION "${SYSTEMD_UNIT_DIR}")

# TODO: support systemd socket handover
#install(FILES "${PROJECT_SOURCE_DIR}/systemd/imud.socket" DESTINATION "${SYSTEMD_UNIT_DIR}")