// ELEC4740 A1
// Tors Webster c3376513

#pragma once

#include <atomic> 

#include "Enumerations.h"
#include "LED.h"

// LED for the sound sensor
class SoundLED : public LED
{
public:
    SoundLED(uint16_t red_pin, uint16_t green_pin, uint16_t timer_flash_period_ms = 200);
    void get_next_state();
    bool sound_detected = false;
    bool lights_on = false;
private:
    Timer timeout;
    void TimeoutCallback();
    std::atomic_bool sound_timeout = false;
    bool timer_is_running = false;
};