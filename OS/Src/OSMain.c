/*******************************************************************************
********************************************************************************
** File Name    : OSMain.c
** Description  :
**      Main function for My RTOS
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
/* Global Variable     */
/************************/

#define TASK1_STACK_SIZE	128
#define TASK3_STACK_SIZE	128
#define TASK4_STACK_SIZE	128

STACK_TYPE xStackDummy1[TASK1_STACK_SIZE];
STACK_TYPE xStackDummy3[TASK3_STACK_SIZE];
STACK_TYPE xStackDummy4[TASK4_STACK_SIZE];

STACK_TYPE xStackIdle[32];

UINT64 u64ResultTemp3;
UINT64 u64ResultTemp4;

const BYTE baConstTemp[] = {0xF0, 0x0E, 0x05, 0xFF, 0x01, 0x00, 0x00, 0x00}; 


void FuncDummy1(void * pvPar)
{
    BYTE bHandle;
    BYTE bTemp;

    UINT32 u32Count;

    bTemp = *(BYTE *)pvPar;

    while(1)
    {
        IOSET1 = 0x00200000;
        for(u32Count=0; u32Count<1000000; u32Count+=bTemp );
        IOCLR1 = 0x00200000;

        bHandle = OS_Sem_GetHandle(1);
        OS_Sem_Release(bHandle);
    }
}

void FuncDummy3(void * pvPar)
{
    BYTE bHandle;
    BYTE bTemp;

    bTemp = *(BYTE *)pvPar;

    u64ResultTemp3 = 1;

	while(1)
	{
        IOSET1 = 0x00100000;
		u64ResultTemp3 *= bTemp;
		if(u64ResultTemp3 == 0)
		{
            bHandle = OS_Sem_GetHandle(1);
            OS_Sem_Aquire(bHandle);
			u64ResultTemp3 = bTemp;
		}

	}
}

void FuncDummy4(void * pvPar)
{
    BYTE bHandle;
    BYTE bTemp;

    bTemp = *(BYTE *)pvPar;

    u64ResultTemp4 = 0;

	while(1)
	{
        IOCLR1 = 0x00100000;
		u64ResultTemp4 += bTemp;
        if(u64ResultTemp4 > 20000000)
        {
            bHandle = OS_Sem_GetHandle(1);
            OS_Sem_Aquire(bHandle);
            u64ResultTemp4 = bTemp;
        }
	}
}

void TaskIdle(void * pvPar)
{
    pvPar = pvPar;
    while(1);
}

void Blink(void)
{

    if((IOPIN1&0x00100000) == 0)
    {
        IOSET1 = 0x00100000;
    }
    else
    {
        IOCLR1 = 0x00100000;
    }

}

void Blink0(void)
{

    if((IOPIN1&0x00010000) == 0)
    {
        IOSET1 = 0x00010000;
    }
    else
    {
        IOCLR1 = 0x00010000;
    }

}

void Blink1(void)
{

    if((IOPIN1&0x00020000) == 0)
    {
        IOSET1 = 0x00020000;
    }
    else
    {
        IOCLR1 = 0x00020000;
    }

}

void Blink2(void)
{

    if((IOPIN1&0x00040000) == 0)
    {
        IOSET1 = 0x00040000;
    }
    else
    {
        IOCLR1 = 0x00040000;
    }

}

void Blink3(void)
{

    if((IOPIN1&0x00080000) == 0)
    {
        IOSET1 = 0x00080000;
    }
    else
    {
        IOCLR1 = 0x00080000;
    }

}


/*******************************************************************************
* Function Name     : main()
* Description       : main function
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
    TIMER_TYPE xTimer_Counter, xTimer_Tick;

void main(void)
{

/* Test RTC API to blink the LED in Counter mode. */
#ifdef __TEST_RTC__
    IODIR1 = 0x00FF0000;
    IOCLR1 = 0x00FF0000;
    RTC_Disable();
    RTC_Connect(&RTC_ISR, 0);  /* Use any priority from 0 to 15. */
    RTC_SetIncrementMode(&Blink0, CNTR_MASK_SEC); // Blink every second
    RTC_Enable();
#endif

/* Test Timer API to Blink LED */
#ifdef __TEST_TIMER__
    IODIR1 = 0x00FF0000;
    IOCLR1 = 0x00FF0000;

    Timer0_Disable();
    Timer0_Connect(Timer0_ISR, 1);
    Timer0_InitPrescalerMode(14, TIMER_MODE_PCLK);  /* 1 micro second every tick */

    Timer0_SetCounter0(Blink0, 1000000,TIMER_CNTR0_MODE);   /* 1 second */
    Timer0_SetCounter1(Blink1, 100000,TIMER_CNTR1_MODE);   /* 0.1 second */
    Timer0_SetCounter2(Blink2, 200000,TIMER_CNTR2_MODE);   /* 0.2 second */
    Timer0_SetCounter3(Blink3, 300000,TIMER_CNTR3_MODE);   /* 0.3 second */

    Timer0_Enable();
//---------------------
    Timer1_Disable();
    Timer1_Connect(Timer1_ISR, 2);
    Timer1_InitPrescalerMode(14, TIMER_MODE_PCLK);  /* 1 micro second every tick */

    Timer1_SetCounter0(Blink, 1000000,TIMER_CNTR0_MODE);   /* 1 second */
    Timer1_SetCounter1(Blink, 100000,TIMER_CNTR1_MODE);   /* 0.1 second */
    Timer1_SetCounter2(Blink, 200000,TIMER_CNTR2_MODE);   /* 0.2 second */
    Timer1_SetCounter3(Blink, 300000,TIMER_CNTR3_MODE);   /* 0.3 second */

    Timer1_Enable();

    while(1)
    {
        xTimer_Counter = Timer0_GetMaxCounter0();
        if(xTimer_Counter != 1000000)
        {
            Timer0_Disable();
            while(1);
        }
        xTimer_Tick = Timer0_GetTick();
    }
#endif  //__TEST_TIMER__

//#ifdef __TEST_SCHEDULE__

    IODIR1 = 0x00FF0000;
    IOCLR1 = 0x00FF0000;
    OSTASK_TCBInit();

    OS_Sem_Init();

    OS_Sem_Create(1, 10); /* Id, Val */

    OSTASK_Create(TaskIdle, NULL,&xStackIdle[32-1],IDLE_PRIORITY);

    OSTASK_Create(FuncDummy1, &baConstTemp[4],
					&xStackDummy1[TASK1_STACK_SIZE-1],MAX_PRIORITY - 1);

    OSTASK_Create(FuncDummy3, &baConstTemp[1],
					&xStackDummy3[TASK3_STACK_SIZE-1],MAX_PRIORITY);
    OSTASK_Create(FuncDummy4, &baConstTemp[0],
					&xStackDummy4[TASK4_STACK_SIZE-1],MAX_PRIORITY);

/* Dummy to avoid error during first Scheduling. */
    psCurrentTCB = &sTCB_Background;

    OS_VTimer_Init();
    OS_VTimer_Start();
    OS_Schedule_Int();

//#endif

    while(1)
    {
    }

}