#include "DataManager.h"

#include "Particle.h"
#include "MutexLockGuard.h"

DataManager::DataManager()
{
    os_mutex_create(&data_mutex);
}

// Getters
// SN1

bool DataManager::IsConnectedSN1()
{
    MutexLockGuard lock(&data_mutex);
    return sn1_data.connected;
}

bool DataManager::IsCallButtonActivatedSN1()
{
    MutexLockGuard lock(&data_mutex);
    return sn1_data.call_button_activated;
}

uint8_t DataManager::GetLightLevel()
{
    MutexLockGuard lock(&data_mutex);
    return sn1_data.light_level;
}

uint16_t DataManager::GetPowerSN1()
{
    MutexLockGuard lock(&data_mutex);
    return sn1_data.power;
}

// SN2
bool DataManager::IsConnectedSN2()
{
    MutexLockGuard lock(&data_mutex);
    return sn2_data.connected;
}

bool DataManager::IsCallButtonActivatedSN2()
{
    MutexLockGuard lock(&data_mutex);
    return sn2_data.call_button_activated;
}

bool DataManager::IsSoundDetected()
{
    MutexLockGuard lock(&data_mutex);
    return sn2_data.sound_detected;
}

double DataManager::GetTemperatureLevel()
{
    MutexLockGuard lock(&data_mutex);
    return sn2_data.temperature_level;
}

uint16_t DataManager::GetFanSpeed()
{
    MutexLockGuard lock(&data_mutex);
    return sn2_data.fan_speed;
}

uint16_t DataManager::GetPowerSN2()
{
    MutexLockGuard lock(&data_mutex);
    return sn2_data.power;
}

// Setters
// SN1
void DataManager::SetConnectedSN1(bool value)
{
    MutexLockGuard lock(&data_mutex);
    sn1_data.connected = value;
}

void DataManager::SetCallButtonActivatedSN1(bool value)
{
    MutexLockGuard lock(&data_mutex);
    sn1_data.call_button_activated = value;
}

void DataManager::SetLightLevel(uint8_t value)
{
    MutexLockGuard lock(&data_mutex);
    sn1_data.light_level = value;
}

void DataManager::SetPowerSN1(uint16_t value)
{
    MutexLockGuard lock(&data_mutex);
    sn1_data.power = value;
}

// SN2
void DataManager::SetConnectedSN2(bool value)
{
    MutexLockGuard lock(&data_mutex);
    sn2_data.connected = value;
}

void DataManager::SetCallButtonActivatedSN2(bool value)
{
    MutexLockGuard lock(&data_mutex);
    sn2_data.call_button_activated = value;
}

void DataManager::SetSoundDetected(bool value)
{
    MutexLockGuard lock(&data_mutex);
    sn2_data.sound_detected = value;
}

void DataManager::SetTemperatureLevel(double value)
{
    MutexLockGuard lock(&data_mutex);
    sn2_data.temperature_level = value;
}

void DataManager::SetFanSpeed(uint16_t value)
{
    MutexLockGuard lock(&data_mutex);
    sn2_data.fan_speed = value;
}

void DataManager::SetPowerSN2(uint16_t value)
{
    MutexLockGuard lock(&data_mutex);
    sn2_data.power = value;
}
