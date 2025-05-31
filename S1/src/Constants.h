#pragma once

enum class Sn1Led1State
{
  OFF,
  GREEN_FLASHING,
  GREEN_SOLID,
  RED_FLASHING
  // RED_SOLID (향후 CN 피드백 시 추가 가능)
};

typedef struct
{
  bool controlled;
  uint8_t brightness;
}LightlvlMessage;
