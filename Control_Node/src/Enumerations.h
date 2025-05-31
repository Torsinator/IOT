#pragma once

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
    SECURITY,
    SN1_PWM_VALUE,
    SN2_PWM_VALUE,
    SET_TEMPERATURE_LIGHTS_OFF,
    SET_TEMPERATURE_LIGHTS_ON,
    SET_LUX_LEVEL,
    CALL_BUTTON_OFF,
    MOTION_DETECTED
};

enum class Node
{
    CONTROL,
    SN1,
    SN2
};

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

enum class LCD_Message
{
    NEW_SCREEN,
    REFRESH
};