#include "LCD.h"
// #include "application.h"
#include "LiquidCrystal_I2C_Spark.h"
#include "Particle.h"
#include <Wire.h>

#include "DataManager.h" 
#include "Enumerations.h" // LCD_Message enum이 있는 헤더 파일

// Control_Node.cpp (또는 메인 파일)에 정의된 DataManager 객체를 사용하기 위한 extern 선언
extern DataManager data_manager;

LiquidCrystal_I2C lcd(0x27, 16, 2);
// 화면 종류를 나타내는 enum (필요에 따라 추가/수정)

// Particle OS 큐 객체 선언
extern os_queue_t lcd_message_queue; // LCD 업데이트 메시지를 위한 큐

namespace LCD {

// 화면 종류를 나타내는 enum (필요에 따라 추가/수정)
enum class LcdScreenType {
    ENVIRONMENT, // 조도, 온도
    POWER_USAGE, // 전력 사용량
    CONNECTION_STATUS // 연결 상태
    // 추가적인 화면 타입들...
};

// 현재 표시 중인 화면 타입을 저장하는 변수
LcdScreenType currentScreen = LcdScreenType::ENVIRONMENT; // 초기 화면은 환경 정보

void cycleLcdScreen(); // <--- 이 줄을 추가해주세요!

// 화면 전환 주기를 위한 타이머 객체 (Particle OS Timer)
Timer screenCycleTimer(5000, cycleLcdScreen); // 5000ms = 5초마다 cycleLcdScreen 함수 호출
// 타이머에 의해 호출될 함수 (화면 전환 로직) - 수정

void Setup(){
    new Thread("lcd_thread", LCD::processLcdQueueAndUpdate); // 큐를 처리하는 스레드 생성
    Wire.begin();
    setup_lcd_display();
}

//-------------------------------
void cycleLcdScreen() {
    // 다음 화면으로 전환
    switch (currentScreen) {
        case LcdScreenType::ENVIRONMENT:
            currentScreen = LcdScreenType::POWER_USAGE;
            break;
        case LcdScreenType::POWER_USAGE:
            currentScreen = LcdScreenType::CONNECTION_STATUS;
            break;
        case LcdScreenType::CONNECTION_STATUS:
            currentScreen = LcdScreenType::ENVIRONMENT;
            break;
    }

    // LCD 업데이트 큐에 NEW_SCREEN 메시지 전송
    LCD_Message msg_type = LCD_Message::NEW_SCREEN; // enumeration.h의 enum 사용
    os_queue_put(lcd_message_queue, &msg_type, 0, nullptr); // 큐에 메시지 넣기 (non-blocking)

    Serial.print("LCD Screen cycled, sent NEW_SCREEN to queue. New screen: ");
    Serial.println((int)currentScreen);
}
// OLD CODE BELOW
// // 타이머에 의해 호출될 함수 (화면 전환 로직)
// void cycleLcdScreen() {
//     // 다음 화면으로 전환
//     switch (currentScreen) {
//         case LcdScreenType::ENVIRONMENT:
//             currentScreen = LcdScreenType::POWER_USAGE;
//             break;
//         case LcdScreenType::POWER_USAGE:
//             currentScreen = LcdScreenType::CONNECTION_STATUS;
//             break;
//         case LcdScreenType::CONNECTION_STATUS:
//             currentScreen = LcdScreenType::ENVIRONMENT; // 다시 처음으로
//             break;
//         // 다른 화면 타입이 있다면 여기에 case 추가
//     }

    
//     // 화면이 변경되었으므로 LCD 업데이트를 요청해야 함
//     // 지금은 직접 update_lcd_display()를 호출하거나,
//     // 나중에는 _LCD_Message_ 큐에 _NEW_SCREEN_ 메시지를 넣는 방식으로 변경할 수 있음
//     update_lcd_display(); // 즉시 업데이트 (또는 큐 사용)

//     Serial.print("LCD Screen cycled to: ");
//     // currentScreen enum 값을 문자열로 바꿔서 출력하면 더 좋음 (지금은 간단히 숫자)
//     Serial.println((int)currentScreen);
// }
//-------------------------------
//old
// // LCD 메시지 큐를 처리하고 화면을 업데이트하는 함수 (새로 추가 또는 기존 update_lcd_display 수정)
// void processLcdQueueAndUpdate() {
//     LCD_Message received_msg_type;

//     // 큐에서 메시지를 가져옴 (타임아웃 없이 즉시 반환 시도, 또는 짧은 타임아웃)
//     // os_queue_take는 blocking call이 될 수 있으므로, 별도 스레드에서 실행하거나
//     // loop에서 non-blocking 방식으로 확인하는 것이 좋음.
//     // 여기서는 간단하게 loop에서 호출될 것을 가정하고 non-blocking 시도.
//     if (os_queue_take(lcd_message_queue, &received_msg_type, 0, nullptr) == 0) { // 0ms timeout = non-blocking
//         Serial.print("LCD: Received message from queue: ");
//         if (received_msg_type == LCD_Message::NEW_SCREEN) {
//             Serial.println("NEW_SCREEN");
//             // currentScreen은 cycleLcdScreen에서 이미 변경되었음
//             update_lcd_display_content(); // 실제 LCD 출력 함수 호출
//         } else if (received_msg_type == LCD_Message::REFRESH) {
//             Serial.println("REFRESH");
//             // 현재 화면 내용만 새로고침
//             update_lcd_display_content(); // 실제 LCD 출력 함수 호출
//         }
//     }
//     // 큐가 비어있으면 아무것도 안 함
// }
// LCD.cpp의 LCD::processLcdQueueAndUpdate()
void processLcdQueueAndUpdate() {
    // Serial.println("LCD::processLcdQueueAndUpdate: Checking queue...");
    LCD_Message received_msg_type;

    int result = os_queue_take(lcd_message_queue, &received_msg_type, CONCURRENT_WAIT_FOREVER, nullptr); // 0ms timeout

    if (result == 0) { // 메시지를 성공적으로 가져온 경우
        Serial.print("LCD::processLcdQueueAndUpdate: Successfully took message from queue! Type: ");
        Serial.println((int)received_msg_type);

        if (received_msg_type == LCD_Message::NEW_SCREEN) {
            Serial.println("LCD::processLcdQueueAndUpdate: It's a NEW_SCREEN message. Updating display content.");
            update_lcd_display_content();
        } else if (received_msg_type == LCD_Message::REFRESH) {
            Serial.println("LCD::processLcdQueueAndUpdate: It's a REFRESH message. Updating display content.");
            update_lcd_display_content();
        } else {
            Serial.println("LCD::processLcdQueueAndUpdate: Received UNKNOWN message type from queue!");
        }
    } else if (result == ERR_TIMEOUT) { // <--- 여기를 ERR_TIMEOUT으로 수정!
        // Serial.println("LCD::processLcdQueueAndUpdate: Queue is empty (ERR_TIMEOUT)."); // 큐가 비었을 때 (너무 자주 찍히면 주석 처리)
    } else {
        Serial.print("LCD::processLcdQueueAndUpdate: Failed to take message from queue, error code: ");
        Serial.println(result); // 다른 오류 코드 확인
    }
}
//--------------------------------------------

void update_lcd_display_content() {
    lcd.clear();
    char buffer1[17];
    char buffer2[17];

    switch (currentScreen) {
        case LcdScreenType::ENVIRONMENT: {
            uint8_t lightLevel = data_manager.GetLightLevel();
            double temperature = data_manager.GetTemperatureLevel();
            sprintf(buffer1, "Light: %u Lux", lightLevel);
            sprintf(buffer2, "Temp: %.1fC", temperature);
            break;
        }
        case LcdScreenType::POWER_USAGE: {
            uint16_t powerSN1 = data_manager.GetPowerSN1();
            uint16_t powerSN2 = data_manager.GetPowerSN2();
            sprintf(buffer1, "P_SN1: %u mWh", powerSN1);
            sprintf(buffer2, "P_SN2: %u mWh", powerSN2);
            break;
        }
        case LcdScreenType::CONNECTION_STATUS: {
            bool connectedSN1 = data_manager.IsConnectedSN1();
            bool connectedSN2 = data_manager.IsConnectedSN2();
            sprintf(buffer1, "SN1: %s", connectedSN1 ? "Conn" : "Disconn");
            sprintf(buffer2, "SN2: %s", connectedSN2 ? "Conn" : "Disconn");
            break;
        }
    }
    lcd.setCursor(0, 0);
    lcd.print(buffer1);
    lcd.setCursor(0, 1);
    lcd.print(buffer2);
    Serial.println("LCD content updated.");
}

// old code
// // LCD 내용을 업데이트하는 주 함수
// void update_lcd_display() {
//     lcd.clear(); // 화면 전환 시 이전 내용 지우기

//     char buffer1[17];
//     char buffer2[17];

//     switch (currentScreen) {
//         case LcdScreenType::ENVIRONMENT: {
//             uint8_t lightLevel = data_manager.GetLightLevel();
//             double temperature = data_manager.GetTemperatureLevel();
//             sprintf(buffer1, "Light: %u Lux", lightLevel);
//             sprintf(buffer2, "Temp: %.1fC", temperature);
//             break;
//         }
//         case LcdScreenType::POWER_USAGE: {
//             uint16_t powerSN1 = data_manager.GetPowerSN1();
//             uint16_t powerSN2 = data_manager.GetPowerSN2();
//             sprintf(buffer1, "P_SN1: %u mWh", powerSN1);
//             sprintf(buffer2, "P_SN2: %u mWh", powerSN2);
//             break;
//         }
//         case LcdScreenType::CONNECTION_STATUS: {
//             bool connectedSN1 = data_manager.IsConnectedSN1();
//             bool connectedSN2 = data_manager.IsConnectedSN2();
//             sprintf(buffer1, "SN1: %s", connectedSN1 ? "Conn" : "Disconn");
//             sprintf(buffer2, "SN2: %s", connectedSN2 ? "Conn" : "Disconn");
//             break;
//         }
//         // 다른 화면 타입에 대한 case 추가
//     }

//     lcd.setCursor(0, 0);
//     lcd.print(buffer1);
//     lcd.setCursor(0, 1);
//     lcd.print(buffer2);

//     Serial.println("LCD updated based on currentScreen.");
// }
//--------------------------------------------

void setup_lcd_display() {
    // Wire.begin(); // 메인 setup()에 있어야 함

    lcd.init();
    lcd.backlight();
    // lcd.clear(); // update_lcd_display에서 clear 하므로 여기서 안 해도 됨

    // // 초기 화면 표시
    // update_lcd_display(); // currentScreen의 초기값(ENVIRONMENT)에 따라 첫 화면 표시

    // 초기 화면 표시 (큐를 통해)
    LCD_Message initial_msg = LCD_Message::NEW_SCREEN; // 또는 REFRESH
    os_queue_put(lcd_message_queue, &initial_msg, 0, nullptr); // 초기 화면 표시 요청

    // 화면 순환 타이머 시작
    screenCycleTimer.start();

    Serial.println("LCD setup_lcd_display: Initial screen displayed and cycle timer started.");
}

// void setup_lcd_display() {
//     // Wire.begin(); // I2C 통신은 메인 setup()에서 한 번만 호출하는 것이 좋습니다.
//                   // 만약 메인 setup()에 없다면 여기에 임시로 둘 수 있습니다.
//                   // 하지만 Control_Node.cpp의 setup()에 Wire.begin()이 있는지 확인하세요.
//                   // 없다면 추가해야 합니다.

//     lcd.init();       // 'lcd' 객체 사용
//     lcd.backlight();
//     lcd.clear();

//     // DataManager에서 데이터 가져오기 (예: Light Level)
//     uint8_t lightLevel = data_manager.GetLightLevel();

//     char buffer[17];
//     sprintf(buffer, "Light: %u", lightLevel);

//     lcd.setCursor(0, 0);
//     lcd.print(buffer);

//     // 다른 데이터도 표시 (예: Temperature)
//     double temperature = data_manager.GetTemperatureLevel();
//     sprintf(buffer, "Temp: %.1fC", temperature);

//     lcd.setCursor(0, 1);
//     lcd.print(buffer);

//     Serial.println("LCD setup_lcd_display: Displaying data from DataManager.");
// }

// // 나중에 주기적인 업데이트를 위해 만들 함수
// void update_lcd_display() {
//     // 여기에 주기적으로 DataManager에서 데이터를 가져와 LCD를 업데이트하는 로직 추가
// }

} // namespace LCD

