// ELEC4740 A1
// Tors Webster c3376513

#include <atomic>

#include "Particle.h"
#include "FanControl.h"
#include "Constants.h"
#include "Bluetooth.h"

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
    double sum = 0;
    uint32_t num_samples = 0;
    Timer send_timer(10000, []
                     {Bluetooth::SendDutyCycle(sum / num_samples * 100); sum = 0; num_samples = 0; });

    // Setup function called by main thread
    void Setup()
    {
        // Attach interrupt to measure RPM
        attachInterrupt(FAN_SPEED, FanEdgeCallback, FALLING);
        os_mutex_create(&fan_mtx);
        new Thread("Fan_Thread", RunFanThread);
        fan_speed_timer.start();
        send_timer.start();
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
        os_mutex_lock(fan_mtx);
        duty_cycle = duty;
        os_mutex_unlock(fan_mtx);
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
                double duty = CalculateDuty(pot_value);
                SetDutyCycle(duty);
                Log.trace("pot %ld", pot_value);
            }
            analogWrite(FAN_OUT, round(duty_cycle * 255), PWM_FREQUENCY);
            // When getting RPM
            if (fan_timer_complete)
            {
                Log.info("Duty: %.2f", duty_cycle * 100);
                Log.info("Fan RPM: %d", fan_rpm);
                fan_timer_complete = false;
            }
            sum += duty_cycle;
            num_samples++;
            delay(100);
        }
    }
}