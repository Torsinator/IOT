#pragma once

#include "Particle.h"
#include "Enumerations.h"

typedef struct
{
    Node node_id;
    BluetoothMessageId message_type;
    union
    {
        bool bool_data;
        const uint8_t *data; // pointer data
        double double_data;
        uint8_t byte_data;  // 1 byte data directly stored
        uint16_t word_data; // 2 bytes data directly stored
        uint8_t string_data[6];
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
    double power;
    bool move_detected;
} SensorNode1Data;

typedef struct
{
    bool connected;
    bool call_button_activated;
    bool sound_detected;
    double temperature_level;
    uint16_t fan_speed;
    double power;
    uint8_t set_duty_lights_on;
    uint8_t set_duty_lights_off;
    bool fan_controlled;
} SensorNode2Data;

typedef struct
{
    bool controlled;
    uint8_t duty_cycle;
} FanDutyCycleMessage;
