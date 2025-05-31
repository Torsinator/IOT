#include "Bluetooth.h"

#include <vector>
#include <mutex>
#include "Constants.h"
#include "Structs.h"
#include "Enumerations.h"

namespace Bluetooth
{
    // Create the characteristics. Both are tagged with the same service UUID.
    // Create characteristics using the updated constructor signature.
    BleCharacteristic temperature_reading_characteristic(
        "temperature_reading",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
        SN2_TEMP_SENS_CHAR_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID         // Service UUID as BleUuid
    );

    BleCharacteristic call_button_characteristic(
        "call_button",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::WRITE | BleCharacteristicProperty::INDICATE,
        SN2_CALL_BTN_CHAR_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID        // Service UUID as BleUuid
    );

    BleCharacteristic sound_characteristic(
        "sound_characteristic",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::INDICATE,
        SN2_SOUND_UUID,  // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID // Service UUID as BleUuid
    );

    BleCharacteristic fan_duty_characteristic(
        "fan_duty_cycle_override",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::WRITE | BleCharacteristicProperty::NOTIFY,
        SN2_FAN_DUTY_CHAR_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID        // Service UUID as BleUuid
    );

    BleCharacteristic light_on_off_characteristic(
        "fan_duty_cycle_override",
        BleCharacteristicProperty::WRITE,
        CN_LIGHT_ON_OFF_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID      // Service UUID as BleUuid
    );

    BleCharacteristic security_characterisic(
        "security_pin",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::WRITE | BleCharacteristicProperty::NOTIFY,
        CN_SECURITY_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID  // Service UUID as BleUuid
    );

    // BleCharacteristic fan_duty_characteristic;

    BluetoothConnection control_node_connection;

    os_queue_t connection_queue;

    void Setup()
    {
        BLE.on();

#if SYSTEM_VERSION == SYSTEM_VERSION_v310
        // This is required with 3.1.0 only
        BLE.setScanPhy(BlePhy::BLE_PHYS_AUTO);
#endif

        // Add characteristic handlers (for fan duty and call for help ack)
        fan_duty_characteristic.onDataReceived(DutyCycleHandler);

        BLE.setPairingIoCaps(BlePairingIoCaps::NONE);
        BLE.setPairingAlgorithm(BlePairingAlgorithm::LESC_ONLY);

        BLE.onPairingEvent(onPairingEvent);

        os_queue_create(&connection_queue, sizeof(bool), 1, nullptr);

        // Add the sensor service and attach both characteristics.
        BLE.addCharacteristic(temperature_reading_characteristic);
        BLE.addCharacteristic(call_button_characteristic);
        BLE.addCharacteristic(sound_characteristic);
        BLE.addCharacteristic(fan_duty_characteristic);
        BLE.addCharacteristic(light_on_off_characteristic);
        BLE.addCharacteristic(security_characterisic);

        light_on_off_characteristic.onDataReceived(LightOnOffHandler);
        security_characterisic.onDataReceived(PairingHandler);
        call_button_characteristic.onDataReceived(DeactivateCallButtonHandler);

        BLE.onConnected(onConnectHandler);
        BLE.onDisconnected(onDisconnectHandler);
        control_node_connection.service_uuid = CN_SERVICE_UUID;
        control_node_connection.is_connected = false;

        new Thread("Connection_Thread", ControlNodeConnectionThread);

        bool disconnectFlag = true;
        os_queue_put(connection_queue, &disconnectFlag, 0, nullptr);
    }

    void Advertise()
    {
        Log.info("Advertising");
        BleAdvertisingData advData;
        advData.appendServiceUUID(BleUuid(SN2_SERVICE_UUID));
        BLE.advertise(advData);
    }

