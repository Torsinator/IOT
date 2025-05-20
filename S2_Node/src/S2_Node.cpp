// ELEC4740 A1
// Tors Webster c3376513

// Include Particle Device OS APIs
#include "Particle.h"

// Include all sub functions
#include "Constants.h"
#include "Emergency.h"
#include "SoundSensor.h"
#include "FanControl.h"
#include "Temperature.h"
#include "CallLED.h"
#include "SoundLED.h"
#include "Structs.h"
#include "Bluetooth.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

os_queue_t main_queue;

uint8_t value = 0;

// setup() runs once, when the device is first turned on
void setup()
{
    // Initialize Pins
    pinMode(TEMP_SENS, INPUT);
    pinMode(SOUND_SENS, INPUT);
    pinMode(PUSH_BTN, INPUT_PULLUP); // Means no external resistor required
    pinMode(FAN_POT, INPUT);
    pinMode(FAN_OUT, OUTPUT);
    pinMode(FAN_SPEED, INPUT);
    pinMode(CALL_LED_GREEN, OUTPUT);
    pinMode(CALL_LED_RED, OUTPUT);
    pinMode(SOUND_LED_GREEN, OUTPUT);
    pinMode(SOUND_LED_RED, OUTPUT);
    pinMode(TEMP_SENS, INPUT);

    os_queue_create(&main_queue, 10, sizeof(BluetoothMessage), nullptr);

    // Call setup function from each namespace
    Emergency::Setup();
    SoundSensor::Setup();
    Fan::Setup();
    Temperature::Setup();
    Bluetooth::Setup();
}

bool Connect(BluetoothConnection &connection)
{
    BleScanResult scan_results[SCAN_RESULT_MAX];
    BLE.scan(scan_results, SCAN_RESULT_MAX);
    for (auto &result : scan_results)
    {
        for (auto &service : result.advertisingData().serviceUUID())
        {
            if (service == connection.service_uuid)
            {
                connection.device = BLE.connect(result.address());
                connection.is_connected = true;
                Log.info("Successfully connected to device: %s", connection.service_uuid.toString().c_str());
                return true;
            }
        }
    }
    return false;
}

// void loop()
// {
//     // Will use this thread to communicate with the control node in part 2
//     Bluetooth::SendTemperature(value);
//     value++;
//     Log.info("Sent Temp data %d", value);
//     delay(10000);
// }

void loop()
{
    BluetoothMessage message;
    if (os_queue_take(main_queue, &message, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
    {
        Log.info("Got queue item: %d", message.message_type);
        switch (message.message_type)
        {
        case CONNECT:
        {
            Log.info("In Connect");
            break;
        }
        case DISCONNECT:
        {
            Log.info("In Disconnect");
            break;
        }
        case LIGHT:
        {
            Log.info("Got Light Message");
            break;
        }
        case CALL_BTN:
        {
            Log.info("Got Call Message");
            break;
        }
        case FAN_DUTY:
        {
            Log.info("Got Fan Duty Message");
            FanDutyCycleMessage* duty_message = (FanDutyCycleMessage*) message.data;
            Fan::SetDutyCycle(duty_message->duty_cycle);
            Fan::SetOverrideStatus(duty_message->controlled);
            break;
        }
        default:
            break;
        }
    }
}