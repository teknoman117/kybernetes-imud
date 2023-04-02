/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
 
#include <Arduino.h>

#include <chrono>

namespace {
    std::chrono::steady_clock::time_point const begin = std::chrono::steady_clock::now();
}

unsigned long millis() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - begin).count();
    return static_cast<unsigned long>(millis);
}

unsigned long micros() {
    std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now - begin).count();
    return static_cast<unsigned long>(micros);
}