    void ControlNodeConnectionThread()
    {
        while (true)
        {
            bool queue_value;
            if (os_queue_take(connection_queue, &queue_value, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
            {
                if (queue_value)
                {
                    BluetoothMessage disconnect{Node::SN2, BluetoothMessageId::DISCONNECT, NULL};
                    os_queue_put(main_queue, &disconnect, 0, nullptr);
                    // while (true)
                    // {
                    Advertise();
                    Log.info("Attempting to connect to control node");
                    // if (Connect(control_node_connection))
                    // {
                    //     Log.info("Successfully connected to control node");
                    //     BluetoothMessage connect{Node::SN2, BluetoothMessageId::CONNECT, nullptr};
                    //     os_queue_put(main_queue, &connect, 0, nullptr);
                    //     break;
                    // }
                    // Log.error("Failed to connect to control node, retrying in 1s");
                    // delay(1000);
                }
            }
        }
    }

    void SetPairingPasskey(const uint8_t *passkey)
    {
        if (control_node_connection.device.isValid())
        {
            Log.info("Setting passkey %s", passkey);
            security_characterisic.setValue(passkey, 6);
        }
    }

    bool Disconnect(BluetoothConnection &connection)
    {
        Log.info("Disconnecting from node");
        connection.device.disconnect();
        return true;
    }

    void onConnectHandler(const BlePeerDevice &peer)
    {
        control_node_connection.device = peer;
        BluetoothMessage message{Node::SN2, BluetoothMessageId::PAIRING, NULL};
        os_queue_put(main_queue, &message, 0, nullptr);
    }

    void onDisconnectHandler(const BlePeerDevice &peer)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::DISCONNECT, NULL};
        os_queue_put(connection_queue, &message, 0, nullptr);
    }

    void onPairingEvent(const BlePairingEvent &event, void *context)
    {
        if (event.type == BlePairingEventType::REQUEST_RECEIVED)
        {
            Log.info("onPairingEvent REQUEST_RECEIVED");
        }
        else if (event.type == BlePairingEventType::PASSKEY_DISPLAY)
        {
            char passKeyStr[BLE_PAIRING_PASSKEY_LEN + 1];
            memcpy(passKeyStr, event.payload.passkey, BLE_PAIRING_PASSKEY_LEN);
            passKeyStr[BLE_PAIRING_PASSKEY_LEN] = 0;

            Log.info("onPairingEvent PASSKEY_DISPLAY %s", passKeyStr);
        }
        else if (event.type == BlePairingEventType::STATUS_UPDATED)
        {
            Log.info("onPairingEvent STATUS_UPDATED status=%d lesc=%d bonded=%d",
                     event.payload.status.status,
                     (int)event.payload.status.lesc,
                     (int)event.payload.status.bonded);
        }
        else if (event.type == BlePairingEventType::NUMERIC_COMPARISON)
        {
            Log.info("onPairingEvent NUMERIC_COMPARISON");
        }
    }

    void SendTemperature(const uint16_t temperature)
    {
        temperature_reading_characteristic.setValue((uint8_t *)&temperature, sizeof(uint16_t));
    }

    void SendButtonPress(const bool value)
    {
        Log.info("Button press sent");
        call_button_characteristic.setValue((uint8_t *)&value, sizeof(uint8_t));
    }

    void SendSoundEvent(const bool value)
    {
        sound_characteristic.setValue(value);
    }

    void DutyCycleHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::FAN_DUTY};
        message.data_payload.fan_data = *(FanDutyCycleMessage*) data;
        os_queue_put(main_queue, &message, 0, nullptr);
    }

    void LightOnOffHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::LIGHT, *(bool *)data};
        os_queue_put(main_queue, &message, 0, nullptr);
    }

    void PairingHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        if ((bool)*data == true)
        {
            control_node_connection.is_connected = true;
            BluetoothMessage connect{Node::SN2, BluetoothMessageId::CONNECT, NULL};
            os_queue_put(main_queue, &connect, 0, nullptr);
        }
        else
        {
            Log.info("Incorrect Password");
        }
    }

    void SendDutyCycle(const uint8_t value)
    {
        Log.info("Sending duty cycle: %d", value);
        fan_duty_characteristic.setValue((uint8_t *)&value, sizeof(uint8_t));
    }

    void DeactivateCallButtonHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::CALL_BTN_DEACTIVATED, *(bool *)data};
        os_queue_put(main_queue, &message, 0, nullptr);
    }

} // namespace Bluetooth
