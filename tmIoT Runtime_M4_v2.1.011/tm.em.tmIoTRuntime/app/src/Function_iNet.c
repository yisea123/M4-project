/*==========================================================================+
|  File     : Function_iNet.c                                               |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : ZZYuan                                                        |
|  Version  : v1.00                                                         |
|  Creation : 2018Äê11ÔÂ05ÈÕ                                                                                                                                                                               |
|  Revision :                                                               |
+==========================================================================*/
#include "Function_iNet.h"

/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
BOOL_T  Funcblock_iNet_In(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    WORD i;
    WORD Offset = 0;
    RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
    DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
    WORD StartOffset = DesigntimeData->PrivateParam&0xFFFF;
    WORD EndOffset = StartOffset+(DesigntimeData->PrivateParam>>16)-1;
    for(Offset=StartOffset; Offset<=EndOffset; Offset++)
    {
       for(i=0; i<iNetODParaTab.wODNum; i++)
       {
           if(g_ALLCommDataList[Offset].DataID==iNetODParaTab.pODList[i].dwTableID)
           {
               memcpy(&g_ALLCommDataList[Offset].Value, &iNetODParaTab.pODList[i].Value, sizeof(DBVALUE));
               break;
           }
       }
    }
    RuntimeData[DesigntimeData->serialNo].DataType = 8;

    RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[StartOffset];
    return true;
}
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
BOOL_T  Funcblock_iNet_Out(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    WORD i;
    WORD Offset = 0;
    WORD_T   *p;
    DWORD InValue;
    RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
    DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

    WORD StartOffset=DesigntimeData->PrivateParam&0xFFFF;
    WORD EndOffset = StartOffset+(DesigntimeData->PrivateParam>>16)-1;

    p = &(DesigntimeData->InputsID[0]);
    COMMDATATYPE_T *set = NULL;
    set = (COMMDATATYPE_T*)(RuntimeData[*p].outputValue.dwData);
    for(Offset=StartOffset; Offset<=EndOffset; Offset++)
    {
        for(i=0; i<iNetODParaTab.wODNum; i++)
        {
            if(g_ALLCommDataList[Offset].DataID == iNetODParaTab.pODList[i].dwTableID)
            {
                if(RuntimeData[*p].DataType!=8)
                {
                    InValue = RuntimeData[*p].outputValue.dwData;
                }
                else if(RuntimeData[*p].DataType==8)
                {
                    InValue = (set++)->Value.dwData;
                }
                switch(iNetODParaTab.pODList[i].wiNetDataType)
                {
                    case DATATYPE_Byte:
                        if(iNetODParaTab.pODList[i].Value.byData != InValue)
                            iNetODParaTab.pODList[i].Value.byData = InValue;
                        break;
                    case DATATYPE_WORD:
                        if(iNetODParaTab.pODList[i].Value.wData != InValue)
                            iNetODParaTab.pODList[i].Value.wData  = InValue;
                        break;
                    case DATATYPE_DWORD:
                        if(iNetODParaTab.pODList[i].Value.dwData != InValue)
                            iNetODParaTab.pODList[i].Value.dwData = InValue;
                        break;
                    case DATATYPE_Int16:
                        if(iNetODParaTab.pODList[i].Value.sData != InValue)
                            iNetODParaTab.pODList[i].Value.sData  = InValue;
                        break;
                    case DATATYPE_Int32:
                        if(iNetODParaTab.pODList[i].Value.lData != InValue)
                            iNetODParaTab.pODList[i].Value.lData  = InValue;
                        break;
                    case DATATYPE_Float:
                        if(iNetODParaTab.pODList[i].Value.fData != InValue)
                            iNetODParaTab.pODList[i].Value.fData  = InValue;
                        break;
                    default:
                        if(iNetODParaTab.pODList[i].Value.dwData != InValue)
                            iNetODParaTab.pODList[i].Value.dwData = InValue;
                        break;
                }
            }
        }
    }
    return true;
}





