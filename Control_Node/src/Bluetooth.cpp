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

    os_queue_t sn1_disconnect_queue;
    os_queue_t sn2_disconnect_queue;

    BleCharacteristic call_button_characteristic_sn1;
    BleCharacteristic call_button_characteristic_sn2;
    BleCharacteristic temperature_measurement_characteristic;
    BleCharacteristic sound_characteristic;
    BleCharacteristic light_on_off_characteristic;
    BleCharacteristic security_characteristic;

    BleCharacteristic lux_characteristic_sn1; // <--- SN1 lux characteristic

    // Fan duty cycle updates
    BleCharacteristic fan_duty_cycle_characteristic;
    //     "fan_duty_cycle_override",
    //     BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
    //     CN_FAN_DUTY_CHAR_UUID, // Characteristic UUID as BleUuid
    //     CN_SERVICE_UUID        // Service UUID as BleUuid
    // );

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
        security_characteristic.onDataReceived(SecurityHandler, NULL);

        lux_characteristic_sn1.onDataReceived(LuxHandlerSN1, NULL); // <--- SN1 lux callback

        BLE.setPairingIoCaps(BlePairingIoCaps::NONE);
        BLE.setPairingAlgorithm(BlePairingAlgorithm::LESC_ONLY);
        BLE.onPairingEvent(onPairingEvent);
        BLE.onDisconnected(onDisconnectHandler);

        // Set UUIDs and values for the sensor nodes
        sensor_node_2.service_uuid = BleUuid(SN2_SERVICE_UUID);
        sensor_node_2.is_connected = false;
        // TODO: add SN1 UUID
        sensor_node_1.service_uuid = BleUuid(SN1_SERVICE_UUID);
        sensor_node_1.is_connected = false;

        // Create disconnect queues
        os_queue_create(&sn1_disconnect_queue, sizeof(bool), 1, nullptr);
        os_queue_create(&sn2_disconnect_queue, sizeof(bool), 1, nullptr);

        // Create connection watchdog threads
        // new Thread("Bluetooth_SN1_Thread", SensorNode1Thread);
        new Thread("Bluetooth_SN2_Thread", SensorNode2Thread);
        new Thread("Bluetooth_SN1_Thread", SensorNode1Thread);

        // Connect to the sensor nodes
        bool disconnectFlag = true;
        os_queue_put(sn1_disconnect_queue, &disconnectFlag, 0, nullptr);
        os_queue_put(sn2_disconnect_queue, &disconnectFlag, 0, nullptr);
    }

    void Advertise()
    {
        // BleAdvertisingData advData;
        // advData.appendServiceUUID(BleUuid(CN_SERVICE_UUID));
        // BLE.advertise(advData);
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
                    BluetoothMessage disconnect{Node::SN2, BluetoothMessageId::DISCONNECT, (uint8_t *)&queue_value};
                    os_queue_put(control_queue, &disconnect, 0, nullptr);
                    Log.info("Advertising");
                    Advertise();
                    while (true)
                    {
                        Log.info("Attempting to connect to sensor node 2");
                        if (Connect(sensor_node_2))
                        {
                            Log.info("Successfully connected to sensor node 2");
                            BluetoothMessage connect{Node::SN2, BluetoothMessageId::CONNECT, NULL};
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
                            BluetoothMessage connect{Node::SN1, BluetoothMessageId::CONNECT, NULL};
                            os_queue_put(control_queue, &connect, 0, nullptr);
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

                    auto char_uuids = connection.device.discoverAllCharacteristics();
                    // Log.info("Connections %d", char_uuids.size());
                    // for (auto &i : char_uuids)
                    // {
                    //     Log.info(i.UUID().toString());
                    // }
                    // if (connection.device.getCharacteristicByUUID(call_button_characteristic_sn1, BleUuid(SN1_CALL_BTN_CHAR_UUID)))
                    // {
                    //     Serial.println("Found call button characteristic");
                    // }
                    if (connection.service_uuid == BleUuid(SN2_SERVICE_UUID))
                    {
                        Serial.println("YEP node found");
                        if (connection.device.getCharacteristicByUUID(temperature_measurement_characteristic, BleUuid(SN2_TEMP_SENS_CHAR_UUID)))
                        {
                            Serial.println("Found temperature characteristic");
                        }
                        if (connection.device.getCharacteristicByUUID(call_button_characteristic_sn2, BleUuid(SN2_CALL_BTN_CHAR_UUID)))
                        {
                            Serial.println("Found SN2 call button characteristic");
                        }
                        if (connection.device.getCharacteristicByUUID(light_on_off_characteristic, BleUuid(CN_LIGHT_INDICATOR_UUID)))
                        {
                            Serial.println("Found SN2 security characteristic");
                        }
                        if (connection.device.getCharacteristicByUUID(security_characteristic, BleUuid(CN_SECURITY_UUID)))
                        {
                            Serial.println("Found SN2 security characteristic");
                        }
                        if (connection.device.getCharacteristicByUUID(sound_characteristic, BleUuid(SN2_SOUND_UUID)))
                        {
                            Serial.println("Found SN2 sound characteristic");
                        }
                        else
                        {
                            Serial.println("TORS Failed to find characteristic");
                            // BLE.disconnect(connection.device);
                            return false;
                        }
                    }
                    else // 그 외의 경우 (즉, sensor_node_1, 본인 센서노드)
                    {
                        // 이 블록이 sensor_node_1에 해당합니다.
                        Log.info("Device is SN1. Discovering SN1 characteristics...");
                        // SN1_SERVICE_UUID로 연결되었으므로, SN1의 특성을 찾습니다.
                        // SN_PHOTON2_LUX_CHAR_UUID는 Constants.h에 정의된 SN1의 밝기 특성 UUID여야 합니다.
                        if (connection.device.getCharacteristicByUUID(lux_characteristic_sn1, BleUuid(SN1_LUX_CHAR_UUID)))
                        {
                            Serial.println("Found SN1 Lux characteristic");
                        }
                        else
                        {
                            Serial.println("ERROR: SN1 Lux characteristic NOT FOUND");
                            // connection.device.disconnect(); // 필요시 연결 해제
                            // connection.is_connected = false;
                            // return false;
                        }
                        // SN1에 버튼 기능이 있다면 여기서 추가
                        // if (connection.device.getCharacteristicByUUID(call_button_characteristic_sn1, BleUuid(SN1_CALL_BTN_CHAR_UUID))) {
                        //    Serial.println("Found SN1 call button characteristic");
                        // }
                    }
                    BLE.startPairing(connection.device);
                    Log.info("Successfully connected to device: %s", connection.service_uuid.toString().c_str());
                    return true;
                }
            }
        }
        return false;
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
        if (event.type == BlePairingEventType::PASSKEY_INPUT)
        {
            Log.info("Passkey input: Sent 123456");
            BLE.setPairingPasskey(event.peer, (const uint8_t *)"123456");
        }
    }

    bool Disconnect(const Node &node)
    {
        Log.info("Disconnecting from node");
        if (node == Node::SN1)
        {
            sensor_node_1.device.disconnect();
            return true;
        }
        else if (node == Node::SN2)
        {
            sensor_node_2.device.disconnect();
            bool val = true;
            os_queue_put(sn2_disconnect_queue, &val, 0, nullptr);
            return true;
        }
        return false;
    }

    void onDisconnectHandler(const BlePeerDevice &peer)
    {
        if (peer == sensor_node_1.device)
        {
            Log.error("Bluetooth connection to sensor node 1 lost");
            sensor_node_1.is_connected = false;
            bool val = true;
            os_queue_put(sn1_disconnect_queue, &val, 0, nullptr);
        }
        else if (peer == sensor_node_2.device)
        {
            Log.error("Bluetooth connection to sensor node 2 lost");
            sensor_node_2.is_connected = false;
            bool val = true;
            os_queue_put(sn2_disconnect_queue, &val, 0, nullptr);
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
        Log.info("Call button SN2");
        BluetoothMessage message{Node::SN2, BluetoothMessageId::CALL_BTN};
        message.data_payload.bool_data = *(bool *)data;
        os_queue_put(control_queue, &message, 0, NULL);
    }

    void SoundHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        Log.info("Sounds sensor");
        BluetoothMessage message{Node::SN2, BluetoothMessageId::SOUND_CHANGE};
        message.data_payload.bool_data = *(bool *)data;
        os_queue_put(control_queue, &message, 0, nullptr);
    }

    void TemperatureHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        Log.info("Temperature %d", *(uint16_t *)data);
        BluetoothMessage message{Node::SN2, BluetoothMessageId::TEMPERATURE};
        message.data_payload.word_data = *(uint16_t *)data;
        os_queue_put(control_queue, &message, 0, nullptr);
    }

    void SecurityHandler(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        Log.info("Security pin receieved %s", data);
        BluetoothMessage message{Node::SN2, BluetoothMessageId::SECURITY};
        memcpy(message.data_payload.string_data, data, 6);
        os_queue_put(control_queue, &message, 0, nullptr);
    }

    void LuxHandlerSN1(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context)
    {
        if (len > 0 && data != nullptr)
        {
            // // uint8_t received_lux_value = data[0];
            // // Log.info("SN1 Lux data received via BLE: %u", received_lux_value);
            // BluetoothMessage message{Node::SN1, BluetoothMessageId::LIGHT, data};
            // Log.info("SN1 Lux data received via BLE: %u", *message.data);
            // os_queue_put(control_queue, &message, 0, nullptr);

            uint8_t received_lux_value = data[0];
            Log.info("SN1 Lux data received via BLE: %u", received_lux_value);

            BluetoothMessage message;
            message.node_id = Node::SN1;
            message.message_type = BluetoothMessageId::LIGHT;
            // message.value_data = received_lux_value; // <<<< 값을 직접 복사!
            message.data_payload.byte_data = received_lux_value;
            os_queue_put(control_queue, &message, 0, nullptr);
        }
    }

    void SetPairingSuccess(bool success)
    {
        Log.info("Set Pairing Success");
        security_characteristic.setValue(success);
    }
} // namespace Bluetooth
