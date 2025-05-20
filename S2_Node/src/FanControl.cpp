// ELEC4740 A1
// Tors Webster c3376513

#include <atomic>

#include "Particle.h"
#include "FanControl.h"
#include "Constants.h"

namespace Fan
{
    // Set up variables
    uint16_t fan_counts = 0;
    uint16_t fan_rpm = 0;
    Timer fan_speed_timer(FAN_COUNT_PERIOD_MS, FanCountTimerCallback);
    std::atomic_bool fan_timer_complete = false;
    std::atomic_bool overridden = false;
    double duty_cycle = 0;
    os_mutex_t fan_mtx;

    // Setup function called by main thread
    void Setup()
    {
        // Attach interrupt to measure RPM
        attachInterrupt(FAN_SPEED, FanEdgeCallback, FALLING);
        os_mutex_create(&fan_mtx);
        new Thread("Fan_Thread", RunFanThread);
        fan_speed_timer.start();
    }

    // FG pin callback
    void FanEdgeCallback(void)
    {
        fan_counts++;
    }

    // Timer callback for counting RPM
    void FanCountTimerCallback()
    {
        fan_timer_complete = true;

        // Calculate Fan RPM
        fan_rpm = 60 / (FAN_COUNT_PERIOD_MS * 1e-3 * 2 / fan_counts);
        fan_counts = 0;
    }

    // Helper function to calculate duty cycle
    double CalculateDuty(uint32_t pot_value)
    {
        if (pot_value < POT_THRESHOLD)
        {
            return 0;
        }
        return (1.71e-2 * pot_value + 29.914) / 100.0;
    }

    void SetOverrideStatus(bool status)
    {
        overridden = status;
    }

    void SetDutyCycle(double duty)
    {
        duty_cycle = duty;
    }

    // Thread function
    void RunFanThread(void)
    {
        while (true)
        {
            if (!overridden)
            {
                // Calculate PWM value to send by reading Pot value
                int32_t pot_value = min(analogRead(FAN_POT), pow(2, 12) - 1);
                SetDutyCycle(CalculateDuty(pot_value));
                Log.trace("pot %ld", pot_value);
            }
            analogWrite(FAN_OUT, round(duty_cycle * 255), PWM_FREQUENCY);
            // When getting RPM
            if (fan_timer_complete)
            {
                Log.trace("Duty: %.2f", duty_cycle * 100);
                Log.info("Fan RPM: %d", fan_rpm);
                fan_timer_complete = false;
            }
            delay(100);
        }
    }
}