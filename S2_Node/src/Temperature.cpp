// ELEC4740 A1
// Tors Webster c3376513

#include "Temperature.h"
#include "Constants.h"
#include "Bluetooth.h"

namespace Temperature
{
    // Temperature ADC values, ready for averaging
    uint32_t temperature_data = 0;

    // Creates thread
    void Setup()
    {
        new Thread("Temperature_Thread", RunTemperatureThread);
    }

    // Transfer Function
    double InvTransferFunction(int32_t adc_value)
    {
        if (adc_value > 3350)
        {
            return 0.0235 * adc_value - 49.729;
        }
        return 0.0098*adc_value - 3.5935;
    }

    // main thread function
    void RunTemperatureThread()
    {
        uint16_t count = 0;
        while (true)
        {
            // Check for send to bluetooth period
            if (count >= TEMPERATURE_SEND_RATE_S * TEMPERATURE_MEASURE_RATE_HZ)
            {
                // Apply inverse transfer function to get temperature
                // divide by count to get 10 point average
                double temp = InvTransferFunction(temperature_data / count);
                temperature_data = 0;
                count = 0;
                Log.info("Temperature Reading: %.2f degrees celcius", temp);

                // Send the temperature multiplied by 100
                Bluetooth::SendTemperature((uint16_t) (temp * 100));
            }
            // Add the ADC value to the temperature data
            temperature_data += analogRead(TEMP_SENS);
            count++;
            delay(1000 / TEMPERATURE_MEASURE_RATE_HZ);
        }
    }
}