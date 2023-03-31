kybernetes-imu-server
=====================

A simple program which wraps the SparkFun ICM-20948 Arduino Library for use on a Linux system.

Build Instructions
------------------
```
git clone https://github.com/teknoman117/kybernetes-imu-server
cd kybernetes-imu-server
git submodule update --init lib/ArduinoCore-API lib/SparkFun_ICM-20948_ArduinoLibrary
cmake -GNinja -DCMAKE_BUILD_TYPE=Release -B build .
cmake --build build -j
```

Usage Instructions
------------------
TODO