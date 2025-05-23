#include "Particle.h"

#include "Constants.h"
#include "Structs.h"
#include "Bluetooth.h"
#include "DataManager.h"
#include "LED.h"

SYSTEM_MODE(MANUAL);

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

os_queue_t control_queue;

DataManager data_manager{};

LED led_1(LED_1_RED, LED_1_GREEN);
LED led_2(LED_2_RED, LED_2_GREEN);
LED led_3(LED_3_RED, LED_3_GREEN);

// void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);

void setup()
{
    pinMode(LED_2_GREEN, OUTPUT);
    pinMode(LED_2_RED, OUTPUT);
    os_queue_create(&control_queue, sizeof(BluetoothMessage), 10, nullptr);
    Bluetooth::Setup();
}

void loop()
{
    BluetoothMessage message;
    if (os_queue_take(control_queue, &message, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
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
            data_manager.SetTemperatureLevel((*(uint16_t*) message.data) / 100.0);
            Log.info("Got Temperature: %.2f", data_manager.GetTemperatureLevel());
            break;
        }
        case LIGHT:
        {

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
                if ((bool)*message.data)
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
            data_manager.SetSoundDetected((bool)*message.data);
            break;
        }
        case POWER:
            break;
        default:
            break;
        }
    }
}
