#pragma once

#include <stdint.h>

// Communication and telemetry configuration.

namespace Config {
    constexpr uint16_t TELEMETRY_BALANCE_HZ        = 50;
    constexpr uint32_t TELEMETRY_BALANCE_PERIOD_MS = 1000 / TELEMETRY_BALANCE_HZ;

    constexpr uint16_t HEARTBEAT_LOG_HZ            = 1;
    constexpr uint32_t HEARTBEAT_LOG_PERIOD_MS     = 1000 / HEARTBEAT_LOG_HZ;
}
