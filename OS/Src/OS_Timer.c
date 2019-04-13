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

/************************/
/* Private Variable     */
/************************/
UINT32 u32TickValue;

TS_VTIMER saVTimerPool[MAX_TIMER_WHEEL];

//TS_LIST_ITEM saVTimer_ListItem[MAX_TIMER_WHEEL];
TS_LIST sVTimer_List;

TS_LIST sVTimer_FreeList;


#define VTIMER_INTERRUPT_PRIORITY 1
//#define VTIMER_MAX_COUNTER 1000		/* 1 mS */
//#define VTIMER_MAX_COUNTER 10000		/* 10 mS */
#define VTIMER_MAX_COUNTER 50		/* For Testing */

/************************/
/* API Functions        */
/************************/
void OS_VTimer_ISR(void)/* INTERRUPT */;

/*******************************************************************************
* Function 		: OS_VTimer_Init()
* Description   : Initialize the internal state and variables of VTimer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_VTimer_Init(void)
{
	BYTE bCount;

	/* Initialize the pool and the Free List */
    LIST_Init(&sVTimer_FreeList, MAX_TIMER_WHEEL);

	for(bCount=0; bCount<MAX_TIMER_WHEEL; bCount++)
	{
		/* Initialize the Content of each VTimer */
		saVTimerPool[bCount].xMatchValue = 0;
		LIST_Init(&saVTimerPool[bCount].sTaskList, (MAX_TASK + MAX_SYS_TASK) );

        /* For each List item, add to Free VTimer List */
        LIST_AddNode(&sVTimer_FreeList, &saVTimerPool[bCount].sStateListItem);
		/* Link the list Item with VTimer */
        saVTimerPool[bCount].sStateListItem.pvContainer = (void *)&saVTimerPool[bCount];

	}

	/* Initialize the VTimerList to become an empty list */
	LIST_Init(&sVTimer_List, MAX_TIMER_WHEEL);

	Timer0_Disable();
	u32TickValue = 0;
	Timer0_Connect(OS_VTimer_ISR, VTIMER_INTERRUPT_PRIORITY);
	Timer0_InitPrescalerMode(14, TIMER_MODE_PCLK);	/* 1 uS */
	Timer0_SetCounter0(NULL, VTIMER_MAX_COUNTER, TIMER_CNTR_RST_ON_MR0|TIMER_CNTR_INT_ON_MR0);

}

/*******************************************************************************
* Function 		: OS_VTimer_Stop()
* Description   : Stop the hardware timer, thus preventing the VTimer to run.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_VTimer_Stop(void)
{	/* Stop the Timer Hardware and all the Virtual Timer*/
	Timer0_Disable();
   	T0IR = T0_IR_ALL;   /* Clear Flag (just in case) */

}

/*******************************************************************************
* Function 		: OS_VTimer_Start()
* Description   : Start the hardware timer, thus start running the VTImer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_VTimer_Start(void)
{	/* Run The Hardware Timer again. */
	Timer0_Enable();
}

/*******************************************************************************
* Function 		: OS_VTimer_ResetTick()
* Description   : Reset the hardware counter for VTimer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_VTimer_ResetTick(void)
{
//	Timer0_SetCounter0(NULL, VTIMER_MAX_COUNTER, TIMER_CNTR_RST_ON_MR0|TIMER_CNTR_INT_ON_MR0);
    Timer0_Reset();
}

/*******************************************************************************
* Function 		: OS_VTimer_ReadTick()
* Description   : Read the Tick Value of the VTimer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: return Tick Value.
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
UINT32 OS_VTimer_ReadTick(void)
{
	return u32TickValue;
}

/*******************************************************************************
* Function 		: OS_VTimer_ReadCounter()
* Description   : Read the counter value of the hardware timer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: -
* Output 		: return Counter Value.
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
TIMER_TYPE OS_VTimer_ReadCounter(void)
{
	return Timer0_GetTick();
}

/*******************************************************************************
* Function 		: OS_VTimer_UpdateCounter()
* Description   : Update the hardware timer's counter value to a specific value.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: Counter Value
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_VTimer_UpdateCounter(TIMER_TYPE xCntVal)
{
//	Timer0_SetCounter0(NULL, xCntVal, TIMER_CNTR_RST_ON_MR0|TIMER_CNTR_INT_ON_MR0);
    T0TC = xCntVal;
}

/*******************************************************************************
* Function 		: OS_VTimer_Create()
* Description   : Create new instance of VTimer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: Delay time (relative to current timer), TCB of the task.
* Output 		: return Status.
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
BYTE OS_VTimer_Create(UINT32 xDelayTick, TS_LIST_ITEM * psItemTask)
{

    TS_VTIMER * pVTimerTemp;
    TS_LIST_ITEM * pListItemTemp;
    UINT32 xMatchTemp;

/* Enter Critical Section */
    OS_EnterCritical();

    /* first, find the empty TCB */
    pListItemTemp = LIST_GetHead(&sVTimer_FreeList);

    if(pListItemTemp != (TS_LIST_ITEM *)NULL)
    {   /* initialized? */
        pVTimerTemp = pListItemTemp->pvContainer;
        LIST_RemoveHead(&sVTimer_FreeList);     /* Update VTimer Free List */
/*   Exit Critical Section */
        OS_ExitCritical();

// Store Match Value and Add task to pending task list.
    xMatchTemp = u32TickValue+xDelayTick+1;
    if(xMatchTemp >= MATCH_VALUE_MAX)
    {
        xMatchTemp = (xMatchTemp-MATCH_VALUE_MAX);
    }
	pVTimerTemp->xMatchValue = xMatchTemp;
	LIST_AddNode(&pVTimerTemp->sTaskList, psItemTask);

/* Additional initialization could be added Here */
//....


        OS_EnterCritical();
/* Update the new VTimer to the List */
        LIST_AddNode(&sVTimer_List, pListItemTemp);
        LIST_ItemSetValue(pListItemTemp,
                    pListItemTemp->psNext->xItemValue - 1,
                    (void *) NULL,
                    (void *) pVTimerTemp);

        OS_ExitCritical();
        return STATUS_OK;
    }
    OS_ExitCritical();
    return STATUS_ERR_FULL;

}

