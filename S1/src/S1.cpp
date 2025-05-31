/*
 * Project test
 * Description:
 * Author:
 * Date:
 */
#include "Particle.h"
#include <stdio.h>
#include <math.h>
#include "BluetoothLE_SN1.h" 
#include "Constants.h"

SYSTEM_MODE(MANUAL);

SerialLogHandler logHandler(LOG_LEVEL_INFO);

// Pin Definitions 
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

// Motion sensor variables
unsigned long lastMotionTime = 0;
// LED flashing variables 
unsigned long previousLedBlinkMillis = 0;
const long interval = 500;
bool flashing = false;
bool ledState = LOW;
int currentLED = -1;

// Button state variables
int lastRawButtonState = LOW;
unsigned long lastButtonDebounceTime = 0;
const unsigned long BUTTON_DEBOUNCE_DELAY = 50;
volatile bool buttonToggleState = false;
int lastDebouncedButtonState = LOW;

// Global variables to store current values
float g_lux = 0.0;
int g_potenValue = 0;
int g_brightness = 0;
int g_motionValue = 0;

// --- Potentiometer filtering and LED control variables ---
const int NUM_POT_READINGS = 5;
int potReadings[NUM_POT_READINGS];
int potReadIndex = 0;
long potTotal = 0;
int potAverage = 0;
const int LED_CUTOFF_BRIGHTNESS = 20;

// --- Timing variables for serial output & data queuing ---
unsigned long currentTime = 0;

unsigned long lastPotPrintTime = 0;
const unsigned long POT_PRINT_INTERVAL = 1500;

unsigned long lastLuxPrintTime = 0;
const unsigned long LUX_PRINT_INTERVAL = 2000;
const unsigned long LUX_QUEUE_INTERVAL = 10000; // Queue Lux values ​​every second
unsigned long lastLuxQueueTime = 0;

unsigned long lastMotionValPrintTime = 0;
const unsigned long MOTION_VAL_PRINT_INTERVAL = 1000;

// --- Variables for calculating and queuing 30-second average g_brightness ---
uint32_t g_brightness_sum_30s = 0;
uint16_t g_brightness_sample_count_30s = 0;
unsigned long last_g_brightness_sample_time = 0;
const unsigned long G_BRIGHTNESS_SAMPLE_INTERVAL_MS = 100;

unsigned long last_avg_g_brightness_queue_time = 0;             // Change to queue_time
const unsigned long AVG_G_BRIGHTNESS_QUEUE_INTERVAL_MS = 10000; // Queue the average every 10 seconds
volatile uint8_t TargetLightlvl;
volatile bool controlled = false;


// --- SN1 LED1 (D5, D6) Control ---
Sn1Led1State currentSn1Led1State = Sn1Led1State::OFF;
bool sn1Led1FlashState = false;
Timer sn1Led1FlashTimer(500, []() {  // Blink callback at 500ms intervals
  sn1Led1FlashState = !sn1Led1FlashState;
});

// Function to actually control the LED 
void updateSn1Led1Display()
{
  // 먼저 모든 LED를 끈다
  digitalWrite(greenLEDforbutton, HIGH); 
  digitalWrite(redLEDforbutton, HIGH);  

  switch (currentSn1Led1State)
  {
  case Sn1Led1State::GREEN_SOLID:
    digitalWrite(greenLEDforbutton, LOW);
    break;
  case Sn1Led1State::GREEN_FLASHING:
    if (sn1Led1FlashState)
    {
      digitalWrite(greenLEDforbutton, LOW); 
    }
    break;
  case Sn1Led1State::RED_FLASHING:
    if (sn1Led1FlashState)
    {
      digitalWrite(redLEDforbutton, LOW); 
    }
    break;
  case Sn1Led1State::OFF:
  default:
    break;
  }
}
// Function to determine and change the LED state
void setSn1Led1State(Sn1Led1State newState)
{
  if (currentSn1Led1State != newState)
  {
    Sn1Led1State oldState = currentSn1Led1State;
    currentSn1Led1State = newState;

// Manage blink timer
    bool oldStateWasFlashing = (oldState == Sn1Led1State::GREEN_FLASHING || oldState == Sn1Led1State::RED_FLASHING);
    bool newStateIsFlashing = (newState == Sn1Led1State::GREEN_FLASHING || newState == Sn1Led1State::RED_FLASHING);

    if (newStateIsFlashing)
    {
      if (!oldStateWasFlashing || newState != oldState)
      {                            // Start a new blink or change the blink color
        sn1Led1FlashState = true;  // When the flashing starts, it starts in the on state.
        sn1Led1FlashTimer.start(); // Start or restart the timer (you can also use reset() if you don't want to change the period)
      }
    }
    else
    {
      if (oldStateWasFlashing)
      { // Get out of blinking state
        sn1Led1FlashTimer.stop();
      }
    }

    Log.info("SN1 LED1 State Changed: %d -> %d", (int)oldState, (int)newState);
    updateSn1Led1Display(); // Update display immediately
  }
  else
  {
    // The state is the same, but if it's blinking, the display needs to be updated
    if (newState == Sn1Led1State::GREEN_FLASHING || newState == Sn1Led1State::RED_FLASHING)
    {
      updateSn1Led1Display();
    }
  }
}


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

  digitalWrite(greenLED, HIGH);
  digitalWrite(redLED, HIGH);

  for (int i = 0; i < NUM_POT_READINGS; i++)
  {
    potReadings[i] = 0;
  }

  Serial.begin(9600);
  Serial.println("System Initialized. Main loop starting.");

  // --- Initialize Bluetooth LE module ---
  BluetoothLE_SN1::setup(); 

  setSn1Led1State(Sn1Led1State::GREEN_FLASHING);
  // ------------------------
}

