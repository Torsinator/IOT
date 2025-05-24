#pragma once

#include "Particle.h"
#include "Enumerations.h"

typedef struct
{
    Node node_id;
    BluetoothMessageId message_type;
    union
    {
        const uint8_t *data; // pointer data
        uint8_t byte_data;       // 1 byte data directly stored
        uint16_t word_data;      // 2 bytes data directly stored
    } data_payload;
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
    double temperature_level;
    uint16_t fan_speed;
    uint16_t power;
} SensorNode2Data;

typedef struct
{
    bool controlled;
    uint8_t duty_cycle;
} FanDutyCycleMessage;
