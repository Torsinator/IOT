#pragma once

#include "Enumerations.h"

typedef struct
{
    BleUuid service_uuid;
    bool is_connected;
    BlePeerDevice device;
} BluetoothConnection;

typedef struct
{
    bool controlled;
    uint8_t duty_cycle;
} FanDutyCycleMessage;

typedef struct
{
    Node node_id;
    BluetoothMessageId message_type;
    union
    {
        bool bool_data;
        double double_data;
        uint8_t byte_data;  // 1 byte data directly stored
        uint16_t word_data; // 2 bytes data directly stored
        uint8_t string_data[6];
        FanDutyCycleMessage fan_data;
    } data_payload;
} BluetoothMessage;
