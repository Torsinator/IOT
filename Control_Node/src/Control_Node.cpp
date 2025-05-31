#include "Particle.h"

#include "Constants.h"
#include "Structs.h"
#include "Bluetooth.h"
#include "DataManager.h"
#include "LED.h"
#include "LCD.h"
#include "Enumerations.h"
#include "PowerEstimator.h"
#include "Cloud.h"

// particle serial monitor --follow
SYSTEM_MODE(AUTOMATIC);

SYSTEM_THREAD(ENABLED);

SerialLogHandler logHandler(LOG_LEVEL_TRACE);

os_queue_t control_queue;

DataManager data_manager{};

LED led_1(LED_1_RED, LED_1_GREEN);
LED led_2(LED_2_RED, LED_2_GREEN);
LED led_3(LED_3_RED, LED_3_GREEN);

std::atomic_bool debouncing = false;

Timer debounce_timer(500, []
                     { debouncing = false; }, true);
os_queue_t lcd_message_queue;                                                      
Timer powerCalculationTimer(30000, PowerEstimator::calculateAndStorePowerUsage30s); 

void CallAckBtnCallbackSN1()
{
    if (debouncing)
    {
        return;
    }
    debounce_timer.startFromISR();
    BluetoothMessage message{Node::SN1, BluetoothMessageId::CALL_BUTTON_OFF, true};
    os_queue_put(control_queue, &message, 0, nullptr);
}

void CallAckBtnCallbackSN2()
{
    if (debouncing)
    {
        return;
    }
    BluetoothMessage message{Node::SN2, BluetoothMessageId::CALL_BUTTON_OFF, true};
    os_queue_put(control_queue, &message, 0, nullptr);
}

void setup()
{
    pinMode(LED_2_GREEN, OUTPUT);
    pinMode(LED_2_RED, OUTPUT);
    pinMode(LED_1_GREEN, OUTPUT);
    pinMode(LED_1_RED, OUTPUT);
    pinMode(LED_3_GREEN, OUTPUT);
    pinMode(LED_3_RED, OUTPUT);
    pinMode(CALL_ACK_BTN_SN2, INPUT_PULLDOWN);
    pinMode(CALL_ACK_BTN_SN1, INPUT_PULLDOWN);
    os_queue_create(&control_queue, sizeof(BluetoothMessage), 10, nullptr);
    os_queue_create(&lcd_message_queue, sizeof(LCD_Message), 10, nullptr);
    LCD::Setup();
    Bluetooth::Setup();
    PowerEstimator::setup(data_manager); 
    powerCalculationTimer.start();      
    Cloud::Setup();
    attachInterrupt(CALL_ACK_BTN_SN1, CallAckBtnCallbackSN1, InterruptMode::RISING);
    attachInterrupt(CALL_ACK_BTN_SN2, CallAckBtnCallbackSN2, InterruptMode::RISING);
    led_1.update_LED(LED_STATE::OFF);
    led_2.update_LED(LED_STATE::OFF);
    led_3.update_LED(LED_STATE::OFF);
    data_manager.SetTemperatureLightsOff(100);
    data_manager.SetTemperatureLightsOn(100);
}

