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
// #include "LCD.h" // LCD 

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
const long interval = 500;                // Blink interval (500ms)
bool flashing = false;                    // Flag to indicate if the LED is flashing
bool ledState = LOW;                      // current state of blinking led (HIGH/LOW)
int currentLED = -1;                      // which LED is flashing (0: green, 1: red, -1: none)

// Button state variables
// int debouncedButtonState = 0; // 0: released, 1: pressed (debounced state) // This was from a previous version, now handled by toggle logic
int lastRawButtonState = LOW;                   // Previous raw state of the button
unsigned long lastButtonDebounceTime = 0;       // Last time for button debouncing
const unsigned long BUTTON_DEBOUNCE_DELAY = 50; // 50ms debounce time
bool buttonToggleState = false;                 // false: Green LED ON, true: Red LED ON
int lastDebouncedButtonState = LOW;             // Previous debounced button state (for detecting press-release)

// Global variables to store current values (for use in other logic or LCD)
float g_lux = 0.0;
int g_potenValue = 0; // Filtered potentiometer value
int g_brightness = 0; // Final LED brightness
int g_motionValue = 0;

// --- Potentiometer filtering and LED control variables ---
const int NUM_POT_READINGS = 10;    // Number of samples for moving average filter
int potReadings[NUM_POT_READINGS]; // Array to store potentiometer readings
int potReadIndex = 0;              // Current reading index
long potTotal = 0;                 // Sum of samples
int potAverage = 0;                // Average potentiometer value

const int LED_CUTOFF_BRIGHTNESS = 20; // If brightness is below this value, turn LED OFF

// --- Hysteresis Thresholds for LED ---
const int LED_TURN_OFF_AT_BRIGHTNESS = 20; // LED가 켜져있을 때, 이 값 미만이면 꺼짐 -> translate to English ->
                                            // If brightness is below this value, turn LED OFF
const int LED_TURN_ON_AT_BRIGHTNESS  = 30; // LED가 꺼져있을 때, 이 값 이상이면 켜짐 (반드시 TURN_OFF_AT_BRIGHTNESS 보다 커야 함) -> translate to English ->
                                            // If brightness is above this value, turn LED ON (must be greater than TURN_OFF_AT_BRIGHTNESS)
                                            // 이 값은 LED가 켜져있을 때의 밝기보다 커야 함 (히스테리시스 적용)
                                            // This value must be greater than the brightness when the LED is ON (for hysteresis)


static bool whiteLedEffectivelyOn = false; // LED의 실제 ON/OFF 상태 추적 (히스테리시스용)
                                         // setup()에서 초기화 되도록 static으로 선언하거나, setup에서 false로 초기화

// --- Timing variables for each serial output task ---
unsigned long currentTime = 0; // Updated at the start of the loop

unsigned long lastPotPrintTime = 0;
const unsigned long POT_PRINT_INTERVAL = 1500; // Print potentiometer value every 1.5 seconds

unsigned long lastLuxPrintTime = 0;
const unsigned long LUX_PRINT_INTERVAL = 2000; // Print Lux value every 2 seconds

unsigned long lastMotionValPrintTime = 0;
const unsigned long MOTION_VAL_PRINT_INTERVAL = 1000; // Print Motion Sensor Raw value every 1 second


// --- Power Management Variables ---
unsigned long lastPowerCalcTime = 0;
const unsigned long POWER_CALC_INTERVAL = 30000; // 30초마다 계산

// 전력 상수 (단위: mW - 밀리와트) - 이 값들은 추정치이며, 실제 측정/데이터시트를 통해 개선해야 합니다.
const float P_MCU_AVG_MW = 50.0;       // 예: MCU 평균 활성 전력 (Particle Argon/Photon 활성 모드 추정치)
const float P_LED_MAX_MW = 60.0;       // 예: LED가 100% 밝기(255 PWM)일 때 소비 전력 (3.3V * 18mA 정도)
                                       // 이 값은 실제 LED와 저항에 따라 측정해야 합니다.
const float P_BLE_TX_AVG_MW = 5.0;     // 예: BLE 전송 시 평균 전력 (매우 짧은 시간 동안만 발생)
const float BLE_TX_DURATION_S = 0.01;  // 예: 한 번의 BLE 전송에 걸리는 시간 (초 단위, 예: 10ms)
int ble_transmissions_in_interval = 0; // 30초 간격 동안 BLE 전송 횟수 (나중에 업데이트)

// 누적 에너지 변수 (단위: mWh - 밀리와트시)
float g_totalSn1Energy_mWh = 0.0;
float g_mcuEnergy_mWh_interval = 0.0;
float g_ledEnergy_mWh_interval = 0.0;
float g_bleEnergy_mWh_interval = 0.0;

