// ELEC4740 A1
// Tors Webster c3376513

#include <atomic>

#include "Particle.h"

#include "Enumerations.h"
#include "Emergency.h"
#include "Constants.h"
#include "CallLED.h"
#include "Bluetooth.h"

extern CallLED CALL_LED;

namespace Emergency
{
    // Set up variables
    std::atomic_bool debouncing = false;
    os_queue_t button_queue;
    Timer debounce_timer(BTN_DEBOUNCE_MS, DebounceCallback, true);

    // Setup and initialize threads and interrupts
    void Setup()
    {
        os_queue_create(&button_queue, sizeof(bool), 1, nullptr);
        attachInterrupt(PUSH_BTN, ButtonCallback, FALLING);
        CALL_LED.powered_on = true; // Set LED to powered on
        CALL_LED.get_next_state();
        CALL_LED.update_LED();
        new Thread("Emergency_Thread", Emergency::RunEmergencyThread);
    }

    // Callback for button press
    void ButtonCallback(void)
    {
        // Add the button press to the queue
        bool value = true;
        os_queue_put(button_queue, &value, 0, nullptr);
    }

    // Debounce callback
    void DebounceCallback(void)
    {
        debouncing = false;
    }

    // Thread function
    void RunEmergencyThread(void)
    {
        while (true)
        {
            // Check for state update
            bool queue_value;
            bool temp_value;
            if (os_queue_take(button_queue, &queue_value, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
            {
                // Only action if not debouncing
                if (!debouncing && queue_value)
                {
                    if (!CALL_LED.call_button_pressed)
                    {
                        Log.info("Call button pressed");
                        CALL_LED.call_button_pressed = true;
                        temp_value = true;
                    }
                    else
                    {
                        Log.info("Call button deactivated");
                        CALL_LED.call_button_pressed = false;
                        CALL_LED.call_deactivated = true;
                        temp_value = false;

                    }
                    // Send notification via ble
                    Bluetooth::SendButtonPress(temp_value);
                    
                    // Update the LED state
                    CALL_LED.get_next_state();
                    CALL_LED.update_LED();

                    // Start debouncing
                    debouncing = true;
                    debounce_timer.start();
                }
            }
        }
    }
}