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
    pinMode(LED_3_GREEN, OUTPUT);
    pinMode(LED_3_RED, OUTPUT);
    os_queue_create(&control_queue, sizeof(BluetoothMessage), 10, nullptr);
    os_queue_create(&lcd_message_queue, sizeof(LCD_Message), 10, nullptr);
    LCD::Setup();
    Bluetooth::Setup();
    led_1.update_LED(LED_STATE::OFF);
    led_2.update_LED(LED_STATE::OFF);
    led_3.update_LED(LED_STATE::OFF);
}

void loop()
{
    BluetoothMessage message;
    if (os_queue_take(control_queue, &message, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
    {
        Log.info("Got queue item: %d", (int)message.message_type);
        switch (message.message_type)
        {
        case BluetoothMessageId::CONNECT:
        {
            Log.info("In Connect");
            if (message.node_id == Node::SN1)
            {
                data_manager.SetConnectedSN1(true);
            }
            else if (message.node_id == Node::SN2)
            {
                Log.info("In SN2");
                Log.info("Done");
            }
            break;
        }
        case BluetoothMessageId::DISCONNECT:
        {
            Log.info("In Disconnect");
            Log.info("Node: %d", (int)message.node_id);
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
        case BluetoothMessageId::TEMPERATURE:
        {
            /* code */
            data_manager.SetTemperatureLevel(message.data_payload.word_data / 100.0);
            Log.info("Got Temperature: %.2f", data_manager.GetTemperatureLevel());
            break;
        }
        case BluetoothMessageId::LIGHT:
        {

            break;
        }
        case BluetoothMessageId::CALL_BTN:
        {
            if (message.node_id == Node::SN1)
            {
                data_manager.SetCallButtonActivatedSN1(true);
            }
            else if (message.node_id == Node::SN2)
            {
                if (message.data_payload.bool_data)
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
        case BluetoothMessageId::SOUND_CHANGE:
        {
            Log.info("Sound Change");
            bool value = message.data_payload.bool_data;
            data_manager.SetSoundDetected(value);
            if (value)
            {
                // Check If lights are on
                led_3.update_LED(LED_STATE::GREEN_SOLID);
            }
            else
            {
                //
                led_3.update_LED(LED_STATE::OFF);
            }
            break;
        }
        case BluetoothMessageId::POWER:
            break;
        case BluetoothMessageId::SECURITY:
        {
            Log.info("In security");
            if (message.node_id == Node::SN1)
            {
                data_manager.SetConnectedSN1(false);
            }
            else if (message.node_id == Node::SN2)
            {
                uint8_t *entered_password = message.data_payload.string_data;
                Log.info("Entered Password %s", entered_password);
                if (strncmp((char *)entered_password, "0123456", 6) == 0)
                {
                    Log.info("Correct Pin");
                    // Compare passwords
                    Bluetooth::SetPairingSuccess(true);
                    data_manager.SetConnectedSN2(true);
                    led_2.update_LED(LED_STATE::GREEN_SOLID);
                }
                else
                {
                    Log.info("Incorrect Pin, disconnecting");
                    Bluetooth::Disconnect(Node::SN2);
                }
            }
            break;
        }
        default:
            break;
        }
    }
}
