/**
 * @file CanHandler.c
 * @brief Implementation for processing CAN messages and extracting signals
 * @version 1.0
 * @date 2025-01-18
 * Made by Lee Chaeun *^^*
 * Made by L.SH ㅡㅡ!
 */
#include "CanHandler.h"

/**
 * @brief Extracts a signal value from CAN message data.
 */
uint32_t extractSignalValue(const uint8_t *data, uint8_t startBit, uint8_t length, bool isBigEndian) {
    uint32_t signalValue = 0;
    uint8_t byteOffset = startBit / 8;
    uint8_t bitOffset = startBit % 8;

    for (uint8_t i = 0; i < ((bitOffset + length + 7) / 8); i++) {
        uint8_t byte = data[byteOffset + i];
        if (isBigEndian) {
            signalValue = (signalValue << 8) | (uint32_t)byte;
        } else {
            signalValue |= (uint32_t)byte << (i * 8);
        }
    }

    uint8_t shift = (isBigEndian) ? (uint8_t)((8 - ((startBit + length) % 8)) % 8) : bitOffset;
    signalValue = (signalValue >> shift) & ((1U << length) - 1U);

    return signalValue;
}


/**
 * @brief Packs a signal value into CAN message data.
 */

void packSignalValue(uint8_t *data, uint8_t startBit, uint8_t length, uint32_t value, bool isBigEndian) {
    uint32_t mask = (1U << length) - 1;
    value &= mask;

    for (uint8_t i = 0; i < length; i++) {
        uint8_t bitPosition = startBit + i;
        uint8_t byteIndex = bitPosition / 8;
        uint8_t bitInByte = bitPosition % 8;

        if (value & (1U << i)) {
            data[byteIndex] |= (1U << bitInByte);
        } else {
            data[byteIndex] &= ~(1U << bitInByte);
        }
    }
}


/**
 * @brief Processes a received CAN message.
 */
void processCanMessage(uint32_t messageId, const uint8_t *data) {

    /*id 출력
    uint8_t arr[8] = {0};
    arr[0] = (uint8_t)((messageId >> 24) & 0xFF);
    arr[1] = (uint8_t)((messageId >> 16) & 0xFF);
    arr[2] = (uint8_t)((messageId >> 8) & 0xFF);
    arr[3] = (uint8_t)(messageId & 0xFF);
    Ifx_SizeT dataSize = 4;
    send_ASCLIN_UART_message((uint8_t *)arr, dataSize);
    */

    switch (messageId)
    {
        case CAN_MSG_ENGINE_Rx:
            // 신호 값을 추출
            g_canSignalData_Rx.engine_sig1 = (uint16_t)extractSignalValue(data, 0, 16, false);
            g_canSignalData_Rx.engine_sig2 = (uint8_t)extractSignalValue(data, 16, 8, false);

            /*if (g_canSignalData_Rx.engine_sig1 == 65){
                transmitCanMessage(0x103);
                IfxPort_togglePin(&MODULE_P10, 2); //디버깅용
            }
            */

            uint8_t arr1[8] = {0};
            arr1[0] = (uint8_t)((g_canSignalData_Rx.engine_sig1 >> 8) & 0xFF); // engine_sig1 상위 바이트
            arr1[1] = (uint8_t) (g_canSignalData_Rx.engine_sig1 & 0xFF);        // engine_sig1 하위 바이트
            arr1[2] = g_canSignalData_Rx.engine_sig2;                         // engine_sig2
            Ifx_SizeT dataSize1 = 3;
            send_ASCLIN_UART_message((uint8_t *)arr1, dataSize1);


            break;

        case CAN_MSG_BRAKE_Rx:
            // 신호 값을 추출
            g_canSignalData_Rx.brake_sig1 = (uint16_t)extractSignalValue(data, 0, 12, false);
            g_canSignalData_Rx.brake_sig2 = (uint8_t)extractSignalValue(data, 12, 8, false);

            uint8_t arr2[8] = {0};
            arr2[0] = (uint8_t) ((g_canSignalData_Rx.brake_sig1 >> 8) & 0xFF); // engine_sig1 상위 바이트
            arr2[1] = (uint8_t) (g_canSignalData_Rx.brake_sig1 & 0xFF);        // engine_sig1 하위 바이트
            arr2[2] = g_canSignalData_Rx.brake_sig2;                         // engine_sig2
            Ifx_SizeT dataSize2 = 3;
            send_ASCLIN_UART_message((uint8_t*) arr2, dataSize2);


            break;

        case CAN_MSG_VEHICLE_Rx:
            // 신호 값을 추출
            g_canSignalData_Rx.vehicle_sig1 = (uint16_t)extractSignalValue(data, 0, 16, false);
            g_canSignalData_Rx.vehicle_sig2 = (uint8_t)extractSignalValue(data, 16, 8, false);

            /*
            uint8_t arr3[8] = {0};
            arr3[0] = (uint8_t) ((g_canSignalData_Rx.vehicle_sig1 >> 8) & 0xFF); // engine_sig1 상위 바이트
            arr3[1] = (uint8_t) (g_canSignalData_Rx.vehicle_sig1 & 0xFF);        // engine_sig1 하위 바이트
            arr3[2] = g_canSignalData_Rx.vehicle_sig2;                         // engine_sig2
            Ifx_SizeT dataSize3 = 3;
            send_ASCLIN_UART_message((uint8_t*) arr3, dataSize3);
            */

            break;

        default:
            return;
    }
}


