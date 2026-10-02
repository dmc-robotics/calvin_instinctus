// Calvin Instinctus - Teensy 4.1

// Serial buffer sizes — must precede Arduino.h
#define SERIAL1_TX_BUFFER_SIZE 256
#define SERIAL1_RX_BUFFER_SIZE 256

#include <IntervalTimer.h>
#include "src/utils/LED.h"
#include "src/comms/Comms.h"
#include "src/comms/RobotState.h"
#include "src/config/BoardConfig.h"
#include "src/config/BalanceConfig.h"

static IntervalTimer _balanceTimer;

// TIMING CRITICAL: must complete in under 1ms.
// No Serial, no I2C, no blocking calls of any kind.
// ISR is preemptible by higher-priority interrupts — keep it short.
void balanceISR() {
    // TODO: read IMU, run filter, write robotState
    robotState.loopCount++;
}

void setup() {
    Serial.begin(Config::USB_BAUD_RATE);
    while (!Serial && millis() < Config::USB_SERIAL_TIMEOUT_MS);
    pinMode(LED_BUILTIN, OUTPUT);
    comms.begin();
    _balanceTimer.begin(balanceISR, Config::BALANCE_LOOP_PERIOD_US);
    comms.sendLog("INFO", "Instinctus awakens.");
    ledBlink(5, 100, 100);
}

void loop() {
    comms.update();
}
