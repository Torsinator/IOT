#include "Particle.h"

#include "Constants.h"
#include "Structs.h"
#include "Bluetooth.h"
#include "DataManager.h"
#include "LED.h"
#include "LCD.h"
#include "Enumerations.h"
#include "PowerEstimator.h"

// particle serial monitor --follow
SYSTEM_MODE(MANUAL);

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

os_queue_t control_queue;

DataManager data_manager{};

LED led_1(LED_1_RED, LED_1_GREEN);
LED led_2(LED_2_RED, LED_2_GREEN);
LED led_3(LED_3_RED, LED_3_GREEN);

// void onDataReceived(const uint8_t *data, size_t len, const BlePeerDevice &peer, void *context);
// Tae's edit
os_queue_t lcd_message_queue;                                                       // LCD.cpp의 선언과 동일해야 함
Timer powerCalculationTimer(30000, PowerEstimator::calculateAndStorePowerUsage30s); // 30초마다 호출

void setup()
{
    pinMode(LED_2_GREEN, OUTPUT);
    pinMode(LED_2_RED, OUTPUT);
    pinMode(LED_1_GREEN, OUTPUT);
    pinMode(LED_1_RED, OUTPUT);
    pinMode(LED_3_GREEN, OUTPUT);
    pinMode(LED_3_RED, OUTPUT);
    os_queue_create(&control_queue, sizeof(BluetoothMessage), 10, nullptr);
    os_queue_create(&lcd_message_queue, sizeof(LCD_Message), 10, nullptr);
    LCD::Setup();
    Bluetooth::Setup();
    PowerEstimator::setup(data_manager); // PowerEstimator 초기화, DataManager 인스턴스 전달
    powerCalculationTimer.start(); // 30초 타이머 시작
    led_1.update_LED(LED_STATE::OFF);
    led_2.update_LED(LED_STATE::OFF);
    led_3.update_LED(LED_STATE::OFF);
}

