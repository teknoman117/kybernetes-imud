cmake_minimum_required(VERSION 3.14)

# Arduino Core
add_library(arduino-core STATIC
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/api/Common.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/api/IPAddress.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/api/PluggableUSB.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/api/Print.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/api/Stream.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/api/String.cpp"

    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/test/src/dtostrf.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/test/src/itoa.cpp"
)

target_include_directories(arduino-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/api>
    PRIVATE
        "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/test/include"
)

target_compile_definitions(arduino-core
    PUBLIC
        HOST
)

target_compile_features(arduino-core PUBLIC cxx_std_17)

# Micro Arduino Core for Linux
add_subdirectory(ArduinoCore-Minimal-Linux)

# SparkFun ICM 20948 library
add_library(sparkfun-icm-20948 STATIC
    "${CMAKE_CURRENT_LIST_DIR}/SparkFun_ICM-20948_ArduinoLibrary/src/util/ICM_20948_C.c"
    "${CMAKE_CURRENT_LIST_DIR}/SparkFun_ICM-20948_ArduinoLibrary/src/ICM_20948.cpp"
)

target_include_directories(sparkfun-icm-20948
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/SparkFun_ICM-20948_ArduinoLibrary/src>
)

#target_compile_definitions(sparkfun-icm-20948
#    PUBLIC
#        ICM_20948_USE_DMP
#)

target_link_libraries(sparkfun-icm-20948
    PUBLIC
        arduino-core-linux
)

target_compile_features(sparkfun-icm-20948 PUBLIC cxx_std_17)

# Sparkfun ICM 20948 Example 6
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/SparkFun_ICM-20948_ArduinoLibrary/examples/Arduino/Example6_DMP_Quat9_Orientation/Example6_DMP_Quat9_Orientation.ino"
    "${CMAKE_CURRENT_BINARY_DIR}/Example6_DMP_Quat9_Orientation.ino.cpp"
    COPYONLY
)

add_executable(Example6_DMP_Quat9_Orientation
    "${CMAKE_CURRENT_BINARY_DIR}/Example6_DMP_Quat9_Orientation.ino.cpp"
)

target_link_libraries(Example6_DMP_Quat9_Orientation
    PRIVATE
        sparkfun-icm-20948
)

# Sparkfun ICM 20948 Example 7
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/SparkFun_ICM-20948_ArduinoLibrary/examples/Arduino/Example7_DMP_Quat6_EulerAngles/Example7_DMP_Quat6_EulerAngles.ino"
    "${CMAKE_CURRENT_BINARY_DIR}/Example7_DMP_Quat6_EulerAngles.ino.cpp"
    COPYONLY
)

add_executable(Example7_DMP_Quat6_EulerAngles
    "${CMAKE_CURRENT_BINARY_DIR}/Example7_DMP_Quat6_EulerAngles.ino.cpp"
)

target_link_libraries(Example7_DMP_Quat6_EulerAngles
    PRIVATE
        sparkfun-icm-20948
)

### ICM_20948-AHRS ###
# magneto calibration tool
add_executable(magneto1_4 "${CMAKE_CURRENT_LIST_DIR}/ICM_20948-AHRS/magneto1_4/magneto1_4.c")

target_link_libraries(magneto1_4
    PRIVATE
        $<$<PLATFORM_ID:Linux>:m>
        $<$<PLATFORM_ID:Darwin>:m>
)

# get_cal_data
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/ICM_20948-AHRS/ICM_20948_get_cal_data.ino"
    "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/ICM_20948_get_cal_data.ino.cpp"
)

add_executable(ICM_20948_get_cal_data "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/ICM_20948_get_cal_data.ino.cpp")

target_link_libraries(ICM_20948_get_cal_data
    PRIVATE
        sparkfun-icm-20948
)

# tiltcomp
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/ICM_20948-AHRS/ICM_20948_tiltcomp.ino"
    "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/ICM_20948_tiltcomp.ino.cpp"
)

add_executable(ICM_20948_tiltcomp "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/ICM_20948_tiltcomp.ino.cpp")

target_link_libraries(ICM_20948_tiltcomp
    PRIVATE
        sparkfun-icm-20948
)

# gravity demo
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/ICM_20948-AHRS/Mahony_AHRS/gravity_demo.ino"
    "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/Mahony_AHRS/gravity_demo.ino.cpp"
)

add_executable(ICM_20948_gravity_demo "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/Mahony_AHRS/gravity_demo.ino.cpp")

target_link_libraries(ICM_20948_gravity_demo
    PRIVATE
        sparkfun-icm-20948
)

# Mahony
configure_file(
    "${CMAKE_CURRENT_LIST_DIR}/ICM_20948-AHRS/Mahony_AHRS/ICM_20948_Mahony.ino"
    "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/Mahony_AHRS/ICM_20948_Mahony.ino.cpp"
)

add_executable(ICM_20948_Mahony "${CMAKE_CURRENT_BINARY_DIR}/ICM_20948-AHRS/Mahony_AHRS/ICM_20948_Mahony.ino.cpp")

target_link_libraries(ICM_20948_Mahony
    PRIVATE
        sparkfun-icm-20948
)
