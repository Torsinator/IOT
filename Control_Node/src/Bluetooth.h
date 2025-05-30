#pragma once

#include "Particle.h"
#include "Structs.h"
#include "Enumerations.h"

extern os_queue_t control_queue;

namespace Bluetooth
{
    // void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void Setup();
    bool Connect(BluetoothConnection &connection);
    bool Disconnect(const Node &node);
    void SensorNode1Thread();
    void SensorNode2Thread();
    void Advertise();
    void onPairingEvent(const BlePairingEvent &event, void *context);
    void onDisconnectHandler(const BlePeerDevice &peer);
    void CallButtonSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void CallButtonSN2(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void SoundHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void TemperatureHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void LuxHandlerSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void PotentiometerLedControlHandlerSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context); // <--- SN1 PWM 값 핸들러 추가
    void PotHandlerSN2(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
    void SecurityHandler(const uint8_t* data, size_t len, const BlePeerDevice& peer, void* context);
    void SetPairingSuccess(bool success);
    void SetFanDutyCycle(bool controlled, uint8_t duty);
    void DeactivateCallSN1();
    void DeactivateCallSN2();
    void SetLightOnOff(bool value);

    void MoveHandlerSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);

}