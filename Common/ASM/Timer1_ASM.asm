
$INCLUDE (.\Common\ASM\include.asm)

AREA   Timer_ASM_Code, CODE   // READONLY, ALIGN=4
RSEG Timer_ASM_Code
;    ENTRY

    EXTERN DATA  (vpTimer1Counter0_ISR)
    EXTERN DATA  (vpTimer1Counter1_ISR)
    EXTERN DATA  (vpTimer1Counter2_ISR)
    EXTERN DATA  (vpTimer1Counter3_ISR)

T1_INT_EN_MASK      EQU 0x20
T1_INT_NUMBER       EQU 0x05

T1_IR_ALL           EQU 0xFF
T1_IR_MR0_MASK      EQU 0x01
T1_IR_MR1_MASK      EQU 0x02
T1_IR_MR2_MASK      EQU 0x04
T1_IR_MR3_MASK      EQU 0x08
T1_IR_CR0_MASK      EQU 0x10
T1_IR_CR1_MASK      EQU 0x20
T1_IR_CR2_MASK      EQU 0x40
T1_IR_CR3_MASK      EQU 0x80

T1_TCR_ENABLE       EQU 0x01
T1_TCR_DISABLE      EQU 0x00
T1_TCR_RESET        EQU 0x02

T1_MCR0_MASK        EQU 0x0007
T1_MCR1_MASK        EQU 0x0038
T1_MCR2_MASK        EQU 0x01C0
T1_MCR3_MASK        EQU 0x0E00



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
;;* Function Name     : void Timer1_ISR(void) INTERRUPT
;;* Description       : Main Entry point to the Timer 1 ISR. This function will
;;*         dispatch the execution to the corresponding handler depends on the
;;*         interrupt source (Match on Counter 0, or Counter 1, Counter 2, or Counter 3)
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer1Counter0_ISR, vpTimer1Counter1_ISR,
;;*                     vpTimer1Counter2_ISR, vpTimer1Counter3_ISR
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_ISR
    CODE32
Timer1_ISR PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_ISR?A
Timer1_ISR?A PROC CODE32
    ENDIF
    
    STMFD R13!, {R0-R12}
    MRS R4, SPSR
    TST R4, #0x00000020    ;; Check ARM/Thumb
    SUBEQ LR, LR, #4    ;; Set ARM return
    SUBNE LR, LR, #2    ;; Set Thumb return
    STMFD SP!, {LR}

    LDR R4, =T1IR    ;; Read Timer 1 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T1_IR_MR0_MASK   ;; Counter Increment Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Counter1_Int   ;; skip if zero
;; Handle for Counter 0 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer1Counter0_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer1Counter0_ISR()

Counter1_Int:
    LDR R4, =T1IR    ;; Read Timer 1 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T1_IR_MR1_MASK  ;; Alarm Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Counter2_Int   ;; skip if zero
;; Handle for Counter 1 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer1Counter1_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer1Counter1_ISR()

Counter2_Int:
    LDR R4, =T1IR    ;; Read Timer 1 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T1_IR_MR2_MASK  ;; Alarm Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Counter3_Int   ;; skip if zero
;; Handle for Counter 1 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer1Counter2_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer1Counter1_ISR()

Counter3_Int:
    LDR R4, =T1IR    ;; Read Timer 1 Interrupt Register
    LDR R5,[R4]
    MOV R8, #T1_IR_MR3_MASK  ;; Alarm Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Timer1_ISR_Exit   ;; skip if zero
;; Handle for Counter 1 Interrupt
;;-----------------------------------
    LDR R6, =vpTimer1Counter3_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpTimer1Counter1_ISR()

Timer1_ISR_Exit:
    LDR R4, =T1IR
    MOV R5, #T1_IR_ALL    ;; Clear All Timer 1 Interrupt flag
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
;;* Function Name     : void Timer1_Connect(void CODE * pISRCode, BYTE bPriority);
;;* Description       : Connect the Timer1 to its first level of ISR and assign
;;*         the priority of Timer 1 interrupt.
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
;; void Timer1_Connect(void CODE * pISRCode, BYTE bPriority);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_Connect
    CODE32
Timer1_Connect PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_Connect?A
Timer1_Connect?A PROC CODE32
    ENDIF

;;-----------------------------------
;; Disable Interrupt First
;;-----------------------------------
    LDR R2, =VICIntEnable
    LDR R3, [R2]
    STMFD SP!, {R3}                 ;; Saves the original value of VICIntEnable
    BIC R3, R3, #T1_INT_EN_MASK     ;; Disable Timer 1 Interrupt (bit 4)
    STR R3, [R2]

;;-----------------------------------
;; Initialize RTC Interupt (VIC) and enable RTC
;;-----------------------------------
    AND R1, R1, #PRIORITY_MASK  ;; priority 0 to 15

    LDR R2, =VICVectCntl0
    ADD R2, R2, R1, LSL #0x02   ;; VICVectCntl_N
    MOV R3, #T1_INT_NUMBER     ;; Set IRQ_N to RTC interrupt
    ORR R3, R3, #0x20           ;; Enable IRQ_N
    STR R3, [R2]

    LDR R2, =VICVectAddr0
    ADD R2, R2, R1, LSL #0x02   ;; VICVectAddr_N
