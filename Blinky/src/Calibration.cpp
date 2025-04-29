/*
 * Project test
 * Description:
 * Author:
 * Date:
 */
#include "Particle.h"
#include "dct.h"

// SYSTEM_MODE(MANUAL);
SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

int a0 = A0;

// setup() runs once, when the device is first turned on.
void setup()
{
    pinMode(a0, INPUT);
    pinMode(D2, INPUT);
}

// loop() runs over and over again, as quickly as it can execute.
void loop()
{
    // The core of your code will likely live here.
    // printf("Value: %d", analogRead(a1));
    Log.info("Starting");
    // Light Sensor
    Log.info("Value: %ld", analogRead(a0));

    // Sound sensor
    // Log.info("Value: %ld", digitalRead(D2));
    delay(1000);
}