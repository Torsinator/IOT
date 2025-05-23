/*
 * Project test
 * Description:
 * Author:
 * Date:
 */
#include "Particle.h"
// #include "dct.h" // If not used, comment out or delete
#include <stdio.h>
#include <math.h>

SYSTEM_MODE(MANUAL);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Pin Definitions (Same as before)
int motionSensor = D4;
int greenLED = A0;
int redLED = A1;
int lightSensor = A2;
int luxThreshold = 20;
int potenPin = A3;
int LEDPin = D2;
int redLEDforbutton = D6;
int greenLEDforbutton = D5;
int pushbutton = D3;

// Motion sensor variables (Same as before)
// bool previousMotionValue = LOW; // This variable is not directly used in the current code
unsigned long lastMotionTime = 0;
// LED flashing variables (Same as before)
// unsigned long currentMillis = 0; // Replaced by currentTime within loop
unsigned long previousLedBlinkMillis = 0; // Timer for LED blinking (clarified name)
const long interval = 500;        // Blink interval (500ms)
bool flashing = false;            // Flag to indicate if the LED is flashing
bool ledState = LOW;              // current state of blinking led (HIGH/LOW)
int currentLED = -1;              // which LED is flashing (0: green, 1: red, -1: none)

// Button state variables
// int debouncedButtonState = 0; // 0: released, 1: pressed (debounced state) // This was from a previous version, now handled by toggle logic
int lastRawButtonState = LOW; // Previous raw state of the button
unsigned long lastButtonDebounceTime = 0; // Last time for button debouncing
const unsigned long BUTTON_DEBOUNCE_DELAY = 50; // 50ms debounce time
bool buttonToggleState = false; // false: Green LED ON, true: Red LED ON
int lastDebouncedButtonState = LOW; // Previous debounced button state (for detecting press-release)

// Global variables to store current values (for use in other logic or LCD)
float g_lux = 0.0;
int g_potenValue = 0;         // Filtered potentiometer value
int g_brightness = 0;         // Final LED brightness
int g_motionValue = 0;

// --- Potentiometer filtering and LED control variables ---
const int NUM_POT_READINGS = 5; // Number of samples for moving average filter
int potReadings[NUM_POT_READINGS]; // Array to store potentiometer readings
int potReadIndex = 0;             // Current reading index
long potTotal = 0;                // Sum of samples
int potAverage = 0;               // Average potentiometer value

const int LED_CUTOFF_BRIGHTNESS = 20; // If brightness is below this value, turn LED OFF

// --- Timing variables for each serial output task ---
unsigned long currentTime = 0; // Updated at the start of the loop

unsigned long lastPotPrintTime = 0;
const unsigned long POT_PRINT_INTERVAL = 1500; // Print potentiometer value every 1.5 seconds

unsigned long lastLuxPrintTime = 0;
const unsigned long LUX_PRINT_INTERVAL = 2000; // Print Lux value every 2 seconds

unsigned long lastMotionValPrintTime = 0;
const unsigned long MOTION_VAL_PRINT_INTERVAL = 1000; // Print Motion Sensor Raw value every 1 second

