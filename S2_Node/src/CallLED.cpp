// ELEC4740 A1
// Tors Webster c3376513

#include "CallLED.h"
#include "Enumerations.h"

// Constructor - Creates parent LED class instance
CallLED::CallLED(uint16_t red_pin, uint16_t green_pin, uint16_t timer_flash_period_ms) : LED(red_pin, green_pin, timer_flash_period_ms) {}

// Function to get next state based off input variables
void CallLED::get_next_state()
{
    next_state = current_state;
    switch (current_state)
    {
    case LED_STATE::INIT:
    case LED_STATE::OFF:
    {
        if (powered_on)
        {
            if (bluetooth_connection)
            {
                next_state = LED_STATE::GREEN_SOLID;
            }
            else
            {
                next_state = LED_STATE::GREEN_FLASHING;
            }
        }
        else
        {
            next_state = LED_STATE::OFF;
        }
        break;
    }
    case LED_STATE::GREEN_FLASHING:
    case LED_STATE::GREEN_SOLID:
    {
        if (call_button_pressed)
        {
            call_button_pressed = false;
            next_state = LED_STATE::RED_FLASHING;
        }
        else if (bluetooth_connection)
        {
            next_state = LED_STATE::GREEN_SOLID;
        }
        else
        {
            next_state = LED_STATE::GREEN_FLASHING;
        }
        break;
    }
    case LED_STATE::RED_FLASHING:
    {
        if (call_recieved)
        {
            call_recieved = false;
            next_state = LED_STATE::RED_SOLID;
        }
        else if (call_deactivated)
        {
            call_deactivated = false;
            if (bluetooth_connection)
            {
                next_state = LED_STATE::GREEN_SOLID;
            }
            else
            {
                next_state = LED_STATE::GREEN_FLASHING;
            }
        }
        break;
    }
    case LED_STATE::RED_SOLID:
    {
        if (call_deactivated)
        {
            call_deactivated = false;
            if (bluetooth_connection)
            {
                next_state = LED_STATE::GREEN_SOLID;
            }
            else
            {
                next_state = LED_STATE::GREEN_FLASHING;
            }
        }
        break;
    }
    default:
        break;
    }
}
