/*******************************************************************************
********************************************************************************
** File Name    : OS_Semaphore.c
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

/************************/
/* Global Variable      */
/************************/
TS_SCB saSCB[MAX_SCB];

/************************/
/* Private Variable     */
/************************/

/************************/
/* API Functions        */
/************************/
/*******************************************************************************
* Function 		: OS_Sem_Init()
* Description   : Initialize the Semaphore Table
* NOTE: 
*---------------------------------------------------------------------
* Global 		: saSCB[]
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_Sem_Init(void)
{
    BYTE bCount;

    for(bCount=0; bCount < MAX_SCB; bCount++)
    {
        saSCB[bCount].xId = SCB_ID_MAX;
        saSCB[bCount].xValue = 0;
        saSCB[bCount].xMaxValue = 0;
        saSCB[bCount].xStatus = SCB_STATUS_UNUSED;
        LIST_Init(&saSCB[bCount].sListWaitingTask, (MAX_TASK+MAX_SYS_TASK) );
        saSCB[bCount].psTCBMutexOwner = (TS_TCB *)NULL;
    }
}

/*******************************************************************************
* Function 		: OS_Sem_GetHandle()
* Description   : Get Handle of a Semaphore ID
* NOTE: 
*---------------------------------------------------------------------
* Global 		: saSCB[]
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
BYTE OS_Sem_GetHandle(BYTE xId)
{
    BYTE bCount;

    for(bCount=0; bCount < MAX_SCB; bCount++)
    {
        /* Semaphore with the same ID has been created? */
        if(xId == saSCB[bCount].xId)
        {
            return bCount;
        }
    }
    return SCB_HANDLE_ERROR;
}

/*******************************************************************************
* Function 		: OS_Sem_Create()
* Description   : 
* NOTE: 
*---------------------------------------------------------------------
* Global 		: saSCB[]
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
BYTE OS_Sem_Create(const BYTE xId, BYTE xVal)
{
    BYTE bCount, xHandle;

    xHandle = SCB_HANDLE_ERROR;
    for(bCount=0; bCount < MAX_SCB; bCount++)
    {
        /* Semaphore with the same ID has been created? */
        if(xId == saSCB[bCount].xId)
        {
            return SCB_HANDLE_ERROR;
        }
        if(SCB_STATUS_UNUSED == saSCB[bCount].xStatus)
        {
            /* Set the first empty SCB handle. */
            if(xHandle>bCount)
            {
                xHandle = bCount;
            }
        }
    }

    if(SCB_HANDLE_ERROR == xHandle)
    {
        return SCB_HANDLE_ERROR;
    }

    saSCB[xHandle].xId = xId;
    saSCB[xHandle].xValue = xVal;
    saSCB[xHandle].xMaxValue = xVal;
    saSCB[xHandle].xStatus = SCB_STATUS_ACTIVE;
    LIST_Init(&saSCB[xHandle].sListWaitingTask, (MAX_TASK+MAX_SYS_TASK) );
    saSCB[xHandle].psTCBMutexOwner = (TS_TCB *)NULL; /* No Task that is holding on the Mutex on Creation. */

    return xHandle;
}

#if 0   /* Currently not supported. */
/*******************************************************************************
* Function 		: OS_Sem_Delete
* Description   : 
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OS_Sem_Delete(BYTE xId, BYTE xVal)
{
}
#endif

/*******************************************************************************
* Function 		: OS_Sem_Aquire()
* Description   : Get a semaphore. If it fails, the task that is currently
*       running will be blocked.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_Sem_Aquire(BYTE xHandle)
{

    while(1)
    {
        OS_EnterCritical();
        if(saSCB[xHandle].xValue > 0)
        {   /* Available, then get the semaphore. */
            saSCB[xHandle].xValue--;
            /* Set the owner (in case of Mutex) or last Task that aquire a semaphore. */
            saSCB[xHandle].psTCBMutexOwner = psCurrentTCB;
            OS_ExitCritical();
            return;
        }
        else
        {   /* Not available? Put the current task on Hold. */
            OSTASK_Block(&(saSCB[xHandle].sListWaitingTask), &(psCurrentTCB->sStateListItem));
            OS_VTimer_Stop();
            OS_ExitCritical();

            OS_VTimer_ResetTick();
            OS_VTimer_Start();
            OS_Schedule_Int();  /* Restart Timer and scheduler again. */
        }
    }
}

