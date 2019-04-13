/*******************************************************************************
********************************************************************************
** File Name    : Task.c
** Description  :
**      Task Related Function
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

/************************/
/* Private Variable     */
/************************/

/************************/
/* Global Variable      */
/************************/

TS_LIST sListBlockedTCBTbl[PRIORITY_LEN];
TS_LIST sListReadyTCBTbl[PRIORITY_LEN];
TS_LIST sListFreeTCB;

TS_TCB saTCBTbl[MAX_TASK + MAX_SYS_TASK];
TS_TCB * psCurrentTCB;

TS_TCB sTCB_Background;


BASE_TYPE xOS_NumOfTask;
BYTE bOS_SchedRunning;
BYTE bOS_TopPriority;

/************************/
/* API Functions        */
/************************/

/*******************************************************************************
* Function Name     : OSTASK_TCBSetInfo()
* Description       : Write the information like Top of Stack, Priority, and Status to TCB.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void OSTASK_TCBSetInfo(TS_TCB *psDest, STACK_TYPE  *pTOS,
                        BYTE bPrio, BYTE bStat)
{
    psDest->pTopOfStack = pTOS;
    psDest->bPriority = bPrio;
    psDest->bTCBStat = bStat;

}

/*******************************************************************************
* Function Name     : OSTASK_TCBSetName()
* Description       : Set the name of a Task to the TCB.
*       The function do nothing if Task naming is not supported.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
#if (MAX_TASK_NAME_LEN>1)
void OSTASK_TCBSetName(TS_TCB *psDest, BYTE * bpName)
{
    BASE_TYPE xCount;
    if(bpName == (BYTE *)NULL)
    {
        psDest->pcTaskName[0] = (BYTE)NULL;
    }
    for(xCount=0; xCount<(MAX_TASK_NAME_LEN-1),(*bpName) != NULL; xCount++, bpName++)
    {
        psDest->pcTaskName[xCount] = *bpName;
    }        
    psDest->pcTaskName[xCount] = (BYTE)'\0';

///* To Avoid compiler warning */
//    psDest = psDest;
//    bpName = bpName;

}
#endif


/*******************************************************************************
* Function Name     : OSTASK_GetTopPrio()
* Description       : Get the highest priority number that is active.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
BYTE OSTASK_GetTopPrio(void)
{
    BYTE bCount;
    TS_LIST_ITEM * psItemTemp;
    TS_TCB * pTCBTemp;

    /* Bigger number means higher priority. */
    for(bCount=(PRIORITY_LEN-1); bCount > IDLE_PRIORITY; bCount++)
    {
        psItemTemp = LIST_GetHead(&sListReadyTCBTbl[bCount]);
        /* Continue to check through the list if the list is not empty. */
        while( psItemTemp != (TS_LIST_ITEM *)NULL)
        {
            pTCBTemp = (TS_TCB *)psItemTemp->pvContainer;
            if(pTCBTemp->bTCBStat == TASK_STAT_RDY)
            {   /* Return priority number if a ready task in the priority number is found. */
                return bCount;
            }
            /* Next item in the list */
            psItemTemp = LIST_GetNext(&sListReadyTCBTbl[bCount], psItemTemp);
        }
    }

    /* if not found, this function will return 0 (lowert priority) which is the idle task. */
    return bCount;
}

/*******************************************************************************
* Function Name     : OSTASK_TCBCreate()
* Description       : Find empty TCB, initialize the TCB structure, and add it to ready list of the priority.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OSTASK_TCBCreate(BYTE bPrio, STACK_TYPE * pTopOfStack)
{
    TS_TCB * pTCBTemp;
    TS_LIST_ITEM * pListItemTemp;

/* Enter Critical Section */
    OS_EnterCritical();

    /* first, find the empty TCB */
    pListItemTemp = LIST_GetHead(&sListFreeTCB);
    
    if(pListItemTemp != (TS_LIST_ITEM *)NULL)
    {   /* initialized? */
        pTCBTemp = pListItemTemp->pvContainer;
        LIST_RemoveHead(&sListFreeTCB);     /* Update TCB Free List */
/*   Exit Critical Section */
        OS_ExitCritical();
//        pTCBTemp->pTopOfStack = pTopOfStack;
//        pTCBTemp->bPriority = bPrio;
//        pTCBTemp->bTCBStat = TASK_STAT_RDY;
        OSTASK_TCBSetInfo(pTCBTemp, pTopOfStack, bPrio, TASK_STAT_RDY);


/* Additional initialization could be added for resource and event management of task */
//....


#if (MAX_TASK_NAME_LEN>1)
	pTCBTemp->pcTaskName[0] = '?';
	pTCBTemp->pcTaskName[1] = (BYTE)NULL;
#endif

        OS_EnterCritical();
/* For now, only 1 task per priority is allowed!! */

/* Update the new TCB to Ready List */
        LIST_AddNode(&sListReadyTCBTbl[bPrio], pListItemTemp);
        LIST_ItemSetValue(pListItemTemp,
                    pListItemTemp->psNext->xItemValue - 1,
                    (void *) NULL,
                    (void *) pTCBTemp);

        xOS_NumOfTask++;
        if(bPrio > bOS_TopPriority)
        {   /* Set the top priority if the current one is higher in priority */
            bOS_TopPriority = bPrio;
        }
        OS_ExitCritical();
        return STATUS_OK;
    }
    OS_ExitCritical();
    return STATUS_ERR_FULL;

}