// LCD에 표시할 문자열 버퍼
char powerDisplayBuffer[17]; // "P: XXXX.X mWh"

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

  digitalWrite(greenLED, HIGH); // turn off the green LED at the beginning
  digitalWrite(redLED, HIGH);   // turn off the red LED at the beginning
  // Initial button LED state (e.g., Green ON if buttonToggleState is false)
  if (buttonToggleState)
  {
    digitalWrite(redLEDforbutton, LOW);    // Red ON
    digitalWrite(greenLEDforbutton, HIGH); // Green OFF
  }
  else
  {
    digitalWrite(redLEDforbutton, HIGH);  // Red OFF
    digitalWrite(greenLEDforbutton, LOW); // Green ON
  }

  // Initialize potentiometer filter array
  for (int i = 0; i < NUM_POT_READINGS; i++)
  {
    potReadings[i] = 0;
  }
whiteLedEffectivelyOn = false; // LED 초기 상태는 OFF로 가정
//===============================================================
  // LCD::setup_lcd_display();
//===============================================================
  Serial.begin(9600); // Initialize serial communication
  Serial.println("System Initialized. Serial outputs will be time-sliced. Button toggle & LED filter active.");
}

void calculateAndAccumulatePower() {
    // 1. MCU 전력 계산
    // MCU는 항상 켜져 있다고 가정 (단순화를 위해)
    // 에너지 (mWh) = 전력 (mW) * 시간 (h)
    // 시간 (h) = POWER_CALC_INTERVAL (ms) / (1000 ms/s * 3600 s/h)
    g_mcuEnergy_mWh_interval = P_MCU_AVG_MW * (POWER_CALC_INTERVAL / (1000.0 * 3600.0));

    // 2. LED 액추에이터 전력 계산
    // 현재 g_brightness (0-255) 값을 사용
    // LED가 꺼져있으면 (g_brightness가 매우 낮으면) 전력 소비 0으로 간주
    float currentLedPower_mW = 0.0;
    if (g_brightness > 0 && whiteLedEffectivelyOn) { // LED_CUTOFF_BRIGHTNESS 대신 whiteLedEffectivelyOn 사용
        // g_brightness에 비례하여 전력 소비한다고 가정 (선형 모델)
        currentLedPower_mW = (g_brightness / 255.0) * P_LED_MAX_MW;
    }
    g_ledEnergy_mWh_interval = currentLedPower_mW * (POWER_CALC_INTERVAL / (1000.0 * 3600.0));

    // 3. BLE 통신 전력 계산 (SN1 -> CN)
    // 이 부분은 Part 2에서 BLE 통신 코드가 추가될 때 더 정확해집니다.
    // 지금은 30초마다 N번 전송한다고 가정합니다.
    // 실제로는 BLE 연결 상태, 데이터 전송 빈도에 따라 ble_transmissions_in_interval 값을 업데이트해야 합니다.
    // 예시: 30초 동안 6번 전송한다고 가정 (5초마다 한 번씩)
    ble_transmissions_in_interval = 6; // << 이 값을 실제 통신 빈도에 맞게 조정해야 합니다.
                                       // 또는 실제 전송 이벤트 발생 시 카운트합니다.

    float totalBleTxTime_s_interval = ble_transmissions_in_interval * BLE_TX_DURATION_S;
    g_bleEnergy_mWh_interval = P_BLE_TX_AVG_MW * (totalBleTxTime_s_interval / 3600.0);


    // 4. 총 에너지 누적
    g_totalSn1Energy_mWh += g_mcuEnergy_mWh_interval + g_ledEnergy_mWh_interval + g_bleEnergy_mWh_interval;

    // 디버깅용 시리얼 출력
    Serial.println("--- Power Calculation (30s interval) ---");
    Serial.print("MCU Energy (interval): "); Serial.print(g_mcuEnergy_mWh_interval, 4); Serial.println(" mWh");
    Serial.print("LED Energy (interval): "); Serial.print(g_ledEnergy_mWh_interval, 4); Serial.println(" mWh");
    Serial.print("BLE Energy (interval): "); Serial.print(g_bleEnergy_mWh_interval, 4); Serial.println(" mWh");
    Serial.print("Total SN1 Energy (cumulative): "); Serial.print(g_totalSn1Energy_mWh, 2); Serial.println(" mWh");
    Serial.println("----------------------------------------");
}


