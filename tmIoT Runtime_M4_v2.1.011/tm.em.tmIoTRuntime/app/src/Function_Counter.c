/*==============================================================================+
|  Function : Function_Counter.c                                                |
|  Task     : Function_Counter Source File                                      |
|-------------------------------------------------------------------------------|
|  Compile  :                                                               	|
|  Link     :                                                                	|
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : YW                                                              	|
|  Version  : V2.00                                                             |
|  Creation : 2018/01/19                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include    "Function_Counter.h"

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
|           Constructor and destructor                                       |
+============================================================================*/
/*===========================================================================+
|           Operations                                                       |
+============================================================================*/
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
/*===========================================================================+
|           Inner Global Variable                                            |
+===========================================================================*/
COUTERTYPE_T        u_UpCounter[COMPONENTCNTS];
COUTERTYPE_T        u_DownCounter[COMPONENTCNTS];
PULSECOUNTERTYPE_T  u_PulseCounter[COMPONENTCNTS];

/*--------------------------------------------------------------------------------------------+
| name:         UpSlope Counter                                                               |
| description:  An input pulse increment will be counted or not,depending on the RST        |
|               If RST is reset input pulse increment will be counted,else the UpSlope      |
|               Counter will be cleared                                                       |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Counter_UpSlopeCounter(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		if(RuntimeData[*(p+1)].outputValue.cData == 0x00)
		{
			if(RuntimeData[*p].outputValue.cData != 0x0)
			{
				if(	((u_UpCounter[DesigntimeData->ComponentNo].UpCounter_Count == 0)&&(u_UpCounter[DesigntimeData->ComponentNo].DownCounter_Count == 1))||
				(u_UpCounter[DesigntimeData->ComponentNo].DownCount != u_UpCounter[DesigntimeData->ComponentNo].DownCounter_Count))
				{
				    u_UpCounter[DesigntimeData->ComponentNo].UpCounter_Count++;
				}
				u_UpCounter[DesigntimeData->ComponentNo].DownCount = u_UpCounter[DesigntimeData->ComponentNo].DownCounter_Count;
				RuntimeData[DesigntimeData->serialNo].outputValue.dwData = u_UpCounter[DesigntimeData->ComponentNo].UpCounter_Count;
			}

			else
			{

				if((u_UpCounter[DesigntimeData->ComponentNo].DownCounter_Count == 0)||
				(u_UpCounter[DesigntimeData->ComponentNo].UpCount != u_UpCounter[DesigntimeData->ComponentNo].UpCounter_Count))
				{
				    u_UpCounter[DesigntimeData->ComponentNo].DownCounter_Count++;
				}
				u_UpCounter[DesigntimeData->ComponentNo].UpCount = u_UpCounter[DesigntimeData->ComponentNo].UpCounter_Count;
			}
		}
		else
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0x0;
			u_UpCounter[DesigntimeData->ComponentNo].UpCounter_Count = 0;
			u_UpCounter[DesigntimeData->ComponentNo].DownCounter_Count = 0;
			u_UpCounter[DesigntimeData->ComponentNo].UpCount = 0;
			u_UpCounter[DesigntimeData->ComponentNo].DownCount = 0;
		}
	}
    RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:         DownSlope Counter                                                             |
| description:  An input pulse decrement will be counted or not,depending on the RST        |
|               If RST is reset input pulse increment will be counted,else the UpSlope      |
|               Counter will be cleared                                                       |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Counter_DownSlopeCounter(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*(p+1)].outputValue.cData == 0x00)
	{
		if(RuntimeData[*p].outputValue.cData == 0x0)
		{
			if(	((u_DownCounter[DesigntimeData->ComponentNo].DownCounter_Count == 0)&&(u_DownCounter[DesigntimeData->ComponentNo].UpCounter_Count == 1))||
			(u_DownCounter[DesigntimeData->ComponentNo].UpCount != u_DownCounter[DesigntimeData->ComponentNo].DownCounter_Count))
			{
			    u_DownCounter[DesigntimeData->ComponentNo].DownCounter_Count++;
			}
			u_DownCounter[DesigntimeData->ComponentNo].UpCount = u_DownCounter[DesigntimeData->ComponentNo].UpCounter_Count;
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = u_DownCounter[DesigntimeData->ComponentNo].DownCounter_Count;
		}
		else
		{
			if((u_DownCounter[DesigntimeData->ComponentNo].UpCounter_Count == 0)||
			(u_DownCounter[DesigntimeData->ComponentNo].DownCount != u_DownCounter[DesigntimeData->ComponentNo].DownCounter_Count))
			{
			    u_DownCounter[DesigntimeData->ComponentNo].UpCounter_Count++;
			}
			u_DownCounter[DesigntimeData->ComponentNo].DownCount = u_DownCounter[DesigntimeData->ComponentNo].DownCounter_Count;
		}
	}
	else
	{
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0x0;
		u_DownCounter[DesigntimeData->ComponentNo].UpCounter_Count = 0;
		u_DownCounter[DesigntimeData->ComponentNo].DownCounter_Count = 0;
		u_DownCounter[DesigntimeData->ComponentNo].UpCount = 0;
		u_DownCounter[DesigntimeData->ComponentNo].DownCount = 0;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Pulse Counter                                                                  |
| description:  An input pulse decrement will be counted or not,depending on the input Count   |
|               and the other inputs,and the RST.if the RST is reset and inputs all have been  |
|               set during the input pulse period,the input pulse will be Counted,if the RST is|
|               set the Pulse_Counter will be cleared                                          |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Counter_PulseCounter(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p,i;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*(p+(DesigntimeData->InputCounts-1))].outputValue.cData == 0x00)
	{
	    if(RuntimeData[*p].outputValue.cData != 0x0)
		{
		 	for(i = 0; i <( DesigntimeData->InputCounts-2); i++)//this is upslope or not£¿
			{
				if(RuntimeData[*(p+i+1)].outputValue.cData != 0x0)
				{
					if(RuntimeData[*(p+i+1)].outputValue.cData != u_PulseCounter[DesigntimeData->ComponentNo].InputsValue[i].cData )
					{
						u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[i] = 1;
					}
				}
				u_PulseCounter[DesigntimeData->ComponentNo].InputsValue[i].cData = RuntimeData[*(p+i+1)].outputValue.cData;
			}

		 	if((u_PulseCounter[DesigntimeData->ComponentNo].UpCounter_Count == 0)&&(u_PulseCounter[DesigntimeData->ComponentNo].DownCounter_Count == 1))
		 	{
		 		u_PulseCounter[DesigntimeData->ComponentNo].UpCounter_Count++;
		 	}
		 	else if(u_PulseCounter[DesigntimeData->ComponentNo].DownCount != u_PulseCounter[DesigntimeData->ComponentNo].DownCounter_Count)
			{
	            if(DesigntimeData->InputCounts == 2)//the inputCount = 2,the inputCount is 2 at least
				{
				  	 u_PulseCounter[DesigntimeData->ComponentNo].PulseCounter_Count++;
				}
				else if( DesigntimeData->InputCounts == 3 )//the inputCount = 3
				{
					if( u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[0] == 1 )
					{
						 u_PulseCounter[DesigntimeData->ComponentNo].PulseCounter_Count++;
					}
				}
				else if( DesigntimeData->InputCounts == 4 )//the inputCount = 4
				{
					if((u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[0] == 1)&&(u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[1] == 1))
					{
						 u_PulseCounter[DesigntimeData->ComponentNo].PulseCounter_Count++;
					}
				}
				else if( DesigntimeData->InputCounts == 5 )//the inputCount = 5
				{
					if((u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[0] == 1)&&(u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[1] == 1)
					    &&(u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[2] == 1))
					{
						u_PulseCounter[DesigntimeData->ComponentNo].PulseCounter_Count++;
					}
				}

				u_PulseCounter[DesigntimeData->ComponentNo].UpCounter_Count++;
				for( i = 0; i < (DesigntimeData->InputCounts -2); i++)
				{
					u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[i] = 0;
				}
			}

			u_PulseCounter[DesigntimeData->ComponentNo].DownCount = u_PulseCounter[DesigntimeData->ComponentNo].DownCounter_Count;
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = u_PulseCounter[DesigntimeData->ComponentNo].PulseCounter_Count;
		}
		else
		{
		 	for(i = 0; i < (DesigntimeData->InputCounts - 2); i++)
			{
				if(RuntimeData[*(p+i+1)].outputValue.cData != 0x0)
				{
					if(RuntimeData[*(p+i+1)].outputValue.cData != u_PulseCounter[DesigntimeData->ComponentNo].InputsValue[i].cData )
					{
						u_PulseCounter[DesigntimeData->ComponentNo].InputChangeFlag[i] = 1;
					}
				}
				u_PulseCounter[DesigntimeData->ComponentNo].InputsValue[i].cData = RuntimeData[*(p+i+1)].outputValue.cData;
			}

			if((u_PulseCounter[DesigntimeData->ComponentNo].DownCounter_Count == 0)||
			(u_PulseCounter[DesigntimeData->ComponentNo].UpCount != u_PulseCounter[DesigntimeData->ComponentNo].UpCounter_Count))
			{
				u_PulseCounter[DesigntimeData->ComponentNo].DownCounter_Count++;
			}
			u_PulseCounter[DesigntimeData->ComponentNo].UpCount = u_PulseCounter[DesigntimeData->ComponentNo].UpCounter_Count ;

		}
	}
	else
	{
		u_PulseCounter[DesigntimeData->ComponentNo].UpCounter_Count = 0;
		u_PulseCounter[DesigntimeData->ComponentNo].UpCount = 0;
		u_PulseCounter[DesigntimeData->ComponentNo].DownCounter_Count = 0;
		u_PulseCounter[DesigntimeData->ComponentNo].DownCount = 0;
		u_PulseCounter[DesigntimeData->ComponentNo].PulseCounter_Count = 0;
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

   	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
