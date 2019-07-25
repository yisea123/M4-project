/*===========================================================================+
|  Function : Function_Modbus.c                                              |
|  Task     : Source File                                                    |
|----------------------------------------------------------------------------|
|  Compile  :                                                                |
|  Link     :                                                                |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : jy                                                             |
|  Version  : V2.00                                                          |
|  Creation : 2018/03/19                                                     |
|  Revision :                                                                |
+===========================================================================*/
#include "Function_Modbus.h"
#include "comlib.h"
#include "taskrs485.h"
#include "task_Runtime.h"
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
int Get_UserSelect_StartIndex(DWORD_T dwValue);
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
BYTE_T Modbus_Component_Output[MaxTask*MAXNUM] = {0};
BYTE_T *pModbus_Component_Output = Modbus_Component_Output;
/*===========================================================================+
|           Inner Global variable                                            |
+===========================================================================*/

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M03     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M03(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;

	RUNTIMEDATATYPE_T* RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T* DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

		for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++) {
			Get_ODValueByDataID_2(g_ALLCommDataList[i].DataID, &g_ALLCommDataList[i].Value);
		}

		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[Selected_StartAddr];

		RuntimeData[DesigntimeData->serialNo].DataType = 8;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M04     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M04(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;

	RUNTIMEDATATYPE_T* RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T* DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

		for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++) {
			Get_ODValueByDataID_2(g_ALLCommDataList[i].DataID, &g_ALLCommDataList[i].Value);
		}

		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[Selected_StartAddr];

		RuntimeData[DesigntimeData->serialNo].DataType = 8;    //DataType == 8, 说明outputValue.dwData是一个地址而不是一个值

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M06     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M06(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
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

		for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++) {
			Set_ODValueByDataID_2(g_ALLCommDataList[i].DataID, g_ALLCommDataList[i].Value.dwData);
			if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
				break;
		}

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M10     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M10(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_M06(DesignTime_ObjectListData, RunTime_ObjectData);
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M0F    										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M0F(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_M06(DesignTime_ObjectListData, RunTime_ObjectData);
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M01    										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M01(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_M03(DesignTime_ObjectListData, RunTime_ObjectData);
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M02    										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M02(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_M03(DesignTime_ObjectListData, RunTime_ObjectData);
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_M05    										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_M05(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_M06(DesignTime_ObjectListData, RunTime_ObjectData);
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S03     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S03(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
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

	for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++) {
		Set_RegValue_byDataID_2(g_ALLCommDataList[i].DataID, g_ALLCommDataList[i].Value.dwData);
		if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
			break;
	}

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S04     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S04(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
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

		for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++) {
			Set_RegValue_byDataID_2(g_ALLCommDataList[i].DataID, g_ALLCommDataList[i].Value.dwData);
			if(RuntimeData[DesigntimeData->InputsID[0]].DataType != 8)
				break;
		}

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S06     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S06(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	int i;
	WORD_T Selected_StartAddr;
	WORD_T Selected_Length;

	RUNTIMEDATATYPE_T      *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	Selected_StartAddr = dwtow_L(DesigntimeData->PrivateParam);
	Selected_Length	= dwtow_H(DesigntimeData->PrivateParam);

		for (i = Selected_StartAddr; i < Selected_Length+Selected_StartAddr; i++) {
			Get_RegValue_byDataID_2(g_ALLCommDataList[i].DataID, &g_ALLCommDataList[i].Value);
		}

		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[Selected_StartAddr];

		RuntimeData[DesigntimeData->serialNo].DataType = 8;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S01     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S01(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_S03(DesignTime_ObjectListData, RunTime_ObjectData);
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S02     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S02(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_S03(DesignTime_ObjectListData, RunTime_ObjectData);
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S05     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S05(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_S06(DesignTime_ObjectListData, RunTime_ObjectData);
}

/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S0F     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S0F(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_S06(DesignTime_ObjectListData, RunTime_ObjectData);
}
/*--------------------------------------------------------------------------------------------+
| name:        ModbusRTU_S10     										                      |
| description: 																		          |
+--------------------------------------------------------------------------------------------*/
BOOL_T Funcblock_ModbusRTU_S10(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	return Funcblock_ModbusRTU_S06(DesignTime_ObjectListData, RunTime_ObjectData);
}
/*--------------------------------------------------------------------------------------------+
									END								          |
+--------------------------------------------------------------------------------------------*/
