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
#include "SevenSeg.h"
#include "Security.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(MANUAL);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --folOUTPUT'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

os_queue_t main_queue;

os_queue_t pairing_queue;

uint8_t value = 0;

CallLED CALL_LED(CALL_LED_RED, CALL_LED_GREEN);
SoundLED sound_LED(SOUND_LED_RED, SOUND_LED_GREEN);

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
    pinMode(PIN_BTN, INPUT_PULLUP);
    pinMode(DJ_KNOB, INPUT);
    pinMode(SSEG_A, OUTPUT);
    pinMode(SSEG_B, OUTPUT);
    pinMode(SSEG_C, OUTPUT);
    pinMode(SSEG_D, OUTPUT);
    pinMode(SSEG_E, OUTPUT);
    pinMode(SSEG_F, OUTPUT);
    pinMode(SSEG_G, OUTPUT);
    pinMode(SSEG_DOT, OUTPUT);

    os_queue_create(&main_queue, 10, sizeof(BluetoothMessage), nullptr);
    os_queue_create(&pairing_queue, sizeof(PairingStatus), 2, nullptr);

    // Call setup function from each namespace
    Security::Setup();
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

void loop()
{
    BluetoothMessage message;
    if (os_queue_take(main_queue, &message, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
    {
        Log.info("Got queue item: %d", (uint8_t) message.message_type);
        switch (message.message_type)
        {
        case BluetoothMessageId::CONNECT:
        {
            Log.info("In Connect");
            CALL_LED.bluetooth_connection = true;
            CALL_LED.get_next_state();
            CALL_LED.update_LED();
            break;
        }
        case BluetoothMessageId::DISCONNECT:
        {
            Log.info("In Disconnect");
            Security::SetPairing(false);
            CALL_LED.bluetooth_connection = false;
            CALL_LED.get_next_state();
            CALL_LED.update_LED();
            break;
        }
        case BluetoothMessageId::LIGHT:
        {
            Log.info("Got Light Message");
            bool *light_status = (bool *)message.data;
            sound_LED.lights_on = *light_status;
            sound_LED.get_next_state();
            sound_LED.update_LED();
            break;
        }
        case BluetoothMessageId::CALL_BTN:
        {
            Log.info("Got Call Message");
            break;
        }
        case BluetoothMessageId::FAN_DUTY:
        {
            Log.info("Got Fan Duty Message");
            FanDutyCycleMessage *duty_message = (FanDutyCycleMessage *)message.data;
            Fan::SetDutyCycle(duty_message->duty_cycle);
            Fan::SetOverrideStatus(duty_message->controlled);
            break;
        }
        case BluetoothMessageId::PAIRING:
        {
            PairingStatus status = PairingStatus::PAIRING;
            os_queue_put(pairing_queue, &status, 0, nullptr);
            break;
        }
        default:
            break;
        }
    }
}