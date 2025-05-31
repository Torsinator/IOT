// PowerEstimator.cpp
#include "PowerEstimator.h"
#include "LCD.h"
#include "Enumerations.h"
#include "Cloud.h"
#include <cmath> // std::pow 사용을 위해

// DataManager 인스턴스에 대한 포인터 또는 참조
static DataManager *p_data_manager = nullptr;

// 액추에이터 전력 누적 변수 (유지)
static float sn1_accumulated_actuator_power_mW_for_30s = 0.0; // 변수명 명확화
static uint16_t sn1_actuator_power_sample_count_in_30s = 0;   // 변수명 명확화

static float sn2_accumulated_actuator_power_mW_for_30s = 0.0; // 변수명 명확화
static uint16_t sn2_actuator_power_sample_count_in_30s = 0;   // 변수명 명확화

extern os_queue_t lcd_message_queue;

namespace PowerEstimator
{
    void setup(DataManager &dm)
    {
        p_data_manager = &dm;
        Log.info("PowerEstimator initialized.");
    }

    double ApplyTransferFunction(uint16_t duty)
    {
        // SN2 Fan의 전달 함수 (PWM duty 0-100 -> current mA)
        return 0.0055 * std::pow(static_cast<double>(duty), 2.0) + 0.301 * static_cast<double>(duty) - 0.4289;
    }

    // SN1 액추에이터 PWM 처리
    void processSn1PwmValue(uint8_t pwm_value)
    {
        if (!p_data_manager)
        { /* ... 에러 처리 ... */
            return;
        }

        float current_mA = (SN1_PWM_TO_CURRENT_SLOPE * pwm_value) + SN1_PWM_TO_CURRENT_INTERCEPT;
        if (current_mA < 0)
            current_mA = 0.0f;

        float power_mW = OPERATING_VOLTAGE_V * current_mA;
        if (power_mW < 0)
            power_mW = 0.0f;

        sn1_accumulated_actuator_power_mW_for_30s += power_mW;
        sn1_actuator_power_sample_count_in_30s++;
    }

    // SN2 액추에이터 PWM 처리
    void processSn2PwmValue(uint8_t pwm_value_0_100)
    {
        if (!p_data_manager)
        { /* ... 에러 처리 ... */
            return;
        }

        float current_mA = static_cast<float>(ApplyTransferFunction(static_cast<uint16_t>(pwm_value_0_100)));
        if (current_mA < 0)
            current_mA = 0.0f;

        float power_mW = OPERATING_VOLTAGE_V * current_mA;
        if (power_mW < 0)
            power_mW = 0.0f;

        // 원본 로그 유지
        // Log.info("PE_SN2: PWM_Duty%%=%u, Current=%.2fmA, Power=%.2fmW",
        //          pwm_value_0_100, current_mA, power_mW);

        sn2_accumulated_actuator_power_mW_for_30s += power_mW;
        sn2_actuator_power_sample_count_in_30s++;
    }

