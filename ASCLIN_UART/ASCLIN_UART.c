/*********************************************************************************************************************/
/*-----------------------------------------------------Includes------------------------------------------------------*/
/*********************************************************************************************************************/
#include "IfxAsclin_Asc.h"
#include "IfxCpu_Irq.h"
#include "ASCLIN_UART.h"

/*********************************************************************************************************************/
/*------------------------------------------------------Macros-------------------------------------------------------*/
/*********************************************************************************************************************/
#define MP3_UART_BAUDRATE           9600                                  /* UART baud rate in bit/s                  */
#define UART_USB_BAUDRATE           9600
#define ESP32_UART_BAUDRATE         9600

#define MP3_UART_PIN_RX             IfxAsclin2_RXE_P33_8_IN                 /* UART receive port pin*/
#define MP3_UART_PIN_TX             IfxAsclin2_SCLK_P33_9_OUT                 /* UART transmit port pin*/

#define ESP32_UART_PIN_RX           IfxAsclin1_RXA_P15_1_IN
#define ESP32_UART_PIN_TX           IfxAsclin1_TX_P15_0_OUT

#define UART_USB_RX             IfxAsclin3_RXD_P32_2_IN
#define UART_USB_TX             IfxAsclin3_TX_P15_7_OUT

#define LED                     &MODULE_P10, 2

/* Definition of the interrupt priorities */
#define INTPRIO_ASCLIN0_RX      7     //mp3
#define INTPRIO_ASCLIN0_TX      8

#define INTPRIO_ASCLIN1_RX      5     //ESP32
#define INTPRIO_ASCLIN1_TX      6

#define INTPRIO_ASCLIN3_RX      9     //putty
#define INTPRIO_ASCLIN3_TX      10

#define UART_RX_BUFFER_SIZE     256                                      /* Definition of the receive buffer size    */
#define UART_TX_BUFFER_SIZE     256                                      /* Definition of the transmit buffer size   */
#define SIZE                    1                                      /* Size of the string                       */

/*********************************************************************************************************************/
/*-------------------------------------------------Global variables--------------------------------------------------*/
/*********************************************************************************************************************/
/* Declaration of the ASC handle */
static IfxAsclin_Asc MP3_g_ascHandle;
static IfxAsclin_Asc g_ascHandle_usb;
static IfxAsclin_Asc ESP32_g_ascHandle;

