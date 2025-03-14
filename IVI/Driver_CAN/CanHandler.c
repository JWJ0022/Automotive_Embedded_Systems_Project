/**
 * @file CanHandler.c
 * @brief Implementation for processing CAN messages and extracting signals
 * @version 1.0
 * @date 2025-01-18
 * Made by Lee Chaeun *^^*
 * Made by L.SH ㅡㅡ!
 */
#include "../Driver_CAN/CanHandler.h"

#include "../Driver_UART/ASCLIN_UART.h"
#include "GlobalVar.h"
#include "DFPlayer_driver.h"

/* CAN 작업 플래그 */
bool canTaskFlag = false;
volatile int SongFlag = 0;
volatile int startflag = 0;

/**
 * @brief Extracts a signal value from CAN message data.
 */
uint32_t extractSignalValue (const uint8_t *data, uint8_t startBit, uint8_t length, bool isBigEndian)
{
    uint32_t signalValue = 0;
    uint8_t byteOffset = startBit / 8;
    uint8_t bitOffset = startBit % 8;

    for (uint8_t i = 0; i < ((bitOffset + length + 7) / 8); i++)
    {
        uint8_t byte = data[byteOffset + i];
        if (isBigEndian)
        {
            signalValue = (signalValue << 8) | (uint32_t) byte;
        }
        else
        {
            signalValue |= (uint32_t) byte << (i * 8);
        }
    }

    uint8_t shift = (isBigEndian) ? (uint8_t) ((8 - ((startBit + length) % 8)) % 8) : bitOffset;
    signalValue = (signalValue >> shift) & ((1U << length) - 1U);

    return signalValue;
}

/**
 * @brief Packs a signal value into CAN message data.
 */

void packSignalValue (uint8_t *data, uint8_t startBit, uint8_t length, uint32_t value, bool isBigEndian)
{
    uint32_t mask = (1U << length) - 1;
    value &= mask;

    for (uint8_t i = 0; i < length; i++)
    {
        uint8_t bitPosition = startBit + i;
        uint8_t byteIndex = bitPosition / 8;
        uint8_t bitInByte = bitPosition % 8;

        if (value & (1U << i))
        {
            data[byteIndex] |= (1U << bitInByte);
        }
        else
        {
            data[byteIndex] &= ~(1U << bitInByte);
        }
    }
}

/**
 * @brief Processes a received CAN message.
 */
void processCanMessage (uint32_t messageId, const uint8_t *data)
{
    //IfxPort_togglePin(&MODULE_P10,2);
    switch (messageId)
    {
        case CAN_MSG_TMU_MP3_Rx :
            //IfxPort_togglePin(&MODULE_P10,2);
            g_canSignalData_Rx.MP3_cmd = (uint8_t) extractSignalValue(data, 0, 4, false);
            g_canSignalData_Rx.MP3_parameter = (uint8_t) extractSignalValue(data, 4, 8, false);
            g_canSignalFlags.MP3_cmd_flag = L_SET;
            g_canSignalFlags.MP3_parameter_flag = L_SET;
            break;

        default :
            break;
    }

    canTaskFlag = L_SET;
}

/**
 * @brief Prepares and transmits a CAN message.
 */

void processCanTasks (void)
{
    if (g_canSignalFlags.MP3_cmd_flag == L_SET)
    {
        switch (g_canSignalData_Rx.MP3_cmd)
        {
            case 1 :
                MP3_send_cmd(0x01, 0x0000);
                delay_us(200000);
                MP3_send_cmd(0x06, 15);
                startflag = 0;
                SongFlag++;
                break;
            case 2 :
                MP3_send_cmd(0x02, 0x0000);
                break;
            case 4 :
                MP3_send_cmd(0x04, 0x0000);
                break;
            case 5 :
                MP3_send_cmd(0x05, 0x0000);
                break;
            case 6 :
                MP3_send_cmd(0x06, g_canSignalData_Rx.MP3_parameter);
                break;
            case 7 :
                MP3_send_cmd(0x07, g_canSignalData_Rx.MP3_parameter);
                break;
            case 8 :
                MP3_send_cmd(0x08, g_canSignalData_Rx.MP3_parameter);
                break;
            case 13 :
                MP3_send_cmd(0X0D, 0x0000);
                startflag = 1;
                break;
            case 14 :
                MP3_send_cmd(0x0E, 0x0000);
                break;
            default :
                break;
        }
        g_canSignalFlags.MP3_cmd_flag = L_CLEAR; // 플래그 초기화
    }
}

