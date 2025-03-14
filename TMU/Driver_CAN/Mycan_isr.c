#include "Mycan_isr.h"
#include "Mycan_init.h"
#include "Bsp.h"
#include "CanDefinitions.h"
//#include "ASCLIN_UART.h"
#include <stdint.h>


IFX_INTERRUPT(canIsrTxHandler, 0, ISR_PRIORITY_CAN_TX);
IFX_INTERRUPT(canIsrRxHandler, 0, ISR_PRIORITY_CAN_RX);

void canIsrTxHandler(void)
{
    //IfxPort_togglePin(&MODULE_P00, 5); // 디버깅용
}

void canIsrRxHandler(void)
{
    IfxMultican_Status readStatus;
    uint8_t data[8];

    //to do:: 인터럽트 flag 초기화

    //IfxPort_togglePin(&MODULE_P10, 2); // 디버깅용

    // 수신 메시지 읽기
    readStatus = IfxMultican_Can_MsgObj_readMessage(&g_multican.canDstMsgObj, &g_multican.rxMsg);

    if (!(readStatus & IfxMultican_Status_newData)) {
        while (1);  // 에러 처리
    }

    if (readStatus == IfxMultican_Status_newDataButOneLost) {
        while (1);  // 데이터 손실 처리
    }

    if (readStatus & IfxMultican_Status_newData) {

            data[0] =  g_multican.rxMsg.data[0] & 0xFF;
            data[1] = (g_multican.rxMsg.data[0] >> 8) & 0xFF;
            data[2] = (g_multican.rxMsg.data[0] >> 16) & 0xFF;
            data[3] = (g_multican.rxMsg.data[0] >> 24) & 0xFF;
            data[4] =  g_multican.rxMsg.data[1] & 0xFF;
            data[5] = (g_multican.rxMsg.data[1] >> 8) & 0xFF;
            data[6] = (g_multican.rxMsg.data[1] >> 16) & 0xFF;
            data[7] = (g_multican.rxMsg.data[1] >> 24) & 0xFF;

        // CAN 메시지 처리
        processCanMessage(g_multican.rxMsg.id, data);
    }
}
