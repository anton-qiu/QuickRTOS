/*******************************************************************************
********************************************************************************
** File Name    : OS_Schedule.c
** Description  :
**      Implementation of Task Scheduling function.
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 15/04/07(ABQ)    1.0         Initial Revision
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

/************************/
/* Private Variable     */
/************************/

/************************/
/* API Functions        */
/************************/
/*******************************************************************************
* Function Name     : OS_Schedule()
* Description       : Schedule next task to Run. This is the main scheduling algorithm.
*       The Context Switch are not called here.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 15/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void  OS_Schedule(void)
{
    TS_LIST_ITEM * psItemTemp;

    /* Search through the Priority Table to find the current highest priority? */
    //bOS_TopPriority;

    /* Get Task */
    psItemTemp = LIST_GetTail(&sListReadyTCBTbl[bOS_TopPriority]);
    while((psItemTemp == NULL) && (bOS_TopPriority != IDLE_PRIORITY) )
    {
        bOS_TopPriority--;  /* Get Lower Priority */
        psItemTemp = LIST_GetTail(&sListReadyTCBTbl[bOS_TopPriority]);
    }
    if(bOS_TopPriority == IDLE_PRIORITY)
    {
        /* Later may need to Handle for IDLE TASK. */
        psCurrentTCB = (TS_TCB *)psItemTemp->pvContainer;
    }
    else
    {
        psCurrentTCB = (TS_TCB *)psItemTemp->pvContainer;
    }

}

/*******************************************************************************
* Function Name     : OS_SWI_Schedule()
* Description       : Schedule next task to Run. This is the main scheduling algorithm.
*       The Context Switch are not called here.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 15/04/07(ABQ)     :   Initial Version
*******************************************************************************/
void  OS_SWI_Schedule(UINT32 u32Mode)
{
    if(1 == u32Mode)
    {
        /* Put the Task on the Head */
        OSTASK_Release( &sListReadyTCBTbl[bOS_TopPriority], &(psCurrentTCB->sStateListItem) );
    }
    else if(2 == u32Mode)
    {
        /* Put the task on the Tail */
        OSTASK_Suspend( &(psCurrentTCB->sStateListItem) );
    }
    OS_Schedule();
}






