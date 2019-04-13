/*******************************************************************************
********************************************************************************
** File Name    : RTC.c
** Description  :
**      Implementation of Real Time Clock
**------------------------------------------------------------------------------
** Date(Author)     Version:    Changes Notes
** DD/MM/YY
**------------------------------------------------------------------------------
** 26/01/07(ABQ)    1.0         Initial Revision
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
void (*vpRTC_ALARM_ISR)(void);     /* Global Pointer to RTC ALARM ISR*/
void (*vpRTC_COUNTER_ISR)(void);     /* Global Pointer to RTC COUNTER ISR*/

void (*vpTimer0Counter0_ISR)(void);     /* Global Pointer to Counter 0 ISR*/
void (*vpTimer0Counter1_ISR)(void);     /* Global Pointer to Counter 1 ISR*/
void (*vpTimer0Counter2_ISR)(void);     /* Global Pointer to Counter 2 ISR*/
void (*vpTimer0Counter3_ISR)(void);     /* Global Pointer to Counter 3 ISR*/

void (*vpTimer1Counter0_ISR)(void);     /* Global Pointer to Counter 0 ISR*/
void (*vpTimer1Counter1_ISR)(void);     /* Global Pointer to Counter 1 ISR*/
void (*vpTimer1Counter2_ISR)(void);     /* Global Pointer to Counter 2 ISR*/
void (*vpTimer1Counter3_ISR)(void);     /* Global Pointer to Counter 3 ISR*/

/************************/
/* API Functions        */
/************************/
/*******************************************************************************
* Function Name     : RTC_SetTime()
* Description       : Set the current time
*---------------------------------------------------------------------
* Global Variable   : -
*
* Input Parameter   : Pointer to Time Structure
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 26/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void RTC_SetTime(TS_RTC * psRTCTime)
{
    if (psRTCTime->usYear == 0xFFFF)
    {
        YEAR = (psRTCTime->usYear)&RTC_YEAR_SIZE_MASK;
    }
    if (psRTCTime->usDayofyear == 0xFFFF)
    {
        DOY = psRTCTime->usDayofyear&RTC_DOY_SIZE_MASK;
    }
    if (psRTCTime->bMonth == 0xFF)
    {
        MONTH = psRTCTime->bMonth&RTC_MON_SIZE_MASK;
    }
    if (psRTCTime->bDayofmonth == 0xFF)
    {
        DOM = psRTCTime->bDayofmonth&RTC_DOM_SIZE_MASK;
    }
    if (psRTCTime->bDayofweek == 0xFF)
    {
        DOW = psRTCTime->bDayofweek&RTC_DOW_SIZE_MASK;
    }
    if (psRTCTime->bSecond == 0xFF)
    {
        SEC = psRTCTime->bSecond&RTC_SEC_SIZE_MASK;
    }
    if (psRTCTime->bMinutes == 0xFF)
    {
        MIN = psRTCTime->bMinutes&RTC_MIN_SIZE_MASK;
    }
    if (psRTCTime->bHour == 0xFF)
    {
        HOUR = psRTCTime->bHour&RTC_HOUR_SIZE_MASK;
    }
}

/*******************************************************************************
* Function Name     : RTC_GetTime()
* Description       : Read The current time
*---------------------------------------------------------------------
* Global Variable   : -
*
* Input Parameter   : Pointer to Time Structure
* Output Parameter  : Time Structure updated with the current time from RTC hardware.
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 26/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void RTC_GetTime(TS_RTC * psRTCTime)
{
    psRTCTime->usYear = YEAR;   /* Read Year*/
    psRTCTime->usDayofyear = DOY;   /* Read DOY*/
    psRTCTime->bMonth = MONTH;
    psRTCTime->bDayofmonth = DOM;
    psRTCTime->bDayofweek = DOW;
    psRTCTime->bSecond = SEC;
    psRTCTime->bMinutes = MIN;
    psRTCTime->bHour = HOUR;
}

