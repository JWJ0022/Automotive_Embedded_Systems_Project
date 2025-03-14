/**********************************************************************************************************************
 * \file main.c
 * \brief Main entry point for the application.
 * \author Lee Chaeun *^^*
 * \version 1.0
 * \date 2024-01-18
 **********************************************************************************************************************/
#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Task_sch.h"
#include "Mycan_Init.h"
#include "Bsp.h"
#include "CanDefinitions.h"
#include <string.h>
#include "ASCLIN_UART.h"
#include "Platform_Types.h"
#include "DFPlayer_driver.h"
#include "CanHandler.h"
#include "GlobalVar.h"
#include "memoryshared.h"

IfxCpu_syncEvent g_cpuSyncEvent = 0;

// 전역 변수 정의
volatile uint16_t *sharedTrackNumber = (volatile uint16_t *)SHARED_MEMORY_BASE; // 공유 트랙 번호
volatile int titleUpdated = 0;                                                 // 제목 갱신 플래그

// 트랙 번호와 제목 매핑
const char *trackTitleMap[] = {
    [1] = "Title: Song 1 by Artist",
    [2] = "Title: Song 2 by Artist",
    [3] = "Title: Song 3 by Artist",
    [4] = "Title: Song 4 by Artist",
    [5] = "Title: Song 5 by Artist"
    // 필요한 만큼 추가
};

void updateTrackNumberFromMP3 (void)
{
    uint8_t Rxdata[10];
    Ifx_SizeT len = 10;

    // MP3 플레이어에서 트랙 번호 요청
    MP3_send_cmd(0x4C, 0x0000); // 현재 트랙 요청
    delay_us(200000);           // 응답 대기

    // 트랙 번호 수신
    receive_ASCLIN_UART_message(Rxdata, &len);

    // 트랙 번호 추출
    uint16_t trackNumber = (Rxdata[5] << 8) | Rxdata[6];

    // 공유 메모리에 트랙 번호 업데이트
    *sharedTrackNumber = trackNumber;
    titleUpdated = 1; // 제목 갱신 플래그 설정
}

void initSystem (void)
{
    initMultican();
    initCanTaskScheduler();

}

int core0_main (void)
{
    IfxCpu_enableInterrupts();

    /* Disable Watchdogs (for development) */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);



    /* Rx 확인용 */
    IfxPort_setPinMode(&MODULE_P10, 2, IfxPort_Mode_outputPushPullGeneral);

    initSystem();


    /* Main loop */
    while (1)
    {
        if (canTaskFlag == L_SET)
        {
            processCanTasks();  // CAN 작업 수행
            canTaskFlag = L_CLEAR; // 플래그 클리어
        }
    }
    return 0;
}

