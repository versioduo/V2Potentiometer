// © Kay Sievers <kay@vrfy.org>, 2020-2021
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <Arduino.h>

class V2Potentiometer {
public:
  const struct Config {
    // The number of steps to map the measurement to.
    // 128 steps will emit values from 0 to 127.
    uint16_t n_steps;

    // The normalized 0..1 value of the analog measurement range.
    float min;
    float max;

    // The exponential smoothing constant.
    float alpha;

    // Hysteresis lag; the amount of jitter we accept without changing the step.
    // The unit is a fraction of the normalized 0..1 value of the min..max range.
    float lag;
  } * config;

  constexpr V2Potentiometer(const Config *conf) : config(conf) {}

  // Normalized 0..1 measurement. Smooth the value, convert it to a fraction
  // of the given 'steps', do not call 'handleChange()' if the delta is inside
  // the hysteresis 'lag'.
  void measure(float analog);

  void reset() {
    _history  = {};
    _position = {};
  }

  float getFraction() {
    return _position.fraction;
  }

  uint16_t getStep() {
    return _position.step;
  }

protected:
  virtual void handleChange(float fraction, uint16_t step) {}

  struct {
    // The smoothed-out, normalized (0..1) analog measurement.
    float analog;

    // The edge of the lag range, set by the previous value.
    float lag;
  } _history{};

  struct {
    // The current 0..1 value.
    float fraction;

    // The current step value.
    uint16_t step;
  } _position{};
};
