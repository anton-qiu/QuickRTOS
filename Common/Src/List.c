/*******************************************************************************
********************************************************************************
** File Name    : List.c
** Description  : Generic Double Linked List implementation.
**      See header file for detail description.
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 10/01/07(ABQ)    1.0         Initial Revision
********************************************************************************
*******************************************************************************/

/*------------------------------------------------------------------------------
|        1         2         3         4         5         6         7         8
12345678901234567890123456789012345678901234567890123456789012345678901234567890
------------------------------------------------------------------------------*/

#include "INCLUDE.h"


/*******************************************************************************
* Function Name     : LIST_InitItem()
* Description       : Initialize the link of an item.
*       WARNING: The value of the item will be destroyed.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void LIST_InitItem(TS_LIST_ITEM *psListItem,
                    TS_LIST_ITEM *psNxt,
                    TS_LIST_ITEM *psPrev)
{
    psListItem->psNext = psNxt;
    psListItem->psPrevious = psPrev;

    psListItem->xItemValue = (LIST_VALUE_TYPE)NULL;
    psListItem->pvOwner = (void *)NULL;
    psListItem->pvContainer = (void *)NULL;
    return;
}

/*******************************************************************************
* Function Name     : LIST_ItemSetValue()
* Description       : Set the value of an item.
*       WARNING: Must be called after the link has been established.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void LIST_ItemSetValue(TS_LIST_ITEM *psListItem,
                    LIST_VALUE_TYPE xItemVal,
                    void * pxOwn,
                    void * pxCont)
{
    psListItem->xItemValue = xItemVal;
    psListItem->pvOwner = pxOwn;
    psListItem->pvContainer = pxCont;
    return;
}

/*******************************************************************************
* Function Name     : LIST_InsertItemBefore()
* Description       : Insert an item list before psListItem
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS LIST_InsertItemBefore(TS_LIST *psList, TS_LIST_ITEM *psListItem, TS_LIST_ITEM *psNewListItem)
{
    TS_LIST_ITEM * psTemp;
    /* Return error if any of the pointer is empty */
    if((psListItem == (TS_LIST_ITEM *)NULL) ||
        (psNewListItem == (TS_LIST_ITEM *)NULL))
    {
        return STATUS_ERR_UNDEF;
    }

    psNewListItem->psNext = psListItem;
    psTemp = psListItem->psPrevious;
    if(psTemp != (TS_LIST_ITEM *)NULL)
    {   /* Update pointer of the previous item only if it is not null */
        psTemp->psNext = psNewListItem;
    }
    psNewListItem->psPrevious = psTemp;
    psListItem->psPrevious = psNewListItem;

	/* Updating the Head Pointer if we are inserting at the head. */
	if(psList->psListHead == psListItem)
	{
		psList->psListHead = psNewListItem;
	}

    return STATUS_OK;
}

#if 0
/*******************************************************************************
* Function Name     : LIST_InsertItemAfter()
* Description       : Insert an item list after psListItem
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS LIST_InsertItemAfter(TS_LIST_ITEM *psListItem, TS_LIST_ITEM *psNewListItem)
{
    TS_LIST_ITEM * psTemp;
    /* Return error if any of the pointer is empty */
    if((psListItem == (TS_LIST_ITEM *)NULL) ||
        (psNewListItem == (TS_LIST_ITEM *)NULL))
    {
        return STATUS_ERR_UNDEF;
    }

    psNewListItem->psPrevious = psListItem;
    psTemp = psListItem->psNext;

    if(psTemp != (TS_LIST_ITEM *)NULL)
    {   /* Update pointer of the next item only if it is not null */
        psTemp->psPrevious = psNewListItem;
    }
    psNewListItem->psNext = psTemp;
    psListItem->psNext = psNewListItem;

    return STATUS_OK;
}
#endif

/*******************************************************************************
* Function Name     : LIST_DeleteItem()
* Description       : Delete a list item from the link
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS LIST_DeleteItem(TS_LIST *psList, TS_LIST_ITEM *psItemToDelete)
{
    TS_LIST_ITEM * pNxt;
    TS_LIST_ITEM * pPrev;

    /* Return error if the pointer is empty */
    if((psItemToDelete == (TS_LIST_ITEM *)NULL))
    {
        return STATUS_ERR_UNDEF;
    }

    pNxt = psItemToDelete->psNext;
    pPrev = psItemToDelete->psPrevious;

    if(pNxt != (TS_LIST_ITEM *)NULL)
    {   /* Update pointer of the next item only if it is not null */
        pNxt->psPrevious = pPrev;
    }

    if(pPrev != (TS_LIST_ITEM *)NULL)
    {   /* Update pointer of the previous item only if it is not null */
        pPrev->psNext = pNxt;
    }

    /* Set the deleted item to NULL */
    psItemToDelete->psNext = (TS_LIST_ITEM *)NULL;
    psItemToDelete->psPrevious = (TS_LIST_ITEM *)NULL;

	/* Updating the Head Pointer if we are deleting at the head. */
	if(psList->psListHead == psItemToDelete)
	{
		psList->psListHead = pNxt;
	}

