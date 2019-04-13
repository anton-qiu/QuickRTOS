/*******************************************************************************
********************************************************************************
** File Name    : OSPort.h
** Description  :
**      To ensure portablability
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 05/01/07(ABQ)    1.0         Initial Revision
********************************************************************************
*******************************************************************************/

/*------------------------------------------------------------------------------
|        1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
------------------------------------------------------------------------------*/

#include "INCLUDE.h"

#ifndef __OSPORT_H__
#define __OSPORT_H__

/***********************/
/* OPTIMIZATION OPTION */
/***********************/

/**********************/
/* Value Definitions  */
/**********************/
/* System mode, ARM mode, interrupts enabled. */
#define INITIAL_SPSR				( (STACK_TYPE) 0x1f )

#define THUMB_MODE_BIT				( (STACK_TYPE) 0x20 )
#define INSTRUCTION_SIZE			( (STACK_TYPE) 4 )

#define NO_CRITICAL_NESTING     0

/************************/
/* Variable Declaration */
/************************/
extern BASE_TYPE XDATA xCriticalNesting;

/**********************/
/* Function Prototype */
/**********************/

extern void OS_ExitCritical(void);
extern void OS_EnterCritical(void);
extern STACK_TYPE * OS_InitContextBackup(STACK_TYPE *pxTopOfStack, P_TASK_CODE pxCode,
                                void * pvParameters);


#define OS_Schedule_Int()               \
{                                       \
	__asm{ SWI 0		};              \
}

#endif //__OSPORT_H__