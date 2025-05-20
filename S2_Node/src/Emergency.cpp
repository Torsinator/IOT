// ELEC4740 A1
// Tors Webster c3376513

#include <atomic>

#include "Particle.h"

#include "Enumerations.h"
#include "Emergency.h"
#include "Constants.h"
#include "CallLED.h"
#include "Bluetooth.h"

namespace Emergency
{
    // Set up variables
    bool is_emergency = false;
    std::atomic_bool debouncing = false;
    os_queue_t button_queue;
    Timer debounce_timer(BTN_DEBOUNCE_MS, DebounceCallback, true);
    CallLED CALL_LED(CALL_LED_RED, CALL_LED_GREEN);

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
        os_queue_put(button_queue, (void *)true, 0, nullptr);
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
            if (os_queue_take(button_queue, &queue_value, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
            {
                // Only action if not debouncing
                if (!debouncing && queue_value)
                {
                    // Toggle is_emergency
                    is_emergency = !is_emergency;
                    if (is_emergency)
                    {
                        Log.trace("Call button pressed");
                        CALL_LED.call_button_pressed = true;
                    }
                    else
                    {
                        Log.trace("Call button deactivated");
                        CALL_LED.call_deactivated = true;
                    }
                    // Send notification via ble
                    Bluetooth::SendButtonPress(CALL_LED.call_button_pressed);
                    
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