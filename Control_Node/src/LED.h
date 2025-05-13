// ELEC4740 A1
// Tors Webster c3376513

#pragma once

#include "Particle.h"

#include "Enumerations.h"

class LED
{
    // public functions
public:
    LED(uint16_t red_pin, uint16_t green_pin, uint16_t timer_flash_period_ms = 200);
    ~LED() = default;
    void update_LED(LED_STATE next_state);

    // Able to be accessed by subclasses
protected:
    LED_STATE current_state = LED_STATE::INIT;

private:
    void timer_callback(void);
    void solid_green();
    void flash_green();
    void solid_red();
    void flash_red();
    void turn_off();

    // member variables
    uint16_t red_pin;
    uint16_t green_pin;
    uint16_t timer_flash_period_ms;
    Timer flash_timer;
    bool flash_on = false;
};