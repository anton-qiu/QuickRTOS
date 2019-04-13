/*******************************************************************************
********************************************************************************
** File Name    : RTC.h
** Description  : Header for Real Time Clock functionality.
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

#ifndef __RTC_H__
#define __RTC_H__

/***********************/
/* OPTIMIZATION OPTION */
/***********************/

/**********************/
/* Value Definitions  */
/**********************/
/* Mask of bits in Alarm Mask Register*/
#define AMR_MASK_SEC    0x01
#define AMR_MASK_MIN    0x02
#define AMR_MASK_HOUR   0x04
#define AMR_MASK_DOM    0x08
#define AMR_MASK_DOW    0x10
#define AMR_MASK_DOY    0x20
#define AMR_MASK_MON    0x40
#define AMR_MASK_YEAR   0x80

/* Mask of bits in Counter Increment Interrupt Register*/
#define CNTR_MASK_SEC    0x01
#define CNTR_MASK_MIN    0x02
#define CNTR_MASK_HOUR   0x04
#define CNTR_MASK_DOM    0x08
#define CNTR_MASK_DOW    0x10
#define CNTR_MASK_DOY    0x20
#define CNTR_MASK_MON    0x40
#define CNTR_MASK_YEAR   0x80

/* Mask of RTC Register size */
#define RTC_SEC_SIZE_MASK   0x0000003F  /* 6 bits*/
#define RTC_MIN_SIZE_MASK   0x0000003F  /* 6 bits*/
#define RTC_HOUR_SIZE_MASK  0x0000001F  /* 5 bits*/
#define RTC_DOM_SIZE_MASK   0x0000001F  /* 5 bits*/
#define RTC_DOW_SIZE_MASK   0x00000007  /* 3 bits*/
#define RTC_DOY_SIZE_MASK   0x000001FF  /* 9 bits*/
#define RTC_MON_SIZE_MASK   0x0000000F  /* 4 bits*/
#define RTC_YEAR_SIZE_MASK  0x00000FFF  /* 12 bits*/

/************************/
/* Data Structure       */
/************************/
typedef struct S_RTC
{
    UINT16 usYear;
    UINT16 usDayofyear;   /*1-366*/
    BYTE bMonth;       /*1-12*/
    BYTE bDayofmonth;  /*1-31*/
    BYTE bDayofweek;   /*1-7*/
    BYTE bHour;        /*1-24*/
    BYTE bMinutes;     /*0-59*/
    BYTE bSecond;	    /*0-59*/

/* The Following part is to manage ISR */
    BYTE bTemp1;   /* to ensure proper alignment */
    BYTE bTemp2;
    void (*pRTC_ISR)(void);     /* Pointer to Real Time Clock ISR*/

} TS_RTC;

/************************/
/* Variable Declaration */
/************************/

/**********************/
/* Function Prototype */
/**********************/
extern void RTC_Connect(P_ISR_CODE pISRCode, BYTE bPriority);

extern void RTC_ISR(void) INTERRUPT;   /* Default RTC ISR that handle both Alarm and Counter vector */

extern void RTC_Enable(void);
extern void RTC_Disable(void);
extern void RTC_SetTime(TS_RTC * psRTCTime);
extern void RTC_GetTime(TS_RTC * psRTCTime);
extern void RTC_SetAlarm(TS_RTC * psRTCAlarm);
extern void RTC_GetAlarm(TS_RTC * psRTCAlarm);

extern void RTC_SetIncrementMode(void(*pCounterISR)(void), BYTE bCntrFlag);
extern BYTE RTC_GetIncrementMode(void);


#endif  /* __RTC_H__ */



