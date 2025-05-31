#include "Cloud.h"

#include <string>

#include "Particle.h"
#include "Structs.h"
#include "DataManager.h"

namespace Cloud
{
    void Setup()
    {
        Particle.function("Set_Temperature_Lights_On", SetTemperatureLightsOn);
        Particle.function("Set_Temperature_Lights_Off", SetTemperatureLightsOff);
        Particle.function("SetLightLevel", SetLightLevel);
    }

    int SetTemperatureLightsOn(String command)
    {
        if (command.length() == 0)
        {
            Log.info("Failed to set temperature, was not parsable to double");
            return 1;
        }
        Log.info("Got cloud function call %s", command.c_str());
        double temperature = std::stod(command.c_str());
        BluetoothMessage message{Node::SN2, BluetoothMessageId::SET_TEMPERATURE_LIGHTS_ON};
        message.data_payload.double_data = temperature;
        os_queue_put(control_queue, &message, 0, nullptr);

        return 0;
    }

    int SetTemperatureLightsOff(String command)
    {
        if (command.length() == 0)
        {
            Log.info("Failed to set temperature, was not parsable to double");
            return 1;
        }
        double temperature = std::stod(command.c_str());
        BluetoothMessage message{Node::SN2, BluetoothMessageId::SET_TEMPERATURE_LIGHTS_OFF};
        message.data_payload.double_data = temperature;
        os_queue_put(control_queue, &message, 0, nullptr);
        return 0;
    }

    int SetLightLevel(String command)
    {
        if (command.length() == 0)
        {
            Log.info("Failed to set temperature, was not parsable to double");
            return 1;
        }
        Log.info("Got Set light Level cloud");
        uint16_t light = std::stod(command.c_str());
        BluetoothMessage message{Node::SN1, BluetoothMessageId::SET_LUX_LEVEL};
        message.data_payload.word_data = light;
        os_queue_put(control_queue, &message, 0, nullptr);
        return 0;
    }

    void publishPowerData()
    {
        if (Particle.connected())
        {
            double p_sn1_mWh = data_manager.GetPowerSN1();
            double p_sn2_mWh = data_manager.GetPowerSN2();
            double p_cn_mWh = data_manager.GetPowerCN();

            // 추가: 조도 및 온도 데이터 가져오기
            uint8_t lux_level = data_manager.GetLightLevel();                                           // SN1에서 오는 조도 값
            double temperature = data_manager.GetTemperatureLevel();                                    // SN2에서 오는 온도 값
            Log.info("Cloud.cpp: Value of 'temperature' variable BEFORE publish: %.2f C", temperature); // <--- 이 로그 추가!

            bool success = true;
            String data = String::format("%.2f %.2f %.2f", p_sn1_mWh, p_sn2_mWh, p_cn_mWh);
            if (!Particle.publish("Power_mWh", data, PRIVATE))
            {
                Log.error("Failed to publish power data.");
                success = false;
            }
            Log.info("Publishig time: %ld", Time.now());
            data = String::format("%.2f %d", temperature, lux_level);
            if (!Particle.publish("Environment", data, PRIVATE))
            {                                                     // 온도는 소수점 2자리까지
                Log.error("Failed to publish Environment data."); // 실패 시 값도 로그
                success = false;
            }

            if (success)
            {
                Log.info("Successfully published all power data to cloud. SN1:%.2f, SN2:%.2f, CN:%.2f mWh", p_sn1_mWh, p_sn2_mWh, p_cn_mWh);
            }
            else
            {
                Log.warn("One or more power data publications failed.");
            }
        }
        else
        {
            Log.warn("Cloud not connected. Skipping power data publish.");
        }
    }

    void publishDetectionData()
    {
        if (Particle.connected())
        {
            uint8_t connected_sn1 = (uint8_t)data_manager.IsConnectedSN1();
            uint8_t connected_sn2 = (uint8_t)data_manager.IsConnectedSN2();
            uint8_t call_1 = (uint8_t)data_manager.IsCallButtonActivatedSN1();
            uint8_t call_2 = (uint8_t)data_manager.IsCallButtonActivatedSN2();
            uint8_t move = (uint8_t)data_manager.GetMoveDetectedSN1();
            uint8_t sound = (uint8_t)data_manager.IsSoundDetected();
            String data = String::format("%d %d %d %d %d %d", connected_sn1, connected_sn2, call_1, call_2, move, sound);
            if (!Particle.publish("Detection", data, PRIVATE))
            {
                Log.error("Failed to publish power data.");
            }
        }
    }
}
