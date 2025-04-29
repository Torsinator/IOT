// ELEC4740 A1
// Tors Webster c3376513

// Include Particle Device OS APIs
#include "Particle.h"

// Include all sub functions 
#include "Constants.h"
#include "Emergency.h"
#include "SoundSensor.h"
#include "FanControl.h"
#include "Temperature.h"
#include "CallLED.h"
#include "SoundLED.h" 

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// setup() runs once, when the device is first turned on
void setup()
{
  // Initialize Pins
  pinMode(TEMP_SENS, INPUT);
  pinMode(SOUND_SENS, INPUT);
  pinMode(PUSH_BTN, INPUT_PULLUP);  // Means no external resistor required
  pinMode(FAN_POT, INPUT);
  pinMode(FAN_OUT, OUTPUT);
  pinMode(FAN_SPEED, INPUT);
  pinMode(CALL_LED_GREEN, OUTPUT);
  pinMode(CALL_LED_RED, OUTPUT);
  pinMode(SOUND_LED_GREEN, OUTPUT);
  pinMode(SOUND_LED_RED, OUTPUT);
  pinMode(TEMP_SENS, INPUT);

  // Call setup function from each namespace
  Emergency::Setup();
  SoundSensor::Setup();
  Fan::Setup();
  Temperature::Setup();
}

void loop()
{
  // Will use this thread to communicate with the control node in part 2
  os_thread_yield();
}
