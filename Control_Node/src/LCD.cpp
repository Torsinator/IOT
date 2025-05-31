#include "LCD.h"
#include "LiquidCrystal_I2C_Spark.h"
#include "Particle.h"
#include <Wire.h>

#include "DataManager.h"
#include "Enumerations.h" 

extern DataManager data_manager;

LiquidCrystal_I2C lcd(0x27, 16, 2);



extern os_queue_t lcd_message_queue;

namespace LCD
{
    enum class LcdScreenType
    {
        ENVIRONMENT,      
        POWER_USAGE,      
        CONNECTION_STATUS 
    };

    // present condition 
    LcdScreenType currentScreen = LcdScreenType::ENVIRONMENT; // ini screen is env

    void cycleLcdScreen(); 

    // software timer for screen transition (Particle OS Timer)
    Timer screenCycleTimer(5000, cycleLcdScreen); // 5000ms call funtion

    void Setup()
    {
        new Thread("lcd_thread", LCD::processLcdQueueAndUpdate); // create thread to process queue
        Wire.begin();
        setup_lcd_display();
    }

    void cycleLcdScreen()
    {
        // next screen
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

        // send NEWSCREEN message to LCD update queue
        LCD_Message msg_type = LCD_Message::NEW_SCREEN; 
        os_queue_put(lcd_message_queue, &msg_type, 0, nullptr); // put message into queue (non-blocking)
    }

    void processLcdQueueAndUpdate()
    {
        while (true)
        {
            // Serial.println("LCD::processLcdQueueAndUpdate: Checking queue...");
            LCD_Message received_msg_type;

            int result = os_queue_take(lcd_message_queue, &received_msg_type, CONCURRENT_WAIT_FOREVER, nullptr); // 0ms timeout

            if (result == 0)
            { // if successfully bring message

                if (received_msg_type == LCD_Message::NEW_SCREEN)
                {
                    update_lcd_display_content();
                }
                else if (received_msg_type == LCD_Message::REFRESH)
                {
                    update_lcd_display_content();
                }
                else
                {
                    Serial.println("LCD::processLcdQueueAndUpdate: Received UNKNOWN message type from queue!");
                }
            }
            else if (result == ERR_TIMEOUT)
            {
              // Serial.println("LCD::processLcdQueueAndUpdate: Queue is empty (ERR_TIMEOUT)."); // 큐가 비었을 때 (너무 자주 찍히면 주석 처리)
            }
            else
            {
                Serial.print("LCD::processLcdQueueAndUpdate: Failed to take message from queue, error code: ");
                Serial.println(result); 
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
        case LcdScreenType::POWER_USAGE:
        {
            double powerSN1 = data_manager.GetPowerSN1();
            double powerSN2 = data_manager.GetPowerSN2();
            double powerCN = data_manager.GetPowerCN(); 
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
        lcd.init();
        lcd.backlight();
        LCD_Message initial_msg = LCD_Message::NEW_SCREEN;      
        os_queue_put(lcd_message_queue, &initial_msg, 0, nullptr); 
        screenCycleTimer.start();

        Serial.println("LCD setup_lcd_display: Initial screen displayed and cycle timer started.");
    }

} // namespace LCD
