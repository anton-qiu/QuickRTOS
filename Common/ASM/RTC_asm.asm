
$INCLUDE (.\Common\ASM\include.asm)

AREA   RTC_ASM, CODE   // READONLY, ALIGN=4
    ENTRY

            EXTERN DATA  (vpRTC_ALARM_ISR)
            EXTERN DATA  (vpRTC_COUNTER_ISR)


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
;;* Function Name     : void RTC_ISR(void) INTERRUPT
;;* Description       : RTC Interrupt service routine that dispatch the call
;;*         depending on the type of interrupt: Alarm or Counter increment.
;;*         This function will call the respective handler depending on the
;;*         interrupt source.
;;*     The function to be called is determined by the value of vpRTC_COUNTER_ISR,
;;*     and vpRTC_ALARM_ISR.
;;*---------------------------------------------------------------------
;;* Global Variable   : vpRTC_COUNTER_ISR, vpRTC_ALARM_ISR.
;;*
;;* Input Parameter   : -
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
    IF (ENV_RVDS == 1)
        GLOBAL RTC_ISR
    CODE32
RTC_ISR PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC RTC_ISR?A
RTC_ISR?A PROC CODE32
    ENDIF
    
;;    STMFD R13!, {R4, R5, R6, R7, R8}
    STMFD R13!, {R0-R12}
    MRS R4, SPSR
    TST R4, #0x00000020    ;; Check ARM/Thumb
    SUBEQ LR, LR, #4    ;; Set ARM return
    SUBNE LR, LR, #2    ;; Set Thumb return
    STMFD SP!, {LR}

    LDR R4, =ILR    ;; Read Interrupt Location Register
    LDR R5,[R4]
    MOV R8, #RTC_ILR_CNTR   ;; Counter Increment Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ Alarm_Int   ;; skip if zero
;; Handle for Counter Interrupt
;;-----------------------------------
    LDR R6, =vpRTC_COUNTER_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpRTC_COUNTER_ISR()

Alarm_Int:
    LDR R4, =ILR    ;; Read Interrupt Location Register
    LDR R5,[R4]
    MOV R8, #RTC_ILR_ALARM  ;; Alarm Interrupt Flag
    TST R5, R8      ;; Check the current PIN value
    BEQ RTC_Exit   ;; skip if zero
;; Handle for Alarm Interrupt
;;-----------------------------------
    LDR R6, =vpRTC_ALARM_ISR
    LDR R5, [R6]
    MOV LR, PC      ;; Return Address
    BX R5           ;; CALL vpRTC_ALARM_ISR()

RTC_Exit:
    LDR R4, =ILR
    MOV R5, #RTC_ILR_ALL    ;; Clear alarm and counter increment flag
    STR R5, [R4]

;; MUST DO THIS BEFORE EXIT FROM INTERRUPT
    LDR R4, =VICVectAddr    ;;
    MOV R5, #0x00000000    ;; Dummy
    STR R5,[R4]    	;; Update priority hardware

    LDMFD SP!, {LR}    	;; POP return address
;;    LDMFD SP!, {R4, R5, R6, R7, R8}    	;; POP registers from stack
    LDMFD SP!, {R0-R12}    	;; POP registers from stack
    ADDS PC, LR, #0    	;; Return

    ENDP



;;/*******************************************************************************
;;* Function Name     : void RTC_Connect(void CODE * pISRCode, BYTE bPriority)
;;* Description       : Connect the RTC to ISR and assign the Interrupt priority
;;*             to the RTC Interrupt.
;;*     NOTE: The ISR and interrupt should only be assigned to 1 priority.
;;*---------------------------------------------------------------------
;;* Global Variable   : -
;;*
;;* Input Parameter   : pISRCode (R0), bPriority (R1)
;;* Output Parameter  : -
;;* Subroutine Called : -
;;* DD/MM/YY(Author)      Changes Notes
;;*---------------------------------------------------------------------
;;* 11/01/07(ABQ)     :   Initial Version
;;*******************************************************************************/
;; void RTC_Connect(void CODE * pISRCode, BYTE bPriority);
    IF (ENV_RVDS == 1)
        GLOBAL RTC_Connect
    CODE32
