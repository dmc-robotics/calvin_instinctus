#pragma once
#include <Arduino.h>

inline void ledOn()  { digitalWrite(LED_BUILTIN, HIGH); }
inline void ledOff() { digitalWrite(LED_BUILTIN, LOW); }

inline void ledBlink(uint8_t count, uint16_t onMs = 200, uint16_t offMs = 200) {
    for (uint8_t i = 0; i < count; i++) {
        digitalWrite(LED_BUILTIN, HIGH);
        delay(onMs);
        digitalWrite(LED_BUILTIN, LOW);
        if (i < count - 1) delay(offMs);
    }
}
