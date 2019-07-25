//*****************************************************************************
//
// touch.c - Touch screen driver for the DK-TM4C129X.
//
// Copyright (c) 2013-2015 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.1.71 of the DK-TM4C129X Firmware Package.
//
//*****************************************************************************

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/ssi.h"
#include "driverlib/pin_map.h"
#include "grlib/grlib.h"
#include "grlib/widget.h"
#include "_touch_ph480272.h"
#include "_timer.h"
#include "task.h"
//*****************************************************************************
//
// The most recent raw ADC reading for the X position on the screen.  This
// value is not affected by the selected screen orientation.
//
//*****************************************************************************
volatile int16_t g_i16TouchX;
//*****************************************************************************
//
// The most recent raw ADC reading for the Y position on the screen.  This
// value is not affected by the selected screen orientation.
//
//*****************************************************************************
volatile int16_t g_i16TouchY;
int16_t u_i16TouchState = 0;
int16_t u_i16TouchFlag = FALSE;
//*****************************************************************************
//
// A pointer to the function to receive messages from the touch screen driver
// when events occur on the touch screen (debounced presses, movement while
// pressed, and debounced releases).
//
//*****************************************************************************
static int32_t (*g_pfnTSHandler)(uint32_t ui32Message, int32_t i32X,
                                 int32_t i32Y);
void TouchScreenCallbackSet(int32_t (*pfnCallback)(uint32_t ui32Message,
                                              int32_t i32X, int32_t i32Y));
void RunTaskTouchPh480272(void *vpParam);
unsigned short ReadPosition_Y();
unsigned short ReadPosition_X();
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void InitTouchPh480272()
{
    // Initialize the touch screen driver and have it route its messages to the
    // widget tree.
    //
    TouchScreenInit(g_dwSystemClock);
    TouchScreenCallbackSet(WidgetPointerMessage);
}

/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void CreateTaskTouchPh480272()
{
    TASK    Task;
    Task.wType = TASK_WHILELOOP;
    Task.period.tCounter = 0;
    Task.period.tTimer = 0;
    Task.pRun = RunTaskTouchPh480272;
    Task.pParam = 0;
    Create_Task(&Task, 0);   
}

