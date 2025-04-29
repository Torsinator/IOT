// ELEC4740 A1
// Tors Webster c3376513

#pragma once

#include "Constants.h"

namespace SoundSensor
{
    void Setup();
    void SoundCallback(void);
    void RunSoundSensorThread();
}