// BluetoothLE_SN1.cpp
#include "BluetoothLE_SN1.h"

// --- BLE UUIDs and Characteristics (S1.cpp에서 여기로 이동) ---
const char SN1_SERVICE_UUID_STR[] = "ea30000-eeb4-43c3-afef-6423cce071ae";
const char SN1_LUX_CHAR_UUID_STR[] = "ea30001-eeb4-43c3-afef-6423cce071ae";
const char SN1_POT_CHAR_UUID_STR[] = "ea30002-eeb4-43c3-afef-6423cce071ae";      // g_brightness (현재 및 평균) 전송용
const char SN1_CALL_BTN_CHAR_UUID_STR[] = "ea30003-eeb4-43c3-afef-6423cce071ae"; // <<< [추가]
const char SN1_MOV_CHAR_UUID_STR[] = "ea30004-eeb4-43c3-afef-6423cce071ae";      // <<< [추가]

BleCharacteristic luxCharacteristic(
    "sn1_lux_level",
    BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
    SN1_LUX_CHAR_UUID_STR,
    SN1_SERVICE_UUID_STR);

BleCharacteristic potentiometerLedControlCharacteristic(
    "sn1_pot_led_ctrl",
    BleCharacteristicProperty::READ | BleCharacteristicProperty::NOTIFY,
    SN1_POT_CHAR_UUID_STR,
    SN1_SERVICE_UUID_STR);

BleCharacteristic callButtonCharacteristic( // <<< [추가]
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

// --- 데이터 전송을 위한 큐 ---
os_queue_t ble_sn1_data_queue;
const size_t BLE_SN1_QUEUE_LENGTH = 15;
// -----------------------------

// --- BLE 통신 스레드 ---
Thread *ble_comm_thread = nullptr;
void bleCommunicationLoop(); // 스레드 함수 프로토타입
// -----------------------

namespace BluetoothLE_SN1
{

    void setup()
    {
        os_queue_create(&ble_sn1_data_queue, sizeof(BleSn1DataPacket), BLE_SN1_QUEUE_LENGTH, nullptr); // 네임스페이스 내부에서 BleSn1DataPacket 사용 가능

        BLE.on();
        BLE.addCharacteristic(luxCharacteristic);
        BLE.addCharacteristic(potentiometerLedControlCharacteristic);
        BLE.addCharacteristic(callButtonCharacteristic); // <<< [추가]
        BLE.addCharacteristic(potentiometerLedControlCharacteristic);
        BLE.addCharacteristic(MovementDetection);

        BleAdvertisingData advData;
        advData.appendServiceUUID(BleUuid(SN1_SERVICE_UUID_STR));
        BLE.advertise(&advData);

        // 스레드 생성 및 시작 (우선순위 상수 수정)
        ble_comm_thread = new Thread("sn1_ble_comm", bleCommunicationLoop, OS_THREAD_PRIORITY_DEFAULT + 1); // <--- 수정된 부분

        callButtonCharacteristic.onDataReceived([](const uint8_t *data, size_t len, const BlePeerDevice &peer)
                                                {
            if (len == 1)
            {
                uint8_t buttonState = data[0];
                Log.info("Received BUTTON_STATE: %d", buttonState);
                // 버튼 상태에 따라 LED 제어
                if (buttonState == 1)
                {
                    digitalWrite(D5, HIGH); //g
                    digitalWrite(D6, LOW); //r
                }
                else
                {
                    digitalWrite(D5, LOW);
                    digitalWrite(D6, HIGH);
                }
            } });

        Serial.println("BluetoothLE_SN1 module initialized and communication thread started.");
    }

    void queueDataForSend(Sn1DataType type, uint8_t value)
    {
        BleSn1DataPacket packet = {type, value}; // 네임스페이스 내부에서 BleSn1DataPacket 사용 가능
        if (os_queue_put(ble_sn1_data_queue, &packet, 0, nullptr) != 0)
        {
            // Log.warn("Failed to put data into ble_sn1_data_queue");
        }
    }

} // namespace BluetoothLE_SN1

// --- BLE 통신 스레드 함수 구현 ---
void bleCommunicationLoop()
{
    Log.info("BLE Communication Thread Started.");
    // BleSn1DataPacket을 사용할 때 네임스페이스 명시
    BluetoothLE_SN1::BleSn1DataPacket packet_to_send; // <--- 수정된 부분

    while (true)
    {
        // os_thread_yield();
        // packet_to_send 변수는 이미 위에서 올바르게 선언되었으므로, os_queue_take에서 문제 없음
        if (os_queue_take(ble_sn1_data_queue, &packet_to_send, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
        {
            Log.info("Got data to send");
            if (BLE.connected())
            {
                switch (packet_to_send.type)
                { // packet_to_send.type은 BluetoothLE_SN1::Sn1DataType enum 값
                case BluetoothLE_SN1::Sn1DataType::CURRENT_BRIGHTNESS:
                case BluetoothLE_SN1::Sn1DataType::AVERAGE_BRIGHTNESS:
                    potentiometerLedControlCharacteristic.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                    break;
                case BluetoothLE_SN1::Sn1DataType::LUX_LEVEL:
                    luxCharacteristic.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                    break;
                case BluetoothLE_SN1::Sn1DataType::BUTTON_STATE: // <<< [추가]
                    callButtonCharacteristic.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                    Log.info("Sent BUTTON_STATE via BLE: %d", packet_to_send.value);
                    break;
                case BluetoothLE_SN1::Sn1DataType::MOTION_DETECTED: // <<< [추가]
                    MovementDetection.setValue(&packet_to_send.value, sizeof(packet_to_send.value));
                }
            }
        }
    }
}
// ---------------------------------