#pragma once

enum class Sn1Led1State
{
  OFF,
  GREEN_FLASHING,
  GREEN_SOLID,
  RED_FLASHING
};

typedef struct
{
  bool controlled;
  uint8_t brightness;
}LightlvlMessage;
