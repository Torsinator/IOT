// Cloud.h
#pragma once

#include "Particle.h" // Particle API 사용을 위해

namespace Cloud
{
    // DataManager에 접근해야 하므로, DataManager.h를 포함하거나
    // 함수 파라미터로 DataManager 참조를 받을 수 있습니다.
    // 여기서는 DataManager.h를 포함하고 전역 DataManager를 사용한다고 가정합니다.
    // #include "DataManager.h" // 만약 Cloud.cpp에서 DataManager를 직접 사용한다면

    void setup(); // 클라우드 관련 초기화 (필요하다면)
    void publishPowerData(); // 전력 데이터를 클라우드로 게시하는 함수

} // namespace Cloud