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
        BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
        SN2_CALL_BTN_CHAR_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID        // Service UUID as BleUuid
    );

    BleCharacteristic power_characteristic(
        "power_characteristic",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
        SN2_TEMP_SENS_CHAR_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID         // Service UUID as BleUuid
    );

    BleCharacteristic sound_characteristic(
        "sound_characteristic",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::INDICATE,
        SN2_TEMP_SENS_CHAR_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID         // Service UUID as BleUuid
    );

    BleCharacteristic fan_duty_characteristic(
        "fan_duty_cycle_override",
        BleCharacteristicProperty::WRITE,
        CN_FAN_DUTY_CHAR_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID       // Service UUID as BleUuid
    );

    BleCharacteristic light_on_off_characteristic(
        "fan_duty_cycle_override",
        BleCharacteristicProperty::WRITE,
        CN_LIGHT_ON_OFF_UUID, // Characteristic UUID as BleUuid
        SN2_SERVICE_UUID      // Service UUID as BleUuid
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

        os_queue_create(&connection_queue, sizeof(bool), 1, nullptr);

        // Add the sensor service and attach both characteristics.
        BLE.addCharacteristic(temperature_reading_characteristic);
        BLE.addCharacteristic(call_button_characteristic);
        BLE.addCharacteristic(sound_characteristic);
        BLE.addCharacteristic(fan_duty_characteristic);
        BLE.addCharacteristic(light_on_off_characteristic);

        light_on_off_characteristic.onDataReceived(LightOnOffHandler);

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
                    BluetoothMessage disconnect{Node::SN2, BluetoothMessageId::DISCONNECT, nullptr};
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

    // bool Connect(BluetoothConnection &connection)
    // {
    //     BleScanResult scan_results[SCAN_RESULT_MAX];
    //     for (auto &result : BLE.scan())
    //     {
    //         Log.info("looking at result");
    //         for (auto &service : result.advertisingData().serviceUUID())
    //         {
    //             Log.info("Service UUID: %s", service.toString().c_str());
    //             if (service == connection.service_uuid)
    //             {
    //                 connection.device = BLE.connect(result.address());
    //                 connection.is_connected = true;
    //                 if (connection.device.getCharacteristicByUUID(fan_duty_characteristic, BleUuid(CN_FAN_DUTY_CHAR_UUID)))
    //                 {
    //                     Serial.println("Found fan duty characteristic");
    //                 }

    //                 Log.info("Successfully connected to device: %s", connection.service_uuid.toString().c_str());
    //                 return true;
    //             }
    //         }
    //     }
    //     return false;
    // }

    bool Disconnect(BluetoothConnection &connection)
    {
        Log.info("Disconnecting from node");
        connection.device.disconnect();
        return true;
    }

    void onConnectHandler(const BlePeerDevice &peer)
    {
        control_node_connection.is_connected = true;
        control_node_connection.device = peer;
        BluetoothMessage connect{Node::SN2, BluetoothMessageId::CONNECT, nullptr};
        os_queue_put(main_queue, &connect, 0, nullptr);
    }

    void onDisconnectHandler(const BlePeerDevice &peer)
    {
        os_queue_put(connection_queue, (void *)control_node_connection.is_connected, 0, nullptr);
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
        sound_characteristic.setValue((uint8_t *)&value, sizeof(uint8_t));
    }

    void DutyCycleHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::FAN_DUTY, data};
        os_queue_put(main_queue, &message, 0, nullptr);
    }

    void LightOnOffHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::LIGHT, data};
        os_queue_put(main_queue, &message, 0, nullptr);
    }

} // namespace Bluetooth
