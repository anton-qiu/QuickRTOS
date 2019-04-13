/*******************************************************************************
********************************************************************************
** File Name    : UART.h
** Description  :
**      Definitions and Declaration of UART library
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 26/02/07(ABQ)    1.0         Initial Revision
********************************************************************************
*******************************************************************************/

/*------------------------------------------------------------------------------
|        1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
------------------------------------------------------------------------------*/

#include "INCLUDE.h"

#ifndef __UART_H__
#define __UART_H__

/**********************/
/* Value Definitions  */
/**********************/

#define UART0_INT_EN_MASK   0x00000040
#define UART1_INT_EN_MASK   0x00000080

#define UART0_INT_NUMBER    0x06
#define UART1_INT_NUMBER    0x07

#define UART_DLAB_MASK      0x80

#define UART_7BIT_MODE      0x02
#define UART_8BIT_MODE      0x03

#define UART_1STOP_MODE     0x00
#define UART_2STOP_MODE     0x04

#define UART_PARITY_EVEN    0x18
#define UART_PARITY_ODD     0x08
#define UART_PARITY_NONE    0x00

#define UART_SPEED_9600     0x00
#define UART_SPEED_19200    0x01
#define UART_SPEED_38400    0x02
#define UART_SPEED_57600    0x03
#define UART_SPEED_115200   0x04

#define UART_STAT_THRE      0x20
#define UART_STAT_RDR       0x01


/************************/
/* Data Structure       */
/************************/

/************************/
/* Variable Declaration */
/************************/

/**********************/
/* Function Prototype */
/**********************/
extern void UART0_Connect(P_ISR_CODE pISRCode, BYTE bPriority);
extern void UART0_SetRate(BYTE bBAUD, BYTE bMode);
extern STATUS UART0_IsRx(void);
extern STATUS UART0_IsTxEmpty(void);
extern void UART0_SendByte(BYTE bData);
extern void UART0_SendByteWait(BYTE bData);
extern BYTE UART0_GetByte(void);
extern void UART0_Clear(void);	/* Clear all data in FIFO buffer. */
extern void UART0_SetRxInt(P_ISR_CODE pISRCode);
extern void UART0_SetErrInt(P_ISR_CODE pISRCode);
extern void UART0_SetTxInt(P_ISR_CODE pISRCode);
extern void UART0_ISR(void) INTERRUPT;

//--------------------------------------------------

extern void UART1_Connect(P_ISR_CODE pISRCode, BYTE bPriority);
extern void UART1_SetRate(BYTE bBAUD, BYTE bMode);
extern STATUS UART1_IsRx(void);
extern STATUS UART1_IsTxEmpty(void);
extern void UART1_SendByte(BYTE bData);
extern void UART1_SendByteWait(BYTE bData);
extern BYTE UART1_GetByte(void);
extern void UART1_Clear(void);	/* Clear all data in FIFO buffer. */
extern void UART1_SetRxInt(P_ISR_CODE pISRCode);
extern void UART1_SetErrInt(P_ISR_CODE pISRCode);
extern void UART1_SetTxInt(P_ISR_CODE pISRCode);
extern void UART1_ISR(void) INTERRUPT;


#endif  //__UART_H__