RTC_Connect PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC RTC_Connect?A
RTC_Connect?A PROC CODE32
    ENDIF

;;-----------------------------------
;; Disable Interrupt First
;;-----------------------------------
    LDR R2, =VICIntEnable
    LDR R3, [R2]
    STMFD SP!, {R3}                 ;; Saves the original value of VICIntEnable
    BIC R3, R3, #RTC_INT_EN_MASK    ;; Disable RTC Interrupt (bit 13)
    STR R3, [R2]

;;-----------------------------------
;; Initialize RTC Interupt (VIC) and enable RTC
;;-----------------------------------
    AND R1, R1, #PRIORITY_MASK  ;; priority 0 to 15

    LDR R2, =VICVectCntl0
    ADD R2, R2, R1, LSL #0x02   ;; VICVectCntl_N
    MOV R3, #RTC_INT_NUMBER     ;; Set IRQ_N to RTC interrupt
    ORR R3, R3, #0x20           ;; Enable IRQ_N
    STR R3, [R2]

    LDR R2, =VICVectAddr0
    ADD R2, R2, R1, LSL #0x02   ;; VICVectAddr_N
;;    LDR R0, =RTC_ISR?A    ;; Set The ISR (Fixed)
    STR R0, [R2]    ;; VICVectAddr_N = pISRCode;

    LDR R2, =ILR    ;; Clear interupt flag, just in case
    MOV R3, #RTC_ILR_ALL    ;; Clear alarm and counter increment flag
    STR R3, [R2]


    LDR R2, =VICIntEnable
    LDMIA	SP!,{R3}    ;; Restore the value of VICIntEnable
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void RTC_Enable(void);
;;* Description       : Enable Real Time Clock and the corresponding Interrupt.
;;*     NOTE: The RTC Should have been initialized before enabling.
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
        GLOBAL RTC_Enable
    CODE32
RTC_Enable PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC RTC_Enable?A
RTC_Enable?A PROC CODE32
    ENDIF

    LDR R2, =ILR        ;; Clear interupt flag, just in case
    MOV R3, #RTC_ILR_ALL       ;; Clear alarm and counter increment flag
    STR R3, [R2]

    LDR R2, =VICIntEnable
    LDR R3, [R2]
    ORR R3, R3, #RTC_INT_EN_MASK    ;; Enable RTC Interrupt (bit 13)
    STR R3, [R2]

    LDR R2, =CCR                ;; Initialize Clock Control Register
    MOV R3, #RTC_CCR_ENABLE     ;; Enable RTC, De-Reset RTC, CLK source 32K extenal
    STR R3, [R2]

    BX LR    ;; Return

    ENDP

;;/*******************************************************************************
;;* Function Name     : void RTC_Disable(void);
;;* Description       : Disable RTC and it's interrupt.
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
;; void RTC_Disable(void);
    IF (ENV_RVDS == 1)
        GLOBAL RTC_Disable
    CODE32
RTC_Disable PROC
    ENDIF

    IF (ENV_KEIL == 1)
        PUBLIC RTC_Disable?A
RTC_Disable?A PROC CODE32
    ENDIF

;;-----------------------------------
;; Disable Interrupt First
;;-----------------------------------
    LDR R2, =VICIntEnable
    LDR R3, [R2]
    BIC R3, R3, #RTC_INT_EN_MASK    ;; Disable RTC Interrupt (bit 13)
    STR R3, [R2]

    LDR R2, =CCR                ;; Initialize Clock Control Register
    MOV R3, #00
    STR R3, [R2]

    LDR R2, =ILR    ;; Clear interupt flag, just in case
    MOV R3, #0x03    ;; Clear alarm and counter increment flag
    STR R3, [R2]

    BX LR    ;; Return

    ENDP



        END
