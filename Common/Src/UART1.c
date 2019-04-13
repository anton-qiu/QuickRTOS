/*******************************************************************************
********************************************************************************
** File Name    : UART1.c
** Description  :
**      Implementation of UART1
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 30/03/07(ABQ)    1.0         Initial Revision
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

void (*vpUART1_Tx_ISR)(void);     /* Global Pointer to UART Tx ISR*/
void (*vpUART1_Rx_ISR)(void);     /* Global Pointer to UART Rx ISR*/
void (*vpUART1_Err_ISR)(void);     /* Global Pointer to UART Error ISR*/




/************************/
/* API Functions        */
/************************/
/*******************************************************************************
* Function 		: UART1_Connect()
* Description   : Connect the UART1 ISR to UART1 Interrupt and assign
*		priority to it.
* NOTE: In Case of ARM VIC, this function will program the UART1 as Vectored
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
void UART1_Connect(P_ISR_CODE pISRCode, BYTE bPriority)
{
    UINT32 u32IntEnBackup;
    volatile UINT32 * u32pTemp;    


    u32IntEnBackup = VICIntEnable;  /* Saves */
//    VICIntEnable &= (~UART1_INT_EN_MASK);   /* Disable UART1 Interrupt first */
    VICIntEnClr = (UART1_INT_EN_MASK);   /* Disable UART1 Interrupt first */
	VICIntEnClr = 0;

	if(NULL != pISRCode)	/* If the pointer is NULL, leave the interrupt disabled and return. */
	{
	    bPriority &= PRIORITY_MASK;

	    /* Set the Vector Control with the interrupt number */
	    u32pTemp = (&VICVectCntl0+ (bPriority<<2) );
	    (*u32pTemp) = UART1_INT_NUMBER;
	    /* Write Vector address with the ISR Address */
	    u32pTemp = (&VICVectAddr0+ (bPriority<<2) );
	    (*u32pTemp) = (UINT32)pISRCode;

    	u32pTemp = (UINT32*)U1IIR; /* Dummy Read IIR Register to clear pending interrupt. */

	    VICIntEnable = u32IntEnBackup;  /* Restore */
	}

}

/*******************************************************************************
* Function 		: UART1_SetRate()
* Description   : Initialize UART1 BAUD Rate and the mode of UART1
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
void UART1_SetRate(BYTE bBAUD, BYTE bMode)
{
    U1LCR |= UART_DLAB_MASK;

    switch(bBAUD)
    {
        case UART_SPEED_9600:   /*9664: 0.6%*/
            U1DLL = 97;
            U1DLM = 0;
            break;
        case UART_SPEED_19200:  /*19531: 1.7%*/
            U1DLL = 48;
            U1DLM = 0;
            break;
        case UART_SPEED_38400:  /*39062: 1.7%*/
            U1DLL = 24;
            U1DLM = 0;
            break;
        case UART_SPEED_57600:  /*58593: 1.7%*/
            U1DLL = 16;
            U1DLM = 0;
            break;
        case UART_SPEED_115200: /*117187: 1.7%*/
            U1DLL = 8;
            U1DLM = 0;
            break;
        default:	/* BAUD rate not known. Reset DLAB and return. */
            U1LCR &= (~UART_DLAB_MASK);
            return;
    }

    U1LCR = bMode;
    U1LCR &= (~UART_DLAB_MASK);
	UART1_Clear();
}

/*******************************************************************************
* Function 		: UART1_IsRx()
* Description   : Check if Data has been received by UART1.
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
STATUS UART1_IsRx(void)
{
	if((U1LSR&UART_STAT_RDR))
	{
		return STATUS_TRUE;
	}
	return STATUS_FALSE;
}

/*******************************************************************************
* Function 		: UART1_IsTxEmpty()
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
STATUS UART1_IsTxEmpty(void)
{
	if((U1LSR&UART_STAT_THRE) == 0)
	{
		return STATUS_FALSE;
	}
	return STATUS_TRUE;
}

/*******************************************************************************
* Function 		: UART1_SendByte()
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
void UART1_SendByte(BYTE bData)
{
    U1THR = bData;  /* Send! */
}

/*******************************************************************************
* Function 		: UART1_SendByteWait()
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
void UART1_SendByteWait(BYTE bData)
{
    while(UART1_IsTxEmpty() == STATUS_FALSE)
    {   /* Wait until Transmit holding register empty. */
    }
    U1THR = bData;  /* Send! */
}

/*******************************************************************************
* Function 		: UART1_GetByte()
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
BYTE UART1_GetByte(void)
{
    while(UART1_IsRx() == STATUS_FALSE)
    {   /* Wait until Receive Data Ready. */
    }
    return U1RBR;  /* Receive! */

}

/*******************************************************************************
* Function 		: UART1_Clear()
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
void UART1_Clear(void)	/* Clear all data in FIFO buffer. */
{
	U1FCR = 0x07;
}

/*******************************************************************************
* Function 		: UART1_SetRxInt()
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
void UART1_SetRxInt(P_ISR_CODE pISRCode)
{
	if(pISRCode == NULL)
	{
		U1IER &= 0xFE;
	}
	else
	{
		U1IER |= 0x01;
	}
	vpUART1_Rx_ISR = pISRCode;
}

/*******************************************************************************
* Function 		: UART1_SetErrInt()
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
void UART1_SetErrInt(P_ISR_CODE pISRCode)
{
	if(pISRCode == NULL)
	{
		U1IER &= 0xFB;
	}
	else
	{
		U1IER |= 0x04;
	}
	vpUART1_Err_ISR = pISRCode;
}

/*******************************************************************************
* Function 		: UART1_SetTxInt()
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
void UART1_SetTxInt(P_ISR_CODE pISRCode)
{
	if(pISRCode == NULL)
	{
		U1IER &= 0xFD;
	}
	else
	{
		U1IER |= 0x02;
	}
	vpUART1_Tx_ISR = pISRCode;
}

/*******************************************************************************
* Function 		: UART1_ISR()
* Description   : Main ISR Entry point for UART1 that do the dispatching
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
void UART1_ISR(void) INTERRUPT
{
	BYTE bTemp;
/* Check the cause of the interrupt from U1IIR */
	bTemp = U1IIR;
	bTemp &= 0x0E;

/* And dispatch ISR according to the Interrupt source. */
	switch(bTemp)
	{
		case 0x06:	/* Receive Line Status: Error on Receive Line. */
			if(vpUART1_Err_ISR != NULL)
			{
				vpUART1_Err_ISR();
			}
		break;

		case 0x04:	/* Receive Data Available */
		case 0x0C:	/* Character Time out Indicator */
			if(vpUART1_Err_ISR != NULL)
			{
				vpUART1_Rx_ISR();
			}
		break;

		case 0x02:	/* THRE (Transmit Holding Register Empty) interrupt */
			if(vpUART1_Err_ISR != NULL)
			{
				vpUART1_Tx_ISR();
			}
		break;

		default:
		break;
	}

/* Clear the pending interrupt by writing to VICVectAddr*/
	VICVectAddr = 0;
}






