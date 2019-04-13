/*******************************************************************************
********************************************************************************
** File Name    : Timer.h
** Description  : Header for Timer functionality.
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 30/03/07(ABQ)    1.0         Initial Revision
********************************************************************************
*******************************************************************************/

/*------------------------------------------------------------------------------
|        1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
------------------------------------------------------------------------------*/

#include "INCLUDE.h"

#ifndef __TIMER_H__
#define __TIMER_H__

/***********************/
/* OPTIMIZATION OPTION */
/***********************/

/**********************/
/* Value Definitions  */
/**********************/
#define TIMER_MODE_PCLK    0x00

#define TIMER_CNTR_INT_ON_MR0   0x0001
#define TIMER_CNTR_RST_ON_MR0   0x0002
#define TIMER_CNTR_STOP_ON_MR0  0x0004

#define TIMER_CNTR_INT_ON_MR1   0x0008
#define TIMER_CNTR_RST_ON_MR1   0x0010
#define TIMER_CNTR_STOP_ON_MR1  0x0020

#define TIMER_CNTR_INT_ON_MR2   0x0040
#define TIMER_CNTR_RST_ON_MR2   0x0080
#define TIMER_CNTR_STOP_ON_MR2  0x0100

#define TIMER_CNTR_INT_ON_MR3   0x0200
#define TIMER_CNTR_RST_ON_MR3   0x0400
#define TIMER_CNTR_STOP_ON_MR3  0x0800

#define TIMER_CNTR0_MODE (TIMER_CNTR_RST_ON_MR0|TIMER_CNTR_INT_ON_MR0)
#define TIMER_CNTR1_MODE (TIMER_CNTR_INT_ON_MR1)
#define TIMER_CNTR2_MODE (TIMER_CNTR_INT_ON_MR2)
#define TIMER_CNTR3_MODE (TIMER_CNTR_INT_ON_MR3)

/************************/
/* Data Structure       */
/************************/
typedef struct S_TIMER
{
    BYTE bDummy;
} TS_TIMER;

#define TIMER_TYPE UINT32

/************************/
/* Variable Declaration */
/************************/

/**********************/
/* Function Prototype */
/**********************/
extern void Timer0_ISR(void) INTERRUPT; /* Default Timer 0 ISR Dispatchers for Counter 0-3 */

extern void Timer0_Connect(P_ISR_CODE pISRCode, BYTE bPriority);
extern void Timer0_Enable(void);
extern void Timer0_Disable(void);
extern void Timer0_Reset(void); /* Reset the timer counter. */
extern void Timer0_ClearInt(void);  /* Clear Interrupt pending of Timer 0 */

extern void Timer0_InitPrescalerMode(TIMER_TYPE xPrescaler, BYTE bMode);

extern void Timer0_SetCounter0(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);
extern void Timer0_SetCounter1(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);
extern void Timer0_SetCounter2(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);
extern void Timer0_SetCounter3(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);

extern TIMER_TYPE Timer0_GetMaxCounter0(void);
extern TIMER_TYPE Timer0_GetMaxCounter1(void);
extern TIMER_TYPE Timer0_GetMaxCounter2(void);
extern TIMER_TYPE Timer0_GetMaxCounter3(void);

extern BASE_TYPE Timer0_GetTick(void);

//-----------------------------------------

extern void Timer1_ISR(void) INTERRUPT; /* Default Timer 1 ISR Dispatchers for Counter 0-3 */

extern void Timer1_Connect(P_ISR_CODE pISRCode, BYTE bPriority);
extern void Timer1_Enable(void);
extern void Timer1_Disable(void);
extern void Timer1_Reset(void); /* Reset the timer counter. */
extern void Timer1_ClearInt(void);  /* Clear Interrupt pending of Timer 0 */

extern void Timer1_InitPrescalerMode(TIMER_TYPE xPrescaler, BYTE bMode);

extern void Timer1_SetCounter0(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);
extern void Timer1_SetCounter1(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);
extern void Timer1_SetCounter2(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);
extern void Timer1_SetCounter3(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, UINT16 wMODE);

extern TIMER_TYPE Timer1_GetMaxCounter0(void);
extern TIMER_TYPE Timer1_GetMaxCounter1(void);
extern TIMER_TYPE Timer1_GetMaxCounter2(void);
extern TIMER_TYPE Timer1_GetMaxCounter3(void);

extern BASE_TYPE Timer1_GetTick(void);

#endif  /* __TIMER_H__ */



