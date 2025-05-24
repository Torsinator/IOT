#include "Particle.h"

#include "Constants.h"
#include "Structs.h"
#include "Bluetooth.h"
#include "DataManager.h"
#include "LED.h"
#include "LCD.h"
#include "Enumerations.h"

SYSTEM_MODE(MANUAL);

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

os_queue_t control_queue;

DataManager data_manager{};

LED led_1(LED_1_RED, LED_1_GREEN);
LED led_2(LED_2_RED, LED_2_GREEN);
LED led_3(LED_3_RED, LED_3_GREEN);

// void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
// Tae's edit
os_queue_t lcd_message_queue; // LCD.cpp의 선언과 동일해야 함

void setup()
{
    pinMode(LED_2_GREEN, OUTPUT);
    pinMode(LED_2_RED, OUTPUT);
    os_queue_create(&control_queue, sizeof(BluetoothMessage), 10, nullptr);
    os_queue_create(&lcd_message_queue, sizeof(LCD_Message), 10, nullptr);
    LCD::Setup();
    Bluetooth::Setup();
}

void loop()
{
    BluetoothMessage message;
    if (os_queue_take(control_queue, &message, CONCURRENT_WAIT_FOREVER, nullptr) == 0) // Tae modify
    {
        Log.info("Got queue item: %d", message.message_type);
        switch (message.message_type)
        {
        case CONNECT:
        {
            Log.info("In Connect");
            if (message.node_id == Node::SN1)
            {
                data_manager.SetConnectedSN1(true);
            }
            else if (message.node_id == Node::SN2)
            {
                Log.info("In SN2");
                data_manager.SetConnectedSN2(true);
                led_2.update_LED(LED_STATE::GREEN_SOLID);
                Log.info("Done");
            }
            break;
        }
        case DISCONNECT:
        {
            Log.info("In Disconnect");
            Log.info("Node: %d", message.node_id);
            if (message.node_id == Node::SN1)
            {
                data_manager.SetConnectedSN1(false);
            }
            else if (message.node_id == Node::SN2)
            {
                data_manager.SetConnectedSN2(false);
                led_2.update_LED(LED_STATE::OFF);
            }
            break;
        }
        case TEMPERATURE:
        {
            /* code */
            data_manager.SetTemperatureLevel((*(uint16_t *)message.data_payload.data) / 100.0);
            Log.info("Got Temperature: %.2f", data_manager.GetTemperatureLevel());
            break;
        }
        case LIGHT:
        {
            if (message.node_id == Node::SN1) // message.data 포인터는 이제 사용하지 않음
            {
                uint8_t lux_value_from_sn1 = message.data_payload.byte_data;
                Log.info("CtrlNode - Value in case LIGHT from message.value_data: %u", lux_value_from_sn1);

                data_manager.SetLightLevel((double)lux_value_from_sn1);
                // Log.info("SN1 Lux updated in DataManager: %u", lux_value_from_sn1); // 이 로그는 DataManager.SetLightLevel 내부 로그로 대체 가능
            }

            break;
        }
        case CALL_BTN:
        {
            if (message.node_id == Node::SN1)
            {
                data_manager.SetCallButtonActivatedSN1(true);
            }
            else if (message.node_id == Node::SN2)
            {
                if ((bool)*message.data_payload.data)
                {
                    data_manager.SetCallButtonActivatedSN2(true);
                    led_2.update_LED(LED_STATE::RED_FLASHING);
                }
                else
                {
                    data_manager.SetCallButtonActivatedSN2(false);
                    led_2.update_LED(LED_STATE::GREEN_SOLID);
                }
            }
            break;
        }
        case SOUND_CHANGE:
        {
            data_manager.SetSoundDetected((bool)*message.data_payload.data);
            break;
        }
        case POWER:
            break;
        default:
            break;
        }
    }
}
