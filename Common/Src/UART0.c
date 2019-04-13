/*******************************************************************************
********************************************************************************
** File Name    : UART.c
** Description  :
**      Implementation of UART
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 26/02/07(ABQ)    1.0         Initial Revision
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

void (*vpUART0_Tx_ISR)(void);     /* Global Pointer to UART Tx ISR*/
void (*vpUART0_Rx_ISR)(void);     /* Global Pointer to UART Rx ISR*/
void (*vpUART0_Err_ISR)(void);     /* Global Pointer to UART Error ISR*/




/************************/
/* API Functions        */
/************************/
/*******************************************************************************
* Function 		: UART0_Connect()
* Description   : Connect the UART0 ISR to UART0 Interrupt and assign
*		priority to it.
* NOTE: In Case of ARM VIC, this function will program the UART0 as Vectored
*		Interrupt number of bPriority.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: ISR address, Priority
* Output 		: None
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_Connect(P_ISR_CODE pISRCode, BYTE bPriority)
{
    UINT32 u32IntEnBackup;
    volatile UINT32 * u32pTemp;    


    u32IntEnBackup = VICIntEnable;  /* Saves */
//    VICIntEnable &= (~UART0_INT_EN_MASK);   /* Disable UART0 Interrupt first */
    VICIntEnClr = (UART0_INT_EN_MASK);   /* Disable UART0 Interrupt first */
	VICIntEnClr = 0;

	if(NULL != pISRCode)	/* If the pointer is NULL, leave the interrupt disabled and return. */
	{
	    bPriority &= PRIORITY_MASK;

	    /* Set the Vector Control with the interrupt number */
	    u32pTemp = (&VICVectCntl0+ (bPriority<<2) );
	    (*u32pTemp) = UART0_INT_NUMBER;
	    /* Write Vector address with the ISR Address */
	    u32pTemp = (&VICVectAddr0+ (bPriority<<2) );
	    (*u32pTemp) = (UINT32)pISRCode;

    	u32pTemp = (UINT32*)U0IIR; /* Dummy Read IIR Register to clear pending interrupt. */

	    VICIntEnable = u32IntEnBackup;  /* Restore */
	}

}

/*******************************************************************************
* Function 		: UART0_SetRate()
* Description   : Initialize UART0 BAUD Rate and the mode of UART0
*		(Number of bit per byte, Parity Mode, Stop bit length)
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: BAUD rate descriptor, Mode of UART.
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_SetRate(BYTE bBAUD, BYTE bMode)
{
    U0LCR |= UART_DLAB_MASK;

    switch(bBAUD)
    {
        case UART_SPEED_9600:   /*9664: 0.6%*/
            U0DLL = 97;
            U0DLM = 0;
            break;
        case UART_SPEED_19200:  /*19531: 1.7%*/
            U0DLL = 48;
            U0DLM = 0;
            break;
        case UART_SPEED_38400:  /*39062: 1.7%*/
            U0DLL = 24;
            U0DLM = 0;
            break;
        case UART_SPEED_57600:  /*58593: 1.7%*/
            U0DLL = 16;
            U0DLM = 0;
            break;
        case UART_SPEED_115200: /*117187: 1.7%*/
            U0DLL = 8;
            U0DLM = 0;
            break;
        default:	/* BAUD rate not known. Reset DLAB and return. */
            U0LCR &= (~UART_DLAB_MASK);
            return;
    }

    U0LCR = bMode;
    U0LCR &= (~UART_DLAB_MASK);
	UART0_Clear();
}

/*******************************************************************************
* Function 		: UART0_IsRx()
* Description   : Check if Data has been received by UART0.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: -
* Output 		: Status: STATUS_TRUE if at least 1 byte of data has been received.
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS UART0_IsRx(void)
{
	if((U0LSR&UART_STAT_RDR))
	{
		return STATUS_TRUE;
	}
	return STATUS_FALSE;
}

/*******************************************************************************
* Function 		: UART0_IsTxEmpty()
* Description   : Check if UART Transmit buffer is empty or at least can transfer
*		one byte of data.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: -
* Output 		: Return STATUS_TRUE if the transmiter is able to transmit a byte.
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
STATUS UART0_IsTxEmpty(void)
{
	if((U0LSR&UART_STAT_THRE) == 0)
	{
		return STATUS_FALSE;
	}
	return STATUS_TRUE;
}

/*******************************************************************************
* Function 		: UART0_SendByte()
* Description   : Send a byte (without checking whether the UART is ready to Transmit.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: Byte to send.
* Output 		: None
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_SendByte(BYTE bData)
{
    U0THR = bData;  /* Send! */
}

