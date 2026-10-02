#pragma once
#include <stdint.h>

// Shared state written by high-priority tasks, read by Comms.
// Readers must use noInterrupts()/interrupts() to get a consistent multi-field snapshot.
struct RobotState {
    // Balance (written by balance task @ 1000Hz)
    float    tiltAngle;           // degrees
    float    tiltRate;            // degrees/sec
    float    targetVelocity;      // m/s

    // ToF (written by collision task @ 20Hz)
    uint16_t distFrontMm;
    uint16_t distRearMm;

    // Motors (written by motor task)
    float    motorLeftVelocity;
    float    motorRightVelocity;

    // Safety
    bool     eStop;
    uint8_t  faultFlags;          // bitmask — define fault bits as needed

    // Diagnostics
    uint32_t loopCount;           // incremented each balance loop — stall detection
};

extern volatile RobotState robotState;
