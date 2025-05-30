#pragma once

#include "Particle.h"
#include "DataManager.h"

extern os_queue_t control_queue;
extern DataManager data_manager;

namespace Cloud
{
    void Setup();
    int SetTemperatureLightsOn(String command);
    int SetTemperatureLightsOff(String command);
    void publishPowerData();
}