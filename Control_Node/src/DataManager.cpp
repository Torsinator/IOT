#include "DataManager.h"
#include "MutexLockGuard.h"

DataManager::DataManager()
{
    os_mutex_create(&data_mutex);
}

SensorNode1Data DataManager::GetSensorNode1Data()
{
    // MutexLockGuard guard(&data_mutex);
    return sn1_data;
}

SensorNode2Data DataManager::GetSensorNode2Data()
{
    MutexLockGuard guard(&data_mutex);
    return sn2_data;
}

void DataManager::SetSensorNode1Data(const SensorNode1Data& data)
{
    MutexLockGuard guard(&data_mutex);
    sn1_data = data;
}

void DataManager::SetSensorNode2Data(const SensorNode2Data& data)
{
    MutexLockGuard guard(&data_mutex);
    sn2_data = data;
}