/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void RunTaskTouchPh480272(void *vpParam)
{
    int16_t wSamples_X[10];
    int16_t wSamples_Y[10];
    int16_t wCount = 0,i,j;
    int16_t wTemp = 0;
    int16_t X1,X2,Y1,Y2;

    //
    //判断touch是否按下,如果按下读取坐标值
    //
    if(ROM_GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7) == 0 && u_i16TouchFlag == FALSE)
    {
    	//u_i16TouchFlag = TRUE;
    	//u_i16Flag = TRUE;
        u_i16TouchFlag = TRUE;
        //
        //采样10次
        //
        while(wCount < 10)
        {
            wSamples_X[wCount] = ReadPosition_X();
            wSamples_Y[wCount] = ReadPosition_Y();
            //限制在一个范围，如果不在范围之内，则丢弃
            //if(wSamples_X[wCount] > 100 && wSamples_X[wCount] < 4000 && wSamples_Y[wCount] > 100 && wSamples_Y[wCount] < 4000)
            {
                wCount ++;
            }
        }
        //将X数据升序排列
        for(j = 0; j < wCount; j++)
        {
            for(i = 0; i < wCount-1-j; i++)
            {
                if(wSamples_X[i] > wSamples_X[i+1])
                {
                    wTemp = wSamples_X[i+1];
                    wSamples_X[i+1] = wSamples_X[i];
                    wSamples_X[i] = wTemp;
                }
            }
        }
        //将Y数据升序排列
        for(j = 0; j < wCount; j++)
        {
            for(i = 0; i < wCount-1-j; i++)
            {
                if(wSamples_Y[i] > wSamples_Y[i+1])
                {
                    wTemp = wSamples_Y[i+1];
                    wSamples_Y[i+1] = wSamples_Y[i];
                    wSamples_Y[i] = wTemp;
                }
            }
        }
        X1 = wSamples_X[wCount/2 - 1];
        X2 = wSamples_X[wCount/2];
        Y1 = wSamples_Y[wCount/2 - 1];
        Y2 = wSamples_Y[wCount/2];
        if((X2>X1)&&(X2>X1+30) || (Y2>Y1)&&(Y2>Y1+30))
        {
            return;  //数据离散性太大，丢弃
        }
        else
        {
            g_i16TouchX = (wSamples_X[wCount/2 - 1] + wSamples_X[wCount/2])/2;
            g_i16TouchY = (wSamples_Y[wCount/2 - 1] + wSamples_Y[wCount/2])/2;
        }
        g_i16TouchX = (480 - (480*(g_i16TouchX-200))/3700);
        g_i16TouchY = (272 -(272*(g_i16TouchY-400))/3400);
        //
        //转换后的数据必须在有效范围，否则丢弃
        //
        if(g_i16TouchX < 0 || g_i16TouchX > 479 || g_i16TouchY < 0 || g_i16TouchY > 271)
        {
            return;
        }
        //
        // Send the pen down message to the touch screen event
        // handler.
        //
        g_pfnTSHandler(WIDGET_MSG_PTR_DOWN, g_i16TouchX, g_i16TouchY);
        u_i16TouchState = WIDGET_MSG_PTR_DOWN;

    }

    //
    //判断touch是弹起,如果弹起执行相应动作
    //
    if(ROM_GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7) != 0 && u_i16TouchFlag == TRUE)
    {
    	u_i16TouchFlag = FALSE;
        //
        //判断touch状态是否为被按下状态
        //
        if(u_i16TouchState == WIDGET_MSG_PTR_DOWN)
        {
            g_pfnTSHandler(WIDGET_MSG_PTR_UP, g_i16TouchX, g_i16TouchY);
            u_i16TouchState = WIDGET_MSG_PTR_UP;
        }
    }
}
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
unsigned short ReadPosition_X()
{
    uint32_t wTouchX = 0;
    uint32_t wTemp1, wTemp2;

	//
	// Assert the chip select to the touch plan.
	//
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	//
	// Set the SSI module into write-only mode.
	//
	ROM_SSIAdvModeSet(SSI0_BASE, SSI_ADV_MODE_WRITE);

	ROM_SSIAdvDataPutFrameEnd(SSI0_BASE, CMD_RDX);
	//
	// Wait until the command has been completely transmitted.
	//
	while(ROM_SSIBusy(SSI0_BASE))
	{
    }

	ROM_SSIAdvModeSet(SSI0_BASE, SSI_ADV_MODE_READ_WRITE);

	ROM_SSIDataPut(SSI0_BASE, 0);

	ROM_SSIDataGet(SSI0_BASE, &wTemp1);

	ROM_SSIAdvDataPutFrameEnd(SSI0_BASE, 0);

	ROM_SSIDataGet(SSI0_BASE, &wTemp2);

    wTouchX = (((wTemp1 << 8) | wTemp2) >> 3) & 0xFFF;

    //
    // De-assert the chip select to the touch plan.
    //
    ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
    return wTouchX;
}
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
unsigned short ReadPosition_Y()
{
    uint32_t wTouchY = 0;
    uint32_t wTemp1, wTemp2;
    //
    // Assert the chip select to the touch plan.
    //
    ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
	//
	// Set the SSI module into write-only mode.
	//
	ROM_SSIAdvModeSet(SSI0_BASE, SSI_ADV_MODE_WRITE);

	ROM_SSIAdvDataPutFrameEnd(SSI0_BASE, CMD_RDY);
	//
	// Wait until the command has been completely transmitted.
	//
	while(ROM_SSIBusy(SSI0_BASE))
	{
	}

	ROM_SSIAdvModeSet(SSI0_BASE, SSI_ADV_MODE_READ_WRITE);

    ROM_SSIAdvDataPutFrameEnd(SSI0_BASE, 0);
	ROM_SSIDataGet(SSI0_BASE, &wTemp1);

    ROM_SSIAdvDataPutFrameEnd(SSI0_BASE, 0);
    ROM_SSIDataGet(SSI0_BASE, &wTemp2);

    wTouchY = (((wTemp1 << 8) | wTemp2) >> 3) & 0xFFF;

	// De-assert the chip select to the touch plan.
	//
	ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);
    return wTouchY;
}
//*****************************************************************************
//
//! Handles the ADC interrupt for the touch screen.
//!
//! This function is called when the ADC sequence that samples the touch screen
//! has completed its acquisition.  The touch screen state machine is advanced
//! and the acquired ADC sample is processed appropriately.
//!
//! It is the responsibility of the application using the touch screen driver
//! to ensure that this function is installed in the interrupt vector table for
//! the ADC0 samples sequencer 3 interrupt.
//!
//! \return None.
//
//*****************************************************************************
void
TouchScreenIntHandler(void)
{
    unsigned long ulStatus;

   // u_i16TouchFlag = TRUE;
    //SysCtlDelay(10);
    //
    // Clear the GPIO PortA interrupt.
    //
    ulStatus = ROM_GPIOIntStatus(GPIO_PORTA_BASE, true);
    ROM_GPIOIntClear(GPIO_PORTA_BASE, ulStatus);
    //ROM_IntDisable(INT_GPIOA);
}