/* Declaration of the FIFOs parameters */
static uint8 MP3_g_ascTxBuffer[UART_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
static uint8 MP3_g_ascRxBuffer[UART_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

static uint8 ESP32_g_ascTxBuffer[UART_TX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];
static uint8 ESP32_g_ascRxBuffer[UART_RX_BUFFER_SIZE + sizeof(Ifx_Fifo) + 8];

/*********************************************************************************************************************/
/*---------------------------------------------Function Implementations----------------------------------------------*/
/*********************************************************************************************************************/
/* Adding of the interrupt service routines */

volatile boolean uartRXInterruptOccurred = FALSE;

IFX_INTERRUPT(asclin0TxISR, 0, INTPRIO_ASCLIN0_TX);
void asclin0TxISR(void)
{
    IfxAsclin_Asc_isrTransmit(&MP3_g_ascHandle);
}

IFX_INTERRUPT(asclin0RxISR, 0, INTPRIO_ASCLIN0_RX);
void asclin0RxISR(void)
{
    IfxAsclin_Asc_isrReceive(&MP3_g_ascHandle);
}

IFX_INTERRUPT(asclin1TxISR, 0, INTPRIO_ASCLIN1_TX);
void asclin1TxISR(void)
{
    IfxAsclin_Asc_isrTransmit(&ESP32_g_ascHandle);
}

IFX_INTERRUPT(asclin1RxISR, 0, INTPRIO_ASCLIN1_RX);
void asclin1RxISR(void)
{
    //ESP32로부터 UART 수신 -> 플래그 1 -> main에서 제어로직 수행
    IfxAsclin_Asc_isrReceive(&ESP32_g_ascHandle);
    uartRXInterruptOccurred = TRUE;
}


IFX_INTERRUPT(asclin3TxISR, 0, INTPRIO_ASCLIN3_TX);
void asclin3TxISR(void)
{
    IfxAsclin_Asc_isrTransmit(&g_ascHandle_usb);
}

IFX_INTERRUPT(asclin3RxISR, 0, INTPRIO_ASCLIN3_RX);
void asclin3RxISR(void)
{
    IfxAsclin_Asc_isrReceive(&g_ascHandle_usb);
}

/* This function initializes the ASCLIN UART module */
void init_ASCLIN_UART(void)
{
    IfxAsclin_Asc_Config MP3_ascConfig;
    IfxAsclin_Asc_initModuleConfig(&MP3_ascConfig, &MODULE_ASCLIN0);

    MP3_ascConfig.baudrate.baudrate = MP3_UART_BAUDRATE;

    MP3_ascConfig.interrupt.txPriority = INTPRIO_ASCLIN0_TX;
    MP3_ascConfig.interrupt.rxPriority = INTPRIO_ASCLIN0_RX;
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

    //////////////////////////////////////////////////////////////////////////////////////////////////////////
    IfxAsclin_Asc_Config ESP32_ascConfig;
    IfxAsclin_Asc_initModuleConfig(&ESP32_ascConfig, &MODULE_ASCLIN1);

    ESP32_ascConfig.baudrate.baudrate = ESP32_UART_BAUDRATE;
    ESP32_ascConfig.interrupt.txPriority = INTPRIO_ASCLIN1_TX;
    ESP32_ascConfig.interrupt.rxPriority = INTPRIO_ASCLIN1_RX;
    ESP32_ascConfig.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    ESP32_ascConfig.txBuffer = &ESP32_g_ascTxBuffer;
    ESP32_ascConfig.txBufferSize = UART_TX_BUFFER_SIZE;
    ESP32_ascConfig.rxBuffer = &ESP32_g_ascRxBuffer;
    ESP32_ascConfig.rxBufferSize = UART_RX_BUFFER_SIZE;

    const IfxAsclin_Asc_Pins ESP32_pins =
    {
        NULL_PTR,       IfxPort_InputMode_pullUp,     /* CTS pin not used */
        &ESP32_UART_PIN_RX,   IfxPort_InputMode_pullUp,     /* RX pin           */
        NULL_PTR,       IfxPort_OutputMode_pushPull,  /* RTS pin not used */
        &ESP32_UART_PIN_TX,   IfxPort_OutputMode_pushPull,  /* TX pin           */
        IfxPort_PadDriver_cmosAutomotiveSpeed1
    };

    ESP32_ascConfig.pins = &ESP32_pins;
    IfxAsclin_Asc_initModule(&ESP32_g_ascHandle, &ESP32_ascConfig);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    IfxAsclin_Asc_Config ascConfig_usb;
    IfxAsclin_Asc_initModuleConfig(&ascConfig_usb, &MODULE_ASCLIN3);


    ascConfig_usb.baudrate.baudrate = UART_USB_BAUDRATE;
    ascConfig_usb.interrupt.txPriority = INTPRIO_ASCLIN3_TX;
    ascConfig_usb.interrupt.rxPriority = INTPRIO_ASCLIN3_RX;
    ascConfig_usb.interrupt.typeOfService = IfxCpu_Irq_getTos(IfxCpu_getCoreIndex());

    ascConfig_usb.txBuffer = &MP3_g_ascTxBuffer;
    ascConfig_usb.txBufferSize = UART_TX_BUFFER_SIZE;
    ascConfig_usb.rxBuffer = &MP3_g_ascRxBuffer;
    ascConfig_usb.rxBufferSize = UART_RX_BUFFER_SIZE;

    const IfxAsclin_Asc_Pins pins_usb =
    {
        NULL_PTR,       IfxPort_InputMode_pullUp,     /* CTS pin not used */
        &UART_USB_RX,   IfxPort_InputMode_pullUp,     /* RX pin           */
        NULL_PTR,       IfxPort_OutputMode_pushPull,  /* RTS pin not used */
        &UART_USB_TX,   IfxPort_OutputMode_pushPull,  /* TX pin           */
        IfxPort_PadDriver_cmosAutomotiveSpeed1
    };
    ascConfig_usb.pins = &pins_usb;

    IfxAsclin_Asc_initModule(&g_ascHandle_usb, &ascConfig_usb); /* Initialize module with above parameters */
}

void send_UART_Message_To_MP3(uint8 sendData[10])
{
    Ifx_SizeT sendLen = 10;
    IfxAsclin_Asc_write(&MP3_g_ascHandle, sendData, &sendLen, TIME_INFINITE);   /* Transmit data via TX */
}

void send_UART_Message_To_PC(uint8 *sendData)
{
    Ifx_SizeT sendLen = 1;
    IfxAsclin_Asc_write(&g_ascHandle_usb, sendData, &sendLen, TIME_INFINITE);   /* Transmit data via TX */
}

void receive_ASCLIN_UART_message(uint8 *recData, Ifx_SizeT *recCount)
{
    IfxAsclin_Asc_read(&ESP32_g_ascHandle, recData, recCount, TIME_INFINITE);    /* Receive data via RX  */
}