/*******************************************************************************
* Function 		: UART0_SendByteWait()
* Description   : Send a byte after the Transmitted ready.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: Byte to send.
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_SendByteWait(BYTE bData)
{
    while(UART0_IsTxEmpty() == STATUS_FALSE)
    {   /* Wait until Transmit holding register empty. */
    }
    U0THR = bData;  /* Send! */
}

/*******************************************************************************
* Function 		: UART0_GetByte()
* Description   : Wait and Get a byte from UART Rx Buffer.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: -
* Output 		: BYTE received by UART.
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
BYTE UART0_GetByte(void)
{
    while(UART0_IsRx() == STATUS_FALSE)
    {   /* Wait until Receive Data Ready. */
    }
    return U0RBR;  /* Receive! */

}

/*******************************************************************************
* Function 		: UART0_Clear()
* Description   : Clear All Data in FIFO Buffer (thus also clear the status of UART.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_Clear(void)	/* Clear all data in FIFO buffer. */
{
	U0FCR = 0x07;
}

/*******************************************************************************
* Function 		: UART0_SetRxInt()
* Description   : Set the Data Received and Character Timeout ISR.
*	NOTE: ISR Address equal to NULL is used to disable the interrupt.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: ISR Address
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_SetRxInt(P_ISR_CODE pISRCode)
{
	if(pISRCode == NULL)
	{
		U0IER &= 0xFE;
	}
	else
	{
		U0IER |= 0x01;
	}
	vpUART0_Rx_ISR = pISRCode;
}

/*******************************************************************************
* Function 		: UART0_SetErrInt()
* Description   : Set the UART Error ISR (Framing, Parity, etc)
*	NOTE: ISR Address equal to NULL is used to disable the interrupt.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_SetErrInt(P_ISR_CODE pISRCode)
{
	if(pISRCode == NULL)
	{
		U0IER &= 0xFB;
	}
	else
	{
		U0IER |= 0x04;
	}
	vpUART0_Err_ISR = pISRCode;
}

/*******************************************************************************
* Function 		: UART0_SetTxInt()
* Description   : Set the UART Transmit Empty ISR.
*	NOTE: ISR Address equal to NULL is used to disable the interrupt.
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: ISR Address
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_SetTxInt(P_ISR_CODE pISRCode)
{
	if(pISRCode == NULL)
	{
		U0IER &= 0xFD;
	}
	else
	{
		U0IER |= 0x02;
	}
	vpUART0_Tx_ISR = pISRCode;
}

/*******************************************************************************
* Function 		: UART0_ISR()
* Description   : Main ISR Entry point for UART0 that do the dispatching
*		depending on the type of the UART Interrupt (Tx Empty, Data Rx, or Err).
*---------------------------------------------------------------------
* Global 		: -
*
* Input 		: -
* Output 		: -
* Subroutine	: -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 30/03/07(ABQ)     :   Initial Version
*******************************************************************************/
void UART0_ISR(void) INTERRUPT
{
	BYTE bTemp;
/* Check the cause of the interrupt from U0IIR */
	bTemp = U0IIR;
	bTemp &= 0x0E;

/* And dispatch ISR according to the Interrupt source. */
	switch(bTemp)
	{
		case 0x06:	/* Receive Line Status: Error on Receive Line. */
			if(vpUART0_Err_ISR != NULL)
			{
				vpUART0_Err_ISR();
			}
		break;

		case 0x04:	/* Receive Data Available */
		case 0x0C:	/* Character Time out Indicator */
			if(vpUART0_Err_ISR != NULL)
			{
				vpUART0_Rx_ISR();
			}
		break;

		case 0x02:	/* THRE (Transmit Holding Register Empty) interrupt */
			if(vpUART0_Err_ISR != NULL)
			{
				vpUART0_Tx_ISR();
			}
		break;

		default:
		break;
	}

/* Clear the pending interrupt by writing to VICVectAddr*/
	VICVectAddr = 0;
}