/**
 * @brief Prepares and transmits a CAN message.
 */

void transmitCanMessage(uint32_t messageId) {
    uint8_t data[8] = {0};

    switch (messageId) {
        case CAN_TMU_IGNsta_Tx:
            // CAN 데이터 구성
            packSignalValue(data, 0, 1, g_canSignalData_Tx.IGN1, false);
            break;

        case CAN_TMU_SeatCmd_Tx:
            packSignalValue(data, 0, 1, g_canSignalData_Tx.Seat_RelaxMode, false);
            packSignalValue(data, 1, 2, g_canSignalData_Tx.Move_FrontSeat, false);
            packSignalValue(data, 3, 2, g_canSignalData_Tx.Fold_FrontSeat, false);
            packSignalValue(data, 5, 2, g_canSignalData_Tx.Fold_BackSeat, false);
            break;

        case CAN_TMU_SHVCmd_Tx:
            packSignalValue(data, 0, 2, g_canSignalData_Tx.Cush_Heat, false);
            packSignalValue(data, 2, 2, g_canSignalData_Tx.Back_Heat, false);
            packSignalValue(data, 4, 2, g_canSignalData_Tx.SHV_VentCmd, false);
            break;

        case CAN_TMU_WindowCmd_Tx:
            packSignalValue(data, 0, 1, g_canSignalData_Tx.WindowUp, false);
            packSignalValue(data, 1, 1, g_canSignalData_Tx.WindowDown, false);
            break;

        case CAN_TMU_MP3_Tx:
            packSignalValue(data, 0, 4, g_canSignalData_Tx.MP3_Cmd, false);
            packSignalValue(data, 4, 8, g_canSignalData_Tx.MP3_Parameter, false);
            break;
        default:
            return;
    }

    // UART 전송
    /*
    Ifx_SizeT dataSize = sizeof(data);
    send_ASCLIN_UART_message((uint8_t*) data, dataSize);
    */

    // CAN 메시지 송신
    IfxMultican_Message_init(&g_multican.txMsg, messageId, *(uint32_t *)&data[0], *(uint32_t *)&data[4], 8);
    while (IfxMultican_Status_notSentBusy == IfxMultican_Can_MsgObj_sendMessage(&g_multican.canSrcMsgObj, &g_multican.txMsg)) {}
}