// #include "Constants.h"
// I2C connection address 0x3F

// View logs with CLI using 'particle serial monitor --follow'
// Create the LCD object with I2C address 0x3F, 16 columns and 2 rows
// LiquidCrystal_I2C lcd(0x27, 16, 2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

//================================================================
// extern float g_lux;
// // LCD 설정
// // LiquidCrystal_I2C lcd(주소, 컬럼수, 로우수);
// // 일반적인 16x2 LCD의 경우:
// LiquidCrystal_I2C lcd(0x27, 16, 2); // 주소 0x27, 16글자 2줄

// // LCD 업데이트 타이밍 변수
// unsigned long lastLcdUpdateTime_lcd = 0;            // 변수명 충돌 방지
// const unsigned long LCD_UPDATE_INTERVAL_LCD = 1000; // 1초마다 LCD 업데이트
// // 만약 20x4 LCD라면:
// // LiquidCrystal_I2C lcd(0x27, 20, 4);
// namespace LCD {

// void setup_lcd_display()
// {
//     lcd.init();
//     lcd.backlight();
//     // 초기 LCD 화면 구성 (예: 고정 텍스트)
//     lcd.clear();
//     lcd.setCursor(0, 0);
//     lcd.print("Light Level:");
//     Serial.println("LCD::setup_lcd_display() called. LCD initialized.");
// }

