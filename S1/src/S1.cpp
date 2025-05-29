/*
 * Project test
 * Description:
 * Author:
 * Date:
 */
#include "Particle.h"
#include <stdio.h>
#include <math.h>
#include "BluetoothLE_SN1.h" // 새로운 BLE 모듈 헤더 추가

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
unsigned long lastMotionTime = 0;
// LED flashing variables (Same as before)
unsigned long previousLedBlinkMillis = 0;
const long interval = 500;
bool flashing = false;
bool ledState = LOW;
int currentLED = -1;

// Button state variables
int lastRawButtonState = LOW;
unsigned long lastButtonDebounceTime = 0;
const unsigned long BUTTON_DEBOUNCE_DELAY = 50;
bool buttonToggleState = false;
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
const unsigned long LUX_QUEUE_INTERVAL = 10000; // 1초마다 Lux 값을 큐에 넣음
unsigned long lastLuxQueueTime = 0;

unsigned long lastMotionValPrintTime = 0;
const unsigned long MOTION_VAL_PRINT_INTERVAL = 1000;

// --- 30초 평균 g_brightness 계산 및 큐잉을 위한 변수 ---
uint32_t g_brightness_sum_30s = 0;
uint16_t g_brightness_sample_count_30s = 0;
unsigned long last_g_brightness_sample_time = 0;
const unsigned long G_BRIGHTNESS_SAMPLE_INTERVAL_MS = 100;

unsigned long last_avg_g_brightness_queue_time = 0;             // queue_time으로 변경
const unsigned long AVG_G_BRIGHTNESS_QUEUE_INTERVAL_MS = 10000; // 10초마다 평균을 큐에 넣음

// --- [추가 시작] SN1 LED1 (D5, D6) Control ---
enum class Sn1Led1State
{
  OFF,
  GREEN_FLASHING,
  GREEN_SOLID,
  RED_FLASHING
  // RED_SOLID (향후 CN 피드백 시 추가 가능)
};

Sn1Led1State currentSn1Led1State = Sn1Led1State::OFF;
bool sn1Led1FlashState = false;
Timer sn1Led1FlashTimer(500, []() { // 500ms 간격으로 점멸 콜백
  sn1Led1FlashState = !sn1Led1FlashState;
});

// LED를 실제로 제어하는 함수 (D5, D6가 HIGH로 켜진다고 가정)
void updateSn1Led1Display()
{
  // 먼저 모든 LED를 끈다
  digitalWrite(greenLEDforbutton, HIGH); // D5 HIGH  (꺼짐)
  digitalWrite(redLEDforbutton, HIGH);   // D6 HIGH  (꺼짐)

  switch (currentSn1Led1State)
  {
  case Sn1Led1State::GREEN_SOLID:
    digitalWrite(greenLEDforbutton, LOW); // D5 LOW (켜짐)
    break;
  case Sn1Led1State::GREEN_FLASHING:
    if (sn1Led1FlashState)
    {
      digitalWrite(greenLEDforbutton, LOW); // D5 LOW (켜짐)
    }
    break;
  case Sn1Led1State::RED_FLASHING:
    if (sn1Led1FlashState)
    {
      digitalWrite(redLEDforbutton, LOW); // D6 LOW (켜짐)
    }
    break;
  case Sn1Led1State::OFF:
  default:
    // 이미 위에서 모두 꺼짐
    break;
  }
}
// LED 상태를 결정하고 변경하는 함수
void setSn1Led1State(Sn1Led1State newState)
{
  if (currentSn1Led1State != newState)
  {
    Sn1Led1State oldState = currentSn1Led1State;
    currentSn1Led1State = newState;

    // 점멸 타이머 관리
    bool oldStateWasFlashing = (oldState == Sn1Led1State::GREEN_FLASHING || oldState == Sn1Led1State::RED_FLASHING);
    bool newStateIsFlashing = (newState == Sn1Led1State::GREEN_FLASHING || newState == Sn1Led1State::RED_FLASHING);

    if (newStateIsFlashing)
    {
      if (!oldStateWasFlashing || newState != oldState)
      {                            // 새로 점멸 시작 또는 점멸 색 변경
        sn1Led1FlashState = true;  // 점멸 시작 시 일단 켜진 상태로 시작
        sn1Led1FlashTimer.start(); // 타이머 시작 또는 재시작 (주기 변경 없다면 reset()도 가능)
      }
    }
    else
    {
      if (oldStateWasFlashing)
      { // 점멸 상태에서 벗어남
        sn1Led1FlashTimer.stop();
      }
    }

    Log.info("SN1 LED1 State Changed: %d -> %d", (int)oldState, (int)newState);
    updateSn1Led1Display(); // 즉시 디스플레이 업데이트
  }
  else
  {
    // 상태는 같지만, 점멸 중인 경우 디스플레이 업데이트 필요
    if (newState == Sn1Led1State::GREEN_FLASHING || newState == Sn1Led1State::RED_FLASHING)
    {
      updateSn1Led1Display();
    }
  }
}
// --- [추가 끝] SN1 LED1 (D5, D6) Control ---
// ----------------------------------------------------

