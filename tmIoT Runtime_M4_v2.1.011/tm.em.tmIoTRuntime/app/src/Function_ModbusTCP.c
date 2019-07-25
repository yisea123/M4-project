/*==========================================================================+
|  File     : Function_ModbusTCP.h                                           |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : CP                                                        |
|  Version  : v1.00                                                         |
|  Creation : 2018Äê11ÔÂ21ÈÕ                                                                                                                                                                               |
|  Revision :                                                               |
+==========================================================================*/
#include "Function_ModbusTCP.h"
#include "comlib.h"
#include "taskmodbustcp.h"
#include "task_Runtime.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/


/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/

/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/

/*--------------------------------------------------------------------------------------------+
| name:        ModbusTCP_M03     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusTCP_M03(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;

	RUNTIMEDATATYPE_T* RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T* DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

	for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++)
	{
		Get_ModbusTCPODValue_ByDataID_2(g_ALLCommDataList[i].DataID, &g_ALLCommDataList[i].Value);
	}

	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[Selected_StartAddr];

	RuntimeData[DesigntimeData->serialNo].DataType = 8;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusTCP_M04     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusTCP_M04(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;

	RUNTIMEDATATYPE_T* RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T* DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

	for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++)
	{
		Get_ModbusTCPODValue_ByDataID_2(g_ALLCommDataList[i].DataID, &g_ALLCommDataList[i].Value);
	}

	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[Selected_StartAddr];

	RuntimeData[DesigntimeData->serialNo].DataType = 8;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusTCP_M06     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusTCP_M06(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i, j;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;
	COMMDATATYPE_T *p_ALLCommDataList;
	DWORD_T dwValue;

	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	if(RuntimeData[DesigntimeData->InputsID[0]].DataType == 8)
		p_ALLCommDataList = (void *)RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

	for (j = 0; j < Selected_Length; j++) {
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType == 8)
			dwValue = p_ALLCommDataList[j].Value.dwData;
		else
			dwValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
		memcpy(&g_ALLCommDataList[Selected_StartAddr+j].Value.dwData, &dwValue, sizeof(DWORD_T));
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
			break;
	}

	for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++)
	{
		Set_ModbusTCPODValueByDataID_2(g_ALLCommDataList[i].DataID, g_ALLCommDataList[i].Value.dwData);
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
			break;
	}

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusTCP_S03     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusTCP_S03(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i, j;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;
	COMMDATATYPE_T *p_ALLCommDataList;
	DWORD dwValue;

	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	if(RuntimeData[DesigntimeData->InputsID[0]].DataType == 8)
		p_ALLCommDataList = (void *)RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

	for (j = 0; j < Selected_Length; j++) {
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType == 8)
			dwValue = p_ALLCommDataList[j].Value.dwData;
		else
			dwValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
		memcpy(&g_ALLCommDataList[Selected_StartAddr+j].Value.dwData, &dwValue, sizeof(DWORD_T));
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
			break;
	}

	for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++)
	{
		Set_ModbusTCPRegValue_byDataID_2(g_ALLCommDataList[i].DataID, g_ALLCommDataList[i].Value.dwData);
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
			break;
	}

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusTCP_S04     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusTCP_S04(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i, j;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;
	COMMDATATYPE_T *p_ALLCommDataList;
	DWORD_T dwValue;

	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	if(RuntimeData[DesigntimeData->InputsID[0]].DataType == 8)
		p_ALLCommDataList = (void *)RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

	for (j = 0; j < Selected_Length; j++) {
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType == 8)
			dwValue = p_ALLCommDataList[j].Value.dwData;
		else
			dwValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
		memcpy(&g_ALLCommDataList[Selected_StartAddr+j].Value.dwData, &dwValue, sizeof(DWORD_T));
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
			break;
	}

	for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++)
	{
		Set_ModbusTCPRegValue_byDataID_2(g_ALLCommDataList[i].DataID, g_ALLCommDataList[i].Value.dwData);
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
			break;
	}

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusTCP_S06     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusTCP_S06(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;

	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

		for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++)
		{
			Get_ModbusTCPRegValue_byDataID_2(g_ALLCommDataList[i].DataID, &g_ALLCommDataList[i].Value);
		}

		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[Selected_StartAddr];

		RuntimeData[DesigntimeData->serialNo].DataType = 8;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
									END								          				  |
+--------------------------------------------------------------------------------------------*/
