// PowerEstimator.h
#pragma once

#include "Particle.h"
#include "DataManager.h" // DataManager에 접근해야 할 수 있음 (또는 콜백 사용)

namespace PowerEstimator
{

    // SN1 액추에이터(LED) 전력 계산을 위한 상수
    const float SN1_VOLTAGE = 3.3; // 볼트 (V)
    // 전달 함수 계수 (y = ax + b, y는 전류 mA, x는 PWM 값)
    const float SN1_PWM_TO_CURRENT_SLOPE = 0.3074;
    const float SN1_PWM_TO_CURRENT_INTERCEPT = -0.5135;

    // SN2 액추에이터(Fan) 전력 계산을 위한 상수
    const float SN2_VOLTAGE = 3.3; // 볼트 (V) - SN1과 동일하지만 명시적으로 선언

    double ApplyTransferFunction(uint16_t duty);

    // (SN2용 상수도 필요하다면 여기에 추가)
    // const float SN2_VOLTAGE = ...;
    // const float SN2_PWM_TO_CURRENT_SLOPE = ...;
    // const float SN2_PWM_TO_CURRENT_INTERCEPT = ...;

    void setup(DataManager &dm);                // 초기화 함수 (DataManager 참조 전달)
    void processSn1PwmValue(uint8_t pwm_value); // SN1의 PWM 값을 받아 순간 전력 계산 및 누적
    void processSn2PwmValue(uint8_t pwm_value); // SN2의 PWM 값을 받아 순간 전력 계산 및 누적 <--- 추가



    void calculateAndStorePowerUsage30s(); // 30초마다 호출되어 누적 전력량 계산 및 저장

    // DataManager를 직접 참조하는 대신, 계산된 전력량을 반환하거나
    // 콜백 함수를 통해 Control_Node에 알리는 방식을 사용할 수도 있습니다.
    // 여기서는 DataManager를 직접 업데이트하는 방식으로 우선 제안합니다.

} // namespace PowerEstimator