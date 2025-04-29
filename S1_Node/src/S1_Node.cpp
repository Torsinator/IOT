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
  //===============================================================
  // POTENTIOMETER AND LIGHT INTESNITY CONTROL
  int potenValue = analogRead(potenPin);             // Read potentiometer value
  int brightness = map(potenValue, 0, 4095, 0, 255); // Map potentiometer value to brightness(DutyCycle)
  analogWrite(LEDPin, brightness);                   // set brightness of white LED
  // Control White LED based on potentiometer value
  Serial.print("Potentiometer Value: ");
  Serial.println(potenValue);
  Serial.print("Brightness(DutyCycle): ");
  Serial.println(brightness);
  //===============================================================

  //===============================================================
  // BUTTON STATE WITH DEBOUNCING
  int buttonValue = digitalRead(pushbutton); // Read button state
  if (buttonValue == HIGH)
    currentbuttonMillis = millis();                      // Store the time when the button was pressed
  if (currentbuttonMillis - previousbuttonMillis >= 500) // 0.5 seconds debouncing - only process button after 500ms of stable state
  {
    if (currentbutton == 1) // if the button was pressed
    {
      currentbutton = 0;
      previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
      digitalWrite(greenLEDforbutton, HIGH);      // turn on green LED
      digitalWrite(redLEDforbutton, LOW);         // turn off red LED
      Serial.println("Button Pressed! Red LED ON.");
    }
    else // if the button was released
    {
      currentbutton = 1;
      previousbuttonMillis = currentbuttonMillis; // Store the time when the button was released
      digitalWrite(greenLEDforbutton, LOW);       // turn off green LED
      digitalWrite(redLEDforbutton, HIGH);        // turn on red LED
      Serial.println("Button Released! Green LED ON.");
    }
  }
  //===============================================================

  //===============================================================
  // LIGHT SENSOR
  int lightValue = analogRead(lightSensor);                      // Read light sensor value(12bit,0-4095)
  float vout = (lightValue * 3.3) / 4096;                        // convert to voltage (0 - 3.3V)
  float lightresistance = ((1000 * 3.3) / (vout / 2.35)) - 1000; // gain 2.35, use 1k ohm with lightsensor(voltage divider)
  float lux = pow((lightresistance / 80077.0), (1 / -0.761));    // calculate lux value based on the charactersitics of the light sensor
  Serial.print("Lux Value: ");
  Serial.println(lux);
  //===============================================================

  //===============================================================
  // MOTION SENSOR AND LED FLASHING
  int motionValue = digitalRead(motionSensor); // Read motion sensor value
  Serial.print("Motion Sensor Value: ");
  Serial.println(motionValue);

  // Motion detected (PIR goes HIGH)
  if (motionValue == HIGH)
  {
    lastMotionTime = millis();
    // If motion is detected, update the last motion time

    // if no flashing atm, start flashing
    if (!flashing)
    {
      if (lux > luxThreshold)
      { // Bright condition
        // If the light level is above the threshold, flash GREEN LED
        flashing = true;
        currentLED = 0;
        Serial.println("Motion Detected! Flashing GREEN LED.");
      }
      else
      { // Dark condition
        // If the light level is below the threshold, flash RED LED
        flashing = true;
        currentLED = 1;
        Serial.println("Motion Detected! Flashing RED LED.");
      }
    }
  }

  // If in flashing state
  if (flashing)
  {
    currentMillis = millis(); // Get current time

    // Check for light condition changes during flashing
    if (lux > luxThreshold && currentLED == 1)
    {
      // If the light level is above the threshold and the current LED is RED, switch to GREEN
      Serial.println("Light turned ON during flashing. Switching to GREEN.");
      currentLED = 0;
    }
    else if (lux <= luxThreshold && currentLED == 0)
    {
      // If the light level is below the threshold and the current LED is GREEN, switch to RED
      Serial.println("Light turned OFF during flashing. Switching to RED.");
      currentLED = 1;
    }

    // Blink the appropriate LED every 500ms
    if (currentMillis - previousMillis >= 500)
    {
      previousMillis = currentMillis; // Update last action time to current time
      // to set a new reference point for the next cycle calculation
      // ex: If current time is 1200ms, next toggle occurs at 1700ms (1200+500)
      ledState = !ledState; // Toggle LED state

      // Control currently selected LED (GREEN/RED)
      if (currentLED == 0) // When GREEN LED is selected
      {
        digitalWrite(greenLED, ledState); // Blink only GREEN
        digitalWrite(redLED, HIGH);       // Keep RED OFF
      }
      else if (currentLED == 1) // When RED LED is selected
      {
        digitalWrite(redLED, ledState); // Blink only RED
        digitalWrite(greenLED, HIGH);   // Keep GREEN OFF
      }
    }

    // Timeout check - if no motion for 5 seconds(2.5s sensor + 2.5s delay), turn off
    if (currentMillis - lastMotionTime >= 2500)
    {
      flashing = false;
      digitalWrite(greenLED, HIGH); // Turn off green LED
      digitalWrite(redLED, HIGH);   // Turn off red LED
      currentLED = -1;              // Reset active LED state, -1 = No LED active, 0 = Green, 1 = Red
    }
  }
  else
  {
    digitalWrite(greenLED, HIGH); // Ensure green OFF
    digitalWrite(redLED, HIGH);   // Ensure red OFF
  }
  //===============================================================

  delay(50); // Small delay for system stability
}

//----------------------------------------------------
