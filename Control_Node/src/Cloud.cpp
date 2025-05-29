// Cloud.cpp
#include "Cloud.h"
#include "DataManager.h" // DataManager의 전역 인스턴스 사용을 위해
#include "Particle.h"    // Log 및 Particle API 사용

// Control_Node.cpp에 정의된 전역 DataManager 인스턴스를 사용하기 위한 extern 선언
extern DataManager data_manager;

namespace Cloud
{
    void setup()
    {
        // 클라우드 관련 초기 설정이 필요하다면 여기에 작성합니다.
        // 예를 들어, Particle function이나 variable을 등록할 수 있습니다.
        // 현재 요구사항에서는 Particle.publish만 사용하므로 특별한 setup은 없을 수 있습니다.
        Log.info("Cloud module initialized.");
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
            data = String::format("%.2f %d", temperature, lux_level);
            if (!Particle.publish("Environment", data, PRIVATE))
            {                                                                                               // 온도는 소수점 2자리까지
                Log.error("Failed to publish Environment data."); // 실패 시 값도 로그
                success = false;
            }

            // if (!Particle.publish("SN1_Power_mWh", String(p_sn1_mWh, 2), PRIVATE))
            // {
            //     Log.error("Failed to publish SN1 power data.");
            //     success = false;
            // }
            // if (!Particle.publish("SN2_Power_mWh", String(p_sn2_mWh, 2), PRIVATE))
            // {
            //     Log.error("Failed to publish SN2 power data.");
            //     success = false;
            // }
            // if (!Particle.publish("CN_Power_mWh", String(p_cn_mWh, 2), PRIVATE))
            // {
            //     Log.error("Failed to publish CN power data.");
            //     success = false;
            // }
            // // --- 조도 데이터 게시 ---
            // // 이벤트 이름은 "LuxLevel" 또는 "SN1_Lux" 등 원하는 대로 지정
            // if (!Particle.publish("LuxLevel", String(lux_level), PRIVATE))
            // {
            //     Log.error("Failed to publish Lux level data.");
            //     success = false;
            // }
            // --- 온도 데이터 게시 ---
            // 이벤트 이름은 "Temperature" 또는 "SN2_Temp" 등 원하는 대로 지정
            // if (!Particle.publish("Temperature_SN2", String(temperature, 2), PRIVATE))
            // {                                                                                               // 온도는 소수점 2자리까지
            //     Log.error("Failed to publish Temperature data. Current temp value was: %.2f", temperature); // 실패 시 값도 로그
            //     success = false;
            // }

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

} // namespace Cloud