// void update_lcd_display()
// {
//     unsigned long currentLcdTime = millis();

//     if (currentLcdTime - lastLcdUpdateTime_lcd >= LCD_UPDATE_INTERVAL_LCD)
//     {
//         lastLcdUpdateTime_lcd = currentLcdTime;

//         // g_lux 값을 LCD에 표시
//         char luxBuffer[17]; // 16자 + null
//         sprintf(luxBuffer, "%.1f lux", g_lux);

//         // 두 번째 줄에 Lux 값 표시 (이전 내용 지우고 새로 쓰기)
//         lcd.setCursor(0, 1);
//         for (int i = 0; i < 16; i++)
//         {
//             lcd.print(" ");
//         } // 줄 지우기
//         lcd.setCursor(0, 1);
//         lcd.print(luxBuffer);
//         Serial.print("LCD::update_lcd_display() - Lux: ");
//         Serial.println(g_lux); // Debugging
//         // 만약 다른 정보도 표시하고 싶다면 (예: 버튼 상태)
//         // lcd.setCursor(10, 0); // 위치 조정
//         // extern bool buttonToggleState; // 선언 필요
//         // if (buttonToggleState) { lcd.print("BTN:R"); }
//         // else { lcd.print("BTN:G"); }
//     }
// }
// }
//-----------------------------
// check the address of the LCD
// void setup() {
//   Wire.begin();
//   Serial.begin(9600);
//   delay(1000);
//   Serial.println("I2C Scanner Start");