/*******************************************************************************
* Function 		: OS_VTimer_Delete()
* Description   : Delete an instance of VTimer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: Pointer to List Item containing an instance of VTimer to delete.
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_VTimer_Delete(TS_LIST * psList, TS_LIST_ITEM *psItemToDelete)
{
	TS_VTIMER * psVTimerTemp;

/* Delete From VTimer List */
	LIST_DeleteItem(psList, psItemToDelete);

/* Release the Blocking Tasks to Ready. */
	psVTimerTemp = ( (TS_VTIMER *)(psItemToDelete->pvContainer) );

    OSTASK_ReleaseAll(&(psVTimerTemp->sTaskList));

/* And add the VTimer to the Free VTimer List. */
	LIST_AddNode(&sVTimer_FreeList, psItemToDelete);
}

/*******************************************************************************
* Function 		: OS_VTimer_Cancel()
* Description   : Render an instance of VTimer unused or not running. Useful to temporarily suspend a particular VTimer.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: Pointer to List item containing an instance of VTIMER to cancel.
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void OS_VTimer_Cancel(TS_LIST_ITEM *pItemToCancel)
{
	TS_VTIMER * psVTimerTemp;
	psVTimerTemp = (TS_VTIMER *)pItemToCancel->pvContainer;
	psVTimerTemp->xMatchValue = MATCH_VALUE_MAX;
}

/*******************************************************************************
* Function 		: OS_VTimer_Check()
* Description   : Check if any VTimer instance that match the VTimer Tick.
*       Return the pointer if there is any, otherwise, return NULL.
* NOTE: 
*---------------------------------------------------------------------
* Global 		: -
* Input 		: Pointer List Item containing VTIMER where the checking will start.
* Output 		: Pointer to list item that match the Tick, or return NULL if no match found.
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 01/04/07(ABQ)     :   Initial Version
*******************************************************************************/
TS_LIST_ITEM * OS_VTimer_Check(TS_LIST_ITEM *pItemToCheck)
{
	TS_VTIMER * psVTimerTemp;


    while((pItemToCheck != (&(sVTimer_List.sListEnd)))  /* Not the end of the list? */
		&& pItemToCheck != NULL )	/* Or if the list is empty. */
    {
        psVTimerTemp = (TS_VTIMER *)pItemToCheck->pvContainer;
        if(psVTimerTemp->xMatchValue == u32TickValue)
        {
            return pItemToCheck;
        }
        /* Next Item. */
        pItemToCheck = LIST_GetNext(&sVTimer_List,pItemToCheck);
    }
    return (TS_LIST_ITEM *)NULL;
}

void OS_VTimer_Handler(void)
{
	TS_LIST_ITEM * psItemTemp;
	TS_VTIMER * psVTimerTemp;
    
    /* Put Back the Currently Running Task to the Ready List. */
    OSTASK_Release( &sListReadyTCBTbl[bOS_TopPriority], &(psCurrentTCB->sStateListItem) );

    /* Increment Tick Value Counter. */
    u32TickValue++;
    if(u32TickValue >= MATCH_VALUE_MAX)
    {
        u32TickValue = 0;
    }

    /* Check the VTimer List to see if there are any timer that match. */
    psItemTemp = LIST_GetHead(&sVTimer_List);

    while( ( psItemTemp = OS_VTimer_Check(psItemTemp) ) != NULL )
    {	/* If there is Pending Task that match the counter, update the task to Ready List. */
        psVTimerTemp = (TS_VTIMER *)psItemTemp->pvContainer;

        OSTASK_ReleaseAll(&(psVTimerTemp->sTaskList));
    }

    /* Call The Scheduler to Schedule for Next Task. */
    OS_Schedule();

/* Clear the pending interrupt by writing to VICVectAddr*/
	T0IR = T0_IR_ALL;
	VICVectAddr = 0;

}

