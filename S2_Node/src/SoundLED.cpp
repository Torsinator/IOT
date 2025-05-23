// ELEC4740 A1
// Tors Webster c3376513

#include "SoundLED.h"
#include "Enumerations.h"
#include "Constants.h"
#include "Bluetooth.h"

SoundLED::SoundLED(uint16_t red_pin, uint16_t green_pin, uint16_t timer_flash_period_ms) : LED(red_pin, green_pin, timer_flash_period_ms), timeout(SOUND_TIMEOUT_MS, std::bind(&SoundLED::TimeoutCallback, this), true)
{
}

void SoundLED::get_next_state()
{
    os_mutex_lock(led_mutex);
    next_state = current_state;
    switch (current_state)
    {
    case LED_STATE::OFF:
    case LED_STATE::INIT:
    {
        if (sound_detected && lights_on)
        {
            next_state = LED_STATE::GREEN_FLASHING;
        }
        else if (sound_detected && !lights_on)
        {
            next_state = LED_STATE::RED_FLASHING;
        }
        else
        {
            next_state = LED_STATE::OFF;
        }
        break;
    }
    case LED_STATE::GREEN_FLASHING:
    case LED_STATE::RED_FLASHING:
    {
        if (sound_timeout)
        {
            Log.info("Sound timeout");
            timer_is_running = false;
            sound_timeout = false;
            next_state = LED_STATE::OFF;
            Bluetooth::SendSoundEvent(false);
        }
        else if (timer_is_running && sound_detected)
        {
            timeout.start();
        }
        else if (!sound_detected && !timer_is_running)
        {
            Log.info("Sound timeout start");
            timer_is_running = true;
            timeout.start();
        }
        else if (lights_on)
        {
            next_state = LED_STATE::GREEN_FLASHING;
        }
        else
        {
            next_state = LED_STATE::RED_FLASHING;
        }

        break;
    }
    default:
        break;
    }
    os_mutex_unlock(led_mutex);
}

void SoundLED::TimeoutCallback()
{
    sound_timeout = true;
    get_next_state();
    update_LED();
}
