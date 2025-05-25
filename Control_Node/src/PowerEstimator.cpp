// PowerEstimator.cpp
#include "PowerEstimator.h"
#include "LCD.h"          // LCD 업데이트 큐에 접근하기 위해 (선택적)
#include "Enumerations.h" // LCD_Message enum 사용

// DataManager 인스턴스에 대한 포인터 또는 참조
static DataManager *p_data_manager = nullptr; // DataManager 인스턴스를 가리킬 포인터

// --- 30초 간격 전력량 계산을 위한 내부 변수 (SN1용) ---
static float sn1_accumulated_power_mW_for_30s_interval = 0.0;
static uint16_t sn1_power_sample_count_in_30s_interval = 0;
// (SN2용 변수도 필요하다면 여기에 추가)
// static float sn2_accumulated_power_mW_for_30s_interval = 0.0;
// static uint16_t sn2_power_sample_count_in_30s_interval = 0;

// LCD 업데이트를 위한 큐 (Control_Node.cpp에서 extern으로 선언된 것을 사용)
extern os_queue_t lcd_message_queue;

namespace PowerEstimator
{

    void setup(DataManager &dm)
    {
        p_data_manager = &dm; // 전달받은 DataManager 인스턴스의 주소를 저장
        // 초기화 필요한 다른 작업들...
        Log.info("PowerEstimator initialized.");
    }

    double ApplyTransferFunction(uint16_t duty)
    {
        return 0.0055 * std::pow(duty, 2) + 0.301 * duty - 0.4289;
    }




    void processSn1PwmValue(uint8_t pwm_value)
    {
        if (!p_data_manager)
        {
            Log.error("PowerEstimator: DataManager not initialized!");
            return;
        }

        // 1. 전달 함수를 사용하여 전류 계산 (mA)
        float current_mA = (SN1_PWM_TO_CURRENT_SLOPE * pwm_value) + SN1_PWM_TO_CURRENT_INTERCEPT;
        if (current_mA < 0)
        {
            current_mA = 0.0;
        }

        // 2. 순간 전력 계산 (mW)
        float power_mW = SN1_VOLTAGE * current_mA;
        if (power_mW < 0)
        {
            power_mW = 0.0;
        }

        // Log.info("PE_SN1: PWM=%u, Current=%.2fmA, Power=%.2fmW", pwm_value, current_mA, power_mW);

        // 3. 30초 평균 전력 계산을 위해 현재 전력값 누적
        sn1_accumulated_power_mW_for_30s_interval += power_mW;
        sn1_power_sample_count_in_30s_interval++;
    }

    // SN2용 PWM 처리 함수 (필요시 유사하게 구현)
    /*
    void processSn2PwmValue(uint8_t pwm_value) {
        if (!p_data_manager) return;
        // ... SN2 전류 및 전력 계산 ...
        // sn2_accumulated_power_mW_for_30s_interval += power_mW_sn2;
        // sn2_power_sample_count_in_30s_interval++;
    }
    */

    void calculateAndStorePowerUsage30s()
    {
        if (!p_data_manager)
        {
            Log.error("PowerEstimator: DataManager not initialized for 30s calculation!");
            return;
        }

        // --- SN1 전력량 계산 ---
        if (sn1_power_sample_count_in_30s_interval > 0)
        {
            float average_power_mW_30s = sn1_accumulated_power_mW_for_30s_interval / sn1_power_sample_count_in_30s_interval;
            // 전력량(mWh) = 평균전력(mW) * 시간(h) ; 시간 = 30초
            float energy_mWh_30s = average_power_mW_30s * (30.0 / (3600.0)); // 30초를 시간 단위로

            uint16_t current_total_power_mWh_sn1 = p_data_manager->GetPowerSN1();
            double new_total_power_mWh_sn1 = current_total_power_mWh_sn1 + energy_mWh_30s;
            p_data_manager->SetPowerSN1(new_total_power_mWh_sn1);

            Log.info("PE_SN1 Power Update (30s): AvgP=%.2fmW, Energy=%.2fmWh, TotalE=%.2fmWh",
                     average_power_mW_30s, energy_mWh_30s, new_total_power_mWh_sn1);

            sn1_accumulated_power_mW_for_30s_interval = 0.0;
            sn1_power_sample_count_in_30s_interval = 0;
        }
        else
        {
            // Log.info("PE_SN1 Power Update (30s): No PWM data. Assuming 0 power consumption.");
        }

        // --- SN2 전력량 계산 (필요시 유사하게 구현) ---
        /*
        if (sn2_power_sample_count_in_30s_interval > 0) {
            // ... SN2 평균 전력 및 전력량 계산 ...
            // p_data_manager->SetPowerSN2(...);
            // ... 리셋 ...
        }
        */

        // LCD 업데이트 요청
        LCD_Message lcd_msg = LCD_Message::REFRESH;
        os_queue_put(lcd_message_queue, &lcd_msg, 0, nullptr);
    }

} // namespace PowerEstimator