void loop()
{
  currentTime = millis();
// ---SN1 LED1 State Decision Logic ---
// This logic determines the LED state based on buttonToggleState and BLE.connected() state.
  if (buttonToggleState)
  { // Call button active state (true)
    //Currently there is no CN reception confirmation, so always RED_FLASHING
    setSn1Led1State(Sn1Led1State::RED_FLASHING);
  }
  else
  { //Call button disabled (false)
    if (BLE.connected())
    {
      setSn1Led1State(Sn1Led1State::GREEN_SOLID);
    }
    else
    {
      setSn1Led1State(Sn1Led1State::GREEN_FLASHING);
    }
  }
  //===============================================================
  // POTENTIOMETER AND LIGHT INTENSITY CONTROL
  if (!(controlled&& g_motionValue == HIGH))
  {
    potTotal = potTotal - potReadings[potReadIndex];
    potReadings[potReadIndex] = analogRead(potenPin);
    potTotal = potTotal + potReadings[potReadIndex];
    potReadIndex = (potReadIndex + 1) % NUM_POT_READINGS;
    potAverage = potTotal / NUM_POT_READINGS;
    g_potenValue = potAverage;
    int new_brightness = map(g_potenValue, 0, 4095, 0, 255);
    if (new_brightness < LED_CUTOFF_BRIGHTNESS)
    {
      new_brightness = 0;
    }

    // LED brightness applied immediately
    analogWrite(LEDPin, new_brightness);

    //Update g_brightness value and add to BLE queue when changed
    if (new_brightness != g_brightness)
    {
      g_brightness = new_brightness; // Global g_brightness update
    }
  }

  if (currentTime - lastPotPrintTime >= POT_PRINT_INTERVAL)
  {
    lastPotPrintTime = currentTime;
    Serial.print("Potentiometer (Avg): ");
    Serial.println(g_potenValue);
    Serial.print("Brightness(DutyCycle): ");
    Serial.println(g_brightness);
  }

  //===============================================================

  // --- Calculate 10 second average g_brightness and add to BLE queue ---
  if (currentTime - last_g_brightness_sample_time >= G_BRIGHTNESS_SAMPLE_INTERVAL_MS)
  {
    last_g_brightness_sample_time = currentTime;
    g_brightness_sum_30s += g_brightness; // Use the current g_brightness value
    g_brightness_sample_count_30s++;
  }

  if (currentTime - last_avg_g_brightness_queue_time >= AVG_G_BRIGHTNESS_QUEUE_INTERVAL_MS)
  {
    last_avg_g_brightness_queue_time = currentTime;
    if (g_brightness_sample_count_30s > 0)
    {
      uint8_t avg_brightness_for_ble = (uint8_t)(g_brightness_sum_30s / g_brightness_sample_count_30s);
      BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::AVERAGE_BRIGHTNESS, avg_brightness_for_ble);
      Serial.print("Queued 10s AVG LED Brightness: ");
      Serial.println(avg_brightness_for_ble);

      g_brightness_sum_30s = 0;
      g_brightness_sample_count_30s = 0;
    }
  }
  // ----------------------------------------------------

  //===============================================================
  // BUTTON STATE
  int rawButtonValue = digitalRead(pushbutton);
  if (rawButtonValue != lastRawButtonState)
  {
    lastButtonDebounceTime = currentTime;
  }

  if ((currentTime - lastButtonDebounceTime) >= BUTTON_DEBOUNCE_DELAY)
  {
    if (rawButtonValue == HIGH && lastDebouncedButtonState == LOW)
    {                                         // When the button is pressed and then released (Rising edge)
      buttonToggleState = !buttonToggleState; // Toggle status

      // Send button status via BLE
      uint8_t ble_button_state = buttonToggleState ? 1 : 0; 
      BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::BUTTON_STATE, ble_button_state);
      Serial.print("Button Toggled! Internal State: ");
      Serial.print(buttonToggleState ? "ACTIVATED" : "DEACTIVATED");
      Serial.print(", Queued for BLE: ");
      Serial.println(ble_button_state);
    }
    lastDebouncedButtonState = rawButtonValue;
  }
  lastRawButtonState = rawButtonValue;

  //===============================================================

  //===============================================================
  // LIGHT SENSOR
  int lightValue = analogRead(lightSensor);

  float vout = (lightValue * 3.3) / 4096.0;
  if (vout == 0)
    vout = 0.001;
  float lightresistance = ((1000.0 * 3.3) / (vout / 2.35)) - 1000.0;
  if (lightresistance <= 0)
    lightresistance = 1.0;
  g_lux = pow((lightresistance / 80077.0), (1.0 / -0.761));

  if (currentTime - lastLuxPrintTime >= LUX_PRINT_INTERVAL)
  {
    lastLuxPrintTime = currentTime;
    Serial.print("Lux Value: ");
    Serial.println(g_lux);
  }

  if (controlled && g_lux < TargetLightlvl && g_motionValue == HIGH)
  {
    Log.info("controlled is true increasing brightness");
    analogWrite(LEDPin, g_brightness++);
  }
  // --- Periodically add Lux ​​values ​​to the BLE queue ---
  if (currentTime - lastLuxQueueTime >= LUX_QUEUE_INTERVAL)
  {
    lastLuxQueueTime = currentTime;
    uint8_t lux_for_ble = (uint8_t)constrain(round(g_lux), 0, 255);
    BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::LUX_LEVEL, lux_for_ble);
  }
  //===============================================================

  //===============================================================
  // MOTION SENSOR AND LED FLASHING
  g_motionValue = digitalRead(motionSensor);
  if (currentTime - lastMotionValPrintTime >= MOTION_VAL_PRINT_INTERVAL)
  {
    lastMotionValPrintTime = currentTime;
    Serial.print("Motion Sensor Value (Raw): ");
    Serial.println(g_motionValue);
  }
  if (g_motionValue == HIGH)
  {
    lastMotionTime = currentTime;
    if (!flashing)
    {
      flashing = true;
      if (g_lux > luxThreshold)
      {
        currentLED = 0; // Green
        Serial.println("Motion Detected! Flashing GREEN LED.");
        BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::MOTION_DETECTED, 1); //
      }
      else
      {
        currentLED = 1; // Red
        Serial.println("Motion Detected! Flashing RED LED.");
      }
      previousLedBlinkMillis = currentTime - interval;
    }
  }
  if (flashing)
  {
    if (g_lux > luxThreshold && currentLED == 1)
    {
      Serial.println("Light turned ON during flashing. Switching to GREEN.");
      currentLED = 0;
    }
    else if (g_lux <= luxThreshold && currentLED == 0)
    {
      Serial.println("Light turned OFF during flashing. Switching to RED.");
      currentLED = 1;
    }
    if (currentTime - previousLedBlinkMillis >= interval)
    {
      previousLedBlinkMillis = currentTime;
      ledState = !ledState;
      if (currentLED == 0)
      {
        digitalWrite(greenLED, ledState);
        digitalWrite(redLED, HIGH);
      }
      else if (currentLED == 1)
      {
        digitalWrite(redLED, ledState);
        digitalWrite(greenLED, HIGH);
      }
    }
    if (currentTime - lastMotionTime >= 2500)
    {
      flashing = false;
      digitalWrite(greenLED, HIGH);
      digitalWrite(redLED, HIGH);
      currentLED = -1;
      ledState = LOW;
      BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::MOTION_DETECTED, 0); //
      Serial.println("Motion Timeout. Flashing stopped.");
    }
  }
  else
  {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, HIGH);
  }
  //===============================================================
}