void loop()
{

  //===============================================================
  // POWER MANAGEMENT CALCULATION (every 30 seconds)
  if (currentTime - lastPowerCalcTime >= POWER_CALC_INTERVAL)
  {
    lastPowerCalcTime = currentTime;
    calculateAndAccumulatePower();
    // 참고: 여기서 CN으로 전력 데이터를 전송하는 로직이 Part 2에서 추가될 수 있습니다.
  }
  //===============================================================

  currentTime = millis(); // Update current time at the start of the loop
  //===============================================================
  // POTENTIOMETER AND LIGHT INTENSITY CONTROL (WITH FILTER AND CUTOFF)
  // 1. Apply moving average filter
  potTotal = potTotal - potReadings[potReadIndex];      // Subtract the oldest reading
  potReadings[potReadIndex] = analogRead(potenPin);     // Read the new value and store it
  potTotal = potTotal + potReadings[potReadIndex];      // Add the new reading to the total
  potReadIndex = (potReadIndex + 1) % NUM_POT_READINGS; // Advance the index

  potAverage = potTotal / NUM_POT_READINGS; // Calculate the average
  g_potenValue = potAverage;                // Update global variable (with filtered value)

  // 2. Calculate initial brightness (before hysteresis)
  int mapped_brightness = map(g_potenValue, 0, 4095, 0, 255);

  // 3. Apply Hysteresis
  if (whiteLedEffectivelyOn) 
  {
    // LED가 현재 켜져 있는 상태라면
    if (mapped_brightness < LED_TURN_OFF_AT_BRIGHTNESS) 
    {
      g_brightness = 0; // LED를 끈다
      whiteLedEffectivelyOn = false;
    } 
    else 
    {
      g_brightness = mapped_brightness; // 계속 켜둔다 (밝기 업데이트)
    }
  } 
  else 
  {
    // LED가 현재 꺼져 있는 상태라면
    if (mapped_brightness >= LED_TURN_ON_AT_BRIGHTNESS) 
    {
      g_brightness = mapped_brightness; // LED를 켠다
      whiteLedEffectivelyOn = true;
    } 
    else 
    {
      g_brightness = 0; // 계속 꺼둔다
    }
  }
  
  // 이전 코드:
  // g_brightness = map(g_potenValue, 0, 4095, 0, 255);
  // if (g_brightness < LED_CUTOFF_BRIGHTNESS)
  // {
  //   g_brightness = 0; 
  // }
  
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

  if (rawButtonValue != lastRawButtonState)
  {                                       // If button state has changed
    lastButtonDebounceTime = currentTime; // Reset debounce timer
  }

  if ((currentTime - lastButtonDebounceTime) >= BUTTON_DEBOUNCE_DELAY)
  {
    // If the state has been stable for the debounce duration,
    // current debounced button state is rawButtonValue
    if (rawButtonValue == HIGH && lastDebouncedButtonState == LOW)
    {
      // Button "press" (Rising Edge) detected
      buttonToggleState = !buttonToggleState; // Toggle the state

      if (buttonToggleState)
      {                                        // Toggled to Red LED ON state
        digitalWrite(greenLEDforbutton, HIGH); // Green OFF
        digitalWrite(redLEDforbutton, LOW);    // Red ON
        Serial.println("Button Toggled! Red LED ON.");
      }
      else
      {                                       // Toggled to Green LED ON state
        digitalWrite(greenLEDforbutton, LOW); // Green ON
        digitalWrite(redLEDforbutton, HIGH);  // Red OFF
        Serial.println("Button Toggled! Green LED ON.");
      }
    }
    lastDebouncedButtonState = rawButtonValue; // Store current debounced state for next edge detection
  }
  lastRawButtonState = rawButtonValue; // Store current raw state for next comparison
  //===============================================================

  //===============================================================
  // LIGHT SENSOR (Read value every time, print intermittently)
  int lightValue = analogRead(lightSensor); // Read light sensor value(12bit,0-4095)
  float vout = (lightValue * 3.3) / 4096.0; // convert to voltage (0 - 3.3V) - floating point math
  if (vout == 0)
    vout = 0.001;                                                    // Prevent division by zero
  float lightresistance = ((1000.0 * 3.3) / (vout / 2.35)) - 1000.0; // gain 2.35, use 1k ohm with lightsensor(voltage divider)
  if (lightresistance <= 0)
    lightresistance = 1.0;                                  // Prevent pow error
  g_lux = pow((lightresistance / 80077.0), (1.0 / -0.761)); // calculate lux value based on the characteristics of the light sensor

  if (currentTime - lastLuxPrintTime >= LUX_PRINT_INTERVAL)
  {
    lastLuxPrintTime = currentTime;
    Serial.print("Lux Value: ");
    Serial.println(g_lux);
  }
  // //===============================================================
  // LCD::update_lcd_display();
  // //===============================================================
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
      if (g_lux > luxThreshold)
      {                 // Bright condition
        currentLED = 0; // Green
        Serial.println("Motion Detected! Flashing GREEN LED.");
      }
      else
      {                 // Dark condition
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

    // Blink the appropriate LED every 'interval'
    if (currentTime - previousLedBlinkMillis >= interval)
    {
      previousLedBlinkMillis = currentTime;
      ledState = !ledState; // Toggle LED state

      // Control currently selected LED (GREEN/RED)
      if (currentLED == 0)
      {                                   // When GREEN LED is selected
        digitalWrite(greenLED, ledState); // Blink only GREEN
        digitalWrite(redLED, HIGH);       // Keep RED OFF
      }
      else if (currentLED == 1)
      {                                 // When RED LED is selected
        digitalWrite(redLED, ledState); // Blink only RED
        digitalWrite(greenLED, HIGH);   // Keep GREEN OFF
      }
    }

    // Timeout check - if no motion for 2.5 seconds, turn off
    if (currentTime - lastMotionTime >= 2500)
    {
      flashing = false;
      digitalWrite(greenLED, HIGH);                        // Turn off green LED
      digitalWrite(redLED, HIGH);                          // Turn off red LED
      currentLED = -1;                                     // Reset active LED state, -1 = No LED active, 0 = Green, 1 = Red
      ledState = LOW;                                      // Reset for next blink (since HIGH is OFF)
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