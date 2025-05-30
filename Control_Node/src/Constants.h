#pragma once

#include "Particle.h"

#define LED_1_RED D8
#define LED_1_GREEN D9
#define LED_2_RED D2
#define LED_2_GREEN D3
#define LED_3_RED D4
#define LED_3_GREEN D5

#define CALL_ACK_BTN_SN1 D10
#define CALL_ACK_BTN_SN2 D6

// generated using https://bleid.netlify.app/
// Bluetooth
const char SN2_SERVICE_UUID[] = "ea10000-eeb4-43c3-afef-6423cce071ae";
const char SN2_TEMP_SENS_CHAR_UUID[] = "ea10001-eeb4-43c3-afef-6423cce071ae";
const char SN2_CALL_BTN_CHAR_UUID[] = "ea10002-eeb4-43c3-afef-6423cce071ae";
const char SN2_SOUND_UUID[] = "ea10003-eeb4-43c3-afef-6423cce071ae";

const char SN2_POT_CHAR_UUID[] = "ea10004-eeb4-43c3-afef-6423cce071ae";

const char CN_SERVICE_UUID[] = "ea20000-eeb4-43c3-afef-6423cce071ae";
const char CN_FAN_DUTY_CHAR_UUID[] = "ea20001-eeb4-43c3-afef-6423cce071ae";
const char CN_LIGHT_INDICATOR_UUID[] = "ea20002-eeb4-43c3-afef-6423cce071ae";
const char CN_SECURITY_UUID[] = "ea20003-eeb4-43c3-afef-6423cce071ae";

const char SN1_SERVICE_UUID[] = "ea30000-eeb4-43c3-afef-6423cce071ae";
const char SN1_LUX_CHAR_UUID[] = "ea30001-eeb4-43c3-afef-6423cce071ae";

const char SN1_POT_CHAR_UUID[] = "ea30002-eeb4-43c3-afef-6423cce071ae";

const char SN1_CALL_BTN_CHAR_UUID[] = "ea30003-eeb4-43c3-afef-6423cce071ae";
const char SN1_MOV_CHAR_UUID_STR[] = "ea30004-eeb4-43c3-afef-6423cce071ae";      // <<< [추가]

// bluetooth scan results
const size_t SCAN_RESULT_MAX = 30;