;;    LDR R0, =T1_ISR?A    ;; Set The ISR (Fixed)
    STR R0, [R2]    ;; VICVectAddr_N = pISRCode;

    LDR R2, =T1IR           ;; Clear interupt flag, just in case
    MOV R3, #T1_IR_ALL      ;; Clear all timer 1 interrupt flag
    STR R3, [R2]


    LDR R2, =VICIntEnable
    LDMIA	SP!,{R3}    ;; Restore the value of VICIntEnable
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer1_Reset(void);
;;* Description       : Reset Timer 1 Counter and Prescaler.
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
;; void Timer1_Reset(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_Reset
    CODE32
Timer1_Reset PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_Reset?A
Timer1_Reset?A PROC CODE32
    ENDIF

    LDR R2, =T1TCR              ;; Reset Timer 1 Counter and prescaler
    MOV R3, #T1_TCR_RESET
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer1_ClearInt(void);
;;* Description       : Clear All Timer1 Interrupt Pending Flag.
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
;; void Timer1_ClearInt(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_ClearInt
    CODE32
Timer1_ClearInt PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_ClearInt?A
Timer1_ClearInt?A PROC CODE32
    ENDIF

    LDR R2, =T1IR           ;; Clear Timer 1 interupt flag
    MOV R3, #T1_IR_ALL      ;; Clear all timer 1 interrupt flag
    STR R3, [R2]

    BX LR    ;; Return

    ENDP


;;/*******************************************************************************
;;* Function Name     : void Timer1_Enable(void);
;;* Description       : Anable Timer 1 Interrupt and start the Timer 1.
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
;; void Timer1_Enable(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_Enable
    CODE32
Timer1_Enable PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_Enable?A
Timer1_Enable?A PROC CODE32
    ENDIF

    LDR R2, =VICIntEnable
    LDR R3, [R2]
    ORR R3, R3, #T1_INT_EN_MASK    ;; Enable Timer 1 Interrupt (bit 4)
    STR R3, [R2]

    LDR R2, =T1TCR              ;; Enable Timer 1 (without reseting the timer)
    MOV R3, #T1_TCR_ENABLE
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer1_Disable(void);
;;* Description       : Disable Timer 1 Interrupt and stop Timer 1.
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
;; void Timer1_Disable(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_Disable
    CODE32
Timer1_Disable PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_Disable?A
Timer1_Disable?A PROC CODE32
    ENDIF

;;-----------------------------------
;; Disable Interrupt
;;-----------------------------------
    LDR R2, =VICIntEnable
    LDR R3, [R2]
    BIC R3, R3, #T1_INT_EN_MASK     ;; Disable Timer 1 Interrupt (bit 4)
    STR R3, [R2]

    LDR R2, =T1TCR              ;; Disable Timer 1 (without reseting the timer)
    MOV R3, #T1_TCR_DISABLE
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void Timer1_InitPrescalerMode(TIMER_TYPE xPrescaler, BYTE bMode);
;;* Description       : Set the Timer 1 prescaler and the Timer 1 clock source.
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
;; void Timer1_InitPrescalerMode(TIMER_TYPE xPrescaler, BYTE bMode);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_InitPrescalerMode
    CODE32
Timer1_InitPrescalerMode PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_InitPrescalerMode?A
Timer1_InitPrescalerMode?A PROC CODE32
    ENDIF

    LDR R2, =T1CTCR     ;; Set the mode
    STR R1, [R2]        ;; T1CTCR = bMode;

    LDR R2, =T1PR       ;; Set Maximum prescaler Value
    STR R0, [R2]        ;; T1PR = xPrescaler;

    BX LR    ;; Return

    ENDP

;;--------

;;/*******************************************************************************
;;* Function Name     : Timer1_SetCounter0()
;;* Description       : Initialize ISR for Counter0, Set the Match Register 0,
;;*         and set the Mode of Match on Counter 0.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer1Counter0_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer1_SetCounter0(P_ISR_CODE pISRCode, TIMER_TYPE xCounter, WORD wMODE);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_SetCounter0
    CODE32
Timer1_SetCounter0 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_SetCounter0?A
Timer1_SetCounter0?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer1Counter0_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer1Counter0_ISR = pISRCode;

    LDR R3, =T1MR0      ;; Set the Counter
    STR R1, [R3]        ;; T1MR0 = xCounter;
    
    LDR R3, =T1MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T1_MCR0_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T1_MCR0_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;/*******************************************************************************
;;* Function Name     : Timer1_SetCounter1()
;;* Description       : Initialize ISR for Counter1, Set the Match Register 1,
;;*         and set the Mode of Match on Counter 1.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer1Counter1_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer1_SetCounter1(P_ISR_CODE pISRCode, TIMER_TYPE xCounter);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_SetCounter1
    CODE32
Timer1_SetCounter1 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_SetCounter1?A
Timer1_SetCounter1?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer1Counter1_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer1Counter1_ISR = pISRCode;

    LDR R3, =T1MR1      ;; Set the Counter
    STR R1, [R3]        ;; T1MR1 = xCounter;
    
    LDR R3, =T1MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T1_MCR1_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T1_MCR1_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;/*******************************************************************************
;;* Function Name     : Timer1_SetCounter2()
;;* Description       : Initialize ISR for Counter2, Set the Match Register 2,
;;*         and set the Mode of Match on Counter 2.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer1Counter2_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer1_SetCounter2(P_ISR_CODE pISRCode, TIMER_TYPE xCounter);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_SetCounter2
    CODE32
Timer1_SetCounter2 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_SetCounter2?A
Timer1_SetCounter2?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer1Counter2_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer1Counter2_ISR = pISRCode;

    LDR R3, =T1MR2      ;; Set the Counter
    STR R1, [R3]        ;; T1MR2 = xCounter;
    
    LDR R3, =T1MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T1_MCR2_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T1_MCR2_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;/*******************************************************************************
;;* Function Name     : Timer1_SetCounter3()
;;* Description       : Initialize ISR for Counter3, Set the Match Register 3,
;;*         and set the Mode of Match on Counter 3.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpTimer1Counter3_ISR
;;*
;;* Input Parameter   : pISRCode, xCounter, wMODE
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void Timer1_SetCounter3(P_ISR_CODE pISRCode, TIMER_TYPE xCounter);    // BASE_TYPE xCounter, BASE_TYPE xPrescaler
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_SetCounter3
    CODE32
Timer1_SetCounter3 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_SetCounter3?A
Timer1_SetCounter3?A PROC CODE32
    ENDIF

    LDR R3, =vpTimer1Counter3_ISR     ;; Set the ISR
    STR R0, [R3]        ;; vpTimer1Counter3_ISR = pISRCode;

    LDR R3, =T1MR3      ;; Set the Counter
    STR R1, [R3]        ;; T1MR3 = xCounter;
    
    LDR R3, =T1MCR      ;; Set the Mode
    LDR R0, [R3]
    AND R0, R0, #(~T1_MCR3_MASK)    ;; NEED TO CHECK THIS INSTRUCTION!!
    AND R2, R2, #T1_MCR3_MASK
    ORR R2, R2, R0

    STR R2, [R3]

    BX LR    ;; Return
    ENDP

;;--------

;;/*******************************************************************************
;;* Function Name     : Timer1_GetMaxCounter0()
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
;; TIMER_TYPE Timer1_GetMaxCounter0(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_GetMaxCounter0
    CODE32
Timer1_GetMaxCounter0 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_GetMaxCounter0?A
Timer1_GetMaxCounter0?A PROC CODE32
    ENDIF

    LDR R3, =T1MR0      ;;
    LDR R0, [R3]        ;; return (T1MR0);

    BX LR    ;; Return
    ENDP

;; TIMER_TYPE Timer1_GetMaxCounter1(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_GetMaxCounter1
    CODE32
Timer1_GetMaxCounter1 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_GetMaxCounter1?A
Timer1_GetMaxCounter1?A PROC CODE32
    ENDIF

    LDR R3, =T1MR1      ;;
    LDR R0, [R3]        ;; return (T1MR1);

    BX LR    ;; Return
    ENDP

;; TIMER_TYPE Timer1_GetMaxCounter2(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_GetMaxCounter2
    CODE32
Timer1_GetMaxCounter2 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_GetMaxCounter2?A
Timer1_GetMaxCounter2?A PROC CODE32
    ENDIF

    LDR R3, =T1MR2      ;;
    LDR R0, [R3]        ;; return (T1MR2);

    BX LR    ;; Return
    ENDP

;; TIMER_TYPE Timer1_GetMaxCounter3(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_GetMaxCounter3
    CODE32
Timer1_GetMaxCounter3 PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_GetMaxCounter3?A
Timer1_GetMaxCounter3?A PROC CODE32
    ENDIF

    LDR R3, =T1MR3      ;;
    LDR R0, [R3]        ;; return (T1MR3);

    BX LR    ;; Return
    ENDP

;;--------

;;/*******************************************************************************
;;* Function Name     : Timer1_GetTick()
;;* Description       : Get the current value of Timer Counter.
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
;; TIMER_TYPE Timer1_GetTick(void);
    IF (ENV_RVDS == 1)
        GLOBAL Timer1_GetTick
    CODE32
Timer1_GetTick PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC Timer1_GetTick?A
Timer1_GetTick?A PROC CODE32
    ENDIF

    LDR R3, =T1TC       ;;
    LDR R0, [R3]        ;; return (T1TC);

    BX LR    ;; Return

    ENDP


        END
