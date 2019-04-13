/*******************************************************************************
********************************************************************************
** File Name    : List.h
** Description  : Generic Double Linked List implementation.
**      Generic Linked List implementation
**      Implementation of Linked List without any specific type of object.
**      For each list item, the object could be in any type, pointed by its
**          pvContainer pointer. It is the responsibility of the
**          owner/application to know what type of object it is refering to.
**      pvOwner points to the object that own the list item whenever applicable.
**          For example, for RTOS, a resource could be held by a particular task.
**          In the example above, the pvOwner could point to the TCB of the task.
**      xItemValue is used to store a value, and may be used by list to sort and
**          insert the list.
**      There are 2 type of structure defined:
**          > TS_LIST_ITEM is the primitive list item
**          > TS_LIST is "container" of the list item primitive,
**              to better manage/browse the link list.
**      There are several way to use this linked list:
**          > Simple Linked list, which contain only series of
**              primitive list item(TS_LIST_ITEM). There is no sorting or
**              management of the list build in, user have to manage by themself.
**          > Custom: user could use the simple linked list above and create
**              additional "wrapper" to suit their own needs.
**              This is very useful if the user have different requirement of list
**              than the feature of the list below.
**      TODO:
**          > Managed linked list using TS_LIST with these features:
**              - Some information is added to navigate through the list or to
**                  keep track the current active item.
**              - Track the begining and as end of the list.
**              - Maximum value of xItemValue
**              - Insert new list item before the list with certain xItemValue
**                  or less.
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

#ifndef __LIST_H__
#define __LIST_H__

/***********************/
/* OPTIMIZATION OPTION */
/***********************/

/**********************/
/* Value Definitions  */
/**********************/
#define LIST_VALUE_TYPE     UINT32

//#define __SEQUENTIAL_LIST__     /* The list is always keep sorted by value */
#undef __SEQUENTIAL_LIST__      /* The list does not ensure the value sorting */

/************************/
/* Data Structure       */
/************************/
typedef struct S_LIST_ITEM
{
	LIST_VALUE_TYPE xItemValue;    /*< The value being listed.  In most cases this is used to sort the list in descending order. */
	struct S_LIST_ITEM * psNext;   /*< Pointer to the next xListItem in the list. */
	struct S_LIST_ITEM * psPrevious;/*< Pointer to the previous xListItem in the list. */
	void * pvOwner;     /*< Pointer to the object (normally a TCB) that contains the list item.  There is therefore a two way link between the object containing the list item and the list item itself. */
	void * pvContainer; /*< Pointer to the list in which this list item is placed (if any). */
} TS_LIST_ITEM;

typedef struct S_LIST
{
	LEN_TYPE NumberOfItems;
	TS_LIST_ITEM * psListHead;   /*< Pointer to the xListEnd item.  xListEnd contains a wrap back pointer to true list head. */
	TS_LIST_ITEM * psListIndex;  /*< Used to walk through the list.  Points to the last item returned by a call to pvListGetOwnerOfNextEntry(). */
	TS_LIST_ITEM sListEnd;   /*< List item that contains the maximum possible item value meaning it is always at the end of the list and is therefore used as a marker. */
} TS_LIST;


/************************/
/* Variable Declaration */
/************************/

/**********************/
/* Function Prototype */
/**********************/
extern void LIST_InitItem(TS_LIST_ITEM *psListItem,
                    TS_LIST_ITEM *psNxt,
                    TS_LIST_ITEM *psPrev);
extern void LIST_ItemSetValue(TS_LIST_ITEM *psListItem,
                    LIST_VALUE_TYPE xItemVal,
                    void * pxOwn,
                    void * pxCont);
extern STATUS LIST_InsertItemBefore(TS_LIST *psList, TS_LIST_ITEM *psListItem, TS_LIST_ITEM *psNewListItem);
extern STATUS LIST_InsertItemAfter(TS_LIST_ITEM *psListItem, TS_LIST_ITEM *psNewListItem);
extern STATUS LIST_DeleteItem(TS_LIST *psList, TS_LIST_ITEM *psItemToDelete);


extern void LIST_Init(TS_LIST *psList, LIST_VALUE_TYPE xMaxval);
extern void LIST_AddNode(TS_LIST *psList, TS_LIST_ITEM *psNode);
extern void LIST_AddNodeCurrent(TS_LIST *psList, TS_LIST_ITEM *psNode);
extern void LIST_AppendNode(TS_LIST *psList, TS_LIST_ITEM *psNode);
extern STATUS LIST_RemoveHead(TS_LIST *psList);
extern STATUS LIST_RemoveTail(TS_LIST *psList);

extern TS_LIST_ITEM * LIST_GetHead(TS_LIST *psList);
extern TS_LIST_ITEM * LIST_GetTail(TS_LIST *psList);
extern TS_LIST_ITEM * LIST_GetNext(TS_LIST *psList, TS_LIST_ITEM * psCurItem);
extern TS_LIST_ITEM * LIST_GetPrev(TS_LIST *psList, TS_LIST_ITEM * psCurItem);

extern TS_LIST_ITEM * LIST_SearchContainer(TS_LIST *psList, void * pvCont);

#endif  /* __LIST_H__ */



