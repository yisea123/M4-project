/*==============================================================================+
|  Function :  Function_LogicFormula.c                                          |
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

#include    "Function_LogicFormula.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*--------------------------------------------------------------------------------------------+
| name:         Logic And                                                                      |
| description:  The output of an AND function is only 1 if all inputs are 1,that is,when they  |
|               are closed.                                                                    |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Logic_And(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	BYTE_T   i;
	DBVALUE  Data;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    Data.dwData = RuntimeData[*p].outputValue.dwData;
    if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		for( i = 1; i < DesigntimeData->InputCounts; i++)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData =(Data.dwData && RuntimeData[*(p+i)].outputValue.dwData);
			Data.dwData =  RuntimeData[DesigntimeData->serialNo].outputValue.dwData;
		}
	}
    if(RuntimeData[DesigntimeData->serialNo].outputValue.dwData) RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
    else RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;

    RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:         Logic Or                                                                       |
| description:  The output of an OR function is 1 if at least one input is 1(closed).          |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Logic_Or(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	DBVALUE   Data;
	BYTE_T   i;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    Data.dwData = RuntimeData[*p].outputValue.dwData;
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		for( i = 1; i < DesigntimeData->InputCounts; i++)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData =(Data.dwData || RuntimeData[*(p+i)].outputValue.dwData);
			Data.cData =  RuntimeData[DesigntimeData->serialNo].outputValue.dwData;
		}
	}
    if(RuntimeData[DesigntimeData->serialNo].outputValue.dwData) RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
    else RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;

	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| name:         Logic Not                                                                      |
| description:  The output of an NOT function is 1 if the input is 0,The NOT block inverts the |
|                 input status.                                                                |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Logic_Not(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		if(RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData)
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;
		else
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0xFFFFFFFF;
	}

	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:         Logic Xor                                                                      |
| description:  The output of an Xor function is 1 if the input,if the input is 1 of count is  |
|               Odd, else the output is 0                                                     |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Logic_Xor(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	BYTE_T   i;
	WORD_T   *p;
	DBVALUE  Data;

	p = &(DesigntimeData->InputsID[0]);
	Data.dwData = RuntimeData[*p].outputValue.dwData;
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
	    for( i = 1; i < DesigntimeData->InputCounts; i++)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData =(Data.dwData^RuntimeData[*(p+i)].outputValue.dwData);
			Data.dwData =  RuntimeData[DesigntimeData->serialNo].outputValue.dwData;
		}
	}
    if(RuntimeData[DesigntimeData->serialNo].outputValue.dwData) RuntimeData[DesigntimeData->serialNo].outputValue.dwData = ~0;
    else RuntimeData[DesigntimeData->serialNo].outputValue.dwData = 0;

	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic Add                                                                 |
| description:  The output is depending on the input Count and input Value                       |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_Add(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	BYTE_T   i;
	WORD_T   *p;
	DBVALUE  Data;

    p = &(DesigntimeData->InputsID[0]);
	Data.dwData = RuntimeData[*p].outputValue.dwData;
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		for( i = 1; i < DesigntimeData->InputCounts; i++)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData =(Data.dwData + RuntimeData[*(p+i)].outputValue.dwData);
			Data.dwData =  RuntimeData[DesigntimeData->serialNo].outputValue.dwData;
		}
		RuntimeData[DesigntimeData->serialNo].DataType = 2;
	}

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic Minus                                                               |
| description:  The output is depending on the inputs value,the first input Value is must larger |
|               than or equal the second input value                                          |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_Minus(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;
    p = &(DesigntimeData->InputsID[0]);

	RuntimeData[DesigntimeData->serialNo].outputValue.dwData =(RuntimeData[*p].outputValue.dwData - \
										  RuntimeData[*(p+1)].outputValue.dwData);
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic Multiply                                                            |
| description:  The output is depending on the input count and the inputs value                  |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_Multiply(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	BYTE_T   i;
	WORD_T   *p;
	DBVALUE  Data;

    p = &(DesigntimeData->InputsID[0]);
	Data.dwData = RuntimeData[*p].outputValue.dwData;
    for( i = 1; i < DesigntimeData->InputCounts; i++)
	{
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData =(Data.dwData*RuntimeData[*(p+i)].outputValue.dwData);
		Data.dwData =  RuntimeData[DesigntimeData->serialNo].outputValue.dwData;
	}
    RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic Devide                                                             |
| description:  The output is depending on the inputs Value                                     |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_Devide(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;
    p = &(DesigntimeData->InputsID[0]);

    RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[*p].outputValue.dwData/RuntimeData[*(p+1)].outputValue.dwData;
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic And                                                             |
| description:  The output is depending on the inputs Value                                     |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_And(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	BYTE_T   i;
	DBVALUE  Data;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    Data.dwData = RuntimeData[*p].outputValue.dwData;
    if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		for( i = 1; i < DesigntimeData->InputCounts; i++)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (Data.dwData & RuntimeData[*(p+i)].outputValue.dwData);
			Data.dwData =  RuntimeData[DesigntimeData->serialNo].outputValue.dwData;
		}
	}
    RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic Or                                                             |
| description:  The output is depending on the inputs Value                                     |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_Or(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	DBVALUE   Data;
	BYTE_T   i;
	WORD_T   *p;

    p = &(DesigntimeData->InputsID[0]);
    Data.dwData = RuntimeData[*p].outputValue.dwData;
	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
		for( i = 1; i < DesigntimeData->InputCounts; i++)
		{
			RuntimeData[DesigntimeData->serialNo].outputValue.dwData =(Data.dwData | RuntimeData[*(p+i)].outputValue.dwData);
			Data.dwData =  RuntimeData[DesigntimeData->serialNo].outputValue.dwData;
		}
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic Shift left                                                         |
| description:  The output is depending on the inputs Value                                     |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_ShiftLeft(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;
    p = &(DesigntimeData->InputsID[0]);

    RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[*p].outputValue.dwData << DesigntimeData->PrivateParam;
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}

/*--------------------------------------------------------------------------------------------+
| Name:         Arithmetic Shift right                                                         |
| description:  The output is depending on the inputs Value                                     |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Arithmetic_ShiftRight(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T   *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T   *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T   *p;
    p = &(DesigntimeData->InputsID[0]);

    RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[*p].outputValue.dwData >> DesigntimeData->PrivateParam;
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
