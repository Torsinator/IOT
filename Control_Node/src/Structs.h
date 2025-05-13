#pragma once

#include "Particle.h"
#include "Enumerations.h"

typedef struct
{
    Node node_id;
    BluetoothMessageId message_type;
    const uint8_t *data;
} BluetoothMessage;

typedef struct
{
    BleUuid service_uuid;
    bool is_connected;
    BlePeerDevice device;
} BluetoothConnection;

typedef struct
{
    bool connected;
    bool call_button_activated;
    uint8_t light_level;
    uint16_t power;
} SensorNode1Data;

typedef struct
{
    bool connected;
    bool call_button_activated;
    bool sound_detected;
    uint8_t temperature_level;
    uint16_t fan_speed;
    uint16_t power;
} SensorNode2Data;

typedef struct
{
    bool controlled;
    uint8_t duty_cycle;
} FanDutyCycleMessage;

