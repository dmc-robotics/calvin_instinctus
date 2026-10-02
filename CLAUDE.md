# Calvin Instinctus - Project Instructions

## IMPORTANT:
Instinctus runs on a Teensy 4.1 (single core). The earlier Arduino GIGA R1 WiFi code (`instinctus_m4/`) was removed on 2026-10-01; it's only in git history (last GIGA commit `41a228a`) and isn't a reference for new code.


## System Overview

**Calvin Instinctus** is the low-level reflex and motor control system for Calvin, a self-balancing robot. Running on a Teensy 4.1, it handles real-time balance control, motor actuation, and safety monitoring.

**Calvin's Three-System Architecture:**
- **instinctus** (THIS SYSTEM) - Reflexive motor control and balance
- **cogitator** - High-level AI and planning (Jetson Orin Nano) - `/Users/damoncali/code/robotics/calvin/calvin_cogitator/CLAUDE.md`
- **explorator** - Human monitoring interface (native macOS app) - `/Users/damoncali/code/robotics/calvin/calvin_explorator/CLAUDE.md`

**Integration:**
- Sends status updates to cogitator via serial
- Receives commands from cogitator via serial
- Status data flows through cogitator to explorator for visualization


## Hardware Platform

**Teensy 4.1** - Microcontroller
- **Processor** ARM Cortex-M7 at 600 MHz
- **Memory** 7936K Flash, 1024K RAM (512K tightly coupled), 4K EEPROM (emulated)
- **Connectivity**: 8 serial, 3 SPI, 3 I2C ports, 3 CAN Bus (1 with CAN FD)

**Sensors:**
- ISM330DHCX 6-axis IMU (Adafruit breakout, mounted near the wheel axis) on SPI0 (the first SPI port has a FIFO for higher sustained transfer rates) - Balance sensing and collision detection
- 2x VL53L4CX ToF sensors (I2C, rear 0x30 / front 0x29) - Obstacle detection
  - Both on same bus, differentiated via XSHUT pins
  - On boot: both XSHUT LOW, then rear brought up and reprogrammed to 0x30, then front brought up at default 0x29
  - XSHUT ensures clean power cycle on every MCU reset
- Battery monitor (specific model TBD)

**Actuators:**
- 2x ODrive S1 motor controllers, one CAN bus each at 1 Mbit/s: CAN1 (pins 22/23) and CAN3 (pins 30/31), via Adafruit CAN Pal (TJA1051T/3) transceivers. CAN2 is unusable because its pins 0/1 are Serial1 (Jetson)
- 2x Odrive Dual Shaft Motor - D5312s 330KV


## Architecture

### High Priority Tasks
**Tasks:**
1. **Balance Control** - Read IMU, run complementary filter, calculate tilt
2. **Motor Control** - Generate velocity commands, send CAN messages to ODrives
3. **Collision Detection** - Poll ToF sensors at 20 Hz
4. **Safety Monitoring** - Tilt limits, battery voltage, emergency stops

**Never block high priority tasks** - No delays >1ms, no Serial.print, no blocking I/O

### Low Priortiy Tasks - Communication Hub

**Tasks:**
1. **Jetson Bridge** - Forward events to Jetson


## Communication Protocols

### Teensy ↔ Jetson (Serial)

**Protocol:** Newline-delimited JSON — see `/Users/damoncali/code/robotics/calvin/calvin_cogitator/PROTOCOL.md`
**Physical:** Serial1 at 1,000,000 baud


## Build and Upload
- This project uses the **Grot** tool (`/Users/damoncali/code/robotics/grot`, installed as the `grot` gem) for building and uploading Arduino sketches.
- .grotconfig files specify board settings and port.


## Integration Points

### With Cogitator (Jetson Orin Nano, Python)
**Documentation:** `/Users/damoncali/code/robotics/calvin/calvin_cogitator/CLAUDE.md`

**Interface:** Serial communication — protocol defined in `/Users/damoncali/code/robotics/calvin/calvin_cogitator/PROTOCOL.md`
**Data Flow:**
- Sends: `telemetry`, `tof`, `event`, `log`, `ack` messages
- Receives: `command`, `config`, `ping` messages

### With Explorator (macOS app)
**Documentation:** `/Users/damoncali/code/robotics/calvin/calvin_explorator/CLAUDE.md`

**Interface:** Indirect via Jetson (no direct connection)
**Data Flow:** Teensy → Jetson → Explorator

**Telemetry Provided:**
- Balance status (tilt angle, velocity)
- Motor status (position, velocity, current)
- Battery health (voltage, current, power)
- ToF distance measurements
- I2C bus health
- IMU FFT data for vibration analysis

## Safety Features

**Tilt Limits:**
- Warning and e stop limits.

**Proximity and Collision Detection:**
- Warning when too close to objects (detected by ToF sensors)
- Corrective action when collision detected (TBD).  

**Battery Protection:** (TODO) Critical voltage warning and shutdown at appropriate voltages (4S LiPo)

**Safe State:** Motors stop, balance continues, warning displayed, alert sent

**E Stop** Total motor shutdown.

## Code Style

- Class names: `PascalCase` (e.g., `BalanceIMU`)
- Methods: `camelCase` (e.g., `getTiltAngle()`)
- Constants: `UPPER_SNAKE_CASE` (e.g., `CRITICAL_TILT_ANGLE`)
- Private members: `_camelCase`

## Resources

- [Teensy 4.1](https://www.pjrc.com/store/teensy41.html)
- [ODrive CAN Protocol](https://docs.odriverobotics.com/v/latest/can-protocol.html)
- [ISM330DHCX](https://www.st.com/en/mems-and-sensors/ism330dhcx.html)

## Notes

- **Timing is critical** - Teensy must maintain sufficient loop timing for stable balance - targeting 1,000 Hz
- **Never block on critical tasks**
