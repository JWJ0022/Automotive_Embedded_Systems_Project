/**
 * @file CanDefinitions.h
 * @brief Definitions for CAN messages and signals.
 * @author Lee Chaeun
 * @date 2024-01-18
 */

#ifndef CAN_DEFINITIONS_H_
#define CAN_DEFINITIONS_H_

#include <stdint.h>
#include <stdbool.h>

/* CAN 메시지 정의 구조체 */
typedef struct
{
    uint32_t messageId; /**< CAN 메시지 ID */
    uint8_t dlc; /**< 데이터 길이 */
    const char *name; /**< 메시지 이름 */
} CanMessage;

/* CAN 신호 정의 구조체 */
typedef struct
{
    const char *name; /**< 신호 이름 */
    uint8_t startBit; /**< 신호 시작 비트 */
    uint8_t length; /**< 신호 길이 */
    bool isBigEndian; /**< Big Endian 여부 */
    bool isSigned; /**< Signed 여부 */
} CanSignal;

#define CAN_MSG_TMU_MP3_Rx 0x303
#define BCM_MSG_FuncSta 0x410

/* CAN 메시지 정의 */
extern const CanMessage canMessages_Tx[];
extern const CanMessage canMessages_Rx[];

/* CAN 신호 정의 */
extern const CanSignal canSignals_Tx[];
extern const CanSignal canSignals_Rx[];

typedef struct
{
    uint8_t MP3_cmd;
    /*
     *
     // MP3_cmd signal Definiton
     0x01 : 다음곡 재생
     0x02 : 이전곡 재생
     0x03 : 특정 번호 곡 재생
     0x04 : 볼륨 키우기
     0x05 : 볼륨 줄이기
     0x06 : 특정 볼륨으로 설정
     0x07 : 이퀄라이저 설정
     0x08 : 재생 모드 설정
     0x0D : 노래 재생
     0x0E : 노래 정지
     */

    uint8_t MP3_parameter;

/*
 *
 0x06 명령어 파라미터(10진수)
 0~30
 0x07 명령어 파라미터(10진수)
 0 : Normal
 1 : Pop
 2 : Rock
 3 : Jazz
 4 : Classic
 5 : Base
 0x08 명령어 파라미터(10진수)
 0 : Repeat
 1 : folder repeat
 2 : single repeat
 3 : random
 */

} CanSignalData_Rx;

typedef struct
{
    /*MP3 관련 플래그 */
    bool MP3_cmd_flag;
    bool MP3_parameter_flag;

} CanSignalFlags;

/* 전역 데이터 인스턴스 */
extern CanSignalFlags g_canSignalFlags;
extern CanSignalData_Rx g_canSignalData_Rx;

/* 함수 프로토타입 */
void processCanMessage (uint32_t messageId, const uint8_t *data);
uint32_t extractSignalValue (const uint8_t *data, uint8_t startBit, uint8_t length, bool isBigEndian);
void packSignalValue (uint8_t *data, uint8_t startBit, uint8_t length, uint32_t value, bool isBigEndian);

#endif /* CAN_DEFINITIONS_H_ */
