/*******************************************************************************
********************************************************************************
** File Name    : OS_Semaphore.h
** Description  :
**      Implementation of Semaphore and Mutex.
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 06/05/07(ABQ)    1.0         Initial Revision
********************************************************************************
*******************************************************************************/

/*------------------------------------------------------------------------------
|        1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
------------------------------------------------------------------------------*/

#include "INCLUDE.h"

#ifndef __OS_SEMAPHORE_H__
#define __OS_SEMAPHORE_H__

/**********************/
/* Value Definitions  */
/**********************/
#define MAX_SCB     10

#define SCB_ID_MAX  10

#define SCB_STATUS_UNUSED   0x00
#define SCB_STATUS_ACTIVE   0x01
#define SCB_STATUS_EMPTY    0x02
#define SCB_STATUS_MASK     0x03

#define SCB_STATUS_TYPE_SEM 0x80
#define SCB_STATUS_TYPE_MUT 0xC0
#define SCB_STATUS_TYPE_MASK    0xC0

#define SCB_HANDLE_ERROR    0xFF

/************************/
/* Data Structure       */
/************************/
typedef struct S_SCB
{
    BYTE xId;       /* Identification for the Semaphore or Mutex. (e.g. Semaphore ID for UART) */
    BYTE xValue;    /* List of available Semaphore (for Counting Semaphore). */
    BYTE xMaxValue;
    BYTE xStatus;
    TS_LIST sListWaitingTask;   /* List of task blocking for the Semaphore/Mutex. */
    TS_TCB * psTCBMutexOwner;   /* Task that is holding the mutex. */
}TS_SCB;

/************************/
/* Variable Declaration */
/************************/

/**********************/
/* Function Prototype */
/**********************/
extern void OS_Sem_Init(void);
extern BYTE OS_Sem_GetHandle(BYTE xId);
extern BYTE OS_Sem_Create(const BYTE xId, BYTE xVal);
extern STATUS OS_Sem_Delete(BYTE xId, BYTE xVal);
extern void OS_Sem_Aquire(BYTE xHandle);
extern void OS_Sem_Release(BYTE xHandle);
extern STATUS OS_Sem_Get(BYTE xHandle);
extern BYTE OS_Mutex_Create(const BYTE xId);
extern void OS_Mutex_Delete(void);
extern void OS_Mutex_Lock(BYTE xHandle);
extern STATUS OS_Mutex_Unlock(BYTE xHandle);
extern STATUS OS_Mutex_Get(BYTE xHandle);


#endif	//__OS_SEMAPHORE_H__

