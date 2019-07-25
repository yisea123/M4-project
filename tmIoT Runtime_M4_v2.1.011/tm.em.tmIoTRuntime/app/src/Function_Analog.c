/*===========================================================================+
|  Function : Function_Analoc.c                                              |
|  Task     : Source File                                                    |
|----------------------------------------------------------------------------|
|  Compile  :                                                                |
|  Link     :                                                                |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : YW                                                             |
|  Version  : V2.00                                                          |
|  Creation : 2018/01/19                                                     |
|  Revision :                                                                |
+===========================================================================*/

#include    "Function_Analog.h"
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
|           Helpers  Implement                                               |
+===========================================================================*/

/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
WORD_T         g_AdValue[5][ADVALUECOUNT];
/*===========================================================================+
|           Inner Global variable                                            |
+===========================================================================*/

ADCOLLECTORTICKTYPE_T   u_AdCollector[10];
WORD_T                  u_AdCount;
DWORD_T                 MaxValue[10] = {0};
DWORD_T                 MinValue[10] = {0};

/*--------------------------------------------------------------------------------------------+
| name:         Analog Max      										                      |
| description:  The input is an Analog Signal, The Output is always the Maximum Value         |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Sort_Max(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

//    if(RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData > MaxValue[DesigntimeData->ComponentNo])
//    {
//    	MaxValue[DesigntimeData->ComponentNo] = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
//    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
//    }
//    else
//    {
//    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = MaxValue[DesigntimeData->ComponentNo];
//    }
//	RuntimeData[DesigntimeData->serialNo].DataType = 2;
	WORD_T   *p;
	BYTE_T	  i;
    p = &(DesigntimeData->InputsID[0]);
    MaxValue[DesigntimeData->ComponentNo] = RuntimeData[*p].outputValue.dwData;

	for( i = 1; i < DesigntimeData->InputCounts; i++)
	{
		if(RuntimeData[*(p+i)].outputValue.dwData >= MaxValue[DesigntimeData->ComponentNo])
			MaxValue[DesigntimeData->ComponentNo] = RuntimeData[*(p+i)].outputValue.dwData;
	}
	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = MaxValue[DesigntimeData->ComponentNo];
	RuntimeData[DesigntimeData->serialNo].DataType = 2;


	return   TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:         Analog Min                                                                    |
| description:  The input is an Analog Signal, The Output is always the Minimum Value         |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Sort_Min(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

//    if(RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData < MinValue[DesigntimeData->ComponentNo])
//    {
//    	MinValue[DesigntimeData->ComponentNo] = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
//    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
//    }
//    else
//    {
//    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = MinValue[DesigntimeData->ComponentNo];
//    }
//	RuntimeData[DesigntimeData->serialNo].DataType = 2;
	WORD_T   *p;
	BYTE_T	  i;
    p = &(DesigntimeData->InputsID[0]);
    MaxValue[DesigntimeData->ComponentNo] = RuntimeData[*p].outputValue.dwData;

	for( i = 1; i < DesigntimeData->InputCounts; i++)
	{
		if(RuntimeData[*(p+i)].outputValue.dwData >= MaxValue[DesigntimeData->ComponentNo])
			MaxValue[DesigntimeData->ComponentNo] = RuntimeData[*(p+i)].outputValue.dwData;
	}
	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = MaxValue[DesigntimeData->ComponentNo];
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return   TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Large                                                                         |
| description:  The output is 1 or 0,depending on the difference inputValue1 and inputValue2. |
|               If InputValue1 Larger InputValue2,the output is 1.                            |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Comparator_Large(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    if(RuntimeData[*p].outputValue.dwData > RuntimeData[*(p+1)].outputValue.dwData)
    {
    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0xFFFFFFFF;
    }
    else
    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;

	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         LargeEqual                                                                    |
| description:  The output is 1 or 0,depending on the difference inputValue1 and inputValue2. |
|               If InputValue1 Larger or Equal InputValue2,the output is 1.                  |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Comparator_lagerEqual(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

	p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*p].outputValue.dwData >= RuntimeData[*(p+1)].outputValue.dwData)
	{
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
	}
	else
	{
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0x0;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Equal                                                                         |
| description:  The output is 1 or 0,depending on the difference inputValue1 and inputValue2. |
|               If InputValue1 Equal InputValue2,the output is 1.                             |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Comparator_Equal(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    if(RuntimeData[*p].outputValue.dwData == RuntimeData[*(p+1)].outputValue.dwData)
    {
    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
    }
    else
    	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;

	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:         AnalogSpecial Connector 										              |
| description:  An Analog signal as inputValue,The inputValue depend on the first Input Trg   |
|               If The Trg is Set,then Collect the inputValue every 10ms, Waiting The Trg is  |
|               Set again,Then Stop Collect and Send The Value to the iNet, Turn this cycle   |
|               his Module is just for IMM                                                    |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_InputModule_AnalogSpecialCollector(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p,i;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*p].outputValue.cData != 0x0)
	{
		if(((u_AdCollector[DesigntimeData->ComponentNo].UpCounter_Count == 0)&&(u_AdCollector[DesigntimeData->ComponentNo].DownCounter_Count == 1))|| \
			(u_AdCollector[DesigntimeData->ComponentNo].DownCount != u_AdCollector[DesigntimeData->ComponentNo].DownCounter_Count))//this is new rising or not?
		{
		  	u_AdCollector[DesigntimeData->ComponentNo].UpCounter_Count++;
			u_AdCollector[DesigntimeData->ComponentNo].DownCount = u_AdCollector[DesigntimeData->ComponentNo].DownCounter_Count;
			RuntimeData[DesigntimeData->ComponentNo].outputValue.dwData = ~0;
			u_AdCount = 0;
			for(i = 0 ; i < ADVALUECOUNT; i++)
			{
				g_AdValue[DesigntimeData->ComponentNo][i] = 0; //clean the g_AdValue before reload the new data.
			}
		}
		if(u_AdCollector[DesigntimeData->ComponentNo].UpCounter_Count >=1)
		{
			if(u_AdCount < ADVALUECOUNT)
			{
				if((u_AdCollector[DesigntimeData->ComponentNo].AdCollectorTick == 0)\
				    ||(g_dwSystemTick - u_AdCollector[DesigntimeData->ComponentNo].AdCollectorTick >= 10))
				{
				     g_AdValue[DesigntimeData->ComponentNo][u_AdCount] = RuntimeData[*(p+1)].outputValue.wData; //AD¸³Öµ£»

					 u_AdCollector[DesigntimeData->ComponentNo].AdCollectorTick = g_dwSystemTick;
					 u_AdCount++;
					 return 0;
				}
			}
		}
	}
	else
	{
		if((u_AdCollector[DesigntimeData->ComponentNo].DownCounter_Count == 0)||
		(u_AdCollector[DesigntimeData->ComponentNo].UpCount != u_AdCollector[DesigntimeData->ComponentNo].UpCounter_Count))
		{
			u_AdCollector[DesigntimeData->ComponentNo].DownCounter_Count++;
			u_AdCollector[DesigntimeData->ComponentNo].UpCount = u_AdCollector[DesigntimeData->ComponentNo].UpCounter_Count;
		}
		if(u_AdCollector[DesigntimeData->ComponentNo].UpCounter_Count >=1 )
		{
			if(u_AdCount < ADVALUECOUNT)
			{
			    if((u_AdCollector[DesigntimeData->ComponentNo].AdCollectorTick == 0)\
			       ||(g_dwSystemTick - u_AdCollector[DesigntimeData->ComponentNo].AdCollectorTick >= 10))
				{
				     g_AdValue[DesigntimeData->ComponentNo][u_AdCount] = RuntimeData[*(p+1)].outputValue.wData;//AD¸³Öµ£»
					 u_AdCollector[DesigntimeData->ComponentNo].AdCollectorTick = g_dwSystemTick;
					 u_AdCount++;
					 return 0;
				}
			}
		}
	}
	if(u_AdCollector[DesigntimeData->ComponentNo].DownCounter_Count >= 0xffffffff)
	{
	    u_AdCollector[DesigntimeData->ComponentNo].DownCounter_Count = 0;
	    u_AdCollector[DesigntimeData->ComponentNo].AdCollectorTick = 0;
	    u_AdCollector[DesigntimeData->ComponentNo].DownCount = 0;
	    u_AdCollector[DesigntimeData->ComponentNo].UpCount = 0;
	    u_AdCollector[DesigntimeData->ComponentNo].UpCounter_Count = 0;
	}
    RuntimeData[DesigntimeData->ComponentNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:         Analog Collector										                      |
| description:  A Analog signal as inputValue,And outputValue is also Analog signal           |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_InputModule_AnalogCollector(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T         *p;
	p = &(DesigntimeData->InputsID[0]);
	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[*p].outputValue.dwData;  //Read input Data from function block outputValue
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
