// © Kay Sievers <kay@versioduo.com>, 2020-2022
// SPDX-License-Identifier: Apache-2.0

#include "V2Potentiometer.h"

#include <cmath>

void V2Potentiometer::measure(float analog) {
  struct {
    float    fraction;
    uint16_t step;
  } now;

  // Low-pass filter, smooth the value.
  _history.analog *= 1.f - config->alpha;
  _history.analog += analog * config->alpha;

  if (_history.analog < config->min) {
    // Below minimum.
    now.fraction = 0;
    now.step     = 0;
    _history.lag = 0.f - config->lag;

  } else if (_history.analog > config->max) {
    // Above maximum.
    now.fraction = 1;
    now.step     = config->nSteps - 1;
    _history.lag = 1.f + config->lag;

  } else {
    // Normalized 0..1 fraction of the min..max range.
    now.fraction = (_history.analog - config->min) / (config->max - config->min);

    // The new measurement is inside the lag, don't update the step value.
    if (std::fabs(now.fraction - _history.lag) < config->lag)
      return;

    now.step = roundf(now.fraction * (float)(config->nSteps - 1));
  }

  if (now.step == _step)
    return;

  _step = now.step;

  // Reposition the edge of the lag. We follow monotonic changes immediately,
  // but apply the lag if the direction changes.
  if (now.fraction - _history.lag > 0.f)
    _history.lag = now.fraction - config->lag;

  else
    _history.lag = now.fraction + config->lag;
}
