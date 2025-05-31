// PowerEstimator.h
#pragma once

#include "Particle.h"
#include "DataManager.h"

namespace PowerEstimator
{
    // --- fixed voltage ---
    const float OPERATING_VOLTAGE_V = 3.3f; 

    // --- Argon (Sensor Node) currnet parameter (mA) ---
    // MCU on, peripherals and radio disabled
    const float ARGON_CURRENT_IDLE_MA = 3.52f;
    // Current "added" when sharing data over BLE (based on nRF52840 TX 0dBm)
    const float ARGON_CURRENT_BLE_TX_ADDITIONAL_MA = 4.8f;

    // --- Photon 2 (Control Node) currnet parameter (mA) ---
    // MCU on, peripherals and radio disabled
    const float PHOTON2_CURRENT_IDLE_MA = 23.2f;
    // When sharing data via BLE (full current based on advertising, including MCU)
    const float PHOTON2_CURRENT_BLE_RX_TOTAL_MA = 58.7f; // Using the Ible_adv value
    // When sending data to the cloud (total current based on Wi-Fi AP connection, including MCU)
    const float PHOTON2_CURRENT_WIFI_CLOUD_TX_TOTAL_MA = 60.5f; // Using  Iwifi_conn_ap

    // --- Existing actuator related constants ---
// SN1 actuator (LED)
// const float SN1_VOLTAGE = 3.3; // Replaced with OPERATING_VOLTAGE_V
    const float SN1_PWM_TO_CURRENT_SLOPE = 0.3074f;
    const float SN1_PWM_TO_CURRENT_INTERCEPT = -0.5135f;

// SN2 Actuator (Fan)
// const float SN2_VOLTAGE = 3.3; // Replace with OPERATING_VOLTAGE_V


    double ApplyTransferFunction(uint16_t duty);
    void setup(DataManager &dm);
    void processSn1PwmValue(uint8_t pwm_value);
    void processSn2PwmValue(uint8_t pwm_value_0_100);
    void calculateAndStorePowerUsage30s();

} // namespace PowerEstimator