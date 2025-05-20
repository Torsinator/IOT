#pragma once

#include "Particle.h"
#include "Structs.h"

class DataManager
{
public:
    DataManager();
    ~DataManager() = default;

    // Getters
    // SN1
    bool IsConnectedSN1();
    bool IsCallButtonActivatedSN2();
    uint8_t GetLightLevel();
    uint16_t GetPowerSN1();

    // SN2
    bool IsConnectedSN2();
    bool IsCallButtonActivatedSN1();
    bool IsSoundDetected();
    double GetTemperatureLevel();
    uint16_t GetFanSpeed();
    uint16_t GetPowerSN2();

    // Setters
    // SN1
    void SetConnectedSN1(bool value);
    void SetCallButtonActivatedSN1(bool value);
    void SetLightLevel(double value);
    void SetPowerSN1(uint16_t value);

    // SN2
    void SetConnectedSN2(bool value);
    void SetCallButtonActivatedSN2(bool value);
    void SetSoundDetected(bool value);
    void SetTemperatureLevel(double value);
    void SetFanSpeed(uint16_t value);
    void SetPowerSN2(uint16_t value);

private:
    os_mutex_t data_mutex;
    SensorNode1Data sn1_data{false, false, 0, 0};
    SensorNode2Data sn2_data{false, false, false, 0, 0, 0};
};
