/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
#ifndef WIRE_H_
#define WIRE_H_

#include <HardwareI2C.h>

#include <vector>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

class TwoWire : public arduino::HardwareI2C {
    int fd;
    std::vector<i2c_msg> transactions;
    std::vector<uint8_t> transmitBuffer;
    std::vector<uint8_t> receiveBuffer;

    

public:
    // arduino::HardwareI2C Functions
    void begin() override;
    void begin(uint8_t address) override;
    void end() override;

    void setClock(uint32_t freq) override;
  
    void beginTransmission(uint8_t address) override;
    uint8_t endTransmission(bool stopBit) override;
    uint8_t endTransmission(void) override;

    size_t requestFrom(uint8_t address, size_t len, bool stopBit) override;
    size_t requestFrom(uint8_t address, size_t len) override;

    void onReceive(void(*)(int)) override;
    void onRequest(void(*)(void)) override;

    // arduino::Print methods
    size_t write(uint8_t byte) override;
    size_t write(const uint8_t *buffer, size_t size) override;
    int availableForWrite() override;
    void flush() override;

    // arduino::Stream methods
    int available() override;
    int read() override;
    int peek() override;

public:
    TwoWire();
};

extern TwoWire Wire;

#endif /* WIRE_H_ */