// Do not delete the content.
//    LIST_InitItem(psItemToDelete,(TS_LIST_ITEM *)NULL,(TS_LIST_ITEM *)NULL);

    return STATUS_OK;
}

/*******************************************************************************
* Function Name     : LIST_Init()
* Description       : Initialization of the List Header.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void LIST_Init(TS_LIST *psList, LIST_VALUE_TYPE xMaxval)
{
    /* Both List head and list index is pointing to item end marker to
        indicate that it is empty. */
    psList->psListHead = &(psList->sListEnd);
    psList->psListIndex = psList->psListHead;

    /* Next and Previous point to itself, to indicate that it is empty */
    LIST_InitItem(&(psList->sListEnd), &(psList->sListEnd), &(psList->sListEnd));

    /* Set the maximum Value */
    psList->sListEnd.xItemValue = xMaxval;

    return;
}

/*******************************************************************************
* Function Name     : LIST_AddNode()
* Description       : Add node at the head
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void LIST_AddNode(TS_LIST *psList, TS_LIST_ITEM *psNode)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psList->psListHead;

    psNode->psNext = psTemp;
    psNode->psPrevious = psTemp->psPrevious;

    psTemp->psPrevious = psNode;
    psList->psListHead = psNode;

    /* The value of the item is 1 less from the next one */
    psNode->xItemValue = psTemp->xItemValue - 1;
}

/*******************************************************************************
* Function Name     : LIST_AddNodeCurrent()
* Description       : Insert/Add node at the current index,
*   without changing the current index.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void LIST_AddNodeCurrent(TS_LIST *psList, TS_LIST_ITEM *psNode)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psList->psListIndex;

    /* Check if it is the first node */
    if(psTemp == psList->psListHead)
    {
        LIST_AddNode(psList, psNode);
    }
    else
    {
        LIST_InsertItemBefore(psList,psTemp, psNode);
    }
}

#if 0
/*******************************************************************************
* Function Name     : LIST_AppendNode()
* Description       : Add node at the Tail
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void LIST_AppendNode(TS_LIST *psList, TS_LIST_ITEM *psNode)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psList->sListEnd.psPrevious;

    psNode->psNext = &(psList->sListEnd);
    psNode->psPrevious = psTemp;

    psTemp->psNext = psNode;
    psList->sListEnd.psPrevious = psNode;

    /* The value of the last item is 1 less from the max value */
    psNode->xItemValue = psList->sListEnd.xItemValue - 1;

}
#endif

/*******************************************************************************
* Function Name     : LIST_RemoveHead()
* Description       : Remove Node at the head
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS LIST_RemoveHead(TS_LIST *psList)
{
    TS_LIST_ITEM * psTemp;
//    TS_LIST_ITEM * psTemp2;

    STATUS StatusTemp;

    psTemp = psList->psListHead;

    if(psTemp !=(&(psList->sListEnd)) )
    {   /* Only if the list is not empty */
        /* Perform the delete here */
//        psTemp2 = psTemp->psNext;   /* Save the next pointer */
        StatusTemp = LIST_DeleteItem(psList, psTemp);
        /* Update head pointer */
//        if(StatusTemp == STATUS_OK)
//        {
//            psList->psListHead = psTemp2;
//        }
        return StatusTemp;
    }
    return STATUS_ERR_UNDEF;    
}

#if 0
/*******************************************************************************
* Function Name     : LIST_RemoveTail()
* Description       : Remove Node at the tail, just before the end marker
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS LIST_RemoveTail(TS_LIST *psList)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psList->sListEnd.psPrevious;

    if(psTemp !=(&(psList->sListEnd)) )
    {   /* Only if the list is not empty */
        /* Perform the delete here */
        return LIST_DeleteItem(psList, psTemp);
    }
    return STATUS_ERR_UNDEF;    
}
#endif

