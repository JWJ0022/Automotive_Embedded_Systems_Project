/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "../Driver_UART/ASCLIN_UART.h"

#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
#include <stdint.h>

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define MP3_UART_BAUDRATE           9600                                  /* UART baud rate in bit/s                  */

#define MP3_UART_PIN_RX             IfxAsclin2_RXE_P33_8_IN                 /* UART receive port pin*/
#define MP3_UART_PIN_TX             IfxAsclin2_TX_P33_9_OUT                 /* UART transmit port pin*/

/* Definition of the interrupt priorities */
#define INTPRIO_ASCLIN2_RX      3     //mp3
#define INTPRIO_ASCLIN2_TX      4


#define UART_RX_BUFFER_SIZE     256                                      /* Definition of the receive buffer size    */
#define UART_TX_BUFFER_SIZE     256                                      /* Definition of the transmit buffer size   */
#define SIZE                    1                                      /* Size of the string                       */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
/* Declaration of the ASC handle */
static IfxAsclin_Asc MP3_g_ascHandle;

/* Declaration of the FIFOs parameters */
static uint8 MP3_g_ascTxBuffer[UART_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
static uint8 MP3_g_ascRxBuffer[UART_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

Ifx_SizeT receivedCount = 10;
uint8_t RxFlag = 0;
uint8_t Rxdata[10] = {0};

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Adding of the interrupt service routines */

volatile boolean uartRXInterruptOccurred = FALSE;

IFX_INTERRUPT(asclin2TxISR, 0, INTPRIO_ASCLIN2_TX);
IFX_INTERRUPT(asclin2RxISR, 0, INTPRIO_ASCLIN2_RX);

void asclin2TxISR(void)
{
    //IfxPort_togglePin(&MODULE_P10, 2);
    IfxAsclin_Asc_isrTransmit(&MP3_g_ascHandle);
}

void asclin2RxISR(void)
{
    //IfxPort_togglePin(&MODULE_P10, 2);
    IfxAsclin_Asc_isrReceive(&MP3_g_ascHandle);
}


/* This function initializes the ASCLIN UART module */
void init_ASCLIN_UART(void)
{
    IfxAsclin_Asc_Config MP3_ascConfig;
    IfxAsclin_Asc_initModuleConfig(&MP3_ascConfig, &MODULE_ASCLIN2);

    MP3_ascConfig.baudrate.baudrate = MP3_UART_BAUDRATE;

    MP3_ascConfig.interrupt.txPriority = INTPRIO_ASCLIN2_TX;
    MP3_ascConfig.interrupt.rxPriority = INTPRIO_ASCLIN2_RX;
    MP3_ascConfig.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    MP3_ascConfig.txBuffer = &MP3_g_ascTxBuffer;
    MP3_ascConfig.txBufferSize = UART_TX_BUFFER_SIZE;
    MP3_ascConfig.rxBuffer = &MP3_g_ascRxBuffer;
    MP3_ascConfig.rxBufferSize = UART_RX_BUFFER_SIZE;

    const IfxAsclin_Asc_Pins MP3_pins =
    {
        NULL_PTR,       IfxPort_InputMode_pullUp,     /* CTS pin not used */
        &MP3_UART_PIN_RX,   IfxPort_InputMode_pullUp,     /* RX pin           */
        NULL_PTR,       IfxPort_OutputMode_pushPull,  /* RTS pin not used */
        &MP3_UART_PIN_TX,   IfxPort_OutputMode_pushPull,  /* TX pin           */
        IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    MP3_ascConfig.pins = &MP3_pins;

    IfxAsclin_Asc_initModule(&MP3_g_ascHandle, &MP3_ascConfig);
}

void send_UART_Message_To_MP3(uint8_t sendData[10])
{
    Ifx_SizeT sendLen = 10;

    IfxAsclin_Asc_write(&MP3_g_ascHandle, sendData, &sendLen, TIME_INFINITE);   /* Transmit data via TX */
}


void receive_ASCLIN_UART_message(uint8_t *recData, Ifx_SizeT *recCount)
{
    IfxAsclin_Asc_read(&MP3_g_ascHandle, recData, recCount, TIME_INFINITE);    /* Receive data via RX  */
}
