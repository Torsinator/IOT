// ELEC4740 A1
// Tors Webster c3376513

#include "LED.h"
#include "Enumerations.h"

// LED base class constructor. Sets member variables
LED::LED(uint16_t red_pin, uint16_t green_pin, uint16_t timer_flash_period_ms) : red_pin(red_pin),
                                                                                 green_pin(green_pin),
                                                                                 timer_flash_period_ms(timer_flash_period_ms),
                                                                                 flash_timer(timer_flash_period_ms, [this]
                                                                                             { this->timer_callback(); }) { update_LED(); }

// Function to update the LED output
void LED::update_LED()
{
    if (current_state != next_state)
    {
        switch (next_state)
        {
        case OFF:
        {
            flash_timer.stop();
            turn_off();
            break;
        }
        case LED_STATE::GREEN_FLASHING:
        {
            flash_green();
            break;
        }
        case LED_STATE::GREEN_SOLID:
        {
            flash_timer.stop();
            solid_green();
            break;
        }
        case LED_STATE::RED_FLASHING:
        {
            flash_red();
            break;
        }
        case LED_STATE::RED_SOLID:
        {
            flash_timer.stop();
            solid_red();
            break;
        }
        }
        current_state = next_state;
    }
}

// Helper functions to perform LED behaviours
void LED::solid_green()
{
    // Green
    digitalWrite(green_pin, LOW);
    digitalWrite(red_pin, HIGH);
}

void LED::flash_green()
{
    flash_timer.start();
}

void LED::solid_red()
{
    // Red
    digitalWrite(green_pin, HIGH);
    digitalWrite(red_pin, LOW);
}

void LED::flash_red()
{
    flash_timer.start();
}

void LED::turn_off()
{
    digitalWrite(green_pin, HIGH);
    digitalWrite(red_pin, HIGH);
}

// Callback for flash timer
void LED::timer_callback()
{
    flash_on = !flash_on;
    if (flash_on)
    {
        if (current_state == GREEN_FLASHING)
        {
            solid_green();
        }
        else if (current_state == RED_FLASHING)
        {
            solid_red();
        }
        else
        {
            flash_timer.stop();
        }
    }
    else
    {
        turn_off();
    }
}