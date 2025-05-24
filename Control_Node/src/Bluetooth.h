#pragma once

#include "Particle.h"
#include "Structs.h"

extern os_queue_t control_queue;

namespace Bluetooth
{
    // void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void Setup();
    bool Connect(BluetoothConnection &connection);
    void SensorNode1Thread();
    void SensorNode2Thread();
    void Advertise();
    void onDisconnectHandler(const BlePeerDevice &peer);
    void CallButtonSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void CallButtonSN2(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void SoundHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void TemperatureHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void LuxHandlerSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);

}