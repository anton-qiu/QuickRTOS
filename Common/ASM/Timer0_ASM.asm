
$INCLUDE (.\Common\ASM\include.asm)

;; For some reason, Keil does not allow 2 segment in the same file.
;;AREA   Timer_ASM_Data, DATA, READWRITE, ALIGN=4
;;RSEG Timer_ASM_Data
;;vpTimer0Counter1_ISR:
;;    DSD 1
;;vpTimer0Counter2_ISR:
;;    DSD 1
;;vpTimer0Counter3_ISR:
;;    DSD 1
;;vpTimer0Counter4_ISR:
;;    DSD 1

AREA   Timer_ASM_Code, CODE   // READONLY, ALIGN=4
RSEG Timer_ASM_Code
;    ENTRY

    EXTERN DATA  (vpTimer0Counter0_ISR)
    EXTERN DATA  (vpTimer0Counter1_ISR)
    EXTERN DATA  (vpTimer0Counter2_ISR)
    EXTERN DATA  (vpTimer0Counter3_ISR)

T0_INT_EN_MASK      EQU 0x10
T0_INT_NUMBER       EQU 0x04

T0_IR_ALL           EQU 0xFF
T0_IR_MR0_MASK      EQU 0x01
T0_IR_MR1_MASK      EQU 0x02
T0_IR_MR2_MASK      EQU 0x04
T0_IR_MR3_MASK      EQU 0x08
T0_IR_CR0_MASK      EQU 0x10
T0_IR_CR1_MASK      EQU 0x20
T0_IR_CR2_MASK      EQU 0x40
T0_IR_CR3_MASK      EQU 0x80

T0_TCR_ENABLE       EQU 0x01
T0_TCR_DISABLE      EQU 0x00
T0_TCR_RESET        EQU 0x02

T0_MCR0_MASK        EQU 0x0007
T0_MCR1_MASK        EQU 0x0038
T0_MCR2_MASK        EQU 0x01C0
T0_MCR3_MASK        EQU 0x0E00



;;/*******************************************************************************
;;* MACRO Name        : 
;;* Description       : 
;;*
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
TEMPLATE MACRO   /* Currently no parameters */
//LOCAL local-labels    /* Currently no LOCAL */

    ENDM

