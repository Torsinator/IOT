// ELEC4740 A1
// Tors Webster c3376513

#pragma once

#include "stdint.h"

// Temperature Functions
namespace Temperature
{
    void Setup();
    double InvTransferFunction(int32_t adc_value);
    void RunTemperatureThread();
}