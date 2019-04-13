/*******************************************************************************
********************************************************************************
** File Name    : Include.h
** Description  :
**      General entry point for header files
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 08/12/06(ABQ)    1.0         Initial Revision
********************************************************************************
*******************************************************************************/

/*------------------------------------------------------------------------------
|        1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
------------------------------------------------------------------------------*/

#ifndef __INCLUDE_H__
#define __INCLUDE_H__

#ifndef __STANDARD__
    #define __STANDARD__

    #define BYTE unsigned char
    #define UINT8   unsigned char
    #define UINT16  unsigned short
    #define UINT32  unsigned long
    #define UINT64  unsigned long long

    #define STATUS BYTE

/*--------------------------------------------*/
typedef void (*P_ISR_CODE)(void);

    #define XDATA

    #define INTERRUPT           __irq
    #define FAST_INTERRUPT      __fiq


    #define BASE_TYPE   UINT32  /* Size of register */
    #define LEN_TYPE    UINT32  /* Type of Size or length variable */
    #define STACK_TYPE  UINT32 XDATA    /* Type of stack element */
    /* Stack Element should be big enough to store register and pointer value */
/*--------------------------------------------*/
    #define STATUS_FALSE    0x01
    #define STATUS_TRUE     0xF0

    #define STATUS_OK       0xF0

    #define STATUS_EQUAL    0x02
    #define STATUS_GREATER  0x01
    #define STATUS_LESSTHAN 0xFF
    #define STATUS_NOTEQUAL 0xFF

    #define STATUS_ERR_LEN  0x80

    #define STATUS_OVERFLOW 0xFF

    #define STATUS_PRIO_EXIST   0x80
    #define STATUS_PRIO_INVALID 0x81
    #define STATUS_ERR_FULL     0x82

    #define STATUS_TCB_INVALID  0x82

    #define STATUS_ERR_UNDEF    0x8F

/*--------------------------------------------*/
    #define NULL    0

#endif

/* Header files here */

#include "lpc213x.h"

#include ".\Common\Header\List.h"
#include ".\Common\Header\Timer.h"
#include ".\Common\Header\RTC.h"
#include ".\Common\Header\UART.h"

#include ".\OS\Header\Task.h"
#include ".\OS\Header\OSPort.h"

#include ".\OS\Header\OS_Timer.h"
#include ".\OS\Header\OS_Schedule.h"
#include ".\OS\Header\OS_Semaphore.h"

#endif  //__INCLUDE_H__
