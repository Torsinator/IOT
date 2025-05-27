// ELEC4740 A1
// Tors Webster c3376513

#pragma once

// enum class for LED state
enum class LED_STATE
{
    INIT,
    OFF,
    GREEN_FLASHING,
    GREEN_SOLID,
    RED_FLASHING,
    RED_SOLID
};

enum class BluetoothMessageId
{
    TEMPERATURE,
    SOUND_CHANGE,
    LIGHT,
    CALL_BTN,
    POWER,
    CONNECT,
    DISCONNECT,
    FAN_DUTY,
    PAIRING
};

enum class Node
{
    CONTROL,
    SN1,
    SN2
};

enum class PairingStatus
{
    OFF,
    PAIRING,
    BUTTON
};