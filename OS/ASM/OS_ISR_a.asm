
$INCLUDE (.\Common\ASM\include.asm)

AREA   OS_ISR_ASM, CODE   // READONLY, ALIGN=4
    ENTRY
            EXTERN DATA  (psCurrentTCB)
            EXTERN CODE32  (OS_SWI_Schedule?A)

            EXTERN CODE32  (OS_VTimer_Handler?A)


;;/*******************************************************************************
;;* Function Name     : void SAVECONTEXT_INT(void)
;;* Description       : Saves current processor state to (Task)Stack
;;*                     and update psCurrentTCB->pTopOfStack with the current SP.
;;*     Should only be called from EXCEPTION MODE (IRQ/FIQ/SWI).
;;*---------------------------------------------------------------------
;;* Global Variable   : psCurrentTCB
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
SAVECONTEXT_INT MACRO   /* Currently no parameters */
//LOCAL local-labels    /* Currently no LOCAL */
    /* First Save the R1 before using it */
    STMDB 	SP!, {R1}

    MRS R1, SPSR
    TST R1, #0x00000020    ;; Check ARM/Thumb
    SUBEQ LR, LR, #4    ;; Set ARM return
    SUBNE LR, LR, #2    ;; Set Thumb return

	/* Now Get the SP of System/User Mode to R1 */
    STMDB	SP,{SP}^
    SUB		SP, SP, #4
    LDMIA	SP!,{R1}
    /* Saves the Return address (PC of the User/System mode) to System/User Stack */
    STMDB	R1!, {LR}
    /* Use LR as User/System mode Stack and restore back the original R1 */
    MOV		LR, R1
    LDMIA	SP!, {R1}
    /* Push all User/System mode registers and update the stack address */
    STMDB	LR,{R0-R12, LR}^
    SUB		LR, LR, #56 /* 60 bytes = 15 word registers */
    /* Push the SPSR onto the task stack. */
    MRS		R1, SPSR
    STMDB	LR!, {R1}
    ;; Saves Top of Stack to psCurrentTCB->pTopOfStack
    /* Saves the task stack (System/User) into (psCurrentTCB->pTopOfStack) */
    LDR		R1, =psCurrentTCB   ;; R1 = &psCurrentTCB
    LDR		R2, [R1]            ;; R2 = psCurrentTCB
    STR		LR, [R2]            ;; psCurrentTCB->pTopOfStack = SP

    ENDM

SAVECONTEXT_INT_SWI MACRO   /* Currently no parameters */
//LOCAL local-labels    /* Currently no LOCAL */
    /* First Save the R1 before using it */
    STMDB 	SP!, {R1}

	/* Now Get the SP of System/User Mode to R1 */
    STMDB	SP,{SP}^
    SUB		SP, SP, #4
    LDMIA	SP!,{R1}
    /* Saves the Return address (PC of the User/System mode) to System/User Stack */
    STMDB	R1!, {LR}
    /* Use LR as User/System mode Stack and restore back the original R1 */
    MOV		LR, R1
    LDMIA	SP!, {R1}
    /* Push all User/System mode registers and update the stack address */
    STMDB	LR,{R0-R12, LR}^
    SUB		LR, LR, #56 /* 60 bytes = 15 word registers */
    /* Push the SPSR onto the task stack. */
    MRS		R1, SPSR
    STMDB	LR!, {R1}
    ;; Saves Top of Stack to psCurrentTCB->pTopOfStack
    /* Saves the task stack (System/User) into (psCurrentTCB->pTopOfStack) */
    LDR		R1, =psCurrentTCB   ;; R1 = &psCurrentTCB
    LDR		R2, [R1]            ;; R2 = psCurrentTCB
    STR		LR, [R2]            ;; psCurrentTCB->pTopOfStack = SP

    ENDM

RESTORECONTEXT_INT MACRO   /* Currently no parameters */

    LDR     R1, =psCurrentTCB   ;;R1 = &(psCurrentTCB)
    LDR     R2, [R1]            ;;R0 = psCurrentTCB
    LDR     LR, [R2]            ;;SP = (psCurrentTCB->pTopOfStack)

    LDMFD   LR!, {R2}           ;; Pop CPSR
    MSR     SPSR_cxsf, R2

    LDMFD	LR, {R0-R12}^       ;; Pop system registers
    NOP                     ;; Make sure no access to Banked registers
    ;; And then update the stack address
    ADD		LR, LR, #52         ;; 52 bytes = 13 registers

    ;; Get the R12 and SP into exception's Stack?
    STMDB 	SP!, {R12}      ;; PUSH R12

    ADD R12, LR, #8     ;; with value of LR and PC
    STMDB 	SP!, {R12}  ;; PUSH SP

    LDMFD   SP!, {R13}^     ;; Pop Task SP from Exception's stack 
    LDMFD   SP!, {R12}      ;; Pop back R12 

;;    LDMFD	LR, {LR, PC}^   ;; POP LR and PC (Return)
    LDMFD	LR, {LR}^   ;; POP LR and PC (Return)
    NOP                     ;; Make sure no access to Banked registers
    ADD		LR, LR, #4
    LDMFD	LR, {PC}^   ;; POP PC (Return)

    ENDM

;;/*******************************************************************************
;;* Function Name     : void SWI_Handler(TS_TCB * psDest)
;;* Description       : Handler of SWI Interrupt for Interrupt driven context switch.
;;*---------------------------------------------------------------------
;;* Global Variable   : psCurrentTCB
;;*
;;* Input Parameter   : R0 contain the destination TCB pointer (psDest)
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
    CODE32
        GLOBAL SWI_Handler
SWI_Handler       PROC
    ENDIF

    IF (ENV_KEIL == 1)
            PUBLIC  SWI_Handler?A
SWI_Handler?A       PROC    CODE32
    ENDIF
    /* Save Context to the current context */
    SAVECONTEXT_INT_SWI

	MOV	R0, #0


    BL OS_SWI_Schedule?A    ;; OS_Schedule();

    RESTORECONTEXT_INT    

                ENDP


/*******************************************************************************
* Function 		: OS_VTimer_ISR()
* Description   : Handle for VTIMER Interrupt.
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
    IF (ENV_RVDS == 1)
    CODE32
        GLOBAL OS_VTimer_ISR
OS_VTimer_ISR       PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC  OS_VTimer_ISR?A
OS_VTimer_ISR?A     PROC    CODE32
    ENDIF

    SAVECONTEXT_INT

    BL OS_VTimer_Handler?A

    RESTORECONTEXT_INT    

        ENDP

        END
