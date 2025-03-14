/**
 * @file CanHandler.h
 * @brief Header file for CAN message processing and signal extraction.
 * @version 1.0
 * @date 2025-01-18
 * @author Lee Chaeun
 */

#ifndef CAN_HANDLER_H_
#define CAN_HANDLER_H_

/* Include necessary headers */
#include "IfxMultican.h"
#include <string.h>

#include "../Driver_CAN/CanDefinitions.h"
#include "../Driver_CAN/Mycan_init.h"
#include "../Driver_UART/ASCLIN_UART.h"

/* CAN 작업 플래그 */
extern bool canTaskFlag; // CAN 작업 플래그
extern uint8_t sibal_flag;

/* 함수 프로토타입 */
void processCanTasks(void);

uint32_t extractSignalValue(const uint8_t *data, uint8_t startBit, uint8_t length, bool isBigEndian);

void packSignalValue(uint8_t *data, uint8_t startBit, uint8_t length, uint32_t value, bool isBigEndian);

void processCanMessage(uint32_t messageId, const uint8_t *data);

void transmitCanMessage(uint32_t messageId);

#endif /* CAN_HANDLER_H_ */