;;/*******************************************************************************
;;* Function Name     : void Timer0_ISR(void) INTERRUPT
;;* Description       : Main Entry point to the Timer 0 ISR. This function will
;;*         dispatch the execution to the corresponding handler depends on the
;;*         interrupt source (Match on Counter 0, or Counter 1, Counter 2, or Counter 3)
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer0Counter0_ISR, vpTimer0Counter1_ISR,
;;*                     vpTimer0Counter2_ISR, vpTimer0Counter3_ISR
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 16/02/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_ISR
    CODE32
Timer0_ISR PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_ISR?A
Timer0_ISR?A PROC CODE32
    ENDIF
    
    STMFD R13!, {R0-R12}
    MRS R4, SPSR
    TST R4, #0x00000020    ;; Check ARM/Thumb
    SUBEQ LR, LR, #4    ;; Set ARM return
    SUBNE LR, LR, #2    ;; Set Thumb return
    STMFD SP!, {LR}

    LDR R4, =T0IR    ;; Read Timer 0 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T0_IR_MR0_MASK   ;; Counter Increment Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Counter1_Int   ;; skip if zero
;; Handle for Counter 0 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer0Counter0_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer0Counter0_ISR()

Counter1_Int:
    LDR R4, =T0IR    ;; Read Timer 0 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T0_IR_MR1_MASK  ;; Alarm Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Counter2_Int   ;; skip if zero
;; Handle for Counter 1 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer0Counter1_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer0Counter1_ISR()

Counter2_Int:
    LDR R4, =T0IR    ;; Read Timer 0 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T0_IR_MR2_MASK  ;; Alarm Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Counter3_Int   ;; skip if zero
;; Handle for Counter 1 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer0Counter2_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer0Counter1_ISR()

Counter3_Int:
    LDR R4, =T0IR    ;; Read Timer 0 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T0_IR_MR3_MASK  ;; Alarm Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Timer0_ISR_Exit   ;; skip if zero
;; Handle for Counter 1 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer0Counter3_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer0Counter1_ISR()

Timer0_ISR_Exit:
    LDR R4, =T0IR
    MOV R5, #T0_IR_ALL    ;; Clear All Timer 0 Interrupt flag
    STR R5, [R4]

;; MUST DO THIS BEFORE EXIT FROM INTERRUPT
    LDR R4, =VICVectAddr    ;;
    MOV R5, #0x00000000    ;; Dummy
    STR R5,[R4]    	;; Update priority hardware

    LDMFD SP!, {LR}    	;; POP return address
    LDMFD SP!, {R0-R12}    	;; POP registers from stack
    ADDS PC, LR, #0    	;; Return

    ENDP


;;/*******************************************************************************
;;* Function Name     : void Timer0_Connect(void CODE * pISRCode, BYTE bPriority);
;;* Description       : Connect the Timer0 to its first level of ISR and assign
;;*         the priority of Timer 0 interrupt.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : pISRCode, bPriority
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_Connect
    CODE32
Timer0_Connect PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_Connect?A
Timer0_Connect?A PROC CODE32
    ENDIF

;;-----------------------------------
;; Disable Interrupt First
;;-----------------------------------
    LDR R2, =VICIntEnable
    LDR R3, [R2]
    STMFD SP!, {R3}                 ;; Saves the original value of VICIntEnable
    BIC R3, R3, #T0_INT_EN_MASK     ;; Disable Timer 0 Interrupt (bit 4)
    STR R3, [R2]

;;-----------------------------------
;; Initialize RTC Interupt (VIC) and enable RTC
;;-----------------------------------
    AND R1, R1, #PRIORITY_MASK  ;; priority 0 to 15

    LDR R2, =VICVectCntl0
    ADD R2, R2, R1, LSL #0x02   ;; VICVectCntl_N
    MOV R3, #T0_INT_NUMBER     ;; Set IRQ_N to RTC interrupt
    ORR R3, R3, #0x20           ;; Enable IRQ_N
    STR R3, [R2]

    LDR R2, =VICVectAddr0
    ADD R2, R2, R1, LSL #0x02   ;; VICVectAddr_N
;;    LDR R0, =T0_ISR?A    ;; Set The ISR (Fixed)
    STR R0, [R2]    ;; VICVectAddr_N = pISRCode;

    LDR R2, =T0IR           ;; Clear interupt flag, just in case
    MOV R3, #T0_IR_ALL      ;; Clear all timer 0 interrupt flag
    STR R3, [R2]


    LDR R2, =VICIntEnable
    LDMIA	SP!,{R3}    ;; Restore the value of VICIntEnable
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer0_Reset(void);
;;* Description       : Reset Timer 0 Counter and Prescaler.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_Reset
    CODE32
Timer0_Reset PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_Reset?A
Timer0_Reset?A PROC CODE32
    ENDIF

    LDR R2, =T0TCR              ;; Reset Timer 0 Counter and prescaler
    MOV R3, #T0_TCR_RESET
    STR R3, [R2]

    MOV R2, #33
T0_RESET_WAIT:
    SUBS R2, R2, #1
    BNE T0_RESET_WAIT

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer0_ClearInt(void);
;;* Description       : Clear All Timer0 Interrupt Pending Flag.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_ClearInt
    CODE32
Timer0_ClearInt PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_ClearInt?A
Timer0_ClearInt?A PROC CODE32
    ENDIF

    LDR R2, =T0IR           ;; Clear Timer 0 interupt flag
    MOV R3, #T0_IR_ALL      ;; Clear all timer 0 interrupt flag
    STR R3, [R2]

    BX LR    ;; Return

    ENDP


;;/*******************************************************************************
;;* Function Name     : void Timer0_Enable(void);
;;* Description       : Anable Timer 0 Interrupt and start the Timer 0.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_Enable
    CODE32
Timer0_Enable PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_Enable?A
Timer0_Enable?A PROC CODE32
    ENDIF

    LDR R2, =VICIntEnable
    LDR R3, [R2]
    ORR R3, R3, #T0_INT_EN_MASK    ;; Enable Timer 0 Interrupt (bit 4)
    STR R3, [R2]

    LDR R2, =T0TCR              ;; Enable Timer 0 (without reseting the timer)
    MOV R3, #T0_TCR_ENABLE
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer0_Disable(void);
;;* Description       : Disable Timer 0 Interrupt and stop Timer 0.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_Disable
    CODE32
Timer0_Disable PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_Disable?A
Timer0_Disable?A PROC CODE32
    ENDIF

;;-----------------------------------
;; Disable Interrupt
;;-----------------------------------
    LDR R2, =VICIntEnable
    LDR R3, [R2]
    BIC R3, R3, #T0_INT_EN_MASK     ;; Disable Timer 0 Interrupt (bit 4)
    STR R3, [R2]

    LDR R2, =T0TCR              ;; Disable Timer 0 (without reseting the timer)
    MOV R3, #T0_TCR_DISABLE
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer0_InitPrescalerMode(TIMER_TYPE xPrescaler, BYTE bMode);
;;* Description       : Set the Timer 0 prescaler and the Timer 0 clock source.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : xPrescaler, bMode (for clock source, etc)
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_InitPrescalerMode
    CODE32
Timer0_InitPrescalerMode PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_InitPrescalerMode?A
Timer0_InitPrescalerMode?A PROC CODE32
    ENDIF

    LDR R2, =T0CTCR     ;; Set the mode
    STR R1, [R2]        ;; T0CTCR = bMode;

    LDR R2, =T0PR       ;; Set Maximum prescaler Value
    STR R0, [R2]        ;; T0PR = xPrescaler;

    BX LR    ;; Return

    ENDP

;;--------

;;/*******************************************************************************
;;* Function Name     : Timer0_SetCounter0()
;;* Description       : Initialize ISR for Counter0, Set the Match Register 0,
;;*         and set the Mode of Match on Counter 0.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer0Counter0_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer0_SetCounter0(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, WORD wMODE);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_SetCounter0
    CODE32
Timer0_SetCounter0 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_SetCounter0?A
Timer0_SetCounter0?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer0Counter0_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer0Counter0_ISR = pISRCode;

    LDR R3, =T0MR0      ;; Set the Counter
    STR R1, [R3]        ;; T0MR0 = xCounter;
    
    LDR R3, =T0MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T0_MCR0_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T0_MCR0_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;/*******************************************************************************
;;* Function Name     : Timer0_SetCounter1()
;;* Description       : Initialize ISR for Counter1, Set the Match Register 1,
;;*         and set the Mode of Match on Counter 1.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer0Counter1_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer0_SetCounter1(P_ISR_CODE pISRCode, TIMER_TYPE xCounter);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_SetCounter1
    CODE32
Timer0_SetCounter1 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_SetCounter1?A
Timer0_SetCounter1?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer0Counter1_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer0Counter1_ISR = pISRCode;

    LDR R3, =T0MR1      ;; Set the Counter
    STR R1, [R3]        ;; T0MR1 = xCounter;
    
    LDR R3, =T0MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T0_MCR1_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T0_MCR1_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;/*******************************************************************************
;;* Function Name     : Timer0_SetCounter2()
;;* Description       : Initialize ISR for Counter2, Set the Match Register 2,
;;*         and set the Mode of Match on Counter 2.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer0Counter0_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer0_SetCounter2(P_ISR_CODE pISRCode, TIMER_TYPE xCounter);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_SetCounter2
    CODE32
Timer0_SetCounter2 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_SetCounter2?A
Timer0_SetCounter2?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer0Counter2_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer0Counter2_ISR = pISRCode;

    LDR R3, =T0MR2      ;; Set the Counter
    STR R1, [R3]        ;; T0MR2 = xCounter;
    
    LDR R3, =T0MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T0_MCR2_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T0_MCR2_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;/*******************************************************************************
;;* Function Name     : Timer0_SetCounter3()
;;* Description       : Initialize ISR for Counter3, Set the Match Register 3,
;;*         and set the Mode of Match on Counter 3.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer0Counter0_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer0_SetCounter3(P_ISR_CODE pISRCode, TIMER_TYPE xCounter);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_SetCounter3
    CODE32
Timer0_SetCounter3 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_SetCounter3?A
Timer0_SetCounter3?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer0Counter3_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer0Counter3_ISR = pISRCode;

    LDR R3, =T0MR3      ;; Set the Counter
    STR R1, [R3]        ;; T0MR3 = xCounter;
    
    LDR R3, =T0MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T0_MCR3_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T0_MCR3_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;--------

;;/*******************************************************************************
;;* Function Name     : Timer0_GetMaxCounter0()
;;* Description       : Get the Maximum Counter Tick Value (value of Match Register)
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : -
;;* Output Parameter  : Return Maximum Counter Tick Value.
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; TIMER_TYPE Timer0_GetMaxCounter0(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_GetMaxCounter0
    CODE32
Timer0_GetMaxCounter0 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_GetMaxCounter0?A
Timer0_GetMaxCounter0?A PROC CODE32
    ENDIF

    LDR R3, =T0MR0      ;;
    LDR R0, [R3]        ;; return (T0MR0);

    BX LR    ;; Return
    ENDP

;; TIMER_TYPE Timer0_GetMaxCounter2(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_GetMaxCounter1
    CODE32
Timer0_GetMaxCounter1 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_GetMaxCounter1?A
Timer0_GetMaxCounter1?A PROC CODE32
    ENDIF

    LDR R3, =T0MR1      ;;
    LDR R0, [R3]        ;; return (T0MR1);

    BX LR    ;; Return
    ENDP

;; TIMER_TYPE Timer0_GetMaxCounter2(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_GetMaxCounter2
    CODE32
Timer0_GetMaxCounter2 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_GetMaxCounter2?A
Timer0_GetMaxCounter2?A PROC CODE32
    ENDIF

    LDR R3, =T0MR2      ;;
    LDR R0, [R3]        ;; return (T0MR2);

    BX LR    ;; Return
    ENDP

;; TIMER_TYPE Timer0_GetMaxCounter3(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_GetMaxCounter3
    CODE32
Timer0_GetMaxCounter3 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_GetMaxCounter3?A
Timer0_GetMaxCounter3?A PROC CODE32
    ENDIF

    LDR R3, =T0MR3      ;;
    LDR R0, [R3]        ;; return (T0MR3);

    BX LR    ;; Return
    ENDP

;;--------

;;/*******************************************************************************
;;* Function Name     : Timer0_GetTick()
;;* Description       : Get the current value of Timer Counter.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : -
;;* Output Parameter  : Return Timer Counter Value.
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; TIMER_TYPE Timer0_GetTick(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer0_GetTick
    CODE32
Timer0_GetTick PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer0_GetTick?A
Timer0_GetTick?A PROC CODE32
    ENDIF

    LDR R3, =T0TC       ;;
    LDR R0, [R3]        ;; return (T0TC);

    BX LR    ;; Return

    ENDP

        END
