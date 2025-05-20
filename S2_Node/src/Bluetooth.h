#pragma once

#include "Particle.h"
#include "Structs.h"

extern os_queue_t main_queue;

namespace Bluetooth
{
    // void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void Setup();
    bool Connect(BluetoothConnection &connection);
    void Advertise();
    void ControlNodeConnectionThread();
    void onDisconnectHandler(const BlePeerDevice &peer);
    void DutyCycleHandler(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
    void SendTemperature(const uint16_t temperature);
    void SendButtonPress(const bool value);
    void SendSoundEvent(const bool value);
}
