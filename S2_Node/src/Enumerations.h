// ELEC4740 A1
// Tors Webster c3376513

#pragma once

// Enum for LED state
enum LED_STATE
{
    INIT,
    OFF,
    GREEN_FLASHING,
    GREEN_SOLID,
    RED_FLASHING,
    RED_SOLID
};

enum BluetoothMessageId
{
    TEMPERATURE,
    SOUND_CHANGE,
    LIGHT,
    CALL_BTN,
    POWER,
    CONNECT,
    DISCONNECT,
    FAN_DUTY
};

enum Node
{
    CONTROL,
    SN1,
    SN2
};