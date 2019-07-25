
/*==============================================================================+
|  Function :  IO_Acquisition function                                          |
|  Task     :  IO_Acquisition function Source File                              |
|-------------------------------------------------------------------------------|
|  Compile  :                                               	                |
|  Link     :                                                  	                |
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : YW                                                              	|
|  Version  : V2.00                                                             |
|  Creation : 2018/01/19                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include	"task_IO_Acquisition.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|           Helpers  Implement                                               |
+===========================================================================*/
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Type definition                                                  |
+============================================================================*/

/*===========================================================================+
|           Operations                                                       |
+============================================================================*/

/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
HTASK           g_htask_IO_Acquisition;
/*===========================================================================+
|           Inner Global Variable                                            |
+===========================================================================*/
BYTE_T          u_InputValue [  MAX_INPUT  ];
WORD_T          u_AdTastValue[  MAX_AD     ];
DWORD_T         dwStartADCollecTick	=	0;

DWORD_T         u_wADCollectinTick   =    0;

/*===========================================================================+
|           Constructor and destructor                                       |
+============================================================================*/


void    Create_TaskIOAcquisition()
{
    TASK    Task;
    Task.wType = TASK_WHILELOOP;
    Task.period.tCounter = 0;
    Task.period.tTimer = 1;
    Task.pRun = IO_Acquisition_task;
    Task.pParam = 0;
    Create_Task(&Task, 0);
}
/*==============================================================================+
|           IO_Acquisition_task                                                 |
+==============================================================================*/
void        IO_Acquisition_task(void *pParam)
{
//#if (defined M2C2132) || (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131)
  	input_test( NULL );
	output_test( NULL );
//#endif
//#ifdef  S2C4111
	Ad_Test( NULL );
	Da_Test( NULL );
//#endif
}
/*===========================================================================+
|Name:         input_test                                                    |
|Description:  Read the input from tmIoT-ixxx DI, and set the input Value to |
|              the database by offset                                        |
+===========================================================================*/
void        input_test(void *pParam)
{
#if (defined M2C2132) || (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131) || (defined IO2C1122)
	DBVALUE     DbValue;
	DWORD_T     wDBIndex ;
	WORD_T	  i;
	wDBIndex    =   ENUM_CARD_0_INTERFACE_IO_INPUTG1;

	Device_Read (g_hInput, u_InputValue, MAX_INPUT, 0);

	for( i = 0; i < MAX_INPUT; i++ )
	{
		DbValue.dwData    =   u_InputValue[i];
		DBSetValueByOffset(wDBIndex, DbValue);
		wDBIndex++;
	}
#endif
}
/*===========================================================================+
|Name:         output_test                                                   |
|Description:  Get the Output Value from the database by offset, and write   |
|               the OututValue to the tmIoT-ixxx DO                          |
+===========================================================================*/
void        output_test(void *pParam)
{
#if (defined M2C2132) || (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131) || (defined IO2C1122)
	BYTE_T 	      u_OutputValue[MAX_OUTPUT];
	WORD_T	  i;
	WORD_T      wIndex;

	for( i = 0; i < MAX_OUTPUT; i++ )
	{
		wIndex =  ENUM_CARD_0_INTERFACE_IO_OUTPUTG1 + i ;
//		u_OutputValue[i] =  DBData[ wIndex ].dbValue.wData ;
		u_OutputValue[i] = g_DBData[wIndex].dbValue.cData;
	}

	Device_Write (g_hOutput, u_OutputValue, MAX_OUTPUT, 0);
#endif
}
/*===========================================================================+
|Name:         Ad_Test                                                       |
|Description:  Read the input from tmIoT-ixxx AI, and set the input Value to |
|              the database by offset                                        |
+===========================================================================*/

void        Ad_Test(void *pParam)
{
#ifdef  S2C4111
	DBVALUE     AdDbValue;

	WORD_T	  i;
	DWORD_T     wDBIndex                 =   ENUM_CARD_0_INTERFACE_IO_AD1;

	Device_Read (g_hAd, (BYTE_T *)u_AdTastValue, MAX_AD, 0);

	for( i = 0; i < MAX_AD; i++ )
	{
		AdDbValue.dwData  = u_AdTastValue[i];
		DBSetValueByOffset(wDBIndex, AdDbValue) ;
		wDBIndex++;
	}
#endif
}

/*===========================================================================+
|Name:         output_test                                                   |
|Description:  Get the Output Value from the database by offset, and write   |
|              the OututValue to the tmIoT-ixxx AO                           |
+===========================================================================*/
void       Da_Test(void *pParam)
{
#ifdef  S2C4111
	WORD_T      i;
	WORD_T      wIndex;
	WORD_T	   g_bDA[MAX_DA];

	for( i = 0 ; i < MAX_DA ; i++ )
	{
		wIndex   = ENUM_CARD_0_INTERFACE_IO_DA1 + i;
		g_bDA[i] = g_DBData[ wIndex ].dbValue.wData ;
	}

	Device_Write (g_hDa, (BYTE_T *)g_bDA, MAX_DA, 0);
#endif
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