//*****************************************************************************
//
//! Initializes the touch screen driver.
//!
//! \param ui32SysClock is the frequency of the system clock.
//!
//! This function initializes the touch screen driver, beginning the process of
//! reading from the touch screen.  This driver uses the following hardware
//! resources:
//!
//! - ADC 0 sample sequence 3
//! - Timer 5 subtimer B
//!
//! \return None.
//
//*****************************************************************************
void
TouchScreenInit(uint32_t ui32SysClock)
{
    //
    //Enable SSI0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    ROM_GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    ROM_GPIOPinConfigure(GPIO_PA4_SSI0XDAT0);
    ROM_GPIOPinConfigure(GPIO_PA5_SSI0XDAT1);
    ROM_GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);

    ROM_GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
    ROM_GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, GPIO_PIN_3);

    ROM_SSIConfigSetExpClk(SSI0_BASE, ui32SysClock, SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 100000, 8);
    //
    // Enable the advanced mode of operation, defaulting to read/write mode.
    //
    ROM_SSIAdvModeSet(SSI0_BASE, SSI_ADV_MODE_READ_WRITE);

    //
    // Enable the frame hold feature.
    //
    ROM_SSIAdvFrameHoldEnable(SSI0_BASE);

    ROM_SSIEnable(SSI0_BASE);

    //
    //配置PA7为IO中断,下降沿触发
    //
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_6);
    ROM_GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_7);
    // ROM_GPIOIntTypeSet(GPIO_PORTA_BASE, GPIO_PIN_7, GPIO_FALLING_EDGE);
    // ROM_GPIOIntEnable(GPIO_PORTA_BASE, GPIO_PIN_7);
    // ROM_IntEnable(INT_GPIOA);
    // ROM_IntMasterEnable();
}

//*****************************************************************************
//
//! Sets the callback function for touch screen events.
//!
//! \param pfnCallback is a pointer to the function to be called when touch
//! screen events occur.
//!
//! This function sets the address of the function to be called when touch
//! screen events occur.  The events that are recognized are the screen being
//! touched (``pen down''), the touch position moving while the screen is
//! touched (``pen move''), and the screen no longer being touched (``pen
//! up'').
//!
//! \return None.
//
//*****************************************************************************
void
TouchScreenCallbackSet(int32_t (*pfnCallback)(uint32_t ui32Message,
                                              int32_t i32X, int32_t i32Y))
{
    //
    // Save the pointer to the callback function.
    //
    g_pfnTSHandler = pfnCallback;
}

//*****************************************************************************
//
// Close the Doxygen group.
//! @}
//
//*****************************************************************************
