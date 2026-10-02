#pragma once
#include <stdint.h>
#include "../config/CommsConfig.h"

class Comms {
public:
    void begin();
    void update();

    // All public methods are safe to call from loop() context only — NOT from any ISR.
    void sendLog(const char* level, const char* msg);
    void sendEvent(const char* event);
    void sendEvent(const char* event, const char* dataKey, const char* dataValue);

private:
    uint32_t _lastTelemetryMs = 0;
    uint32_t _lastHeartbeatMs = 0;

    void _sendTelemetry();
    void _readCommands();
    void _printEscaped(const char* str);
};

extern Comms comms;
