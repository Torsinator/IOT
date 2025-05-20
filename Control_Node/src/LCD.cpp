#include "LCD.h"
// #include "application.h"
#include "LiquidCrystal_I2C_Spark.h"
#include "Particle.h"
#include <Wire.h>
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
// basic LCD test
LiquidCrystal_I2C *lcd;

void lcdsetup() {
  Wire.begin();
  lcd = new LiquidCrystal_I2C(0x27, 16, 2); // 주소 바꿔야 할 수도 있음
  lcd->init();
  lcd->backlight();
  lcd->print("LCD Test!");
}

void loopinggg() {}

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