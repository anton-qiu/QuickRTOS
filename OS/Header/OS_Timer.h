/*******************************************************************************
********************************************************************************
** File Name    : OS_Timer.c
** Description  :
**      Timer abstraction for OS use.
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 01/04/07(ABQ)    1.0         Initial Revision
********************************************************************************
*******************************************************************************/

/*------------------------------------------------------------------------------
|        1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
------------------------------------------------------------------------------*/

#include "INCLUDE.h"

#ifndef __OS_TIMER_H__
#define __OS_TIMER_H__

/**********************/
/* Value Definitions  */
/**********************/

#define T0_IR_ALL	0xFF
#define MAX_TIMER_WHEEL 32

#define MATCH_VALUE_MAX		0xFFFFFFFF

/************************/
/* Data Structure       */
/************************/
typedef struct S_VTIMER
{
	UINT32 xMatchValue;
	TS_LIST_ITEM sStateListItem;
	TS_LIST sTaskList;
}TS_VTIMER;

/************************/
/* Variable Declaration */
/************************/

/**********************/
/* Function Prototype */
/**********************/
extern void OS_VTimer_Init(void);
extern void OS_VTimer_Stop(void);
extern void OS_VTimer_Start(void);
extern void OS_VTimer_ResetTick(void);
extern UINT32 OS_VTimer_ReadTick(void);
extern TIMER_TYPE OS_VTimer_ReadCounter(void);
extern void OS_VTimer_UpdateCounter(TIMER_TYPE xCntVal);
extern BYTE OS_VTimer_Create(UINT32 xDelayTick, TS_LIST_ITEM * psItemTask);
extern void OS_VTimer_Delete(TS_LIST * psList, TS_LIST_ITEM *psItemToDelete);
extern void OS_VTimer_Cancel(TS_LIST_ITEM *pItemToCancel);


#endif	//__OS_TIMER_H__