/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef ARDUINO_H_
#define ARDUINO_H_

#include <ArduinoAPI.h>

// Host stdio wrapper implementing "Stream"
class HostConsole : public Stream {
public:
    HostConsole() {}

    void begin(int) {}

    // arduino::Print methods
    size_t write(uint8_t byte) override;
    size_t write(const uint8_t *buffer, size_t size);
    int availableForWrite() override;
    void flush() override;

    // arduino::Stream methods
    int available() override;
    int read() override;
    int peek() override;

    constexpr bool operator!() {
        return false;
    }
};

extern HostConsole Serial;

extern void setup();
extern void loop();

#endif /* ARDUINO_H_ */