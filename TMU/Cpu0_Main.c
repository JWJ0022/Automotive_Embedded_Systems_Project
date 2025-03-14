/**********************************************************************************************************************
 * \file main.c
 * \brief Main entry point for the application.
 * \author Lee Chaeun *^^*
 * \version 1.0
 * \date 2024-01-18
 **********************************************************************************************************************/

#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Task_sch.h"
#include "Mycan_Init.h"
#include "Bsp.h"
#include "CanDefinitions.h"
#include "ASCLIN_UART.h"
#include "SPI_CPU.h"
#include "Forwarding.h"
#include <string.h>

IfxCpu_syncEvent g_cpuSyncEvent = 0;

void initSystem(void)
{
    IfxPort_setPinMode(&MODULE_P10, 2, IfxPort_Mode_outputPushPullGeneral);

    initMultican();
//    init_ASCLIN_UART();
    initQSPI();
    initCanTaskScheduler();
    initForwarding();
}

int core0_main(void)
{
    IfxCpu_enableInterrupts();

    /* Disable Watchdogs (for development) */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    /* Wait for CPU sync event */
    IfxCpu_emitEvent(&g_cpuSyncEvent);
    IfxCpu_waitEvent(&g_cpuSyncEvent, 1);

    initSystem();

    volatile uint8 BufCount = 0;

    /* Main loop */
    while (1)
    {
        if(g_SPIFlag == 1)
        {
            g_SPIFlag = 0;
            CANBuf[BufCount] = getSPIBuf();
            ++BufCount;
            if(BufCount > 3)
            {
                if(CANBuf[1] != 0x00 && CANBuf[0] == 0x00)
                {
                    CANBuf[0] = CANBuf[1];
                    CANBuf[1] = CANBuf[2];
                    BufCount = 3;
                }
                else
                {
                    forwardingSPI();
                    BufCount = 0;
                }
            }
        }
    }

    return 0;
}
