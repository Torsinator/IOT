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
    Node node_id;
    BluetoothMessageId message_type;
    const uint8_t *data;
} BluetoothMessage;

typedef struct
{
    bool controlled;
    uint8_t duty_cycle;
} FanDutyCycleMessage;
