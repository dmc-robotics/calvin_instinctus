#pragma once

#include <stdint.h>

// Teensy 4.1 board-level configuration.

// Jetson serial port — Serial1 is a HardwareSerial object, not a constexpr value
#define JETSON_SERIAL Serial1

namespace Config {
    // USB debug serial
    constexpr uint32_t USB_BAUD_RATE         = 115200;
    constexpr uint16_t USB_SERIAL_TIMEOUT_MS = 3000;

    // Jetson serial
    constexpr uint32_t JETSON_BAUD_RATE = 1000000;

}
