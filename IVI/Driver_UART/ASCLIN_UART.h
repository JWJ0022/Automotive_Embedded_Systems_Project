#ifndef ASCLIN_UART_H_
#define ASCLIN_UART_H_

#include "Platform_Types.h"
#include "Ifx_Types.h"
#include <stdint.h>

extern volatile boolean uartRXInterruptOccurred;

/*********************************************************************************************************************/
/*------------------------------------------------Function Prototypes------------------------------------------------*/
/*********************************************************************************************************************/
void init_ASCLIN_UART(void);                 /* Initialization function   */
void send_UART_Message_To_MP3(uint8 sendData[10]);
void send_UART_Message_To_PC(uint8 *sendData);
void receive_ASCLIN_UART_message(uint8 *recData, Ifx_SizeT *recCount);

extern Ifx_SizeT receivedCount;
extern uint8_t  RxFlag;
extern uint8_t Rxdata[10];

#endif /* ASCLIN_UART_H_ */
