#pragma once

#include "Particle.h"

#define LED_1_RED D0
#define LED_1_GREEN D1
#define LED_2_RED D2
#define LED_2_GREEN D3
#define LED_3_RED D4
#define LED_3_GREEN D5

// generated using https://bleid.netlify.app/
// Bluetooth
const char SN2_SERVICE_UUID[] = "ea10000-eeb4-43c3-afef-6423cce071ae";
const char SN2_TEMP_SENS_CHAR_UUID[] = "ea10001-eeb4-43c3-afef-6423cce071ae";
const char SN2_CALL_BTN_CHAR_UUID[] = "ea10002-eeb4-43c3-afef-6423cce071ae";
const char SN2_SOUND_UUID[] = "ea10003-eeb4-43c3-afef-6423cce071ae";
const char CN_SERVICE_UUID[] = "ea20000-eeb4-43c3-afef-6423cce071ae";
const char CN_FAN_DUTY_CHAR_UUID[] = "ea20001-eeb4-43c3-afef-6423cce071ae";

// bluetooth scan results
const size_t SCAN_RESULT_MAX = 30;