//   for (byte address = 1; address < 127; address++) {
//     Wire.beginTransmission(address);
//     if (Wire.endTransmission() == 0) {
//       Serial.print("I2C device found at address 0x");
//       Serial.println(address, HEX);
//     }
//   }
// }

// void loop() {}
//-------------------------------
// // basic LCD test
// LiquidCrystal_I2C lcd(0x27, 16, 2);

// namespace LCD {
// void setup_lcd_display()
// {
//     lcd.init();
//     lcd.backlight();
//     lcd.clear();
//     lcd.print(" test");
//     // 초기 화면 구성 변경 없음 (또는 필요에 따라 수정)
// }

// void testlooping() {}
// }
// ---------------------------
// // 외부 변수 선언 (main .ino 파일에 정의된 변수들)
// extern float g_lux;
// extern float g_totalSn1Energy_mWh; // 새로 추가된 전력 변수

// LiquidCrystal_I2C lcd(0x27, 16, 2);

// unsigned long lastLcdUpdateTime_lcd = 0;
// const unsigned long LCD_UPDATE_INTERVAL_LCD = 1000; // 1초마다 LCD 업데이트

// namespace LCD {

// void setup_lcd_display()
// {
//     lcd.init();
//     lcd.backlight();
//     lcd.clear();
//     // 초기 화면 구성 변경 없음 (또는 필요에 따라 수정)
//     Serial.println("LCD::setup_lcd_display() called. LCD initialized.");
// }

// void update_lcd_display()
// {
//     unsigned long currentLcdTime = millis();

//     if (currentLcdTime - lastLcdUpdateTime_lcd >= LCD_UPDATE_INTERVAL_LCD)
//     {
//         lastLcdUpdateTime_lcd = currentLcdTime;

//         // 첫 번째 줄: Lux 값 표시
//         char luxBuffer[17];
//         sprintf(luxBuffer, "Lux: %.1f", g_lux);
//         lcd.setCursor(0, 0);
//         for (int i = 0; i < 16; i++) { lcd.print(" "); } // 줄 지우기
//         lcd.setCursor(0, 0);
//         lcd.print(luxBuffer);

//         // 두 번째 줄: 누적 전력 사용량 표시
//         char powerBuffer[17];
//         // g_totalSn1Energy_mWh 값을 mWh 단위로 표시
//         sprintf(powerBuffer, "Pwr: %.1fmWh", g_totalSn1Energy_mWh); // 소수점 한자리까지
//         lcd.setCursor(0, 1);
//         for (int i = 0; i < 16; i++) { lcd.print(" "); } // 줄 지우기
//         lcd.setCursor(0, 1);
//         lcd.print(powerBuffer);

//         Serial.print("LCD Update - Lux: "); Serial.print(g_lux);
//         Serial.print(" | Power: "); Serial.println(g_totalSn1Energy_mWh);
//     }
// }
// }

//---------------------------
