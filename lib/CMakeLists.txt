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
    "${CMAKE_CURRENT_LIST_DIR}/ArduinoCore-API/test/src/MillisFake.cpp"
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

target_compile_definitions(sparkfun-icm-20948
    PUBLIC
        ICM_20948_USE_DMP
)

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