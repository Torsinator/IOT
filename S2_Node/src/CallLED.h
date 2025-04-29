// ELEC4740 A1
// Tors Webster c3376513

#pragma once

#include "Enumerations.h"
#include "LED.h"

// Class for the emergency call LED
class CallLED : public LED
{
public:
    CallLED(uint16_t red_pin, uint16_t green_pin, uint16_t timer_flash_period_ms = 200);
    void get_next_state();
    bool powered_on = false;
    bool bluetooth_connection = false;
    bool call_button_pressed = false;
    bool call_recieved = false;
    bool call_deactivated = false;
};