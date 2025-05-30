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
    double GetPowerSN1();
    bool GetLightsOn();
    bool GetMoveDetectedSN1();

    // SN2
    bool IsConnectedSN2();
    bool IsCallButtonActivatedSN1();
    bool IsSoundDetected();
    double GetTemperatureLevel();
    uint16_t GetFanSpeed();
    double GetPowerSN2();
    double GetTemperatureLightsOn();
    double GetTemperatureLightsOff();
    bool GetFanControlled();

    // CN
    double GetPowerCN();

    // Setters
    // SN1
    void SetConnectedSN1(bool value);
    void SetCallButtonActivatedSN1(bool value);
    void SetLightLevel(uint8_t value);
    void SetPowerSN1(double value);
    void SetMoveDetectedSN1(bool value);
    // SN2
    void SetConnectedSN2(bool value);
    void SetCallButtonActivatedSN2(bool value);
    void SetSoundDetected(bool value);
    void SetTemperatureLevel(double value);
    void SetFanSpeed(uint16_t value);
    void SetPowerSN2(double value);
    void SetTemperatureLightsOn(double temp);
    void SetTemperatureLightsOff(double temp);
    void SetFanControlled(bool value);

    // CN
    void SetPowerCN(double value);

private:
    os_mutex_t data_mutex;
    SensorNode1Data sn1_data{false, false, 0, 0};
    SensorNode2Data sn2_data{false, false, false, 0, 0, 0};
    double cn_accumulated_power_mWh = 0.0; // <--- CN 누적 전력량 변수 추가
};