void loop()
{
    BluetoothMessage message;
    if (os_queue_take(control_queue, &message, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
    {
        Log.info("Got queue item: %d", (int)message.message_type);
        switch (message.message_type)
        {
        case BluetoothMessageId::CONNECT:
        {
            Log.info("In Connect");
            if (message.node_id == Node::SN1)
            {
                data_manager.SetConnectedSN1(true);
                // [추가] SN1 연결 시 CN LED1 녹색 고정
                if (!data_manager.IsCallButtonActivatedSN1())
                { // 호출 버튼이 활성화 상태가 아닐 때만 녹색으로 변경
                    led_1.update_LED(LED_STATE::GREEN_SOLID);
                    Log.info("CN: LED1 set to GREEN_SOLID (SN1 Connected)");
                }
            }
            else if (message.node_id == Node::SN2)
            {
                Log.info("In SN2");
                Log.info("Done");
            }
            break;
        }
        case BluetoothMessageId::DISCONNECT:
        {
            Log.info("In Disconnect");
            Log.info("Node: %d", (int)message.node_id);
            if (message.node_id == Node::SN1)
            {
                data_manager.SetConnectedSN1(false);
                // [추가] SN1 연결 끊김 시 CN LED1 꺼짐 (또는 다른 상태, 여기서는 OFF로 가정)
                // 만약 호출 버튼이 활성화된 상태에서 연결이 끊겼다면, 빨간색 점멸을 유지할지 결정 필요.
                // 여기서는 일단 호출 상태와 관계없이 연결 끊김을 우선하여 LED OFF.
                led_1.update_LED(LED_STATE::OFF);
                Log.info("CN: LED1 set to OFF (SN1 Disconnected)");
            }
            else if (message.node_id == Node::SN2)
            {
                data_manager.SetConnectedSN2(false);
                led_2.update_LED(LED_STATE::OFF);
            }
            break;
        }
        case BluetoothMessageId::TEMPERATURE:
        {
            /* code */
            data_manager.SetTemperatureLevel(message.data_payload.word_data / 100.0);
            Log.info("Got Temperature: %.2f", data_manager.GetTemperatureLevel());
            break;
        }
        case BluetoothMessageId::LIGHT:
        {
            if (message.node_id == Node::SN1)
            {
                uint8_t lux_value_from_sn1 = message.data_payload.byte_data;
                Log.info("CtrlNode - Value in case LIGHT from message.value_data: %u", lux_value_from_sn1);

                data_manager.SetLightLevel((double)lux_value_from_sn1);
                // Log.info("SN1 Lux updated in DataManager: %u", lux_value_from_sn1); // 이 로그는 DataManager.SetLightLevel 내부 로그로 대체 가능
            }

            break;
        }
        case BluetoothMessageId::SN1_PWM_VALUE:
        {
            if (message.node_id == Node::SN1)
            {
                uint8_t pwm_value_from_sn1 = message.data_payload.byte_data;
                PowerEstimator::processSn1PwmValue(pwm_value_from_sn1); // PowerEstimator로 PWM 값 전달
            }
            break;
        }
        case BluetoothMessageId::SN2_PWM_VALUE: // <--- 이 케이스 추가 또는 수정
        {
            if (message.node_id == Node::SN2)
            {
                uint8_t pwm_value_from_sn2 = message.data_payload.byte_data; // 0-100 범위의 값
                Log.info("CtrlNode - SN2_PWM_VALUE (Fan Duty %%) from SN2: %u", pwm_value_from_sn2);
                PowerEstimator::processSn2PwmValue(pwm_value_from_sn2); // PowerEstimator로 SN2 PWM 값 전달

                // 팬 속도(PWM 값)를 DataManager에도 저장 (선택 사항, LCD 표시 등에 사용 가능)
                // DataManager의 SetFanSpeed는 uint16_t를 받으므로 캐스팅
                data_manager.SetFanSpeed(static_cast<uint16_t>(pwm_value_from_sn2));
                Log.info("CtrlNode - SN2 Fan Speed (PWM %%) set in DataManager: %u", pwm_value_from_sn2);
            }
            break;
        }
        case BluetoothMessageId::CALL_BTN:
        {
            Log.info("Call Button Pressed");
            if (message.node_id == Node::SN1)
            {
                if (message.data_payload.bool_data)
                {
                    data_manager.SetCallButtonActivatedSN1(true);
                    led_1.update_LED(LED_STATE::RED_FLASHING);
                }
                else
                {
                    data_manager.SetCallButtonActivatedSN1(false);
                    led_1.update_LED(LED_STATE::GREEN_SOLID);
                }
            }

            else if (message.node_id == Node::SN2)
            {
                if (message.data_payload.bool_data)
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
        case BluetoothMessageId::SOUND_CHANGE:
        {
            Log.info("Sound Change");
            bool value = message.data_payload.bool_data;
            data_manager.SetSoundDetected(value);
            if (value)
            {
                // Check If lights are on
                led_3.update_LED(LED_STATE::GREEN_SOLID);
            }
            else
            {
                //
                led_3.update_LED(LED_STATE::OFF);
            }
            break;
        }
        case BluetoothMessageId::POWER:
            break;
        case BluetoothMessageId::SECURITY:
        {
            Log.info("In security");
            if (message.node_id == Node::SN1)
            {
                data_manager.SetConnectedSN1(false);
            }
            else if (message.node_id == Node::SN2)
            {
                uint8_t *entered_password = message.data_payload.string_data;
                Log.info("Entered Password %s", entered_password);
                if (strncmp((char *)entered_password, "0123456", 6) == 0)
                {
                    Log.info("Correct Pin");
                    // Compare passwords
                    Bluetooth::SetPairingSuccess(true);
                    data_manager.SetConnectedSN2(true);
                    led_2.update_LED(LED_STATE::GREEN_SOLID);
                }
                else
                {
                    Log.info("Incorrect Pin, disconnecting");
                    Bluetooth::Disconnect(Node::SN2);
                }
            }
            break;
        }
        default:
            break;
        }
    }
}
