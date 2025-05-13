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
    os_queue_create(&control_queue, sizeof(BluetoothMessage), 10, nullptr);
    led_1 = LED(LED_1_RED, LED_1_GREEN);
    led_2 = LED(LED_2_RED, LED_2_GREEN);
    led_3 = LED(LED_3_RED, LED_3_GREEN);
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
                SensorNode1Data sn1_data = data_manager.GetSensorNode1Data();
                sn1_data.connected = true;
                data_manager.SetSensorNode1Data(sn1_data);
            }
            else if (message.node_id == Node::SN2)
            {
                Log.info("In SN2");
                SensorNode2Data sn2_data = data_manager.GetSensorNode2Data();
                sn2_data.connected = true;
                data_manager.SetSensorNode2Data(sn2_data);
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
                SensorNode1Data sn1_data = data_manager.GetSensorNode1Data();
                sn1_data.connected = false;
                data_manager.SetSensorNode1Data(sn1_data);
            }
            else if (message.node_id == Node::SN2)
            {
                SensorNode2Data sn2_data = data_manager.GetSensorNode2Data();
                sn2_data.connected = false;
                data_manager.SetSensorNode2Data(sn2_data);
            }
            break;
        }
        case TEMPERATURE:
        {
            /* code */
            SensorNode2Data sn2_data = data_manager.GetSensorNode2Data();
            sn2_data.temperature_level = (uint8_t)*message.data;
            data_manager.SetSensorNode2Data(sn2_data);
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
                SensorNode1Data sn1_data = data_manager.GetSensorNode1Data();
                sn1_data.call_button_activated = true;
                data_manager.SetSensorNode1Data(sn1_data);
            }
            else if (message.node_id == Node::SN2)
            {
                SensorNode2Data sn2_data = data_manager.GetSensorNode2Data();
                sn2_data.call_button_activated = true;
                data_manager.SetSensorNode2Data(sn2_data);
            }
            break;
        }
        case SOUND_CHANGE:
        {
            SensorNode2Data sn2_data = data_manager.GetSensorNode2Data();
            sn2_data.sound_detected = (bool)*message.data;
            data_manager.SetSensorNode2Data(sn2_data);
            break;
        }
        case POWER:
            break;
        default:
            break;
        }
    }
}

// void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
// {
//     uint8_t flags = data[0];

//     uint16_t rate;
//     if (flags & 0x01)
//     {
//         // Rate is 16 bits
//         memcpy(&rate, &data[1], sizeof(uint16_t));
//     }
//     else
//     {
//         // Rate is 8 bits (normal case)
//         rate = data[1];
//     }
//     if (rate != lastRate)
//     {
//         lastRate = rate;
//         updateDisplay = true;
//     }

//     Log.info("heart rate=%u", rate);
// }