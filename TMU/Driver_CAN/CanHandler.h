#ifndef CAN_HANDLER_H_
#define CAN_HANDLER_H_

#include "Mycan_init.h"
#include "IfxMultican.h"
#include "CanDefinitions.h"
#include <string.h>
#include "ASCLIN_UART.h"

uint32_t extractSignalValue(const uint8_t *data, uint8_t startBit, uint8_t length, bool isBigEndian);
void packSignalValue(uint8_t *data, uint8_t startBit, uint8_t length, uint32_t value, bool isBigEndian);
void processCanMessage(uint32_t messageId, const uint8_t *data);
void transmitCanMessage(uint32_t messageId);




#endif