void setup()
{
  pinMode(motionSensor, INPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(lightSensor, INPUT);
  pinMode(potenPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(pushbutton, INPUT_PULLDOWN);
  pinMode(redLEDforbutton, OUTPUT);
  pinMode(greenLEDforbutton, OUTPUT);

  digitalWrite(greenLED, HIGH);          // turn off the green LED at the beginning
  digitalWrite(redLED, HIGH);            // turn off the red LED at the beginning
  // Initial button LED state (e.g., Green ON if buttonToggleState is false)
  if (buttonToggleState) {
    digitalWrite(redLEDforbutton, LOW);    // Red ON
    digitalWrite(greenLEDforbutton, HIGH); // Green OFF
  } else {
    digitalWrite(redLEDforbutton, HIGH);   // Red OFF
    digitalWrite(greenLEDforbutton, LOW);  // Green ON
  }


  // Initialize potentiometer filter array
  for (int i = 0; i < NUM_POT_READINGS; i++) {
    potReadings[i] = 0;
  }

  Serial.begin(9600);                    // Initialize serial communication
  Serial.println("System Initialized. Serial outputs will be time-sliced. Button toggle & LED filter active.");
}

void loop()
{
  currentTime = millis(); // Update current time at the start of the loop

  //===============================================================
  // POTENTIOMETER AND LIGHT INTENSITY CONTROL (WITH FILTER AND CUTOFF)
  // 1. Apply moving average filter
  potTotal = potTotal - potReadings[potReadIndex]; // Subtract the oldest reading
  potReadings[potReadIndex] = analogRead(potenPin); // Read the new value and store it
  potTotal = potTotal + potReadings[potReadIndex];  // Add the new reading to the total
  potReadIndex = (potReadIndex + 1) % NUM_POT_READINGS; // Advance the index

  potAverage = potTotal / NUM_POT_READINGS; // Calculate the average
  g_potenValue = potAverage; // Update global variable (with filtered value)

  // 2. Calculate brightness
  g_brightness = map(g_potenValue, 0, 4095, 0, 255);

  // 3. Apply cutoff (turn off completely below a certain value)
  if (g_brightness < LED_CUTOFF_BRIGHTNESS) {
    g_brightness = 0; // Turn LED off completely
  }
  analogWrite(LEDPin, g_brightness);

  // Serial output (time-sliced)
  if (currentTime - lastPotPrintTime >= POT_PRINT_INTERVAL)
  {
    lastPotPrintTime = currentTime;
    Serial.print("Potentiometer (Avg): "); // Indicate that it's a filtered value
    Serial.println(g_potenValue);
    Serial.print("Brightness(DutyCycle): ");
    Serial.println(g_brightness);
  }
  //===============================================================

  //===============================================================
   // BUTTON STATE WITH TOGGLE AND IMPROVED DEBOUNCING
  int rawButtonValue = digitalRead(pushbutton);

  if (rawButtonValue != lastRawButtonState) { // If button state has changed
    lastButtonDebounceTime = currentTime; // Reset debounce timer
  }

  if ((currentTime - lastButtonDebounceTime) >= BUTTON_DEBOUNCE_DELAY) {
    // If the state has been stable for the debounce duration,
    // current debounced button state is rawButtonValue
    if (rawButtonValue == HIGH && lastDebouncedButtonState == LOW) {
      // Button "press" (Rising Edge) detected
      buttonToggleState = !buttonToggleState; // Toggle the state

      if (buttonToggleState) { // Toggled to Red LED ON state
        digitalWrite(greenLEDforbutton, HIGH); // Green OFF
        digitalWrite(redLEDforbutton, LOW);    // Red ON
        Serial.println("Button Toggled! Red LED ON.");
      } else { // Toggled to Green LED ON state
        digitalWrite(greenLEDforbutton, LOW);  // Green ON
        digitalWrite(redLEDforbutton, HIGH);   // Red OFF
        Serial.println("Button Toggled! Green LED ON.");
      }
    }
    lastDebouncedButtonState = rawButtonValue; // Store current debounced state for next edge detection
  }
  lastRawButtonState = rawButtonValue; // Store current raw state for next comparison
  //===============================================================

  //===============================================================
  // LIGHT SENSOR (Read value every time, print intermittently)
  int lightValue = analogRead(lightSensor);                      // Read light sensor value(12bit,0-4095)
  float vout = (lightValue * 3.3) / 4096.0;                        // convert to voltage (0 - 3.3V) - floating point math
  if (vout == 0) vout = 0.001; // Prevent division by zero
  float lightresistance = ((1000.0 * 3.3) / (vout / 2.35)) - 1000.0; // gain 2.35, use 1k ohm with lightsensor(voltage divider)
  if (lightresistance <=0) lightresistance = 1.0; // Prevent pow error
  g_lux = pow((lightresistance / 80077.0), (1.0 / -0.761));    // calculate lux value based on the characteristics of the light sensor

  if (currentTime - lastLuxPrintTime >= LUX_PRINT_INTERVAL)
  {
    lastLuxPrintTime = currentTime;
    Serial.print("Lux Value: ");
    Serial.println(g_lux);
  }
  //===============================================================

  //===============================================================
  // MOTION SENSOR AND LED FLASHING
  g_motionValue = digitalRead(motionSensor); // Read value every time

  if (currentTime - lastMotionValPrintTime >= MOTION_VAL_PRINT_INTERVAL)
  {
    lastMotionValPrintTime = currentTime;
    Serial.print("Motion Sensor Value (Raw): ");
    Serial.println(g_motionValue);
  }

  // Motion detected (PIR goes HIGH)
  if (g_motionValue == HIGH)
  {
    lastMotionTime = currentTime; // Update actual motion detection time
    // if no flashing atm, start flashing
    if (!flashing)
    {
      flashing = true;
      if (g_lux > luxThreshold) { // Bright condition
        currentLED = 0; // Green
        Serial.println("Motion Detected! Flashing GREEN LED.");
      } else { // Dark condition
        currentLED = 1; // Red
        Serial.println("Motion Detected! Flashing RED LED.");
      }
      // Adjust previousLedBlinkMillis so the first blink happens immediately
      previousLedBlinkMillis = currentTime - interval;
    }
  }

  // If in flashing state
  if (flashing)
  {
    // Check for light condition changes during flashing (print only on event)
    if (g_lux > luxThreshold && currentLED == 1) {
      Serial.println("Light turned ON during flashing. Switching to GREEN.");
      currentLED = 0;
    } else if (g_lux <= luxThreshold && currentLED == 0) {
      Serial.println("Light turned OFF during flashing. Switching to RED.");
      currentLED = 1;
    }

    // Blink the appropriate LED every 'interval'
    if (currentTime - previousLedBlinkMillis >= interval)
    {
      previousLedBlinkMillis = currentTime;
      ledState = !ledState; // Toggle LED state

      // Control currently selected LED (GREEN/RED)
      if (currentLED == 0) { // When GREEN LED is selected
        digitalWrite(greenLED, ledState); // Blink only GREEN
        digitalWrite(redLED, HIGH);       // Keep RED OFF
      } else if (currentLED == 1) { // When RED LED is selected
        digitalWrite(redLED, ledState); // Blink only RED
        digitalWrite(greenLED, HIGH);   // Keep GREEN OFF
      }
    }

    // Timeout check - if no motion for 2.5 seconds, turn off
    if (currentTime - lastMotionTime >= 2500)
    {
      flashing = false;
      digitalWrite(greenLED, HIGH); // Turn off green LED
      digitalWrite(redLED, HIGH);   // Turn off red LED
      currentLED = -1;              // Reset active LED state, -1 = No LED active, 0 = Green, 1 = Red
      ledState = LOW;               // Reset for next blink (since HIGH is OFF)
      Serial.println("Motion Timeout. Flashing stopped."); // Print on event
    }
  }
  else // Not flashing
  {
    digitalWrite(greenLED, HIGH); // Ensure green OFF
    digitalWrite(redLED, HIGH);   // Ensure red OFF
  }
  //===============================================================

  // delay(50); // This delay can be replaced by the intervals of each task,
               // or can be very short or removed.
  // If there are no other tasks, a very small delay(1) can be left
  // to prevent the CPU from looping too fast.
}