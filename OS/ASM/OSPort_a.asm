
$INCLUDE (.\Common\ASM\include.asm)

AREA   OS_PORT_ASM, CODE   // READONLY, ALIGN=4
    ENTRY

            EXTERN DATA  (psCurrentTCB)

            PUBLIC  OS_RestoreContext?A
            PUBLIC  OS_ContextSwitch?A

;;/*******************************************************************************
;;* Function Name     : void TASK_SAVECONTEXT(void)
;;* Description       : Saves current processor state to (Task)Stack
;;*                     and update psCurrentTCB->pTopOfStack with the current SP.
;;*     Should only be called from System/User Mode.
;;*     DO NOT CALL FROM EXCEPTION MODE.
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

TASK_SAVECONTEXT MACRO   /* Currently no parameters */
;;-----------------------------
    STMFD   SP!, {LR}               ;;  Push return address
    STMFD   SP, {LR}^               ;;  Push User LR
    SUB		SP, SP, #4      ;; then update SP
    STMFD   SP!, {R0-R12}           ;;  Push registers
    MRS     R1,  CPSR               ;;  Push current CPSR
    STMFD   SP!, {R1}

    ;; Saves Top of Stack to psCurrentTCB->pTopOfStack
    LDR     R1, =psCurrentTCB   ;; R1 = &psCurrentTCB
    LDR     R2, [R1]    ;; R2 = psCurrentTCB
    STR     SP, [R2]    ;; psCurrentTCB->pTopOfStack = SP

    ENDM

;;/*******************************************************************************
;;* Function Name     : void OS_RestoreContext(void)
;;* Description       : Restore Context from psCurrentTCB->pTopOfStack
;;*     (Current Task Stack) to restore processor registers state.
;;*                     and jump to the task code (in Program Counter).
;;*     Should only be called from System/User Mode.
;;*     DO NOT CALL FROM EXCEPTION MODE.
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
    IF (ENV_RVDS == 1)
    CODE32
OS_RestoreContext?A       PROC
    ENDIF

    IF (ENV_KEIL == 1)
OS_RestoreContext?A       PROC    CODE32
    ENDIF
        LDR     R1, =psCurrentTCB   ;;R1 = &(psCurrentTCB)
        LDR     R0, [R1]            ;;R0 = psCurrentTCB
        LDR     SP, [R0]            ;;SP = (psCurrentTCB->pTopOfStack)

        LDMFD   SP!, {R0}           ;; Pop CPSR
        MSR     SPSR_cxsf, R0

        LDMFD   SP!, {R0-R12,LR,PC}^    ;; Pop  registers and jump 
        
        ENDP

;;/*******************************************************************************
;;* Function Name     : void OS_ContextSwitch(TS_TCB *psDest)
;;* Description       : Perform software context switch (Force Context Switch).
;;*         This function will internally save the context, and restore
;;*         context from the target TCB.
;;*     Should only be called from System/User Mode.
;;*     DO NOT CALL FROM EXCEPTION MODE.
;;*---------------------------------------------------------------------
;;* Global Variable   : psCurrentTCB
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : OS_RestoreContext()
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
    CODE32
OS_ContextSwitch?A       PROC
    ENDIF

    IF (ENV_KEIL == 1)
OS_ContextSwitch?A       PROC    CODE32
    ENDIF
;; SAVE CONTEXT
    TASK_SAVECONTEXT

    STR     R0, [R1]    ;; psCurrentTCB = psDest;

;; RESTORE CONTEXT
    B OS_RestoreContext?A
        ENDP

;;----------------------------------------------------------------------------
;;----------------------------------------------------------------------------


        END
