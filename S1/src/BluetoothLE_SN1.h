// BluetoothLE_SN1.h
#pragma once
#include "Particle.h"

namespace BluetoothLE_SN1 {

    // 데이터 타입을 식별하기 위한 enum
    enum class Sn1DataType {
        CURRENT_BRIGHTNESS, // 현재 g_brightness (실시간 제어용)
        AVERAGE_BRIGHTNESS, // 30초 평균 g_brightness
        LUX_LEVEL,           // 현재 g_lux
        BUTTON_STATE        // <<< [추가] 버튼 상태
    };

    // 큐에 저장될 데이터 패킷 구조체
    typedef struct {
        Sn1DataType type;
        uint8_t value;
    } BleSn1DataPacket;

    void setup(); // BLE 모듈 초기화 및 스레드 시작 함수
    void queueDataForSend(Sn1DataType type, uint8_t value); // 데이터를 전송 큐에 넣는 함수

} // namespace BluetoothLE_SN1