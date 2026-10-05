# Calvin Instinctus

Calvin is a robotics project contained in `~/code/robotics/calvin/`. See `~/code/robotics/calvin/CLAUDE.md` for project level information. This file is for `calvin_instinctus` infromation only.

Real-time firmware for Calvin's Teensy 4.1: balance control, motor control over CAN, and safety. Single core.

## Current state

A skeleton. What exists:

```
instinctus/
  instinctus.ino          1 kHz IntervalTimer ISR (balanceISR: only counts loops so far) + loop() running comms
  src/comms/Comms.*       newline JSON to the Jetson over Serial1: telemetry at 50 Hz, 1 Hz heartbeat log,
                          events; incoming command parsing is a TODO
  src/comms/RobotState.*  volatile struct shared between the ISR and loop(); readers snapshot with noInterrupts()
  src/config/*.h          constants in the Config namespace (loop rate, baud rates, telemetry rates)
  src/utils/LED.h         blocking blink helpers (setup() only)
InstinctusCore/           Arduino library shared with calvin_bench's bench sketch (scaffold only so far)
docs/                     Teensy/i.MX RT1060 and Cortex-M7 reference manuals and errata
```

No IMU, ToF, CAN or balance code yet. The next firmware work is `InstinctusCore`'s ODrive CAN driver, built through `calvin_bench` (see `../calvin_bench/PLAN.md`), then the balance loop here.

## InstinctusCore

- Shared library: ODrive CAN driver (`ODriveCan`), `SafetySupervisor`, `Recorder`. Symlinked as `~/code/arduino/libraries/InstinctusCore`; also used by `../calvin_bench/firmware/bench/`.
- A library can't see a sketch's config headers, so settings (CAN bus, node ID, limits, buffers) are passed in as parameters.
- **Changing its API means updating and rebuilding the bench sketch too.**

## Hardware on the Teensy

- **IMU:** ISM330DHCX (Adafruit breakout, near the wheel axis) on SPI0.
- **ToF:** 2× VL53L4CX on one I2C bus; rear reprogrammed to 0x30, front at the default 0x29, using XSHUT pins at boot (both low → bring up rear, readdress → bring up front). The front sensor isn't always connected; handle it missing. The STM32duino VL53L4CX library retries I2C forever when a sensor doesn't answer; if ToF start-up hangs, that's why.
- **Motors:** 2× ODrive S1, left on CAN1 (pins 22/23, node 1), right on CAN3 (pins 30/31, node 2), 1 Mbit/s, torque control.
- **Jetson:** UART on Serial1 (pins 0/1) at 1,000,000 baud to the Jetson's header UART; protocol in `../calvin_cogitator/PROTOCOL.md`. USB serial (`Serial`) is for debugging and flashing only.

## Planned architecture

- **1 kHz balance ISR (must finish well under 1 ms):** read the IMU, estimate tilt, read the latest ODrive feedback, run the controller (LQR, later a small RL policy), safety check, send torque commands, record. No Serial, no blocking I/O, no dynamic allocation.
- **`loop()`, best effort:** ToF at ~20 Hz, Jetson JSON comms, everything else.
- **Safety:** tilt warning and e-stop limits; ToF proximity warnings; battery cutoff; e-stop = all motors idle. The safe state stops the motors and reports why.

## Build

`cd instinctus && grot build && grot load` (Teensy 4.1, `teensy:avr:teensy41`). The `.grotconfig` (gitignored) holds the port.

## Resources

- [ODrive CAN protocol](https://docs.odriverobotics.com/v/latest/manual/can-protocol.html)
- [ISM330DHCX](https://www.st.com/en/mems-and-sensors/ism330dhcx.html)