/*******************************************************************************
* Function Name     : LIST_GetHead()
* Description       : Get the pointer to the head of list Head.
*   Return NULL if the list is empty.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
TS_LIST_ITEM * LIST_GetHead(TS_LIST *psList)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psList->psListHead;

    if(psTemp !=(&(psList->sListEnd)) )
    {   /* Only if the list is not empty */
        return psTemp;
    }
    return (TS_LIST_ITEM *)NULL;    
}

/*******************************************************************************
* Function Name     : LIST_GetTail()
* Description       : Get the pointer to the tail of list Head.
*   (just before the End-Marker). Return NULL if the list is empty.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
TS_LIST_ITEM * LIST_GetTail(TS_LIST *psList)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psList->sListEnd.psPrevious;

    /* walk through the list to find the node to delete. */
    if(psTemp !=(&(psList->sListEnd)) )
    {   /* Only if the list is not empty */
        return psTemp;
    }
    return (TS_LIST_ITEM *)NULL;    
}

/*******************************************************************************
* Function Name     : LIST_GetNext()
* Description       : Get the pointer to the Next Item on the list.
*   Return NULL if there is no Next Item.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
TS_LIST_ITEM * LIST_GetNext(TS_LIST *psList, TS_LIST_ITEM * psCurItem)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psCurItem->psNext;

    if(psTemp !=(&(psList->sListEnd)) )
    {   /* Only if the list is not empty */
        return psTemp;
    }
    return (TS_LIST_ITEM *)NULL;    

//    psTemp = psList->sListEnd.psPrevious;

}

#if 0
/*******************************************************************************
* Function Name     : LIST_GetPrev()
* Description       : Get the pointer to the Previous Item on the list.
*   Return NULL if there is no Previous Item.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
TS_LIST_ITEM * LIST_GetPrev(TS_LIST *psList, TS_LIST_ITEM * psCurItem)
{
    TS_LIST_ITEM * psTemp;

    psTemp = psCurItem->psPrevious;

    if(psTemp !=(&(psList->sListEnd)) )
    {   /* Only if the list is not empty */
        return psTemp;
    }
    return (TS_LIST_ITEM *)NULL;    


}
#endif


#if 0
/*******************************************************************************
* Function Name     : LIST_AddNode()
* Description       : Add node at the head
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
TS_LIST_ITEM * LIST_SearchContainer(TS_LIST *psList, void * pvCont)
{
    TS_LIST_ITEM * psStatus;
    TS_LIST_ITEM * psTemp;

    psStatus = (TS_LIST_ITEM *)NULL;

    psTemp = psList->psListHead;
    while((psTemp != (TS_LIST_ITEM *)NULL) &&
            (psTemp != (TS_LIST_ITEM *)&psList->sListEnd) )
    {
        if(psTemp->pvContainer == pvCont)
        {
            psStatus = psTemp;
            break;
        }
        psTemp = psTemp->psNext;
    }
    return psStatus;
}
#endif

#ifdef __SEQUENTIAL_LIST__
/*******************************************************************************
* Function Name     : LIST_SearchNode()
* Description       : Search List item node by its value
*   This routine will return the pointer to the biggest node that is <= xVal.
*   This routine assume that the list value has been sorted in ascending order.
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
TS_LIST_ITEM * LIST_SearchNode(TS_LIST *psList, LIST_VALUE_TYPE xval)
{
    

}
#endif  /* __SEQUENTIAL_LIST__ */

#if 0
/*******************************************************************************
* Function Name     : LIST_DeleteNode()
* Description       : Delete a node from the list.
*   Ensure the list is not empty before deleting the node.
*   Check that the node is really belong to the list?
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS LIST_DeleteNode(TS_LIST *psList, TS_LIST_ITEM *psNode)
{
    TS_LIST_ITEM * psTemp;

    pTemp = psList->psListHead;

    /* walk through the list to find the node to delete. */
    while( (pTemp != (TS_LIST_ITEM *)NULL) &&
            ( pTemp !=(&(psList->sListEnd)) ) )
    {
        if(pTemp == psNode)
        {
            /* Perform the delete here */
            return LIST_DeleteItem(psList, psNode);
        }
        pTemp++;    /* Next Node in the list */
    }
    return STATUS_ERR_UNDEF;    

}
#endif

/*******************************************************************************
* Function Name     : ()
* Description       : 
*---------------------------------------------------------------------
* Global Variable   : 
*
* Input Parameter   : -
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 10/01/07(ABQ)     :   Initial Version
*******************************************************************************/

