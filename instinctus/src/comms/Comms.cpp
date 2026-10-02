#include "Comms.h"
#include "RobotState.h"
#include "../config/BoardConfig.h"
#include "../config/CommsConfig.h"
#include <Arduino.h>

Comms comms;

static char _rxBuf[256];
static uint8_t _rxPos = 0;

void Comms::begin() {
    JETSON_SERIAL.begin(Config::JETSON_BAUD_RATE);
}

void Comms::update() {
    uint32_t now = millis();

    _readCommands();

    if (now - _lastHeartbeatMs >= Config::HEARTBEAT_LOG_PERIOD_MS) {
        _lastHeartbeatMs = now;
        sendLog("INFO", "heartbeat");
    }

    if (now - _lastTelemetryMs >= Config::TELEMETRY_BALANCE_PERIOD_MS) {
        _lastTelemetryMs = now;
        _sendTelemetry();
    }
}

void Comms::sendLog(const char* level, const char* msg) {
    JETSON_SERIAL.printf("{\"type\":\"log\",\"ms\":%lu,\"level\":\"%s\",\"msg\":\"", millis(), level);
    _printEscaped(msg);
    JETSON_SERIAL.print("\"}\n");
}

void Comms::sendEvent(const char* event) {
    JETSON_SERIAL.printf("{\"type\":\"event\",\"ms\":%lu,\"event\":\"%s\"}\n", millis(), event);
}

void Comms::sendEvent(const char* event, const char* dataKey, const char* dataValue) {
    JETSON_SERIAL.printf(
        "{\"type\":\"event\",\"ms\":%lu,\"event\":\"%s\",\"data\":{\"%s\":\"",
        millis(), event, dataKey
    );
    _printEscaped(dataValue);
    JETSON_SERIAL.print("\"}}\n");
}

void Comms::_sendTelemetry() {
    noInterrupts();
    float    tilt      = robotState.tiltAngle;
    float    rate      = robotState.tiltRate;
    float    targetVel = robotState.targetVelocity;
    float    motorL    = robotState.motorLeftVelocity;
    float    motorR    = robotState.motorRightVelocity;
    uint32_t lc        = robotState.loopCount;
    interrupts();

    JETSON_SERIAL.printf(
        "{\"type\":\"telemetry\",\"ms\":%lu,\"tilt\":%.2f,\"tiltRate\":%.2f,"
        "\"targetVel\":%.2f,\"motorL\":%.2f,\"motorR\":%.2f,\"loopCount\":%lu}\n",
        millis(), tilt, rate, targetVel, motorL, motorR, lc
    );
}

void Comms::_readCommands() {
    while (JETSON_SERIAL.available()) {
        char c = JETSON_SERIAL.read();
        if (c == '\n') {
            _rxBuf[_rxPos] = '\0';
            if (_rxPos > 0) {
                // TODO: parse JSON command and dispatch
                sendLog("WARN", "command parsing not implemented");
            }
            _rxPos = 0;
        } else if (_rxPos < sizeof(_rxBuf) - 1) {
            _rxBuf[_rxPos++] = c;
        }
        // else: overflow, silently drop until newline
    }
}

void Comms::_printEscaped(const char* str) {
    while (*str) {
        char c = *str++;
        if (c == '"')       JETSON_SERIAL.print("\\\"");
        else if (c == '\\') JETSON_SERIAL.print("\\\\");
        else if (c == '\n') JETSON_SERIAL.print("\\n");
        else                JETSON_SERIAL.write(c);
    }
}