void loop()
{
    Particle.process(); 

    BluetoothMessage message;
    Log.trace("Waiting for queue");
    if (os_queue_take(control_queue, &message, CONCURRENT_WAIT_FOREVER, nullptr) == 0)
    {
        Log.info("Got queue item: %d", (int)message.message_type);
        if (!(message.node_id == Node::SN2 && !data_manager.IsConnectedSN2() && message.message_type != BluetoothMessageId::SECURITY))
        {
            switch (message.message_type)
            {
            case BluetoothMessageId::CONNECT:
            {
                Log.info("In Connect");
                if (message.node_id == Node::SN1)
                {
                    data_manager.SetConnectedSN1(true);
                    if (!data_manager.IsCallButtonActivatedSN1())
                    { 
                        led_1.update_LED(LED_STATE::GREEN_SOLID);
                        Log.info("CN: LED1 set to GREEN_SOLID (SN1 Connected)");
                    }
                    Cloud::publishDetectionData();
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
                    led_1.update_LED(LED_STATE::OFF);
                    Log.info("CN: LED1 set to OFF (SN1 Disconnected)");
                }
                else if (message.node_id == Node::SN2)
                {
                    data_manager.SetConnectedSN2(false);
                    led_2.update_LED(LED_STATE::OFF);
                }
                Cloud::publishDetectionData();
                break;
            }
            case BluetoothMessageId::TEMPERATURE:
            {
                double temperature = message.data_payload.word_data / 100.0;
                data_manager.SetTemperatureLevel(message.data_payload.word_data / 100.0);
                if (data_manager.GetLightsOn())
                {
                    Log.info("Lights are on");
                    if (temperature > data_manager.GetTemperatureLightsOn() && !data_manager.GetFanControlled())
                    {
                        uint8_t duty = min(100, 30 + 10 * abs(temperature - data_manager.GetTemperatureLightsOn()));
                        Log.info("Overriding fan with duty: %d", duty);
                        Bluetooth::SetFanDutyCycle(true, duty);
                        data_manager.SetFanControlled(true);
                    }
                    else if (temperature <= data_manager.GetTemperatureLightsOn() && data_manager.GetFanControlled())
                    {
                        Log.info("Cancelling overriding fan with 0 duty");
                        data_manager.SetFanControlled(false);
                        Bluetooth::SetFanDutyCycle(false, 0);
                    }
                }
                else
                {
                    Log.info("Lights are off");
                    if (temperature > data_manager.GetTemperatureLightsOff())
                    {
                        uint8_t duty = min(100, 30 + 10 * abs(temperature - data_manager.GetTemperatureLightsOn()));
                        Log.info("Overriding fan with duty: %d", duty);
                        Bluetooth::SetFanDutyCycle(true, duty);
                        data_manager.SetFanControlled(true);
                    }
                    else if (data_manager.GetFanControlled())
                    {
                        data_manager.SetFanControlled(false);
                        Bluetooth::SetFanDutyCycle(false, 0);
                    }
                }
                Log.info("Got Temperature: %.2f", data_manager.GetTemperatureLevel());
                break;
            }
            case BluetoothMessageId::LIGHT:
            {
                if (message.node_id == Node::SN1)
                {
                    uint8_t lux_value_from_sn1 = message.data_payload.byte_data;
                    Log.info("CtrlNode - Value in case LIGHT from message.value_data: %u", lux_value_from_sn1);

                    if (lux_value_from_sn1 > 50 && !data_manager.GetLightsOn())
                    {
                        Log.info("Setting lights on");
                        Bluetooth::SetLightOnOff(true);
                    }
                    else if (lux_value_from_sn1 <= 50 && data_manager.GetLightsOn())
                    {
                        Bluetooth::SetLightOnOff(false);
                    }

                    data_manager.SetLightLevel((double)lux_value_from_sn1);
                }

                break;
            }
            case BluetoothMessageId::SN1_PWM_VALUE:
            {
                if (message.node_id == Node::SN1)
                {
                    uint8_t pwm_value_from_sn1 = message.data_payload.byte_data;
                    PowerEstimator::processSn1PwmValue(pwm_value_from_sn1);
                }
                break;
            }
            case BluetoothMessageId::SN2_PWM_VALUE:
            {
                if (message.node_id == Node::SN2)
                {
                    uint8_t pwm_value_from_sn2 = message.data_payload.byte_data; 
                    Log.info("CtrlNode - SN2_PWM_VALUE (Fan Duty %%) from SN2: %u", pwm_value_from_sn2);
                    PowerEstimator::processSn2PwmValue(pwm_value_from_sn2); 
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
                Cloud::publishDetectionData();
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
                    if (data_manager.GetLightsOn())
                    {
                        led_3.update_LED(LED_STATE::GREEN_FLASHING);
                    }
                    else
                    {
                        led_3.update_LED(LED_STATE::RED_FLASHING);
                    }
                }
                else
                {
                    // Sound not detected
                    led_3.update_LED(LED_STATE::GREEN_SOLID);
                }
                // Cloud::publishDetectionData(); // TODO: uncomment for demo
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
                        if (data_manager.GetLightsOn())
                        {
                            Log.info("Setting lights on");
                            Bluetooth::SetLightOnOff(true);
                        }
                        else
                        {
                            Bluetooth::SetLightOnOff(false);
                        }
                    }
                    else
                    {
                        Log.info("Incorrect Pin, disconnecting");
                        Bluetooth::Disconnect(Node::SN2);
                    }
                }
                Cloud::publishDetectionData();
                break;
            }
            case BluetoothMessageId::SET_TEMPERATURE_LIGHTS_ON:
            {
                data_manager.SetTemperatureLightsOn(message.data_payload.double_data);
                break;
            }
            case BluetoothMessageId::SET_TEMPERATURE_LIGHTS_OFF:
            {
                data_manager.SetTemperatureLightsOff(message.data_payload.double_data);
                break;
            }
            case BluetoothMessageId::CALL_BUTTON_OFF:
            {
                Log.info("Got button press");
                if (message.node_id == Node::SN1)
                {
                    Bluetooth::DeactivateCallSN1();
                    data_manager.SetCallButtonActivatedSN1(false);
                    led_1.update_LED(LED_STATE::GREEN_SOLID);
                }
                else if (message.node_id == Node::SN2)
                {
                    Bluetooth::DeactivateCallSN2();
                    data_manager.SetCallButtonActivatedSN2(false);
                    led_2.update_LED(LED_STATE::GREEN_SOLID);
                }
                Cloud::publishDetectionData();
                break;
            }
            case BluetoothMessageId::MOTION_DETECTED:
            {
                Log.info("Motion Detected");
                // Motion detected, turn on LED 3
                if (message.data_payload.byte_data == 1)
                {
                    if (data_manager.GetLightsOn())
                    {
                        led_3.update_LED(LED_STATE::GREEN_FLASHING);
                    }
                    else
                    {
                        led_3.update_LED(LED_STATE::RED_FLASHING);
                    }
                }
                else
                {
                    led_3.update_LED(LED_STATE::GREEN_SOLID);
                }
                data_manager.SetMoveDetectedSN1((bool)message.data_payload.byte_data);
                // Cloud::publishDetectionData(); // TODO: uncomment for demo
                break;
            }
            case BluetoothMessageId::SET_LUX_LEVEL:
            {
                data_manager.SetTargetLightLevel(message.data_payload.word_data);
                Bluetooth::SetTargetLightLevel(message.data_payload.word_data);
                break;
            }
            default:
                break;
            }
        }
    }
}
