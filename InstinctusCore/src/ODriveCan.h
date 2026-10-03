#pragma once

#include <stdint.h>

// Driver for one ODrive S1 on its own CAN bus (classic CAN, 1 Mbit/s).
// Protocol: https://docs.odriverobotics.com/v/latest/manual/can-protocol.html
// Arbitration ID = node_id << 5 | cmd_id. Payloads are little-endian; floats are float32.
// Units are ODrive-native: revolutions, rev/s, motor-side N·m, amps.
//
// TODO (M1): FlexCAN_T4 setup, commands, and feedback parsing in the receive interrupt.

// Latest feedback from one ODrive. Each *ReceivedMicros is micros() when that
// message arrived, so sample age can be measured.
struct MotorFeedback {
    // Heartbeat (0x01)
    uint32_t axisError;
    uint8_t  axisState;
    uint32_t heartbeatReceivedMicros;

    // Get_Encoder_Estimates (0x09)
    float    positionRevolutions;
    float    velocityRevolutionsPerSecond;
    uint32_t encoderReceivedMicros;

    // Get_Iq (0x14)
    float    iqSetpointAmps;
    float    iqMeasuredAmps;
    uint32_t iqReceivedMicros;

    // Get_Torques (0x1C)
    float    torqueTargetNewtonMeters;
    float    torqueEstimateNewtonMeters;
    uint32_t torquesReceivedMicros;

    // Get_Bus_Voltage_Current (0x17)
    float    busVoltage;
    float    busCurrentAmps;

    // Get_Temperature (0x15)
    float    fetTemperatureCelsius;
    float    motorTemperatureCelsius;
};

class ODriveCan {
public:
    // direction is +1 or -1 so that positive torque drives the robot forward.
    ODriveCan(uint8_t nodeId, int8_t direction);

private:
    uint8_t _nodeId;
    int8_t  _direction;
};
