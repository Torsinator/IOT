// BluetoothLE_SN1.cpp
#include "BluetoothLE_SN1.h"

// --- BLE UUIDs and Characteristics  ---
const char SN1_SERVICE_UUID_STR[] = "ea30000-eeb4-43c3-afef-6423cce071ae";
const char SN1_LUX_CHAR_UUID_STR[] = "ea30001-eeb4-43c3-afef-6423cce071ae";
const char SN1_POT_CHAR_UUID_STR[] = "ea30002-eeb4-43c3-afef-6423cce071ae";     
const char SN1_CALL_BTN_CHAR_UUID_STR[] = "ea30003-eeb4-43c3-afef-6423cce071ae"; 
const char SN1_MOV_CHAR_UUID_STR[] = "ea30004-eeb4-43c3-afef-6423cce071ae";      

BleCharacteristic luxCharacteristic(
    "sn1_lux_level",
    BleCharacteristicProperty::READ | BleCharacteristicProperty::WRITE | BleCharacteristicProperty::NOTIFY,
    SN1_LUX_CHAR_UUID_STR,
    SN1_SERVICE_UUID_STR);

BleCharacteristic potentiometerLedControlCharacteristic(
    "sn1_pot_led_ctrl",
    BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
    SN1_POT_CHAR_UUID_STR,
    SN1_SERVICE_UUID_STR);

BleCharacteristic callButtonCharacteristic(
    "sn1_call_btn",
    BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY | BleCharacteristicProperty::WRITE,
    SN1_CALL_BTN_CHAR_UUID_STR,
    SN1_SERVICE_UUID_STR);

BleCharacteristic MovementDetection(
    "sn1_movement_char",
    BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
    SN1_MOV_CHAR_UUID_STR,
    SN1_SERVICE_UUID_STR);
// -----------------------------------------------------------------

// --- data sending queue ---
os_queue_t ble_sn1_data_queue;
const size_t BLE_SN1_QUEUE_LENGTH = 15;
// -----------------------------

// --- BLE comm thread ---
Thread *ble_comm_thread = nullptr;
void bleCommunicationLoop(); 
// -----------------------

namespace BluetoothLE_SN1
{
    void LightlvlHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        LightlvlMessage message = *(LightlvlMessage *)data;

        TargetLightlvl = message.brightness;
        controlled = message.controlled; // Update whether control is available
        Log.info("LightlvlHandler called with data controlled: %d level %d", controlled, TargetLightlvl);
    }
    void setup()
    {
        os_queue_create(&ble_sn1_data_queue, sizeof(BleSn1DataPacket), BLE_SN1_QUEUE_LENGTH, nullptr); // 

        BLE.on();
        BLE.addCharacteristic(luxCharacteristic);
        BLE.addCharacteristic(potentiometerLedControlCharacteristic);
        BLE.addCharacteristic(callButtonCharacteristic);
        BLE.addCharacteristic(potentiometerLedControlCharacteristic);
        BLE.addCharacteristic(MovementDetection);
        luxCharacteristic.onDataReceived(LightlvlHandler);

        BleAdvertisingData advData;
        advData.appendServiceUUID(BleUuid(SN1_SERVICE_UUID_STR));
        BLE.advertise(&advData);

        // Creating and starting threads (modifying priority constants)
        ble_comm_thread = new Thread("sn1_ble_comm", bleCommunicationLoop, OS_THREAD_PRIORITY_DEFAULT + 1);

        callButtonCharacteristic.onDataReceived([](const uint8_t *data, size_t len, const BlePeerDevice &peer)
                                                {
            if (len == 1)
            {
                uint8_t buttonState = *(bool*) data;
                Log.info("Received BUTTON_STATE: %d", buttonState);
                // Control LED based on button status
                if (buttonState == false)
                {
                    buttonToggleState = false; // Set the button to not pressed
                }
                else
                {
                    digitalWrite(D5, HIGH);
                    digitalWrite(D6, LOW);
                }
            } });

        Serial.println("BluetoothLE_SN1 module initialized and communication thread started.");
    }

    void queueDataForSend(Sn1DataType type, uint8_t value)
    {
        BleSn1DataPacket packet = {type, value};
        if (os_queue_put(ble_sn1_data_queue, &packet, 0, nullptr) != 0)
        {
            // Log.warn("Failed to put data into ble_sn1_data_queue");
        }
    }

} // namespace BluetoothLE_SN1

// --- Implementing BLE communication thread functions ---
void bleCommunicationLoop()
{
    Log.info("BLE Communication Thread Started.");
    BluetoothLE_SN1::BleSn1DataPacket packet_to_send;

    while (true)
    {
        if (os_queue_take(ble_sn1_data_queue, &packet_to_send, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
        {
            Log.info("Got data to send");
            if (BLE.connected())
            {
                switch (packet_to_send.type)
                { 
                case BluetoothLE_SN1::Sn1DataType::CURRENT_BRIGHTNESS:
                case BluetoothLE_SN1::Sn1DataType::AVERAGE_BRIGHTNESS:
                    potentiometerLedControlCharacteristic.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                    break;
                case BluetoothLE_SN1::Sn1DataType::LUX_LEVEL:
                    luxCharacteristic.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                    break;
                case BluetoothLE_SN1::Sn1DataType::BUTTON_STATE: 
                    callButtonCharacteristic.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                    Log.info("Sent BUTTON_STATE via BLE: %d", packet_to_send.value);
                    break;
                case BluetoothLE_SN1::Sn1DataType::MOTION_DETECTED:
                    MovementDetection.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                }
            }
        }
    }
}
// ---------------------------------