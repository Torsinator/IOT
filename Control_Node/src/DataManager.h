#pragma once

#include "Particle.h"
#include "Structs.h"

class DataManager
{
public:
    DataManager();
    ~DataManager() = default;
    SensorNode1Data GetSensorNode1Data();
    SensorNode2Data GetSensorNode2Data();
    void SetSensorNode1Data(const SensorNode1Data &data);
    void SetSensorNode2Data(const SensorNode2Data &data);

private:
    os_mutex_t data_mutex;
    SensorNode1Data sn1_data{false, false, 0, 0};
    SensorNode2Data sn2_data{false, false, false, 0, 0, 0};
};
