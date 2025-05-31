// PowerEstimator.cpp
#include "PowerEstimator.h"
#include "LCD.h"
#include "Enumerations.h"
#include "Cloud.h"
#include <cmath> // std::pow 


static DataManager *p_data_manager = nullptr;


static float sn1_accumulated_actuator_power_mW_for_30s = 0.0; 
static uint16_t sn1_actuator_power_sample_count_in_30s = 0;   

static float sn2_accumulated_actuator_power_mW_for_30s = 0.0; 
static uint16_t sn2_actuator_power_sample_count_in_30s = 0;   

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
        // SN2 Fan transfer function (PWM duty 0-100 -> current mA)
        return 0.0055 * std::pow(static_cast<double>(duty), 2.0) + 0.301 * static_cast<double>(duty) - 0.4289;
    }

    // process SN1 actuator
    void processSn1PwmValue(uint8_t pwm_value)
    {
        if (!p_data_manager)
        {
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

    // process SN2 actuator
    void processSn2PwmValue(uint8_t pwm_value_0_100)
    {
        if (!p_data_manager)
        {
            return;
        }

        float current_mA = static_cast<float>(ApplyTransferFunction(static_cast<uint16_t>(pwm_value_0_100)));
        if (current_mA < 0)
            current_mA = 0.0f;

        float power_mW = OPERATING_VOLTAGE_V * current_mA;
        if (power_mW < 0)
            power_mW = 0.0f;
        sn2_accumulated_actuator_power_mW_for_30s += power_mW;
        sn2_actuator_power_sample_count_in_30s++;
    }

    void calculateAndStorePowerUsage30s()
    {
        if (!p_data_manager)
        { 
            return;
        }

        const double INTERVAL_S = 30.0;
        const double INTERVAL_H = INTERVAL_S / 3600.0;
        const float VOLTAGE = OPERATING_VOLTAGE_V;

        // --- SN1 (Argon) power est ---
        float sn1_actuator_avg_power_mW = 0.0f;
        // Since actuator power can be calculated when a PWM message arrives regardless of the connection status,
        // it is necessary to decide whether to treat the actuator power as 0 when not connected. 
        // Here, all power is assumed to be 0 when not connected.

        float sn1_mcu_comm_avg_power_mW = 0.0f;
        float sn1_total_avg_power_mW = 0.0f; // ini 0

        if (p_data_manager->IsConnectedSN1())
        { // <--- once SN1 is connected, then cal the power
            // 1. actuator avg power
            if (sn1_actuator_power_sample_count_in_30s > 0)
            {
                sn1_actuator_avg_power_mW = sn1_accumulated_actuator_power_mW_for_30s / sn1_actuator_power_sample_count_in_30s;
            }

            // 2. MCU and comm avg power
            const float TX_DURATION_S = 0.22f;
            const int TX_COUNT_IN_INTERVAL = static_cast<int>(INTERVAL_S / 10.0);
            float total_current_for_tx_mA = ARGON_CURRENT_IDLE_MA + ARGON_CURRENT_BLE_TX_ADDITIONAL_MA;
            float idle_current_mA = ARGON_CURRENT_IDLE_MA; // connectiong... (idle)
            float total_tx_duration_s = TX_DURATION_S * TX_COUNT_IN_INTERVAL;
            float total_idle_duration_s = INTERVAL_S - total_tx_duration_s;
            if (total_idle_duration_s < 0)
                total_idle_duration_s = 0;
            float sn1_avg_current_mA = ((total_current_for_tx_mA * total_tx_duration_s) + (idle_current_mA * total_idle_duration_s)) / INTERVAL_S;
            sn1_mcu_comm_avg_power_mW = VOLTAGE * sn1_avg_current_mA;

            sn1_total_avg_power_mW = sn1_mcu_comm_avg_power_mW + sn1_actuator_avg_power_mW;
        }

        if (sn1_total_avg_power_mW > 0)
        { // Accumulate only when there is a calculated average power
            float sn1_energy_mWh_30s = sn1_total_avg_power_mW * static_cast<float>(INTERVAL_H);
            p_data_manager->SetPowerSN1(p_data_manager->GetPowerSN1() + sn1_energy_mWh_30s);
        }
        // Actuator accumulation value reset is always performed
        sn1_accumulated_actuator_power_mW_for_30s = 0.0f;
        sn1_actuator_power_sample_count_in_30s = 0;
        Log.info("PE_SN1 (30s): IsConnected=%d, TotalE=%.2fmWh (MCU_Comm:%.2fmW, Act:%.2fmW)",
                 p_data_manager->IsConnectedSN1(), p_data_manager->GetPowerSN1(), sn1_mcu_comm_avg_power_mW, sn1_actuator_avg_power_mW);

        // --- SN2 (Argon) power est ---
        float sn2_actuator_avg_power_mW = 0.0f;

        float sn2_mcu_comm_avg_power_mW = 0.0f;
        float sn2_total_avg_power_mW = 0.0f; 

        if (p_data_manager->IsConnectedSN2())
        { 
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

        if (sn2_total_avg_power_mW > 0)
        { 
            float sn2_energy_mWh_30s = sn2_total_avg_power_mW * static_cast<float>(INTERVAL_H);
            p_data_manager->SetPowerSN2(p_data_manager->GetPowerSN2() + sn2_energy_mWh_30s);
        }
        sn2_accumulated_actuator_power_mW_for_30s = 0.0f;
        sn2_actuator_power_sample_count_in_30s = 0;
        Log.info("PE_SN2 (30s): IsConnected=%d, TotalE=%.2fmWh (MCU_Comm:%.2fmW, Act:%.2fmW)",
                 p_data_manager->IsConnectedSN2(), p_data_manager->GetPowerSN2(), sn2_mcu_comm_avg_power_mW, sn2_actuator_avg_power_mW);

                // --- CN (Photon 2) power est ---

        // Assumptions:
// - Cloud TX once every 30 seconds, lasting CLOUD_TX_DURATION_S.
// - BLE RX once every 10 seconds, lasting BLE_RX_DURATION_PER_SN_S per SN.
// - The rest is IDLE.

        const float CLOUD_TX_DURATION_S = 1.946f;      // cloud delay
        const float BLE_RX_DURATION_PER_SN_S = 0.22f; // ble delay
        const int BLE_EVENTS_PER_30S = static_cast<int>(INTERVAL_S / 10.0); // event occur every 10 s

        float actual_time_spent_cloud_tx_s = 0.0f;
        float actual_time_spent_ble_rx_s = 0.0f;
        float actual_time_spent_idle_s = 0.0f;

        // 1. Determining cloud transfer time
        if (Particle.connected()) {
            actual_time_spent_cloud_tx_s = CLOUD_TX_DURATION_S;
        }

        // 2. Determining total BLE reception time (excluding cloud transmission time)
        float potential_ble_rx_time_total_s = 0.0f;
        if (p_data_manager->IsConnectedSN1()) {
            potential_ble_rx_time_total_s += BLE_RX_DURATION_PER_SN_S * BLE_EVENTS_PER_30S;
        }
        if (p_data_manager->IsConnectedSN2()) {
            potential_ble_rx_time_total_s += BLE_RX_DURATION_PER_SN_S * BLE_EVENTS_PER_30S;
        }
        // Allocate BLE RX time from remaining non-cloud TX time
        actual_time_spent_ble_rx_s = std::min(potential_ble_rx_time_total_s, static_cast<float>(INTERVAL_S) - actual_time_spent_cloud_tx_s);
        if (actual_time_spent_ble_rx_s < 0) actual_time_spent_ble_rx_s = 0; 

        // 3. IDLE time setting
        actual_time_spent_idle_s = INTERVAL_S - actual_time_spent_cloud_tx_s - actual_time_spent_ble_rx_s;
        if (actual_time_spent_idle_s < 0) actual_time_spent_idle_s = 0; 

        // 4. Energy calculation and summation for each state (in mWs)
        double cn_total_energy_mWs = 0.0;

        // IDLE cond. power
        cn_total_energy_mWs += (VOLTAGE * PHOTON2_CURRENT_IDLE_MA) * actual_time_spent_idle_s;

        // BLE RX cond. power
        // PHOTON2_CURRENT_BLE_RX_TOTAL_MA is the total current when the BLE radio is enabled and receiving.
        cn_total_energy_mWs += (VOLTAGE * PHOTON2_CURRENT_BLE_RX_TOTAL_MA) * actual_time_spent_ble_rx_s;

        // Cloud TX cond. power
        // PHOTON2_CURRENT_WIFI_CLOUD_TX_TOTAL_MA is the total current when the Wi-Fi radio is enabled and transmitting.
        cn_total_energy_mWs += (VOLTAGE * PHOTON2_CURRENT_WIFI_CLOUD_TX_TOTAL_MA) * actual_time_spent_cloud_tx_s;

        // 5. Calculate average power (mW)
        float cn_total_avg_power_mW = 0.0f;
        if (INTERVAL_S > 0) { 
            cn_total_avg_power_mW = static_cast<float>(cn_total_energy_mWs / INTERVAL_S);
        }

        // 6. power conversion (mWh) and accumulation
        if (cn_total_avg_power_mW > 0) {
            float cn_energy_mWh_30s = cn_total_avg_power_mW * static_cast<float>(INTERVAL_H);
            p_data_manager->SetPowerCN(p_data_manager->GetPowerCN() + cn_energy_mWh_30s);
        }

        Log.info("PE_CN (30s): TotalE=%.2fmWh (AvgPwr:%.2fmW, Idle:%.1fs, BLE_RX:%.1fs, Cloud_TX:%.1fs)",
                 p_data_manager->GetPowerCN(), cn_total_avg_power_mW, actual_time_spent_idle_s, actual_time_spent_ble_rx_s, actual_time_spent_cloud_tx_s);
        // --- LCD Updates and Cloud Publishing ---
        LCD_Message lcd_msg = LCD_Message::REFRESH;
        os_queue_put(lcd_message_queue, &lcd_msg, 0, nullptr);
        Cloud::publishPowerData();
    }

} // namespace PowerEstimator