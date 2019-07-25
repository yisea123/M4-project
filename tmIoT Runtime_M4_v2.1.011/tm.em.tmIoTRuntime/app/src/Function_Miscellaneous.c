/*==============================================================================+
|  Function :                                                                   |
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
#include    "Function_Miscellaneous.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Inner Global variable                                            |
+===========================================================================*/

SQUAREWAVETYPE_T   u_SquareWave_Generator[20];

/*--------------------------------------------------------------------------------------------+
| Name:         Latching  RS                                                                  |
| description:  The output is 1 at the RST is rest and the input is 1                         |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Miscellaneous_RSLatching(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*(p+1)].outputValue.cData == 0x0)
	{
		if(RuntimeData[*p].outputValue.cData != 0x0)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
		}
	}
	else
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Latching  SR                                                                  |
| description:  The output is 1 at the RST is rest and the input is 1                         |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Miscellaneous_SRLatching(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
	if(RuntimeData[*(p+1)].outputValue.cData == 0x0)
	{
		if(RuntimeData[*p].outputValue.cData != 0x0)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData =0x0;
		}
	}
	else
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Square Wave Generator                                                         |
| description:  The Pulse shape at the output can be modified by the configurable the pulse   |
|               ratio.                                                                        |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Miscellaneous_SquareWaveGenertor(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
	if( RuntimeData[*p].outputValue.cData != 0x0)
	{
		if(u_SquareWave_Generator[DesigntimeData->ComponentNo].SquareWave_Tick == 0)
		{
		    RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[*p].outputValue;
			u_SquareWave_Generator[DesigntimeData->ComponentNo].SquareWave_Tick = g_dwSystemTick;
			return 0;
		}
		if((g_dwSystemTick - u_SquareWave_Generator[DesigntimeData->ComponentNo].SquareWave_Tick >= RuntimeData[*(p+1)].outputValue.dwData)&& \
		     (g_dwSystemTick - u_SquareWave_Generator[DesigntimeData->ComponentNo].SquareWave_Tick < \
		     (RuntimeData[*(p+1)].outputValue.dwData + RuntimeData[*(p+2)].outputValue.dwData)))
		{
			 RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0x0;
			 return 0;
		}
		if(g_dwSystemTick - u_SquareWave_Generator[DesigntimeData->ComponentNo].SquareWave_Tick >= \
		     (RuntimeData[*(p+1)].outputValue.dwData + RuntimeData[*(p+2)].outputValue.dwData))
		{
			 u_SquareWave_Generator[DesigntimeData->ComponentNo].SquareWave_Tick = 0;
			 return 0;
		}
	}
	else
	{
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0x0;
		u_SquareWave_Generator[DesigntimeData->ComponentNo].SquareWave_Tick = 0;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Constant       										                          |
| description:  The output is depending on its configurable the parameter                    |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Miscellaneous_Constant(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = DesigntimeData->PrivateParam;
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Constant       										                          |
| description:  The output is depending on its configurable the parameter                    |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Miscellaneous_Random(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	srand((unsigned int)g_dwSystemTick);
	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = rand() % DesigntimeData->PrivateParam + 1;
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         GetValueAddress       										                  |
| description:  The output is depending on its configurable the parameter                     |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Miscellaneous_GetValueAddress(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T    *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	COMMDATATYPE_T *p_ALLCommDataList;

	if(RuntimeData[DesigntimeData->InputsID[0]].DataType == 8)
	{
		p_ALLCommDataList = (void *)RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = p_ALLCommDataList[DesigntimeData->PrivateParam].Value.dwData;
	}
	else
	{
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         linker       										                  |
| description:                      |
+--------------------------------------------------------------------------------------------*/
BOOL_T  Funcblock_High_Linker(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
    DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
    WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    if(RuntimeData[*(p+1)].outputValue.dwData != 0x0)
    {
        RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[*p].outputValue.dwData;
    }else
    {
        return TRUE;
    }

    RuntimeData[DesigntimeData->serialNo].DataType = 1;

    return TRUE;
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