// --- BLE 관련 전역 변수들은 BluetoothLE_SN1.cpp로 이동 ---
// const char SN1_SERVICE_UUID[] = ...
// BleCharacteristic ...

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
  // if (buttonToggleState)
  // {
  //   digitalWrite(redLEDforbutton, LOW);
  //   digitalWrite(greenLEDforbutton, HIGH);
  // }
  // else
  // {
  //   digitalWrite(redLEDforbutton, HIGH);
  //   digitalWrite(greenLEDforbutton, LOW);
  // }

  for (int i = 0; i < NUM_POT_READINGS; i++)
  {
    potReadings[i] = 0;
  }

  Serial.begin(9600);
  Serial.println("System Initialized. Main loop starting.");

  // --- Bluetooth LE 모듈 초기화 ---
  BluetoothLE_SN1::setup(); // BLE 모듈의 setup 함수 호출

  // [수정] 초기 LED 상태 설정: 시스템 켜짐, BLE 미연결 -> GREEN_FLASHING
  setSn1Led1State(Sn1Led1State::GREEN_FLASHING);
  // ------------------------
}

void loop()
{
  currentTime = millis();
  // --- [추가/수정] SN1 LED1 상태 결정 로직 ---
  // 이 로직은 buttonToggleState와 BLE.connected() 상태에 따라 LED 상태를 결정합니다.
  if (buttonToggleState)
  { // 호출 버튼 활성화 상태 (true)
    // 현재는 CN 수신 확인 없으므로 항상 RED_FLASHING
    setSn1Led1State(Sn1Led1State::RED_FLASHING);
  }
  else
  { // 호출 버튼 비활성화 상태 (false)
    if (BLE.connected())
    {
      setSn1Led1State(Sn1Led1State::GREEN_SOLID);
    }
    else
    {
      setSn1Led1State(Sn1Led1State::GREEN_FLASHING);
    }
  }
  // 점멸 상태일 때 주기적으로 디스플레이 업데이트 (setSn1Led1State에서 처리하도록 수정)
  // if (currentSn1Led1State == Sn1Led1State::GREEN_FLASHING || currentSn1Led1State == Sn1Led1State::RED_FLASHING) {
  //     updateSn1Led1Display();
  // }
  //===============================================================
  // POTENTIOMETER AND LIGHT INTENSITY CONTROL
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

  // LED 밝기 즉시 적용
  analogWrite(LEDPin, new_brightness);

  // g_brightness 값 업데이트 및 변경 시 BLE 큐에 추가
  if (new_brightness != g_brightness)
  {
    g_brightness = new_brightness; // 전역 g_brightness 업데이트
    // BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::CURRENT_BRIGHTNESS, (uint8_t)g_brightness);
    // Serial.print("Queued CURRENT_BRIGHTNESS: "); Serial.println(g_brightness); // 디버깅용
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

  // --- 10초 평균 g_brightness 계산 및 BLE 큐에 추가 ---
  if (currentTime - last_g_brightness_sample_time >= G_BRIGHTNESS_SAMPLE_INTERVAL_MS)
  {
    last_g_brightness_sample_time = currentTime;
    g_brightness_sum_30s += g_brightness; // 현재 g_brightness 값을 사용
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
   // BUTTON STATE (수정: BLE 전송 추가, 로컬 LED 직접 제어 제거)
  int rawButtonValue = digitalRead(pushbutton);
  if (rawButtonValue != lastRawButtonState) {
    lastButtonDebounceTime = currentTime;
  }

  if ((currentTime - lastButtonDebounceTime) >= BUTTON_DEBOUNCE_DELAY) {
    if (rawButtonValue == HIGH && lastDebouncedButtonState == LOW) { // 버튼이 눌렸다가 떼어졌을 때 (Rising edge)
      buttonToggleState = !buttonToggleState; // 상태 토글

      // [수정] BLE로 버튼 상태 전송
      uint8_t ble_button_state = buttonToggleState ? 1 : 0; // true는 1, false는 0
      BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::BUTTON_STATE, ble_button_state);
      Serial.print("Button Toggled! Internal State: "); Serial.print(buttonToggleState ? "ACTIVATED" : "DEACTIVATED");
      Serial.print(", Queued for BLE: "); Serial.println(ble_button_state);

      // [제거] 로컬 LED 직접 제어 (D5, D6). 이 로직은 loop 상단의 SN1 LED1 상태 결정 로직으로 통합됨.
      // if (buttonToggleState) {
      //   digitalWrite(greenLEDforbutton, HIGH);
      //   digitalWrite(redLEDforbutton, LOW);
      // } else {
      //   digitalWrite(greenLEDforbutton, LOW);
      //   digitalWrite(redLEDforbutton, HIGH);
      // }
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

  // --- 주기적으로 Lux 값을 BLE 큐에 추가 ---
  if (currentTime - lastLuxQueueTime >= LUX_QUEUE_INTERVAL)
  {
    lastLuxQueueTime = currentTime;
    uint8_t lux_for_ble = (uint8_t)constrain(round(g_lux), 0, 255);
    BluetoothLE_SN1::queueDataForSend(BluetoothLE_SN1::Sn1DataType::LUX_LEVEL, lux_for_ble);
    // Serial.print("Queued LUX_LEVEL: "); Serial.println(lux_for_ble); // 디버깅용
  }
  // -------------------------------------
  //===============================================================

  //===============================================================
  // MOTION SENSOR AND LED FLASHING
  // ... (기존 모션 센서 로직은 그대로 유지) ...
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
      Serial.println("Motion Timeout. Flashing stopped.");
    }
  }
  else
  {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, HIGH);
  }
  //===============================================================

  // loop() 마지막에 짧은 delay를 넣어 다른 스레드에게 실행 기회를 줄 수 있습니다.
  // delay(1); // 또는 os_thread_yield();
}