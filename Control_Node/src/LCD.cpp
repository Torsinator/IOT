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

namespace LCD
{

    // 화면 종류를 나타내는 enum (필요에 따라 추가/수정)
    enum class LcdScreenType
    {
        ENVIRONMENT,      // 조도, 온도
        POWER_USAGE,      // 전력 사용량
        CONNECTION_STATUS // 연결 상태
        // 추가적인 화면 타입들...
    };

    // 현재 표시 중인 화면 타입을 저장하는 변수
    LcdScreenType currentScreen = LcdScreenType::ENVIRONMENT; // 초기 화면은 환경 정보

    void cycleLcdScreen(); // <--- 이 줄을 추가해주세요!

    // 화면 전환 주기를 위한 타이머 객체 (Particle OS Timer)
    Timer screenCycleTimer(5000, cycleLcdScreen); // 5000ms = 5초마다 cycleLcdScreen 함수 호출
    // 타이머에 의해 호출될 함수 (화면 전환 로직) - 수정

    void Setup()
    {
        new Thread("lcd_thread", LCD::processLcdQueueAndUpdate); // 큐를 처리하는 스레드 생성
        Wire.begin();
        setup_lcd_display();
    }

    //-------------------------------
    void cycleLcdScreen()
    {
        // 다음 화면으로 전환
        switch (currentScreen)
        {
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
        LCD_Message msg_type = LCD_Message::NEW_SCREEN;         // enumeration.h의 enum 사용
        os_queue_put(lcd_message_queue, &msg_type, 0, nullptr); // 큐에 메시지 넣기 (non-blocking)
    }

    void processLcdQueueAndUpdate()
    {
        while (true)
        {
            // Serial.println("LCD::processLcdQueueAndUpdate: Checking queue...");
            LCD_Message received_msg_type;

            int result = os_queue_take(lcd_message_queue, &received_msg_type, CONCURRENT_WAIT_FOREVER, nullptr); // 0ms timeout

            if (result == 0)
            { // 메시지를 성공적으로 가져온 경우

                if (received_msg_type == LCD_Message::NEW_SCREEN)
                {
                    // Serial.println("LCD::processLcdQueueAndUpdate: It's a NEW_SCREEN message. Updating display content.");
                    update_lcd_display_content();
                }
                else if (received_msg_type == LCD_Message::REFRESH)
                {
                    // Serial.println("LCD::processLcdQueueAndUpdate: It's a REFRESH message. Updating display content.");
                    update_lcd_display_content();
                }
                else
                {
                    Serial.println("LCD::processLcdQueueAndUpdate: Received UNKNOWN message type from queue!");
                }
            }
            else if (result == ERR_TIMEOUT)
            { // <--- 여기를 ERR_TIMEOUT으로 수정!
              // Serial.println("LCD::processLcdQueueAndUpdate: Queue is empty (ERR_TIMEOUT)."); // 큐가 비었을 때 (너무 자주 찍히면 주석 처리)
            }
            else
            {
                Serial.print("LCD::processLcdQueueAndUpdate: Failed to take message from queue, error code: ");
                Serial.println(result); // 다른 오류 코드 확인
            }
        }
    }
    //--------------------------------------------

    void update_lcd_display_content()
    {
        lcd.clear();
        char buffer1[17];
        char buffer2[17];

        switch (currentScreen)
        {
        case LcdScreenType::ENVIRONMENT:
        {
            uint8_t lightLevel = data_manager.GetLightLevel();
            double temperature = data_manager.GetTemperatureLevel();
            sprintf(buffer1, "Light: %u Lux", lightLevel);
            sprintf(buffer2, "Temp: %.1fC", temperature);
            break;
        }
        // case LcdScreenType::POWER_USAGE:
        // {
        //     uint16_t powerSN1 = data_manager.GetPowerSN1();
        //     uint16_t powerSN2 = data_manager.GetPowerSN2();
        //     sprintf(buffer1, "P_SN1: %u mWh", powerSN1);
        //     sprintf(buffer2, "P_SN2: %u mWh", powerSN2);
        //     break;
        // }
        case LcdScreenType::POWER_USAGE:
        {
            // DataManager에서 float 값을 가져온다고 가정 (해결책 1 적용 시)
            // float powerSN1_mWh = data_manager.GetPowerSN1_float(); // 예시 함수명
            // float powerSN2_mWh = data_manager.GetPowerSN2_float();

            // 현재는 uint16_t로 되어 있으므로, 기존 방식대로
            double powerSN1 = data_manager.GetPowerSN1();
            double powerSN2 = data_manager.GetPowerSN2();
            double powerCN = data_manager.GetPowerCN(); // CN 전력량 가져오기

            // 만약 DataManager가 float을 반환한다면:
            // sprintf(buffer1, "P_SN1: %.1f mWh", powerSN1_mWh);
            // sprintf(buffer2, "P_SN2: %.1f mWh", powerSN2_mWh);

            // 현재 uint16_t 방식:
            // sprintf(buffer1, "P_SN1: %5.1f mWh", powerSN1);
            // sprintf(buffer2, "P_SN2: %5.1f mWh", powerSN2);
            snprintf(buffer1, sizeof(buffer2), "CN:%5.1f (mWh)", powerCN);

            snprintf(buffer2, sizeof(buffer1), "S1:%5.1fS2:%5.1f", powerSN1, powerSN2);

            break;
        }
        case LcdScreenType::CONNECTION_STATUS:
        {
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
    }

    void setup_lcd_display()
    {
        // Wire.begin(); // 메인 setup()에 있어야 함

        lcd.init();
        lcd.backlight();
        // lcd.clear(); // update_lcd_display에서 clear 하므로 여기서 안 해도 됨

        // // 초기 화면 표시
        // update_lcd_display(); // currentScreen의 초기값(ENVIRONMENT)에 따라 첫 화면 표시

        // 초기 화면 표시 (큐를 통해)
        LCD_Message initial_msg = LCD_Message::NEW_SCREEN;         // 또는 REFRESH
        os_queue_put(lcd_message_queue, &initial_msg, 0, nullptr); // 초기 화면 표시 요청

        // 화면 순환 타이머 시작
        screenCycleTimer.start();

        Serial.println("LCD setup_lcd_display: Initial screen displayed and cycle timer started.");
    }

} // namespace LCD
