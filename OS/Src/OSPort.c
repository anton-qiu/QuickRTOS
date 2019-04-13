/*******************************************************************************
********************************************************************************
** File Name    : OSPort.c
** Description  :
**      Implementation of portable functions
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
/* Global Variable      */
/************************/
BASE_TYPE XDATA xCriticalNesting;

/************************/
/* Private Variable     */
/************************/

/************************/
/* API Functions        */
/************************/
/*******************************************************************************
* Function Name     : OS_EnterCritical()
* Description       : Enter Critical Section
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
void OS_EnterCritical(void)
{
	/* Disable interrupts */
/* There is no need to save R0 as it is considered as scratchpad register */
//	__asm{ STMDB	SP!, {R0}		};	/* Push R0. */
	__asm{ MRS		R0, CPSR		};	/* Get CPSR. */
	__asm{ ORR		R0, R0, #0xC0	};	/* Disable IRQ, FIQ. */
	__asm{ MSR		CPSR_CXSF, R0	};	/* Write back modified value. */
//	__asm{ LDMIA	SP!, {R0}		};	/* Pop R0. */

    /* After the interrupt disabled, the xCriticalNesting value can be incremented */
	xCriticalNesting++;
}


/*******************************************************************************
* Function Name     : OS_ExitCritical()
* Description       : Exit Critical Section
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
void OS_ExitCritical(void)
{
	if(xCriticalNesting > NO_CRITICAL_NESTING)
	{
		/* Decrement the nesting level */
		xCriticalNesting--;

		/* If the nesting level has reached zero then interrupts should be
		re-enabled. */
		if( xCriticalNesting == NO_CRITICAL_NESTING )
		{
			/* Enable interrupts as per portEXIT_CRITICAL(). */
			__asm{ MRS		R0, CPSR		};	/* Get CPSR.						*/
			__asm{ BIC		R0, R0, #0xC0	};	/* Enable IRQ, FIQ.					*/
			__asm{ MSR		CPSR_CXSF, R0	};	/* Write back modified value.		*/
		}
	}
}

/*******************************************************************************
* Function Name     : OS_InitContextBackup()
* Description       : Initialize the context backup in software stack
*   Clear and initialize Stack: This includes initializing the following to the stack:
*       > TaskCode
*       > Registers
*       > Argument pointer
*       > Additional information
*       (for example, if ARM and Thumb mode supported,
*       the type of code for the task (ARM or THUMB) need to be stored as well)
*   Task Stack may be created and used exclusively for each task. 
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
STACK_TYPE * OS_InitContextBackup(STACK_TYPE *pxTopOfStack, P_TASK_CODE pxCode,
                                void * pvParameters)
{
//    STACK_TYPE *pxOriginalTOS;

	/* Setup the initial stack of the task.  The stack is set exactly as 
	expected by the portRESTORE_CONTEXT() macro.

	Remember where the top of the (simulated) stack is before we place 
	anything on it. */
//	pxOriginalTOS = pxTopOfStack;

	/* First on the stack is the return address - which in this case is the
	start of the task.  The offset is added to make the return address appear
	as it would within an IRQ ISR. */
	*pxTopOfStack = ( STACK_TYPE ) pxCode;		
	pxTopOfStack--;

	*pxTopOfStack = (STACK_TYPE) 0xaaaaaaaa;	/* R14 */
	pxTopOfStack--;	
//	*pxTopOfStack = (STACK_TYPE) pxOriginalTOS;
//	pxTopOfStack--;
	*pxTopOfStack = (STACK_TYPE) 0x12121212;	/* R12 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x11111111;	/* R11 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x10101010;	/* R10 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x09090909;	/* R9 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x08080808;	/* R8 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x07070707;	/* R7 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x06060606;	/* R6 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x05050505;	/* R5 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x04040404;	/* R4 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x03030303;	/* R3 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x02020202;	/* R2 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) 0x01010101;	/* R1 */
	pxTopOfStack--;	
	*pxTopOfStack = (STACK_TYPE) pvParameters; /* R0 */
	pxTopOfStack--;

	/* The last thing onto the stack is the status register, which is set for
	system mode, with interrupts enabled. */
	*pxTopOfStack = (STACK_TYPE) INITIAL_SPSR;

//    #ifdef KEIL_THUMB_INTERWORK
//    {		
//        /* We want the task to start in thumb mode. */
//        *pxTopOfStack |= portTHUMB_MODE_BIT;
//    }
//    #endif

//	pxTopOfStack--;

//    /* The code generated by the Keil compiler does not maintain separate
//    stack and frame pointers. The portENTER_CRITICAL macro cannot therefore
//    use the stack as per other ports.  Instead a variable is used to keep
//    track of the critical section nesting.  This variable has to be stored
//    as part of the task context and is initially set to zero. */
//    *pxTopOfStack = NO_CRITICAL_NESTING;

	return pxTopOfStack;
}

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
* 05/01/07(ABQ)     :   Initial Version
*******************************************************************************/