/*******************************************************************************
* Function Name     : OSTASK_Create()
* Description       : Main interface for Task Creation.
*   Will initialize the stack context, create new Task, and call scheduler if necessary.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OSTASK_Create(P_TASK_CODE pTaskCode,
                    void* pArg,
                    STACK_TYPE *pxTopOfStack,
                    BYTE bPriority)
{
    BYTE bStatus;

/* Check Priority */
    if(bPriority > MAX_PRIORITY)
    {
        return STATUS_PRIO_INVALID;
    }

/* Enter Critical Section */
    OS_EnterCritical();
/* Check if task exist in current priority? */
/* if not exist, the task can be created. */
/* For now, only 1 task per priority is allowed!! */

//    /* Check that both Ready and Blocked list for the priority are empty */
//    if((LIST_GetHead(&sListReadyTCBTbl[bPriority]) == (TS_LIST_ITEM *)NULL) &&
//        (LIST_GetHead(&sListBlockedTCBTbl[bPriority]) == (TS_LIST_ITEM *)NULL))
//    {

/*   Clear and initialize Stack */
        pxTopOfStack = OS_InitContextBackup(pxTopOfStack, pTaskCode, pArg);
/*   Initialize TCB for the Task */
        bStatus = OSTASK_TCBCreate(bPriority, pxTopOfStack);
        OS_ExitCritical();
        if(bStatus == STATUS_OK)
        {
            if(bOS_SchedRunning == STATUS_TRUE)
            {   /* if OS currently running, run the scheduler to schedule the task. */
/* ABQ: Need to call the Scheduler through the System Call (Software Interrupt) */
//                OSTASK_Schedule();
            }
        }

/*   return OK if everything OK*/
/*   or return the status if the TCB initialization fail */
        return STATUS_OK;

//    }
///* else, if priority exist*/
///*   Exit Critical Section */
//    OS_ExitCritical();
//
///*   return PRIORITY NOT OK */
//    return STATUS_PRIO_EXIST;
}

#ifdef TASK_DELETE_SUPPORTED
/*******************************************************************************
* Function Name     : OSTASK_Delete()
* Description       : Delete a task and free the TCB associated with the task.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OSTASK_Delete(TS_TCB * psTCBToDelete, BYTE bPriority)
{

    TS_LIST_ITEM *psListItemTemp;
/* Does not allow to delete IDLE_PRIORITY */
    if(IDLE_PRIORITY == bPriority)
    {
        return STATUS_PRIO_INVALID;
    }

/* Check if priority > PRIORITY_LEN */
    if(bPriority > PRIORITY_LEN)
    {
        return STATUS_PRIO_INVALID;
    }

/* Enter Critical Section */
    OS_EnterCritical();

/* If bPriority == PRIORITY_LEN, Delete Current Task */
    if(PRIORITY_LEN == bPriority)
    {
        psTCBToDelete = psCurrentTCB;
        bPriority = psCurrentTCB->bPriority;
    }

/* Check the validity of the psTCBToDelete, is it in the list? */
/* If TCB is not valid, exit critical section and return error */
    psListItemTemp = LIST_SearchContainer(&sListBlockedTCBTbl[bPriority],(void *)psTCBToDelete);
    if((TS_LIST_ITEM *)NULL == psListItemTemp)
    {
        psListItemTemp = LIST_SearchContainer(&sListReadyTCBTbl[bPriority],(void *)psTCBToDelete);
    }

    if(psListItemTemp != (TS_LIST_ITEM *)NULL)
    {

/* Additional checking to be added here (e.g. if mutex is involved) */
/* ... */

/* Remove task from ready/blocked lisk */
        LIST_DeleteItem(&sListBlockedTCBTbl[bPriority],psListItemTemp);    

/* Erase additional item here (e.g. release the resource, event, etc.) */
/* ... */

/* Erase the content of the TCB */
        OSTASK_TCBSetInfo(psListItemTemp->pvContainer, (STACK_TYPE *)NULL,
                        PRIORITY_LEN, TASK_STAT_FREE);

#if (MAX_TASK_NAME_LEN>1)
        OSTASK_TCBSetName(psListItemTemp->pvContainer, "?");
#endif

/* ... and add to free task list */
        LIST_AppendNode(&sListFreeTCB, psListItemTemp);

/* Decrement number of task if it is not zero */
        if(xOS_NumOfTask != 0)
        {
            xOS_NumOfTask--;
        }
/* Adjust the Top priority if necessary (if the deleted task is top priority task) */
        if(bPriority == bOS_TopPriority)
        {
            /* if Ready List of the current priority empty, update the top priority */
            if(LIST_GetHead(&sListReadyTCBTbl[bPriority]) == NULL)
            {
                bOS_TopPriority = OSTASK_GetTopPrio();
            }
        }

/* Exit Critical Section */
        OS_ExitCritical();
/* Finally, call the scheduler if the OS is running. */
/* ABQ: Need to call the Scheduler through the System Call (Software Interrupt) */
//        OSTASK_Schedule();
        return STATUS_OK;
    }
