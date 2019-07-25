/*==============================================================================+
|  Function :  Function_Timer.c                                                 |
|  Task     :  Source File                                                      |
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
#include    "Function_Timer.h"

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
|           Inner Global Variable                                            |
+===========================================================================*/
TIMETYPE_T                      u_DelayTime[COMPONENTCNTS];
TIMECOUNTTYPE_T                 u_HighLevelTime[COMPONENTCNTS];
TIMECOUNTTYPE_T                 u_LowLevelTime[COMPONENTCNTS];
PERIODTIMETYPE_T                u_PeriodTime[COMPONENTCNTS];

//DWORD_T                         HighValue[5][30];
//DWORD_T                         LowValue[5][30];
//DWORD_T                         PeriodValue[30];

//BYTE_T                          u_HighCount[5];
//BYTE_T                          u_LowCount[5];
BYTE_T                          u_PulseCount = 0;
DWORD_T							u_HighLevelTotalTime[COMPONENTCNTS];
DWORD_T							u_LowLevelTotalTime[COMPONENTCNTS];
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
/*---------------------------------------------------------------------------+
| Name:         DelayTime On                                                 |
| description:  The output does not switch on until a configured delay time  |
|               has expired                                                  |
+----------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Timer_DelayTime_On(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		if( RuntimeData[*p].outputValue.cData != 0x0)
		{
			if(u_DelayTime[DesigntimeData->ComponentNo].DelayTimeClock == 0)
			{
				u_DelayTime[DesigntimeData->ComponentNo].DelayTimeClock = g_dwSystemTick;
				return 0;
			}
			if(g_dwSystemTick - u_DelayTime[DesigntimeData->ComponentNo].DelayTimeClock < RuntimeData[*(p+1)].outputValue.dwData)    return 0;
			u_DelayTime[DesigntimeData->ComponentNo].DelayTimeClock = 0;
			RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[*p].outputValue;
		}
		else
		{
			u_DelayTime[DesigntimeData->ComponentNo].DelayTimeClock = 0;
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0x0;
		}
	}
    RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         DelayTime Off                                                                 |
| description:  The output resets after a configured time has expired                         |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Timer_DelayTime_Off(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    if(DesigntimeData->serialNo < g_wTotal_FunctionBlockNum)
	{
        if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
		{
			if( RuntimeData[*p].outputValue.cData == 0x0)
			{
				if(u_DelayTime[DesigntimeData->serialNo].DelayTimeClock == 0)
				{
					u_DelayTime[DesigntimeData->serialNo].DelayTimeClock = g_dwSystemTick;
					return 0;
				}
				if(g_dwSystemTick - u_DelayTime[DesigntimeData->serialNo].DelayTimeClock < RuntimeData[*(p+1)].outputValue.dwData)   return 0;
				u_DelayTime[DesigntimeData->serialNo].DelayTimeClock = 0;
				RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[*p].outputValue;
			}
			else
			{
				u_DelayTime[DesigntimeData->serialNo].DelayTimeClock = 0;
				RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
			}
		}
	}
    RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         HighLevelTime                                                                 |
| description:  The output is the input High level Time                                       |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Timer_HighLevelTime(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
    DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
    WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    if(RuntimeData[*p].outputValue.cData != 0x0)
    {
    	if( ((u_HighLevelTime[DesigntimeData->ComponentNo].UpCounter_Count == 0)&&(u_HighLevelTime[DesigntimeData->ComponentNo].DownCounter_Count == 1))|| \
            (u_HighLevelTime[DesigntimeData->ComponentNo].DownCount != u_HighLevelTime[DesigntimeData->ComponentNo].DownCounter_Count))
    	{
    		u_HighLevelTime[DesigntimeData->ComponentNo].UpCounter_Count++;
    		if( u_HighLevelTime[DesigntimeData->ComponentNo].UpCounter_Count == u_HighLevelTime[DesigntimeData->ComponentNo].DownCounter_Count )
    		{

    			u_HighLevelTime[DesigntimeData->ComponentNo].TimeTick = g_dwSystemTick;
    		}
    	}
//    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = g_dwSystemTick - u_HighLevelTime[DesigntimeData->ComponentNo].TimeTick + u_HighLevelTotalTime[DesigntimeData->ComponentNo];
    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = g_dwSystemTick - u_HighLevelTime[DesigntimeData->ComponentNo].TimeTick;
    	u_HighLevelTime[DesigntimeData->ComponentNo].DownCount = u_HighLevelTime[DesigntimeData->ComponentNo].DownCounter_Count;
	}
	else
	{
		if((u_HighLevelTime[DesigntimeData->ComponentNo].DownCounter_Count == 0)|| \
				(u_HighLevelTime[DesigntimeData->ComponentNo].UpCount != u_HighLevelTime[DesigntimeData->ComponentNo].UpCounter_Count))
		{
			u_HighLevelTime[DesigntimeData->ComponentNo].DownCounter_Count++;
			if((u_HighLevelTime[DesigntimeData->ComponentNo].UpCounter_Count!= 0) \
				&&(u_HighLevelTime[DesigntimeData->ComponentNo].UpCounter_Count != u_HighLevelTime[DesigntimeData->ComponentNo].DownCounter_Count) )
			{
//				RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (g_dwSystemTick - u_HighLevelTime[DesigntimeData->ComponentNo].TimeTick);
//				u_HighLevelTotalTime[DesigntimeData->ComponentNo] = g_dwSystemTick - u_HighLevelTime[DesigntimeData->ComponentNo].TimeTick;
			}
		}
		u_HighLevelTime[DesigntimeData->ComponentNo].UpCount = u_HighLevelTime[DesigntimeData->ComponentNo].UpCounter_Count;
	}
    RuntimeData[DesigntimeData->serialNo].DataType = 2;

    return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         LowLevelTime                                                                  |
| description:  The output is the input Low level Time                                        |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Timer_LowLevelTime(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
    DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
    WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*p].outputValue.cData == 0x0)
	{
		if( ((u_LowLevelTime[DesigntimeData->ComponentNo].DownCounter_Count == 0)&&(u_LowLevelTime[DesigntimeData->ComponentNo].UpCounter_Count == 1))|| \
            (u_LowLevelTime[DesigntimeData->ComponentNo].UpCount != u_LowLevelTime[DesigntimeData->ComponentNo].UpCounter_Count))
		{
			u_LowLevelTime[DesigntimeData->ComponentNo].DownCounter_Count++;
			if(u_LowLevelTime[DesigntimeData->ComponentNo].DownCounter_Count == u_LowLevelTime[DesigntimeData->ComponentNo].UpCounter_Count)
			{

				u_LowLevelTime[DesigntimeData->ComponentNo].TimeTick = g_dwSystemTick;
			}
		}
//		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = g_dwSystemTick - u_LowLevelTime[DesigntimeData->ComponentNo].TimeTick + u_LowLevelTotalTime[DesigntimeData->ComponentNo];
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = g_dwSystemTick - u_LowLevelTime[DesigntimeData->ComponentNo].TimeTick;
		u_LowLevelTime[DesigntimeData->ComponentNo].UpCount = u_LowLevelTime[DesigntimeData->ComponentNo].UpCounter_Count;
	}
	else
	{
		if((u_LowLevelTime[DesigntimeData->ComponentNo].UpCounter_Count == 0)|| \
            (u_LowLevelTime[DesigntimeData->ComponentNo].DownCount != u_LowLevelTime[DesigntimeData->ComponentNo].DownCounter_Count))
		{
			u_LowLevelTime[DesigntimeData->ComponentNo].UpCounter_Count++;
			if((u_LowLevelTime[DesigntimeData->ComponentNo].DownCounter_Count != 0) \
				&&(u_LowLevelTime[DesigntimeData->ComponentNo].DownCounter_Count != u_LowLevelTime[DesigntimeData->ComponentNo].UpCounter_Count))
			{
//				RuntimeData[DesigntimeData->serialNo].outputValue.dwData = g_dwSystemTick - u_LowLevelTime[DesigntimeData->ComponentNo].TimeTick;
//				u_LowLevelTotalTime[DesigntimeData->ComponentNo] = g_dwSystemTick - u_LowLevelTime[DesigntimeData->ComponentNo].TimeTick;
			}
		}
		u_LowLevelTime[DesigntimeData->ComponentNo].DownCount = u_LowLevelTime[DesigntimeData->ComponentNo].DownCounter_Count;
	}
    RuntimeData[DesigntimeData->serialNo].DataType = 2;

    return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:         PulseTimer										                              |
| description:  The output is depending on the input Pulse and input Count, and other inputs  |
|               if Inputs all have been set during the input pulse period,then the Period time|
|               will be the output value                                                      |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Timer_PulseTime(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p,i;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*p].outputValue.cData != 0x0)
	{
		for(i = 0; i <( DesigntimeData->InputCounts-1); i++)//this is upslope or not£¿
		{
			if(RuntimeData[*(p+i+1)].outputValue.cData != 0x0)
			{
				if(RuntimeData[*(p+i+1)].outputValue.cData != u_PeriodTime[DesigntimeData->ComponentNo].InputsValue[i].cData )
				{
                	u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[i] = 1;
				}
			}
			u_PeriodTime[DesigntimeData->ComponentNo].InputsValue[i].cData = RuntimeData[*(p+i+1)].outputValue.cData;
		}
		if((u_PeriodTime[DesigntimeData->ComponentNo].UpCounter_Count == 0)&&(u_PeriodTime[DesigntimeData->ComponentNo].DownCounter_Count == 1))
		{
			u_PeriodTime[DesigntimeData->ComponentNo].TimeTick = g_dwSystemTick;
			u_PeriodTime[DesigntimeData->ComponentNo].UpCounter_Count++;
			u_PeriodTime[DesigntimeData->ComponentNo].DownCount = u_PeriodTime[DesigntimeData->ComponentNo].DownCounter_Count;
		}
		else if(u_PeriodTime[DesigntimeData->ComponentNo].DownCount != u_PeriodTime[DesigntimeData->ComponentNo].DownCounter_Count)
		{
			if(DesigntimeData->InputCounts == 1)
			{
				RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (g_dwSystemTick - u_PeriodTime[DesigntimeData->ComponentNo].TimeTick);
				u_PeriodTime[DesigntimeData->ComponentNo].TimeTick = g_dwSystemTick;
			}
			else if(DesigntimeData->InputCounts == 2)
			{
				if(u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[0] == 1)
				{
					RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (g_dwSystemTick - u_PeriodTime[DesigntimeData->ComponentNo].TimeTick);
					u_PeriodTime[DesigntimeData->ComponentNo].TimeTick = g_dwSystemTick;
				}
			}
			else if(DesigntimeData->InputCounts == 3)
			{
				if((u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[0] == 1)&&(u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[1] == 1))
				{
					RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (g_dwSystemTick - u_PeriodTime[DesigntimeData->ComponentNo].TimeTick);
					u_PeriodTime[DesigntimeData->ComponentNo].TimeTick = g_dwSystemTick;
				}
			}
			else if(DesigntimeData->InputCounts == 4)
			{
				if((u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[0] == 1)&&(u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[1] == 1) \
					&&(u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[2] == 1))
				{
					RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (g_dwSystemTick - u_PeriodTime[DesigntimeData->ComponentNo].TimeTick);
					u_PeriodTime[DesigntimeData->ComponentNo].TimeTick = g_dwSystemTick;
				}
			}
			for(i = 0; i < (DesigntimeData->InputCounts - 1); i++)
			{
				u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[i] = 0;
			}
			u_PeriodTime[DesigntimeData->ComponentNo].UpCounter_Count++;
			u_PeriodTime[DesigntimeData->ComponentNo].DownCount = u_PeriodTime[DesigntimeData->ComponentNo].DownCounter_Count;
		}
	}
	else
	{
		for(i = 0; i < ( DesigntimeData->InputCounts-1); i++)//this is upslope or not£¿
		{
			if(RuntimeData[*(p+i+1)].outputValue.cData != 0x0)
			{
				if(RuntimeData[*(p+i+1)].outputValue.cData != u_PeriodTime[DesigntimeData->ComponentNo].InputsValue[i].cData )
				{
					u_PeriodTime[DesigntimeData->ComponentNo].InputChangeFlag[i] = 1;
				}
			}
			u_PeriodTime[DesigntimeData->ComponentNo].InputsValue[i].cData = RuntimeData[*(p+i+1)].outputValue.cData;
		}
		if((u_PeriodTime[DesigntimeData->ComponentNo].DownCounter_Count == 0)|| \
			(u_PeriodTime[DesigntimeData->ComponentNo].UpCount != u_PeriodTime[DesigntimeData->ComponentNo].UpCounter_Count))
		{
			u_PeriodTime[DesigntimeData->ComponentNo].DownCounter_Count++;
		}
		u_PeriodTime[DesigntimeData->ComponentNo].UpCount = u_PeriodTime[DesigntimeData->ComponentNo].UpCounter_Count;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
