/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include <Wire.h>

#include <limits>

#include <unistd.h>

#include <sys/fcntl.h>
#include <sys/ioctl.h>

TwoWire Wire;

TwoWire::TwoWire()
        : fd(-1), transactions{}, transmitBuffer{}, receiveBuffer{} {
    /* empty */
}

// arduino::HardwareI2C Functions
void TwoWire::begin() {
    fd = open("/dev/i2c-1", O_RDWR);
    if (fd < 0) {
        perror("Failed to open the i2c interface");
        return;
    }
}

void TwoWire::begin(uint8_t address) {
    fprintf(stderr, "I2C Slave Mode is not implemented\n");
    exit(EXIT_FAILURE);
}

void TwoWire::end() {
    if (fd != -1) {
        close(fd);
    }
}

void TwoWire::setClock(uint32_t) {

}

void TwoWire::beginTransmission(uint8_t address) {
    // Add an i2c transaction to our transmit queue
    transactions.push_back({
        .addr = address,
        .flags = 0,
        .len = 0,
        .buf = nullptr,
    });
}

uint8_t TwoWire::endTransmission(bool stop) {
    // establish this transaction's length
    auto it = transactions.rbegin();
    if (it == transactions.rend()) {
        // other error: endTransmission called without calling beginTransmission
        return 4;
    }

    size_t pending = 0;
    for (const auto& xfer : transactions) {
        if (!(xfer.flags & I2C_M_RD)) {
            pending += xfer.len;
        }
    }
    it->len = transmitBuffer.size() - pending;

    // if a stop is requested, perform operations
    if (stop) {
        auto it = transactions.begin();
        it->buf = transmitBuffer.data();
        for (auto nit = it + 1; nit != transactions.end(); nit++, it++) {
            nit->buf = it->buf + it->len;
        }

        i2c_rdwr_ioctl_data transfer = {
            .msgs = transactions.data(),
            .nmsgs = (uint32_t) transactions.size()
        };

        int rc = ioctl(fd, I2C_RDWR, &transfer);
        if (rc < 0) {
            perror("i2c endTransmission failed");
            return 4;
        }

        transactions.clear();
        transmitBuffer.clear();
    }

    return 0;
}

uint8_t TwoWire::endTransmission(void) {
    return endTransmission(true);
}

size_t TwoWire::requestFrom(uint8_t address, size_t len, bool stop) {
    if (!stop) {
        fprintf(stderr, "Read requests without stops are not supported\n");
        exit(EXIT_FAILURE);
    }

    // if there are pending transmissions, setup pointers
    auto it = transactions.begin();
    if (it != transactions.end()) {
        it->buf = transmitBuffer.data();
        for (auto nit = it + 1; nit != transactions.end(); nit++, it++) {
            nit->buf = it->buf + it->len;
        }
    }

    // setup the read operation
    receiveBuffer.clear();
    receiveBuffer.reserve(len);
    receiveBuffer.resize(len);
    transactions.push_back({
        .addr = address,
        .flags = I2C_M_RD,
        .len = (uint16_t) len,
        .buf = receiveBuffer.data(),
    });

    // perform the transfer
    i2c_rdwr_ioctl_data transfer = {
        .msgs = transactions.data(),
        .nmsgs = (uint32_t) transactions.size()
    };

    int rc = ioctl(fd, I2C_RDWR, &transfer);
    if (rc < 0) {
        perror("i2c requestFrom failed");
        return 4;
    }

    transactions.clear();
    transmitBuffer.clear();
    return len;
}

size_t TwoWire::requestFrom(uint8_t address, size_t len) {
    return requestFrom(address, len, true);
}


void TwoWire::onReceive(void(*)(int)) {
    
}

void TwoWire::onRequest(void(*)(void)) {

}


// arduino::Print methods
size_t TwoWire::write(uint8_t byte) {
    transmitBuffer.push_back(byte);
    return 1;
}

size_t TwoWire::write(const uint8_t *buffer, size_t size) {
    transmitBuffer.insert(transmitBuffer.end(), buffer, buffer + size);
    return size;
}

int TwoWire::availableForWrite() {
    return std::numeric_limits<int>::max();
}

void TwoWire::flush() {

}


// arduino::Stream methods
int TwoWire::available() {
    return receiveBuffer.size();
}

int TwoWire::read() {
    auto it = receiveBuffer.begin();
    if (it != receiveBuffer.end()) {
        auto value = *it;
        receiveBuffer.erase(it);
        return value;
    }
    return 0;
}

int TwoWire::peek() {
    if (receiveBuffer.size() > 0) {
        return receiveBuffer[0];
    }
    return 0;
}
