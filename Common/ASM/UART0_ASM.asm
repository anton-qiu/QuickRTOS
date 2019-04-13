

$INCLUDE (.\Common\ASM\include.asm)

AREA   UART_ASM_Code, CODE   // READONLY, ALIGN=4
RSEG UART_ASM_Code


;;******************************************************************************
;;** void UART0_Connect(P_ISR_CODE pISRCode, UINT16 wMODE)
;;******************************************************************************
    IF (ENV_RVDS == 1)
        GLOBAL UART0_Connect
    CODE32
UART0_Connect PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC UART0_Connect?A
UART0_Connect?A PROC CODE32
    ENDIF

    ENDP


        END
