/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
#include <Arduino.h>

#include <chrono>
#include <cstdio>
#include <limits>
#include <thread>

extern "C" {
    #include <sys/ioctl.h>
}

HostConsole Serial;

// arduino::Print methods
size_t HostConsole::write(uint8_t byte) {
    return ::fwrite(&byte, sizeof byte, 1, stdout);
}

size_t HostConsole::write(const uint8_t *buffer, size_t size) {
    return ::fwrite(buffer, size, 1, stdout);
}

int HostConsole::availableForWrite() {
    return std::numeric_limits<int>::max();
}

void HostConsole::flush() {
    ::fflush(stdout);
}

// arduino::Stream methods
int HostConsole::available() {
    int available = 0;
    if (ioctl(fileno(stdin), FIONREAD, &available) == -1) {
        // this function has no ability to return an error
        return 0;
    }

    return available;
}

int HostConsole::read() {
    return ::fgetc(stdin);
}

int HostConsole::peek() {
    int c = ::fgetc(stdin);
    ::ungetc(c, stdin);
    return c;
}

// Delay implementation
void delay(unsigned long ms) {
    auto delay = std::chrono::milliseconds(ms);
    std::this_thread::sleep_for(delay);
}

void delayMicroseconds(unsigned int us) {
    auto delay = std::chrono::microseconds(us);
    std::this_thread::sleep_for(delay);
}

// Ignore digital IO accesses
void pinMode(pin_size_t pinNumber, PinMode pinMode) {

}

void digitalWrite(pin_size_t pinNumber, PinStatus status) {

}

PinStatus digitalRead(pin_size_t pinNumber) {
    return PinStatus::LOW;
}

// main function to run arduino loop
int main () {
  setup();
  while (1) {
    loop();
  }
  return 0;
}