/*******************************************************************************
* Function 		: OS_Sem_Release()
* Description   : 
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_Sem_Release(BYTE xHandle)
{
    BYTE bPriorityTemp;

    OS_EnterCritical();
    if(saSCB[xHandle].xValue < saSCB[xHandle].xMaxValue)
    {
        OS_VTimer_Stop();
        saSCB[xHandle].xValue++;
        OSTASK_ReleaseAll( &(saSCB[xHandle].sListWaitingTask) );
        OS_ExitCritical();

        saSCB[xHandle].psTCBMutexOwner = NULL;

        bPriorityTemp = OSTASK_GetTopPrio();
        if(bPriorityTemp > bOS_TopPriority)
        {   /* Schedule for higher priority task that has been recently released. */

            /* Put Back the Currently Running Task to the Ready List. */
            OSTASK_Release( &sListReadyTCBTbl[bOS_TopPriority], &(psCurrentTCB->sStateListItem) );

            bOS_TopPriority = bPriorityTemp;    /* Update priority after task has been released. */

            OS_VTimer_ResetTick();
            OS_VTimer_Start();
            OS_Schedule_Int();  /* Restart Timer and scheduler again. */
            return;
        }
        else    /* Resume the operation of the current task. */
        {
            OS_VTimer_Start();
            return;
        }
    }
    else
    {
        OS_ExitCritical();
    }
}

/*******************************************************************************
* Function 		: OS_Sem_Get()
* Description   : Aquire a Semaphore, but it does not block when it fails.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OS_Sem_Get(BYTE xHandle)
{
    OS_EnterCritical();
    if(saSCB[xHandle].xValue > 0)
    {   /* Available, then get the semaphore. */
        saSCB[xHandle].xValue--;

        /* Set the owner (in case of Mutex) or last Task that aquire a semaphore. */
        saSCB[xHandle].psTCBMutexOwner = psCurrentTCB;

        OS_ExitCritical();
        return STATUS_OK;
    }
    OS_ExitCritical();
    return STATUS_FALSE;
}

/*******************************************************************************
* Function 		: OS_Mutex_Create()
* Description   : 
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
BYTE OS_Mutex_Create(const BYTE xId)
{
    return OS_Sem_Create(xId, 1);
}

#if 0   /* Currently not supported. */
/*******************************************************************************
* Function 		: OS_Mutex_Delete()
* Description   : 
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_Mutex_Delete(void)
{
}
#endif

/*******************************************************************************
* Function 		: OS_Mutex_Lock()
* Description   : 
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_Mutex_Lock(BYTE xHandle)
{
    OS_Sem_Aquire(xHandle);
}

/*******************************************************************************
* Function 		: OS_Mutex_Unlock()
* Description   : 
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OS_Mutex_Unlock(BYTE xHandle)
{
    OS_EnterCritical();
    /* Only the rightful owner can release a mutex. */
    if((saSCB[xHandle].psTCBMutexOwner == psCurrentTCB) )
    {
        OS_ExitCritical();
        OS_Sem_Release(xHandle);
        return STATUS_OK;
    }
    /* Otherwise, return error. */
    OS_ExitCritical();
    return STATUS_FALSE;
}

/*******************************************************************************
* Function 		: OS_Mutex_Get()
* Description   : Get a Mutex and Lock it, but it does not block when it fails.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 06/05/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OS_Mutex_Get(BYTE xHandle)
{
    return OS_Sem_Get(xHandle);
}



