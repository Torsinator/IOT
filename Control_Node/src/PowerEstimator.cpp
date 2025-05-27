// PowerEstimator.cpp
#include "PowerEstimator.h"
#include "LCD.h"          // LCD 업데이트 큐에 접근하기 위해 (선택적)
#include "Enumerations.h" // LCD_Message enum 사용

// DataManager 인스턴스에 대한 포인터 또는 참조
static DataManager *p_data_manager = nullptr; // DataManager 인스턴스를 가리킬 포인터

// --- 30초 간격 전력량 계산을 위한 내부 변수 (SN1용) ---
static float sn1_accumulated_power_mW_for_30s_interval = 0.0;
static uint16_t sn1_power_sample_count_in_30s_interval = 0;

// --- 30초 간격 전력량 계산을 위한 내부 변수 (SN2용) --- // <--- 추가
static float sn2_accumulated_power_mW_for_30s_interval = 0.0;
static uint16_t sn2_power_sample_count_in_30s_interval = 0;

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

    // SN2용 PWM 처리 함수 (팬)
    void processSn2PwmValue(uint8_t pwm_value_0_100) // SN2로부터 받은 PWM 값 (0-100 범위)
    {
        if (!p_data_manager)
        {
            Log.error("PowerEstimator: DataManager not initialized for SN2!");
            return;
        }

         // 1. 전달 함수를 사용하여 전류 계산 (mA)
        // pwm_value_0_100은 이미 0-100 범위의 값이므로 변환 없이 바로 사용합니다.
        // ApplyTransferFunction은 uint16_t를 받으므로 캐스팅해줍니다.
        float current_mA = ApplyTransferFunction(static_cast<uint16_t>(pwm_value_0_100));

        if (current_mA < 0)
        {
            current_mA = 0.0;
        }

        // 2. 순간 전력 계산 (mW)
        float power_mW = SN2_VOLTAGE * current_mA;
        if (power_mW < 0)
        {
            power_mW = 0.0;
        }

        Log.info("PE_SN2: PWM_Duty%%=%u, Current=%.2fmA, Power=%.2fmW",
                 pwm_value_0_100, current_mA, power_mW);

        // 3. 30초 평균 전력 계산을 위해 현재 전력값 누적
        sn2_accumulated_power_mW_for_30s_interval += power_mW;
        sn2_power_sample_count_in_30s_interval++;
    }

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

        // --- SN2 전력량 계산 --- // <--- 추가된 부분 시작
        if (sn2_power_sample_count_in_30s_interval > 0)
        {
            float average_power_mW_30s_sn2 = sn2_accumulated_power_mW_for_30s_interval / sn2_power_sample_count_in_30s_interval;
            float energy_mWh_30s_sn2 = average_power_mW_30s_sn2 * (30.0 / (3600.0)); // 30초를 시간 단위로

            double current_total_power_mWh_sn2 = p_data_manager->GetPowerSN2(); // GetPowerSN2는 double을 반환
            double new_total_power_mWh_sn2 = current_total_power_mWh_sn2 + energy_mWh_30s_sn2;
            p_data_manager->SetPowerSN2(new_total_power_mWh_sn2);

            Log.info("PE_SN2 Power Update (30s): AvgP=%.2fmW, Energy=%.2fmWh, TotalE=%.2fmWh",
                     average_power_mW_30s_sn2, energy_mWh_30s_sn2, new_total_power_mWh_sn2);

            sn2_accumulated_power_mW_for_30s_interval = 0.0;
            sn2_power_sample_count_in_30s_interval = 0;
        }
        else
        {
            // Log.info("PE_SN2 Power Update (30s): No PWM data. Assuming 0 power consumption.");
            // 만약 PWM 데이터가 없을 때도 SetPowerSN2(0) 또는 현재 값을 유지해야 한다면,
            // DataManager의 SetPowerSN2는 현재 값을 누적하므로, 데이터가 없을 때는 호출하지 않아도 됩니다.
            // 또는, 0으로 설정하고 싶다면 p_data_manager->SetPowerSN2(p_data_manager->GetPowerSN2()); 와 같이 현재 값을 다시 설정하거나
            // p_data_manager->SetPowerSN2(0.0); // 만약 0으로 리셋하고 싶다면 (이 경우는 아닐 듯)
        }
        // --- SN2 전력량 계산 --- // <--- 추가된 부분 끝

        // LCD 업데이트 요청
        LCD_Message lcd_msg = LCD_Message::REFRESH;
        os_queue_put(lcd_message_queue, &lcd_msg, 0, nullptr);
    }

} // namespace PowerEstimator