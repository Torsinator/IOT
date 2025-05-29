// PowerEstimator.h
#pragma once

#include "Particle.h"
#include "DataManager.h"

namespace PowerEstimator
{
    // --- 기본 전압 ---
    const float OPERATING_VOLTAGE_V = 3.3f; // 모든 장치에 공통적으로 사용될 전압

    // --- Argon (Sensor Node) 전류 상수 (mA) ---
    // MCU on, peripherals and radio disabled (기본 MCU 작동)
    const float ARGON_CURRENT_IDLE_MA = 3.52f;
    // BLE로 데이터 공유 시 "추가되는" 전류 (nRF52840 TX 0dBm 기준)
    const float ARGON_CURRENT_BLE_TX_ADDITIONAL_MA = 4.8f;

    // --- Photon 2 (Control Node) 전류 상수 (mA) ---
    // MCU on, peripherals and radio disabled (기본 MCU 작동)
    const float PHOTON2_CURRENT_IDLE_MA = 23.2f;
    // BLE로 데이터 공유받을 때 (Advertising 기준 전체 전류, MCU 포함)
    const float PHOTON2_CURRENT_BLE_RX_TOTAL_MA = 58.7f; // Ible_adv 값 사용
    // 클라우드에 데이터 보낼 때 (Wi-Fi AP 연결 기준 전체 전류, MCU 포함)
    const float PHOTON2_CURRENT_WIFI_CLOUD_TX_TOTAL_MA = 60.5f; // Iwifi_conn_ap 값 사용

    // --- 기존 액추에이터 관련 상수 (유지) ---
    // SN1 액추에이터(LED)
    // const float SN1_VOLTAGE = 3.3; // OPERATING_VOLTAGE_V 로 대체
    const float SN1_PWM_TO_CURRENT_SLOPE = 0.3074f;
    const float SN1_PWM_TO_CURRENT_INTERCEPT = -0.5135f;

    // SN2 액추에이터(Fan)
    // const float SN2_VOLTAGE = 3.3; // OPERATING_VOLTAGE_V 로 대체

    // --- 함수 선언 (유지) ---
    double ApplyTransferFunction(uint16_t duty);
    void setup(DataManager &dm);
    void processSn1PwmValue(uint8_t pwm_value);
    void processSn2PwmValue(uint8_t pwm_value_0_100); // 함수명 오타 수정 (pwm_value -> pwm_value_0_100)
    void calculateAndStorePowerUsage30s();

} // namespace PowerEstimator