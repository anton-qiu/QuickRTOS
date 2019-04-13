/*******************************************************************************
********************************************************************************
** File Name    : Task.h
** Description  :
**      Task Related Definitions
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

#ifndef __TASK_H__
#define __TASK_H__

/***********************/
/* OPTIMIZATION OPTION */
/***********************/

/**********************/
/* Value Definitions  */
/**********************/

//#define MAX_TASK_NAME_LEN   20
#define MAX_TASK_NAME_LEN   0

#define PRIORITY_LEN    8
#define MAX_PRIORITY    (PRIORITY_LEN-1)    /* Lowest priority */
#define PRIORITY_MASK   0x07

//#define IDLE_PRIORITY   MAX_PRIORITY
#define IDLE_PRIORITY   0

#define MAX_TASK        5
#define MAX_SYS_TASK    2

#define TASK_STAT_RDY       0   /* Task Status: Ready for execution */
#define TASK_STAT_FREE      1   /* Task Status: Free/unused */
#define TASK_STAT_BLOCKED   2   /* Task Status: Blocked, waiting for resource */

/* FOR NOW, MAX TASK FOR EACH PRIORITY IS ONE -> Priority based only */
#define TASK_LIST_VAL_MAX   0x01    /* MAX TASK FOR EACH PRIORITY */

//#define TASK_DELETE_SUPPORTED

/************************/
/* Data Structure       */
/************************/

typedef void (*P_TASK_CODE)( void * );

typedef struct S_TCB
{
/* Minimum mandatory element in TCB for task management:*/
    /* TOP OF THE STACK MUST BE THE FIRST ITEM, for simplicity. */
	STACK_TYPE  *pTopOfStack;  /*< Points to the location of the last item placed on the tasks stack.  THIS MUST BE THE FIRST MEMBER OF THE STRUCT. */

	BYTE    bPriority;     /*< The priority of the task where 0 is the lowest priority. */
    BYTE bTCBStat;

	TS_LIST_ITEM    sStateListItem;   /* List item used to place the TCB in ready and blocked queues. */

/* Additional element could be added for resource and event management of task */

/* Optional element in TCB to update in the future:*/
//	STACK_TYPE  *pStack;   /*< Points to the start of the stack. */
//    BYTE bTCBId;
/* Event list not used for now */
//	TS_LIST_ITEM    sEventListItem;     /*< List item used to place the TCB in event lists. */
//	LEN_TYPE  StackDepth;   /*< Total depth of the stack (when empty).  This is defined as the number of Stack Element the stack can hold, not the number of bytes. */

#if (MAX_TASK_NAME_LEN>1)
	BYTE pcTaskName[ MAX_TASK_NAME_LEN ];/*< Descriptive name given to the task when created.  Facilitates debugging only. */
#endif

} TS_TCB;

/************************/
/* Variable Declaration */
/************************/

extern TS_LIST sListBlockedTCBTbl[PRIORITY_LEN];
extern TS_LIST sListReadyTCBTbl[PRIORITY_LEN];
extern TS_LIST sListFreeTCB;

extern TS_TCB saTCBTbl[MAX_TASK + MAX_SYS_TASK];    /* Pool of TCB */
extern TS_TCB * psCurrentTCB;   /* Pointer to currently running TCB */

extern BASE_TYPE xOS_NumOfTask; /* Track the number of running task. */
extern BYTE bOS_SchedRunning;   /* Flag of OS Scheduler running */
extern BYTE bOS_TopPriority;    /* The highest priority currently running */

extern TS_TCB sTCB_Background;

/**********************/
/* Function Prototype */
/**********************/
//extern void  OSTASK_Schedule(void);
extern void OSTASK_TCBSetInfo(TS_TCB *psDest, STACK_TYPE  *pTOS,
                        BYTE bPrio, BYTE bStat);
extern void OSTASK_TCBSetName(TS_TCB *psDest, BYTE * bpName);
extern BYTE OSTASK_GetTopPrio(void);
extern STATUS OSTASK_TCBCreate(BYTE bPrio, STACK_TYPE * pTopOfStack);
extern STATUS OSTASK_Create(P_TASK_CODE pTaskCode,
                    void* pArg,
                    STACK_TYPE *pxTopOfStack,
                    BYTE bPriority);
extern STATUS OSTASK_Delete(TS_TCB * psTCBToDelete, BYTE bPriority);

extern void OSTASK_Release(TS_LIST * psListSource, TS_LIST_ITEM * psItem);

extern void OSTASK_Suspend(TS_LIST_ITEM * psItem);
extern void OSTASK_Block(TS_LIST * psBlockList, TS_LIST_ITEM * psItem);

extern void OSTASK_ReleaseAll(TS_LIST * psList);

extern STATUS OSTASK_TCBInit(void);


#endif  /* __TASK_H__ */


