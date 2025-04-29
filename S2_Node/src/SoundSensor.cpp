// ELEC4740 A1
// Tors Webster c3376513

#include <atomic>

#include "SoundLED.h"
#include "SoundSensor.h"
#include "Constants.h"

namespace SoundSensor
{
    // Set up queue and LED variables
    os_queue_t sound_queue;
    SoundLED sound_LED(SOUND_LED_RED, SOUND_LED_GREEN);

    // Setup function called from main
    void Setup()
    {
        Log.trace("Sound Sensor Setup Called");

        // Create the queue with 10 elements max
        os_queue_create(&sound_queue, sizeof(bool), 10, nullptr);
        sound_LED.get_next_state();
        sound_LED.update_LED();

        // Attach interrupt for sound sensor and create the thread
        attachInterrupt(SOUND_SENS, SoundCallback, CHANGE);
        new Thread("Sound_Sensor_Thread", RunSoundSensorThread);
    }

    // Callback for sound function
    void SoundCallback(void)
    {
        bool value = (digitalRead(SOUND_SENS) == 0) ? false : true;
        os_queue_put(sound_queue, (void *) value, 0, nullptr);
    }

    // Thread function
    void RunSoundSensorThread()
    {
        Log.trace("Running sound sensor thread");
        while (true)
        {
            // Check for sound state transition
            bool queue_value;
            if (os_queue_take(sound_queue, &queue_value, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
            {
                Log.trace("Got Sound: %d", queue_value);

                // update LED
                sound_LED.sound_detected = queue_value;
                sound_LED.get_next_state();
                sound_LED.update_LED();
            }
        }
    }
}