/* Exit Critical Section */
    OS_ExitCritical();
    return STATUS_TCB_INVALID;

}
#endif

/*******************************************************************************
* Function Name     : OSTASK_Release()
* Description       : Release a Task from a Block List into the ready list.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void OSTASK_Release(TS_LIST * psListSource, TS_LIST_ITEM * psItem)
{
	TS_TCB * psTCBTemp;

	psTCBTemp = (TS_TCB *)psItem->pvContainer;
	/* Release the Task from the current list. */
//	LIST_DeleteItem(&sListReadyTCBTbl[psTCBTemp->bPriority], psItem);
	LIST_DeleteItem(psListSource, psItem);
	/* Then Put it into the ready list. */
	LIST_AddNode(&sListReadyTCBTbl[psTCBTemp->bPriority], psItem);

}

/*******************************************************************************
* Function Name     : OSTASK_Suspend()
* Description       : Put a task on hold and resume immediately.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void OSTASK_Suspend(TS_LIST_ITEM * psItem)
{
	TS_TCB * psTCBTemp;
    TS_LIST * psList;

	psTCBTemp = (TS_TCB *)psItem->pvContainer;
    psList = &sListReadyTCBTbl[psTCBTemp->bPriority];

	/* Release the Task from the current list. */
	LIST_DeleteItem(psList, psItem);
	/* Then Put it into the ready list. */

    /* Add to the tail */
    psList->psListIndex = &(psList->sListEnd);
	LIST_AddNodeCurrent(psList, psItem);

}

/*******************************************************************************
* Function Name     : OSTASK_Block()
* Description       : Put a task to a blocked list.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void OSTASK_Block(TS_LIST * psBlockList, TS_LIST_ITEM * psItem)
{
	TS_TCB * psTCBTemp;
    TS_LIST * psList;

	psTCBTemp = (TS_TCB *)psItem->pvContainer;
    psList = &sListReadyTCBTbl[psTCBTemp->bPriority];

	/* Release the Task from the current Ready list. */
	LIST_DeleteItem(psList, psItem);

	/* Then Put it into the Block list. */
    /* Add to the tail */
    psBlockList->psListIndex = &(psBlockList->sListEnd);
	LIST_AddNodeCurrent(psBlockList, psItem);

}

/*******************************************************************************
* Function Name     : OSTASK_ReleaseAll()
* Description       : Release All Task from a Block List into the Ready list.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void OSTASK_ReleaseAll(TS_LIST * psList)
{
	TS_LIST_ITEM * psTaskItem;

	while( ( psTaskItem = LIST_GetHead(psList) ) != (TS_LIST_ITEM *)NULL )
	{
		/* Release the Task */
        OSTASK_Release(psList, psTaskItem);
	}

}

/*******************************************************************************
* Function Name     : OSTASK_TCBInit()
* Description       : Initialize the Task Lists and TCB content to empty.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 11/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS OSTASK_TCBInit(void)
{
    BASE_TYPE xCount;

    /* Initialize currently running tast to null */
    psCurrentTCB = (TS_TCB *)NULL;

    /* Initialize Ready and Blocked priority table */
    for(xCount=0; xCount<PRIORITY_LEN; xCount++)
    {
        LIST_Init(&sListBlockedTCBTbl[xCount], TASK_LIST_VAL_MAX);
        LIST_Init(&sListReadyTCBTbl[xCount], TASK_LIST_VAL_MAX);
    }

    /* Initialize Free TCB list */
    LIST_Init(&sListFreeTCB, (MAX_TASK + MAX_SYS_TASK));

    for(xCount=0; xCount<(MAX_TASK + MAX_SYS_TASK); xCount++)
    {
        /* For each List item, add to Free TCB List */
        LIST_AddNode(&sListFreeTCB, &saTCBTbl[xCount].sStateListItem);

        /* Link the list with the TCB */
        saTCBTbl[xCount].sStateListItem.pvContainer = (void *)&saTCBTbl[xCount];

        /* Init the content of TCB */
        OSTASK_TCBSetInfo(&saTCBTbl[xCount], (STACK_TYPE  *)NULL,
                        MAX_PRIORITY, TASK_STAT_FREE);

#if (MAX_TASK_NAME_LEN>1)
        OSTASK_TCBSetName(&saTCBTbl[xCount], "?");
#endif
    }

    xOS_NumOfTask = 0;
    bOS_SchedRunning = STATUS_FALSE;
    bOS_TopPriority = IDLE_PRIORITY;     /* No Priority assigned */

    return STATUS_OK;
}

