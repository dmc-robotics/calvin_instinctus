#pragma once

// Fixed-size ring buffer of samples: the 1 kHz ISR pushes, loop() drains and
// streams them to the host. Counts overflows instead of blocking.
// The sketch defines the sample type and the buffer size.
//
// TODO (M2)

template <typename Sample, unsigned Capacity>
class Recorder {
};
