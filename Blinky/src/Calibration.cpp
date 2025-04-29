/*
 * Project test
 * Description:
 * Author:
 * Date:
 */
#include "Particle.h"
#include "dct.h"
#include <stdio.h>
#include <math.h>

// SYSTEM_MODE(MANUAL);
// SYSTEM_THREAD(ENABLED);
SYSTEM_MODE(MANUAL);

// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Pin Definitions
int motionSensor = D4;      // Motion sensor pin
int greenLED = A0;          // Green LED pin
int redLED = A1;            // Red LED pin
int lightSensor = A2;       // Light sensor pin
int luxThreshold = 20;      // Lux threshold for light sensor
int potenPin = A3;          // Potentiometer pin
int LEDPin = D2;            // White LED pin
int redLEDforbutton = D6;   // Red LED for button
int greenLEDforbutton = D5; // Green LED for button
int pushbutton = D3;        // Button pin

// Motion sensor variables
bool previousMotionValue = LOW;   // Store the previous state of the motion sensor
unsigned long lastMotionTime = 0; // Store the last time motion was detected
// LED flashing variables
unsigned long currentMillis = 0;  // Store the current time
unsigned long previousMillis = 0; // Store the last time the LED was updated
const long interval = 500;        // Blink interval (500ms)
bool flashing = false;            // Flag to indicate if the LED is flashing
bool ledState = LOW;              // current state of blinking led (HIGH/LOW)
int currentLED = -1;              // which LED is flashing (0: green, 1: red, -1: none)

// Button state variables
int currentbutton = 0;                  // variable for button state (0: released, 1: pressed)
unsigned long currentbuttonMillis = 0;  // Store the last time the button was pressed
unsigned long previousbuttonMillis = 0; // Store the last time the button was released

void setup()
{
  pinMode(motionSensor, INPUT);        // Motion sensor as input
  pinMode(greenLED, OUTPUT);           // Green LED as output
  pinMode(redLED, OUTPUT);             // Red LED as output
  pinMode(lightSensor, INPUT);         // Light sensor as input
  pinMode(potenPin, INPUT);            // Potentiometer as input
  pinMode(LEDPin, OUTPUT);             // White LED as output
  pinMode(pushbutton, INPUT_PULLDOWN); // Pull-down resistor for button
  pinMode(redLEDforbutton, OUTPUT);    // Red LED for button
  pinMode(greenLEDforbutton, OUTPUT);  // Green LED for button

  // Initialize LEDs to off state
  digitalWrite(greenLED, HIGH);          // turn off the green LED at the beginning
  digitalWrite(redLED, HIGH);            // turn off the red LED at the beginning
  digitalWrite(redLEDforbutton, HIGH);   // turn off the red LED at the beginning
  digitalWrite(greenLEDforbutton, HIGH); // turn off the green LED at the beginning
  Serial.begin(9600);                    // Initialize serial communication
}

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