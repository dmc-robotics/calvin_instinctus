#pragma once

#include <stdint.h>

// Balance control and timing configuration.

namespace Config {
    // Loop rate
    constexpr uint16_t BALANCE_LOOP_HZ        = 1000;
    constexpr uint32_t BALANCE_LOOP_PERIOD_US = 1000000UL / BALANCE_LOOP_HZ;

}
