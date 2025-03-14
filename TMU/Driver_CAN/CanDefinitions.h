#ifndef CAN_DEFINITIONS_H
#define CAN_DEFINITIONS_H

#include <stdint.h>
#include <stdbool.h>

// CAN 메시지 정의 구조체
typedef struct {
    uint32_t messageId;      // CAN 메시지 ID
    uint8_t dlc;             // 데이터 길이
    const char *name;        // 메시지 이름
} CanMessage;

// CAN 신호 정의 구조체
typedef struct {
    const char *name;        // 신호 이름
    uint8_t startBit;        // 신호 시작 비트
    uint8_t length;          // 신호 길이
    bool isBigEndian;        // Big Endian 여부
    bool isSigned;           // Signed 여부
} CanSignal;

// 메시지 ID 매핑
#define CAN_TMU_IGNsta_Tx  0x100
#define CAN_TMU_SeatCmd_Tx   0x300
#define CAN_TMU_SHVCmd_Tx 0x301
#define CAN_TMU_WindowCmd_Tx 0x302
#define CAN_TMU_MP3_Tx 0x303

#define CAN_MSG_ENGINE_Rx  0x61
#define CAN_MSG_BRAKE_Rx   0x62
#define CAN_MSG_VEHICLE_Rx 0x63

// CAN 메시지 정의
extern const CanMessage canMessages_Tx[];
extern const CanMessage canMessages_Rx[];

// CAN 신호 정의
extern const CanSignal canSignals_Tx[];
extern const CanSignal canSignals_Rx[];

// 전역 데이터 구조체 Ex
typedef struct {
    uint8_t IGN1;

    uint8_t Seat_RelaxMode;
    uint8_t Move_FrontSeat;
    uint8_t Fold_FrontSeat;
    uint8_t Fold_BackSeat;

    uint8_t Cush_Heat;
    uint8_t Back_Heat;
    uint8_t SHV_VentCmd;

    uint8_t WindowUp;
    uint8_t WindowDown;

    uint8_t MP3_Cmd;
    uint8_t MP3_Parameter;
} CanSignalData_Tx;

typedef struct {
    uint16_t engine_sig1;
    uint8_t engine_sig2;
    uint16_t brake_sig1;
    uint8_t brake_sig2;
    uint16_t vehicle_sig1;
    uint8_t vehicle_sig2;
} CanSignalData_Rx;

// 전역 데이터 인스턴스
extern CanSignalData_Tx g_canSignalData_Tx;
extern CanSignalData_Rx g_canSignalData_Rx;

// 함수 프로토타입
void processCanMessage(uint32_t messageId, const uint8_t *data);
uint32_t extractSignalValue(const uint8_t *data, uint8_t startBit, uint8_t length, bool isBigEndian);
void packSignalValue(uint8_t *data, uint8_t startBit, uint8_t length, uint32_t value, bool isBigEndian);

#endif // CAN_DEFINITIONS_H
