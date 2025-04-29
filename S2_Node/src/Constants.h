// ELEC4740 A1
// Tors Webster c3376513

#pragma once

#include "Particle.h"

// Pin definitions
#define TEMP_SENS A5
#define SOUND_SENS D2
#define PUSH_BTN D10
#define FAN_POT A4
#define FAN_OUT D6
#define FAN_SPEED D5
#define CALL_LED_RED D3
#define CALL_LED_GREEN D4
#define SOUND_LED_RED D0
#define SOUND_LED_GREEN D1

// Sound
const uint16_t SOUND_TIMEOUT_MS = 5000;

// Emergency Call
const uint16_t BTN_DEBOUNCE_MS = 500;

// Fan
const double POT_THRESHOLD = 10.0;
const uint32_t PWM_FREQUENCY = 25e3;
const uint16_t FAN_COUNT_PERIOD_MS = 1000;

// Temperature
const uint16_t TEMPERATURE_MEASURE_RATE_HZ = 10;


