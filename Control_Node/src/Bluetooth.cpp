#include "Bluetooth.h"

#include <vector>
#include <mutex>
#include "Constants.h"
#include "Structs.h"

namespace Bluetooth
{
    BleScanResult scan_results[SCAN_RESULT_MAX];
    BluetoothConnection sensor_node_1;
    BluetoothConnection sensor_node_2;
    uint8_t num_connections = 0;
    os_queue_t sn1_disconnect_queue;
    os_queue_t sn2_disconnect_queue;

    BleCharacteristic call_button_characteristic_sn1;
    BleCharacteristic call_button_characteristic_sn2;
    BleCharacteristic temperature_measurement_characteristic;
    BleCharacteristic sound_characteristic;

    // Fan duty cycle updates
    BleCharacteristic fan_duty_cycle_characteristic(
        "fan_duty_cycle_override",
        BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
        CN_FAN_DUTY_CHAR_UUID, // Characteristic UUID as BleUuid
        CN_SERVICE_UUID        // Service UUID as BleUuid
    );

    bool updateDisplay = false;

    void Setup()
    {
        BLE.on();

#if SYSTEM_VERSION == SYSTEM_VERSION_v310
        // This is required with 3.1.0 only
        BLE.setScanPhy(BlePhy::BLE_PHYS_AUTO);
#endif
        BLE.addCharacteristic(fan_duty_cycle_characteristic);

        // Add characteristic handlers
        call_button_characteristic_sn1.onDataReceived(CallButtonSN1, NULL);
        call_button_characteristic_sn2.onDataReceived(CallButtonSN2, NULL);
        sound_characteristic.onDataReceived(SoundHandler, NULL);
        temperature_measurement_characteristic.onDataReceived(TemperatureHandler, NULL);

        BLE.onDisconnected(onDisconnectHandler);

        // Set UUIDs and values for the sensor nodes
        sensor_node_2.service_uuid = BleUuid(SN2_SERVICE_UUID);
        sensor_node_2.is_connected = false;
        // TODO: add SN2 UUID

        // Create disconnect queues
        os_queue_create(&sn1_disconnect_queue, sizeof(bool), 1, nullptr);
        os_queue_create(&sn2_disconnect_queue, sizeof(bool), 1, nullptr);

        // Create connection watchdog threads
        // new Thread("Bluetooth_SN1_Thread", SensorNode1Thread);
        new Thread("Bluetooth_SN2_Thread", SensorNode2Thread);

        // Connect to the sensor nodes
        bool disconnectFlag = true;
        os_queue_put(sn1_disconnect_queue, &disconnectFlag, 0, nullptr);
        os_queue_put(sn2_disconnect_queue, &disconnectFlag, 0, nullptr);

        BleAdvertisingData advData;
        advData.appendServiceUUID(BleUuid(CN_SERVICE_UUID));
        BLE.advertise(advData);
    }

