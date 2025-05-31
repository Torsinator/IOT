// BluetoothLE_SN1.h
#pragma once
#include "Particle.h"
#include "Constants.h"

extern void setSn1Led1State(Sn1Led1State newState);
extern volatile bool buttonToggleState;
extern volatile uint8_t TargetLightlvl;
extern volatile bool controlled;

namespace BluetoothLE_SN1 {


    enum class Sn1DataType {
        CURRENT_BRIGHTNESS, // g_brightness 
        AVERAGE_BRIGHTNESS, // 30s avg g_brightness
        LUX_LEVEL,           //  g_lux
        BUTTON_STATE,      
        MOTION_DETECTED     
    };


    typedef struct {
        Sn1DataType type;
        uint8_t value;
    } BleSn1DataPacket;

    void setup(); 
    void queueDataForSend(Sn1DataType type, uint8_t value); 
} // namespace BluetoothLE_SN1