    void calculateAndStorePowerUsage30s()
    {
        if (!p_data_manager)
        { /* ... 에러 처리 ... */
            return;
        }

        const double INTERVAL_S = 30.0;
        const double INTERVAL_H = INTERVAL_S / 3600.0;
        const float VOLTAGE = OPERATING_VOLTAGE_V;

        // --- SN1 (Argon) 전력 계산 ---
        float sn1_actuator_avg_power_mW = 0.0f;
        // 액추에이터 전력은 연결 상태와 무관하게 PWM 메시지가 오면 계산될 수 있으므로,
        // 연결되지 않았을 때는 액추에이터 전력도 0으로 처리할지 결정 필요.
        // 여기서는 연결 안 되면 모든 전력을 0으로 가정.

        float sn1_mcu_comm_avg_power_mW = 0.0f;
        float sn1_total_avg_power_mW = 0.0f; // 초기값을 0으로

        if (p_data_manager->IsConnectedSN1())
        { // <--- SN1이 연결되어 있을 때만 전력 계산
            // 1. 액추에이터 평균 전력 (연결되었을 때만 고려)
            if (sn1_actuator_power_sample_count_in_30s > 0)
            {
                sn1_actuator_avg_power_mW = sn1_accumulated_actuator_power_mW_for_30s / sn1_actuator_power_sample_count_in_30s;
            }

            // 2. MCU 및 통신 평균 전력 (연결되었을 때)
            const float TX_DURATION_S = 0.22f;
            const int TX_COUNT_IN_INTERVAL = static_cast<int>(INTERVAL_S / 10.0);
            float total_current_for_tx_mA = ARGON_CURRENT_IDLE_MA + ARGON_CURRENT_BLE_TX_ADDITIONAL_MA;
            float idle_current_mA = ARGON_CURRENT_IDLE_MA; // 연결 중 IDLE 상태
            float total_tx_duration_s = TX_DURATION_S * TX_COUNT_IN_INTERVAL;
            float total_idle_duration_s = INTERVAL_S - total_tx_duration_s;
            if (total_idle_duration_s < 0)
                total_idle_duration_s = 0;
            float sn1_avg_current_mA = ((total_current_for_tx_mA * total_tx_duration_s) + (idle_current_mA * total_idle_duration_s)) / INTERVAL_S;
            sn1_mcu_comm_avg_power_mW = VOLTAGE * sn1_avg_current_mA;

            sn1_total_avg_power_mW = sn1_mcu_comm_avg_power_mW + sn1_actuator_avg_power_mW;
        }
        // else -> 연결 안 되어 있으면 sn1_total_avg_power_mW는 0으로 유지됨

        if (sn1_total_avg_power_mW > 0)
        { // 계산된 평균 전력이 있을 때만 누적
            float sn1_energy_mWh_30s = sn1_total_avg_power_mW * static_cast<float>(INTERVAL_H);
            p_data_manager->SetPowerSN1(p_data_manager->GetPowerSN1() + sn1_energy_mWh_30s);
        }
        // 액추에이터 누적값 리셋은 항상 수행
        sn1_accumulated_actuator_power_mW_for_30s = 0.0f;
        sn1_actuator_power_sample_count_in_30s = 0;
        Log.info("PE_SN1 (30s): IsConnected=%d, TotalE=%.2fmWh (MCU_Comm:%.2fmW, Act:%.2fmW)",
                 p_data_manager->IsConnectedSN1(), p_data_manager->GetPowerSN1(), sn1_mcu_comm_avg_power_mW, sn1_actuator_avg_power_mW);

        // --- SN2 (Argon) 전력 계산 ---
        float sn2_actuator_avg_power_mW = 0.0f;
        // 액추에이터 전력은 연결 상태와 무관하게 PWM 메시지가 오면 계산될 수 있으므로,
        // 연결되지 않았을 때는 액추에이터 전력도 0으로 처리할지 결정 필요.
        // 여기서는 연결 안 되면 모든 전력을 0으로 가정.

        float sn2_mcu_comm_avg_power_mW = 0.0f;
        float sn2_total_avg_power_mW = 0.0f; // 초기값을 0으로

        if (p_data_manager->IsConnectedSN2())
        { // SN2가 연결되어 있을 때만 전력 계산
            if (sn2_actuator_power_sample_count_in_30s > 0)
            {
                sn2_actuator_avg_power_mW = sn2_accumulated_actuator_power_mW_for_30s / sn2_actuator_power_sample_count_in_30s;
            }

            const float TX_DURATION_S = 0.22f;
            const int TX_COUNT_IN_INTERVAL = static_cast<int>(INTERVAL_S / 10.0);
            float total_current_for_tx_mA = ARGON_CURRENT_IDLE_MA + ARGON_CURRENT_BLE_TX_ADDITIONAL_MA;
            float idle_current_mA = ARGON_CURRENT_IDLE_MA;
            float total_tx_duration_s = TX_DURATION_S * TX_COUNT_IN_INTERVAL;
            float total_idle_duration_s = INTERVAL_S - total_tx_duration_s;
            if (total_idle_duration_s < 0)
                total_idle_duration_s = 0;
            float sn2_avg_current_mA = ((total_current_for_tx_mA * total_tx_duration_s) + (idle_current_mA * total_idle_duration_s)) / INTERVAL_S;
            sn2_mcu_comm_avg_power_mW = VOLTAGE * sn2_avg_current_mA;

            sn2_total_avg_power_mW = sn2_mcu_comm_avg_power_mW + sn2_actuator_avg_power_mW;
        }
        // else -> 연결 안 되어 있으면 sn2_total_avg_power_mW는 0으로 유지됨

        if (sn2_total_avg_power_mW > 0)
        { // 계산된 평균 전력이 있을 때만 누적
            float sn2_energy_mWh_30s = sn2_total_avg_power_mW * static_cast<float>(INTERVAL_H);
            p_data_manager->SetPowerSN2(p_data_manager->GetPowerSN2() + sn2_energy_mWh_30s);
        }
        // 액추에이터 누적값 리셋은 항상 수행
        sn2_accumulated_actuator_power_mW_for_30s = 0.0f;
        sn2_actuator_power_sample_count_in_30s = 0;
        Log.info("PE_SN2 (30s): IsConnected=%d, TotalE=%.2fmWh (MCU_Comm:%.2fmW, Act:%.2fmW)",
                 p_data_manager->IsConnectedSN2(), p_data_manager->GetPowerSN2(), sn2_mcu_comm_avg_power_mW, sn2_actuator_avg_power_mW);

                // --- CN (Photon 2) 전력 계산 ---

        // 가정:
        // - 클라우드 TX는 30초에 한 번, CLOUD_TX_DURATION_S 동안 지속.
        // - BLE RX는 10초에 한 번, 각 SN당 BLE_RX_DURATION_PER_SN_S 동안 지속.
        // - 나머지는 IDLE 상태.

        const float CLOUD_TX_DURATION_S = 1.946f;      // 클라우드 전송 1회당 지속 시간 (가정)
        const float BLE_RX_DURATION_PER_SN_S = 0.22f; // SN 1개로부터 BLE 수신 1회당 지속 시간 (가정)
        const int BLE_EVENTS_PER_30S = static_cast<int>(INTERVAL_S / 10.0); // 10초마다 BLE 이벤트 발생 가정

        float actual_time_spent_cloud_tx_s = 0.0f;
        float actual_time_spent_ble_rx_s = 0.0f;
        float actual_time_spent_idle_s = 0.0f;

        // 1. 클라우드 전송 시간 결정
        if (Particle.connected()) {
            actual_time_spent_cloud_tx_s = CLOUD_TX_DURATION_S;
        }

        // 2. BLE 수신 총 시간 결정 (클라우드 전송 시간 제외)
        float potential_ble_rx_time_total_s = 0.0f;
        if (p_data_manager->IsConnectedSN1()) {
            potential_ble_rx_time_total_s += BLE_RX_DURATION_PER_SN_S * BLE_EVENTS_PER_30S;
        }
        if (p_data_manager->IsConnectedSN2()) {
            potential_ble_rx_time_total_s += BLE_RX_DURATION_PER_SN_S * BLE_EVENTS_PER_30S;
        }
        // 클라우드 TX가 아닌 남은 시간 중에서 BLE RX 시간 할당
        actual_time_spent_ble_rx_s = std::min(potential_ble_rx_time_total_s, static_cast<float>(INTERVAL_S) - actual_time_spent_cloud_tx_s);
        if (actual_time_spent_ble_rx_s < 0) actual_time_spent_ble_rx_s = 0; // 방어 코드

        // 3. IDLE 시간 결정
        actual_time_spent_idle_s = INTERVAL_S - actual_time_spent_cloud_tx_s - actual_time_spent_ble_rx_s;
        if (actual_time_spent_idle_s < 0) actual_time_spent_idle_s = 0; // 방어 코드

        // 4. 각 상태별 에너지 계산 및 합산 (mWs 단위)
        double cn_total_energy_mWs = 0.0;

        // IDLE 상태 에너지
        cn_total_energy_mWs += (VOLTAGE * PHOTON2_CURRENT_IDLE_MA) * actual_time_spent_idle_s;

        // BLE RX 상태 에너지
        // PHOTON2_CURRENT_BLE_RX_TOTAL_MA는 BLE 라디오가 활성화되어 수신 중일 때의 전체 전류
        cn_total_energy_mWs += (VOLTAGE * PHOTON2_CURRENT_BLE_RX_TOTAL_MA) * actual_time_spent_ble_rx_s;

        // Cloud TX 상태 에너지
        // PHOTON2_CURRENT_WIFI_CLOUD_TX_TOTAL_MA는 Wi-Fi 라디오가 활성화되어 전송 중일 때의 전체 전류
        cn_total_energy_mWs += (VOLTAGE * PHOTON2_CURRENT_WIFI_CLOUD_TX_TOTAL_MA) * actual_time_spent_cloud_tx_s;

        // 5. 평균 전력 계산 (mW)
        float cn_total_avg_power_mW = 0.0f;
        if (INTERVAL_S > 0) { // 0으로 나누기 방지
            cn_total_avg_power_mW = static_cast<float>(cn_total_energy_mWs / INTERVAL_S);
        }

        // 6. 에너지 변환 (mWh) 및 누적
        if (cn_total_avg_power_mW > 0) {
            float cn_energy_mWh_30s = cn_total_avg_power_mW * static_cast<float>(INTERVAL_H);
            p_data_manager->SetPowerCN(p_data_manager->GetPowerCN() + cn_energy_mWh_30s);
        }

        Log.info("PE_CN (30s): TotalE=%.2fmWh (AvgPwr:%.2fmW, Idle:%.1fs, BLE_RX:%.1fs, Cloud_TX:%.1fs)",
                 p_data_manager->GetPowerCN(), cn_total_avg_power_mW, actual_time_spent_idle_s, actual_time_spent_ble_rx_s, actual_time_spent_cloud_tx_s);
        // --- LCD 업데이트 및 클라우드 게시 ---
        LCD_Message lcd_msg = LCD_Message::REFRESH;
        os_queue_put(lcd_message_queue, &lcd_msg, 0, nullptr);
        Cloud::publishPowerData();
    }

} // namespace PowerEstimator