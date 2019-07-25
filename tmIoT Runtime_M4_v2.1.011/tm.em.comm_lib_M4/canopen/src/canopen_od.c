/*===========================================================================+
|  Function : Canopen object dictionary                                      |
|  Task     : Canopen object dictionary Source File                          |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   :                                                                |
|  Version  : V1.00                                                          |
|  Creation : 2016.3.5                                                       |
|  Revision :                                                                |
+===========================================================================*/
#include "canopen/include/canopen.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/

/*===========================================================================+
|           Globle Variables                                                 |
+===========================================================================*/

/*===========================================================================+
|           Class declaration                                                |
+===========================================================================*/

/*===========================================================================+
|           Helpers Implementation                                           |
+===========================================================================*/

/*===========================================================================+
|           Inner Globle Variable                                            |
+===========================================================================*/
const CANOPEN_BYTE_T u_byDatatypeSize[] = {
        0,
        2,      // OD_DATATYPE_BOOL
        1,      // OD_DATATYPE_INT8
        2,      // OD_DATATYPE_INT16
        4,      // OD_DATATYPE_INT32
        1,      // OD_DATATYPE_UINT8
        2,      // OD_DATATYPE_UINT16
        4,      // OD_DATATYPE_UINT32
        4       // OD_DATATYPE_REAL32
};

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_DWORD_T Canopen_OD_Read(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf)
{
        CANOPEN_LONG_T lOffset;

        //+++++
        //  Debug
        //+++++
        CANOPEN_ASSERT(pObjDict);
        CANOPEN_ASSERT(pBuf);
        //+++++
        //  get index
        //+++++
        lOffset = Canopen_OD_IndexToOffset(pObjDict, wIndex, bySubIndex);

        return Canopen_OD_ReadByIndex(pCanopen, pObjDict, lOffset, pbySize, pBuf);
}

CANOPEN_DWORD_T Canopen_OD_Write(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf)
{
        CANOPEN_LONG_T lOffset;

        //+++++
        //  Debug
        //+++++
        CANOPEN_ASSERT ( pCanopen );
        CANOPEN_ASSERT ( pBuf   );
        //+++++
        //  get index
        //+++++
        lOffset = Canopen_OD_IndexToOffset(pObjDict, wIndex, bySubIndex);

        return Canopen_OD_WriteByIndex(pCanopen, pObjDict, byNodeID, lOffset, pbySize, pBuf);
}

CANOPEN_LONG_T Canopen_OD_IndexToOffset(
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex)
{
        CANOPEN_WORD_T  wMin, wMax, wMid;
        CANOPEN_DWORD_T dwIndex = ((CANOPEN_DWORD_T)wIndex << 8) | bySubIndex;
        CANOPEN_DWORD_T dwRealIndex;

        wMax = pObjDict->wNumber - 1;

        dwRealIndex = pObjDict->pIndex[wMax].dwIndex;

        if (dwIndex <= dwRealIndex) {
                wMin = 0;
                while (wMin <= wMax) {
                        wMid = (wMin + wMax) >> 1;

                        dwRealIndex = pObjDict->pIndex[wMid].dwIndex;
                        if(dwIndex == dwRealIndex)
                                return wMid;

                        if(dwIndex > dwRealIndex )
                                wMin = wMid + 1;
                        else
                                wMax = wMid - 1;
                }
        }

        return  -1;
}

CANOPEN_DWORD_T Canopen_OD_ReadByIndex(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_LONG_T          lOffset,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf)
{
        const CANOPEN_OD_INDEX_T *pIndex;

        if (lOffset  ==  -1 || lOffset >= pObjDict->wNumber)
                return CANOPEN_ERR_OD_NO_SUCH_OBJECT;

        pIndex = &(pObjDict->pIndex[lOffset]);

        //+++++
        //  Check Access
        //+++++
//        if (pIndex->byAccess == OD_ACCESS_WO)
//                return CANOPEN_ERR_OD_READ_NOT_ALLOWED;

        //+++++
        //  Copy value
        //+++++
        Canopen_Memcpy(pBuf, &(pObjDict->pValue[lOffset].DbValue), u_byDatatypeSize[pIndex->byDataType]);

        *pbySize = u_byDatatypeSize[pIndex->byDataType];

        return CANOPEN_ERR_NONE;
}

CANOPEN_DWORD_T Canopen_OD_WriteByIndex(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_LONG_T          lOffset,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf)
{
        const CANOPEN_OD_INDEX_T *pIndex;

        if (lOffset == -1 || lOffset >= pObjDict->wNumber )
                return  CANOPEN_ERR_OD_NO_SUCH_OBJECT;

        pIndex = &(pObjDict->pIndex[lOffset]);

        //+++++
        //  Check Access
        //+++++
        if (pIndex->byAccess == OD_ACCESS_RO)
                return CANOPEN_ERR_OD_WRITE_NOT_ALLOWED;

        //+++++
        //  Copy value
        //+++++
        Canopen_Memcpy(&(pObjDict->pValue[lOffset].DbValue), pBuf, u_byDatatypeSize[pIndex->byDataType]);

        // Call back
        if (pIndex->WriteFunc)
                pIndex->WriteFunc(byNodeID, lOffset);

        *pbySize = u_byDatatypeSize[pIndex->byDataType];

        return CANOPEN_ERR_NONE;
}

void Canopen_Od_SetAddrbyIndex(
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_LONG_T          lOffset,
        CANOPEN_BOOL_T          bUpdate,
        CANOPEN_BYTE_T          byUpDown)
{
        if (lOffset  ==  -1 || lOffset >= pObjDict->wNumber)
                return;

        pObjDict->pValue[lOffset].bUpdate  = bUpdate;
        pObjDict->pValue[lOffset].bDirty   = bUpdate;
        pObjDict->pValue[lOffset].byUpDown = byUpDown;
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
