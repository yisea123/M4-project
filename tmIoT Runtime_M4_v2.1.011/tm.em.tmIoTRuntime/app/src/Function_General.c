/*==============================================================================+
|  Function :  Function_General.c                                               |
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
#include    "Function_General.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/

/*--------------------------------------------------------------------------------------------+
| Name:         Digital input                                                                 |
| description:  The output Value is the input Value                                           |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_InputModule_Digital(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Analog input                                                                  |
| description:  The output Value is the input Value                                           |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_InputModule_Analog(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Global Variable input                                                         |
| description:  The output is the Value of selected global variable                           |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_InputModule_GlobalVariable(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T index;

	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		index = (WORD_T)DesigntimeData->PrivateParam - 1;
		RuntimeData[DesigntimeData->serialNo].outputValue = g_ALLGlobalVariableList[index].Value;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = g_ALLGlobalVariableList[index].GVType;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Digital output                                                                |
| description:  The output Value is the input Value                                           |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_OutputModule_Digital(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Analog output                                                                 |
| description:  The output Value is the input Value                                           |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_OutputModule_Analog(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Global Variable output                                                        |
| description:  The output Value is the input Value                                           |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_OutputModule_GlobalVariable(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T index;

	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		index = (WORD_T)DesigntimeData->PrivateParam - 1;
		g_ALLGlobalVariableList[index].Value.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = g_ALLGlobalVariableList[index].GVType;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