    void SensorNode2Thread()
    {
        while (true)
        {
            bool queue_value;
            if (os_queue_take(sn2_disconnect_queue, &queue_value, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
            {
                if (queue_value)
                {
                    BluetoothMessage disconnect{Node::SN2, BluetoothMessageId::DISCONNECT, nullptr};
                    os_queue_put(control_queue, &disconnect, 0, nullptr);
                    while (true)
                    {
                        Log.info("Attempting to connect to sensor node 2");
                        if (Connect(sensor_node_2))
                        {
                            Log.info("Successfully connected to sensor node 2");
                            BluetoothMessage connect{Node::SN2, BluetoothMessageId::CONNECT, nullptr};
                            os_queue_put(control_queue, &connect, 0, nullptr);
                            break;
                        }
                        Log.error("Failed to connect to sensor node 2, retrying in 1s");
                        delay(1000);
                    }
                }
            }
        }
    }

    void SensorNode1Thread()
    {
        while (true)
        {
            bool queue_value;
            if (os_queue_take(sn1_disconnect_queue, &queue_value, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
            {
                if (queue_value)
                {
                    BluetoothMessage disconnect{Node::SN1, BluetoothMessageId::DISCONNECT, (uint8_t *)&queue_value};
                    os_queue_put(control_queue, &disconnect, 0, nullptr);
                    while (true)
                    {
                        Log.info("Attempting to connect to sensor node 1");
                        if (Connect(sensor_node_1))
                        {
                            Log.info("Successfully connected to sensor node 1");
                            break;
                        }
                        Log.error("Failed to connect to sensor node 1, retrying in 1s");
                        delay(1000);
                    }
                }
            }
        }
    }

    bool Connect(BluetoothConnection &connection)
    {
        BleScanResult scan_results[SCAN_RESULT_MAX];
        for (auto &result : BLE.scan())
        {
            Log.info("looking at result");
            for (auto &service : result.advertisingData().serviceUUID())
            {
                Log.info("Service UUID: %s", service.toString().c_str());
                if (service == connection.service_uuid)
                {
                    connection.device = BLE.connect(result.address());
                    connection.is_connected = true;
                    if (connection.device.getCharacteristicByUUID(call_button_characteristic_sn1, BleUuid(SN2_CALL_BTN_CHAR_UUID)))
                    {
                        Serial.println("Found call button characteristic");
                    }
                    if (connection.device.getCharacteristicByUUID(temperature_measurement_characteristic, BleUuid(SN2_TEMP_SENS_CHAR_UUID)))
                    {
                        Serial.println("Found temperature characteristic");
                    }
                    else
                    {
                        Serial.println("Failed to find characteristic");
                    }

                    Log.info("Successfully connected to device: %s", connection.service_uuid.toString().c_str());
                    return true;
                }
            }
        }
        return false;
    }

    bool Disconnect(BluetoothConnection &connection)
    {
        Log.info("Disconnecting from node");
        connection.device.disconnect();
        return true;
    }

    void onDisconnectHandler(const BlePeerDevice &peer)
    {
        if (peer == sensor_node_1.device)
        {
            Log.error("Bluetooth connection to sensor node 1 lost");
            sensor_node_1.is_connected = false;
            os_queue_put(sn1_disconnect_queue, (void *)sensor_node_1.is_connected, 0, nullptr);
        }
        else if (peer == sensor_node_2.device)
        {
            Log.error("Bluetooth connection to sensor node 2 lost");
            sensor_node_2.is_connected = false;
            os_queue_put(sn2_disconnect_queue, (void *)sensor_node_1.is_connected, 0, nullptr);
        }
        else
        {
            Log.error("Bluetooth connection to unknown device lost");
        }
    }

    void CallButtonSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        // Node node_id;
        // if (peer == sensor_node_1.device)
        // {
        //     node_id = SN1;
        // }
        // else if (peer == sensor_node_2.device)
        // {
        //     node_id = SN2;
        // }
        // else
        // {
        //     Log.error("Invalid peer in call button callback");
        //     return;
        // }
        BluetoothMessage message{Node::SN1, BluetoothMessageId::CALL_BTN, data};
        os_queue_put(control_queue, &message, 0, NULL);
    }

    void CallButtonSN2(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        // Node node_id;
        // if (peer == sensor_node_1.device)
        // {
        //     node_id = SN1;
        // }
        // else if (peer == sensor_node_2.device)
        // {
        //     node_id = SN2;
        // }
        // else
        // {
        //     Log.error("Invalid peer in call button callback");
        //     return;
        // }
        BluetoothMessage message{Node::SN2, BluetoothMessageId::CALL_BTN, data};
        os_queue_put(control_queue, &message, 0, NULL);
    }

    void SoundHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::SOUND_CHANGE, data};
        os_queue_put(control_queue, &message, 0, nullptr);
    }

    void TemperatureHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        BluetoothMessage message{Node::SN2, BluetoothMessageId::TEMPERATURE, data};
        Log.info("Got Temperature: %d", (uint8_t) *data);
        os_queue_put(control_queue, &message, 0, nullptr);
    }

} // namespace Bluetooth
