///*==============================================================================+
//|  Function :  Function_DataBase.c                                              |
//|  Task     :  Source File                                                      |
//|-------------------------------------------------------------------------------|
//|  Compile  :                                                               	|
//|  Link     :                                                                	|
//|  Call     :                                                                   |
//|-------------------------------------------------------------------------------|
//|  Author   : YW                                                              	|
//|  Version  : V2.00                                                             |
//|  Creation : 2018/01/19                                                        |
//|  Revision :                                                                   |
//+==============================================================================*/
//#include    "Function_DataBase.h"
//
///*===========================================================================+
//|           Content                                                          |
//+===========================================================================*/
///*===========================================================================+
//|           Helpers  Implement                                               |
//+===========================================================================*/
///*===========================================================================+
//|           Constant                                                         |
//+===========================================================================*/
///*===========================================================================+
//|           Type definition                                                  |
//+============================================================================*/
///*===========================================================================+
//|           Constructor and destructor                                       |
//+============================================================================*/
///*===========================================================================+
//|           Operations                                                       |
//+============================================================================*/
///*===========================================================================+
//|           Inner Global Variable                                            |
//+===========================================================================*/
///*===========================================================================+
//|           Global Variable                                                  |
//+===========================================================================*/
///*--------------------------------------------------------------------------------------------+
//| name:         Single Input DataBase										                  |
//| description:  Read a Data from the DataBase according to the DataID                         |
//+--------------------------------------------------------------------------------------------*/
//BOOL_T 	Funcblock_InputModule_DataBase_SingleInput(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
//{
////	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
////	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
////	WORD_T    *p,subID;
////	DWORD_T   DataID;
////	DBVALUE   value;
////	p = &(DesigntimeData->InputsID[0]);
////
////	DataID = RuntimeData[*p].outputValue.dwData;
////	subID = DataID >>16;
////	if(subID == 0x0050)
////	{
////		value = g_Data_Userdef[DataID - 0x500000].User_Value;
////	}
////	else
////	{
////		value = DBGetValueByDataID(subID);
////	}
////	RuntimeData[DesigntimeData->serialNo].outputValue = value;
//
//	return TRUE;
//}
///*--------------------------------------------------------------------------------------------+
//| name:         Single Database point input     										      |
//| description:  Read DataID Through its Private parameter                                      |
//+-------------- ------------------------------------------------------------------------------*/
//BOOL_T 	Funcblock_Input_DataBase_SinglePoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
//{
////	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
////	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
////
////	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = DesigntimeData->PrivateParam;
//	return TRUE;
//}
//
///*--------------------------------------------------------------------------------------------+
//| name:        Multi-Input DataBase										                      |
//| description:  Read Multi-Data from the DataBase according to the DataIDs                    |
//+-------------- ------------------------------------------------------------------------------*/
//BOOL_T 	Funcblock_InputModule_DataBase_MultiInput(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
//{
//	return TRUE;
//}
//
///*--------------------------------------------------------------------------------------------+
//| name:         Output Database 										                      |
//| description:  Database output                                                               |
//+--------------------------------------------------------------------------------------------*/
//BOOL_T 	Funcblock_OutputModule_DataBase(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
//{
////	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
////	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
////
////	RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
////	RuntimeData[DesigntimeData->serialNo].DataType = 0x2;
//	return TRUE;
//}
///*--------------------------------------------------------------------------------------------+
//| name:          Single Database point output  										          |
//| description:  Write data to DataBase According to The Data Id                               |
//+--------------------------------------------------------------------------------------------*/
//BOOL_T 	Funcblock_Output_DataBase_SinglePoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
//{
////	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
////	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
////
////	WORD_T    subID;
////	DWORD_T   DataID;
////	DBVALUE   value;
//////	p = &(DesigntimeData->InputsID[0]);
////
////	DataID = DesigntimeData->PrivateParam;
////	subID = DataID >>16;
////
////	if(subID == 0x0050)
////	{
////	    g_Data_Userdef[DataID - 0x500000].User_Value = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
////	}
////	else
////	{
////		value = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
////		DBSetValueByDataID(DataID,value);
////	}
////	RuntimeData[DesigntimeData->serialNo].outputValue = RuntimeData[DesigntimeData->InputsID[0]].outputValue;
//////	RuntimeData[DesigntimeData->serialNo].outputValue = value;
////	RuntimeData[DesigntimeData->serialNo].DataType = 0x2;
//	return TRUE;
//}
///*--------------------------------------------------------------------------------------------+
//|                              END                                                             |
//+---------------------------------------------------------------------------------------------*/
//