/*******************************************************************************
* Function Name     : RTC_SetAlarm()
* Description       : Set the Alarm Clock.
*---------------------------------------------------------------------
* Global Variable   : -
*
* Input Parameter   : Pointer to Alarm structure
* Output Parameter  : -
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 26/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void RTC_SetAlarm(TS_RTC * psRTCAlarm)
{
    unsigned char bAMRTemp;

    bAMRTemp = 0;
    if (psRTCAlarm->pRTC_ISR != NULL) /* if ISR pointer not NULL*/
    {   /* Check each alarm and adjust the mask accordingly*/
        if (psRTCAlarm->usYear == 0xFFFF)
        {
            bAMRTemp |= AMR_MASK_YEAR;      /* Set the mask if not specified*/
            ALYEAR = (psRTCAlarm->usYear)&RTC_YEAR_SIZE_MASK;
        }
        if (psRTCAlarm->usDayofyear == 0xFFFF)
        {
            bAMRTemp |= AMR_MASK_DOY;      /* Set the mask if not specified*/
            ALDOY = psRTCAlarm->usDayofyear&RTC_DOY_SIZE_MASK;
        }
        if (psRTCAlarm->bMonth == 0xFF)
        {
            bAMRTemp |= AMR_MASK_MON;      /* Set the mask if not specified*/
            ALMON = psRTCAlarm->bMonth&RTC_MON_SIZE_MASK;
        }
        if (psRTCAlarm->bDayofmonth == 0xFF)
        {
            bAMRTemp |= AMR_MASK_DOM;      /* Set the mask if not specified*/
            ALDOM = psRTCAlarm->bDayofmonth&RTC_DOM_SIZE_MASK;
        }
        if (psRTCAlarm->bDayofweek == 0xFF)
        {
            bAMRTemp |= AMR_MASK_DOW;      /* Set the mask if not specified*/
            ALDOW = psRTCAlarm->bDayofweek&RTC_DOW_SIZE_MASK;
        }
        if (psRTCAlarm->bSecond == 0xFF)
        {
            bAMRTemp |= AMR_MASK_SEC;      /* Set the mask if not specified*/
            ALSEC = psRTCAlarm->bSecond&RTC_SEC_SIZE_MASK;
        }
        if (psRTCAlarm->bMinutes == 0xFF)
        {
            bAMRTemp |= AMR_MASK_MIN;      /* Set the mask if not specified*/
            ALMIN = psRTCAlarm->bMinutes&RTC_MIN_SIZE_MASK;
        }
        if (psRTCAlarm->bHour == 0xFF)
        {
            bAMRTemp |= AMR_MASK_HOUR;      /* Set the mask if not specified*/
            ALHOUR = psRTCAlarm->bHour&RTC_HOUR_SIZE_MASK;
        }

        if (0xFF != bAMRTemp)   /* If at least one alarm is enabled */
        {
            /*Set the ISR for alarm*/
            vpRTC_ALARM_ISR = psRTCAlarm->pRTC_ISR;
            AMR = bAMRTemp;
            return;
        }
    }
    /* Pointer is NULL, or all ALARM is not set*/
    /* disable alarm */
    vpRTC_ALARM_ISR = NULL;
    AMR = 0x000000FF;   /* Mask All alarm */

}

/*******************************************************************************
* Function Name     : RTC_GetAlarm()
* Description       : Get setting of the current Alarm
*---------------------------------------------------------------------
* Global Variable   : -
*
* Input Parameter   : Pointer to Alarm Structure
* Output Parameter  : Alarm structure updated with the current alarm setting.
* Subroutine Called : -
* DD/MM/YY(Author)      Changes Notes
*---------------------------------------------------------------------
* 26/01/07(ABQ)     :   Initial Version
*******************************************************************************/
void RTC_GetAlarm(TS_RTC * psRTCAlarm)
{
    unsigned char bAMRTemp;

    bAMRTemp = AMR; /* Only read the AMR register once*/

    psRTCAlarm->usYear = 0xFFFF;       /* Clear the destination with 0xFF*/
    psRTCAlarm->usDayofyear = 0xFFFF;
    psRTCAlarm->bMonth = 0xFF;
    psRTCAlarm->bDayofmonth = 0xFF;
    psRTCAlarm->bDayofweek = 0xFF;
    psRTCAlarm->bSecond = 0xFF;
    psRTCAlarm->bMinutes = 0xFF;
    psRTCAlarm->bHour = 0xFF;

    if (!(bAMRTemp & AMR_MASK_YEAR))    /* if not masked */
    {
        psRTCAlarm->usYear = ALYEAR;   /* Read Year Alarm*/
    }
    if (!(bAMRTemp & AMR_MASK_DOY))    /* if not masked */
    {
        psRTCAlarm->usDayofyear = ALDOY;   /* Read DOY Alarm*/
    }
    if (!(bAMRTemp & AMR_MASK_MON))    /* if not masked */
    {
        psRTCAlarm->bMonth = ALMON;
    }
    if (!(bAMRTemp & AMR_MASK_DOM))    /* if not masked */
    {
        psRTCAlarm->bDayofmonth = ALDOM;
    }
    if (!(bAMRTemp & AMR_MASK_DOW))    /* if not masked */
    {
        psRTCAlarm->bDayofweek = ALDOW;
    }
    if (!(bAMRTemp & AMR_MASK_SEC))    /* if not masked */
    {
        psRTCAlarm->bSecond = ALSEC;
    }
    if (!(bAMRTemp & AMR_MASK_MIN))    /* if not masked */
    {
        psRTCAlarm->bMinutes = ALMIN;
    }
    if (!(bAMRTemp & AMR_MASK_HOUR))    /* if not masked */
    {
        psRTCAlarm->bHour = ALHOUR;
    }

    return;
}

/*******************************************************************************
** Function     : RTC_SetIncrementMode()
** Description  : Set counter Increment time and interrupt .
**
** Global       :
** Input        : pointer to ISR handler, Flag
** Output       : None
** Called       : None
**------------------------------------------------------------------------------
**  Author(Date DD/MM/YY)    Comment:
**  ABQ(23/09/05)   Initial Version
*******************************************************************************/
void RTC_SetIncrementMode(void(*pCounterISR)(void), BYTE bCntrFlag)
{
    if (0xFF != bCntrFlag)   /* If at least one Counter Interrupt is enabled */
    {
        /*Set the ISR for Counter Increment*/
        vpRTC_COUNTER_ISR = pCounterISR;
    }
    CIIR = bCntrFlag;
}

/*******************************************************************************
** Function     : RTC_GetIncrementFlag()
** Description  : Get the current setting of Counter Increment.
**
** Global       :
** Input        : None
** Output       : return Counter Increment flags
** Called       : None
**------------------------------------------------------------------------------
**  Author(Date DD/MM/YY)    Comment:
**  ABQ(23/09/05)   Initial Version
*******************************************************************************/
BYTE RTC_GetIncrementMode(void)
{
    return CIIR;
}
