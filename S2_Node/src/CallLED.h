// ELEC4740 A1
// Tors Webster c3376513

#pragma once

#include "Enumerations.h"
#include "LED.h"
#include <atomic> 

// Class for the emergency call LED
class CallLED : public LED
{
public:
    CallLED(uint16_t red_pin, uint16_t green_pin, uint16_t timer_flash_period_ms = 200);
    void get_next_state();
    std::atomic_bool powered_on = false;
    std::atomic_bool bluetooth_connection = false;
    std::atomic_bool call_button_pressed = false;
    std::atomic_bool call_recieved = false;
    std::atomic_bool call_deactivated = false;
};