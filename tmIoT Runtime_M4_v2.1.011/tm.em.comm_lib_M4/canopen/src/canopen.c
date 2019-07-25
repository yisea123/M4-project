/*===========================================================================+
|  Function : Canopen                                                        |
|  Task     : Canopen Source File                                            |
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
|           Class declaration - Canopen                                      |
+===========================================================================*/

/*===========================================================================+
|           Helpers Implementation                                           |
+===========================================================================*/
static void Canopen_Dispatch(CANOPEN_T *pCanopen, CANOPEN_DEV_FRAME_T *pFrame);

/*===========================================================================+
|           Inner Globle Variable                                            |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_Create(CANOPEN_T *pCanopen)
{
        Canopen_Timer_Init(pCanopen);
        Canopen_Node_Init(&(pCanopen->LinkList));
        Canopen_State_Init(pCanopen);
}

void Canopen_Link_Create(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink, CANOPEN_BYTE_T byNodeID, CANOPEN_OD_T *pObjDict)
{
        pLink->byStatus  = CANOPEN_LINK_STATUS_INTIAL;
        pLink->byNodeID  = byNodeID;
        pLink->pObjdict  = pObjDict;
        pLink->wCurIndex = 0;

        Canopen_Pdo_Init(pCanopen, pLink);
        Canopen_Sdo_Init(pCanopen, pLink);
        Canopen_LifeGrd_Init(pCanopen, pLink);

        Canopen_Node_InsertBefore(&(pCanopen->LinkList), &(pLink->Node));

        if (pCanopen->byCliServ == CANOPEN_SERVER) {
                CanSalve_Nmt_SendBootup(pCanopen);
                Canopen_State_Set(pCanopen, CANOPEN_STATE_PREOPERATIONAL);
        }
}

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_LINK_T *Canopen_GetLink(CANOPEN_T *pCanopen, CANOPEN_BYTE_T byNodeID)
{
        CANOPEN_NODE_T *pNode;
        CANOPEN_LINK_T *pLink;

        for (pNode = pCanopen->LinkList.pNext; pNode != &(pCanopen->LinkList); pNode = pNode->pNext) {
                pLink = Canopen_Node_Entry(pNode, CANOPEN_LINK_T, Node);
                if (pLink->byNodeID == byNodeID) {
                        return pLink;
                }
        }

        return CANOPEN_NULL;
}

void Canopen_Run(CANOPEN_T *pCanopen)
{
        CANOPEN_DEV_FRAME_T     RxCanFrame;
        CANOPEN_LINK_T          *pLink;
        CANOPEN_SDO_COMM_T      *pComm;
        CANOPEN_NODE_T          *pNode;
        CANOPEN_LONG_T          lOffset;

        while (Canopen_ReadDev(pCanopen, (CANOPEN_BYTE_T *)&RxCanFrame, 0, sizeof(CANOPEN_DEV_FRAME_T)) != 0) {
                Canopen_Dispatch(pCanopen, &RxCanFrame);
        }

        if (pCanopen->byCliServ == CANOPEN_CLIENT) {
                for (pNode = pCanopen->LinkList.pNext; pNode != &(pCanopen->LinkList); pNode = pNode->pNext) {
                        pLink = Canopen_Node_Entry(pNode, CANOPEN_LINK_T, Node);
                        // wyl 2018.5.10
                        if (pLink->Sdo.byState == CANOPEN_SDO_STATE_ABORTED_INTERNAL)
                        {
                              	pLink->byStatus = CANOPEN_LINK_STATUS_NORMAL;//CANOPEN_LINK_STATUS_INTIAL;
								pLink->Sdo.byState  =   CANOPEN_SDO_STATE_FINISHED;
								pLink->byNmtState   =   CANOPEN_STATE_OPERATIONAL;//CANOPEN_STATE_PREOPERATIONAL;
                        }
                        switch (pLink->byStatus) {
                        case CANOPEN_LINK_STATUS_INTIAL:
                                pLink->byStatus = CANOPEN_LINK_STATUS_CONFIG;
                                // Set canopen state to preoperational
                                Canopen_State_Set(pCanopen, CANOPEN_STATE_PREOPERATIONAL);
                                CanMaster_Nmt_SendStateChange(pCanopen, pLink, CANOPEN_NMT_CMD_ENTER_PREOPERATIONAL);
                                pLink->wCurIndex                = 0;
                                pLink->Sdo.ConfComm.wCurIndex   = 0;
                                pLink->Sdo.NormalComm.wCurIndex = 0;
                                break;
                        case CANOPEN_LINK_STATUS_CONFIG:        // just client
                                pComm = &(pLink->Sdo.ConfComm);
                                if (pLink->Sdo.byState == CANOPEN_SDO_STATE_FINISHED) {
                                        if (pLink->byNmtState != CANOPEN_STATE_PREOPERATIONAL) {
                                                CanMaster_Nmt_SendStateChange(pCanopen, pLink, CANOPEN_NMT_CMD_ENTER_PREOPERATIONAL);
                                        }
                                        if (pComm->wCurIndex >= pComm->wMaxIndex) {
                                                Canopen_State_Set(pCanopen, CANOPEN_STATE_OPERATIONAL);
                                                Canopen_Pdo_Start(pCanopen, pLink);
                                                pLink->byStatus = CANOPEN_LINK_STATUS_NORMAL;
                                                pComm->wCurIndex = 0;
                                                break;
                                        }
                                        if (pComm->pTable[pComm->wCurIndex].byUpDown == CANOPEN_SDO_MODE_DOWNLOAD) {
                                                Canopen_Sdo_DownLoad(
                                                                pCanopen,
                                                                pLink,
                                                                pComm->pTable[pComm->wCurIndex].wIndex,
                                                                pComm->pTable[pComm->wCurIndex].bySubIndex,
                                                                &pComm->pTable[pComm->wCurIndex].dwValue,
                                                                CANOPEN_FALSE);
                                        } else if (pComm->pTable[pComm->wCurIndex].byUpDown == CANOPEN_SDO_MODE_UPLOAD) {
                                                Canopen_Sdo_UpLoad(
                                                                pCanopen,
                                                                pLink,
                                                                pComm->pTable[pComm->wCurIndex].wIndex,
                                                                pComm->pTable[pComm->wCurIndex].bySubIndex,
                                                                CANOPEN_FALSE);
                                        }
                                        pComm->wCurIndex++;
                                }

                                break;
                        case CANOPEN_LINK_STATUS_NORMAL:        // just client
                                pComm = &(pLink->Sdo.NormalComm);
                                if (pLink->byNmtState != CANOPEN_STATE_OPERATIONAL) {
                                        CanMaster_Nmt_SendStateChange(
                                                                pCanopen,
                                                                pLink,
                                                                CANOPEN_NMT_CMD_START_NODE);
                                }
                                if(pLink->Sdo.byState == CANOPEN_SDO_STATE_FINISHED || pLink->Sdo.byState == CANOPEN_SDO_STATE_RESET) {
                                        if (pLink->wCurIndex >= pLink->pObjdict->wNumber) {
                                                pLink->wCurIndex    =   0;
                                                if (pComm->pTable != CANOPEN_NULL) {
                                                        pComm->wCurIndex = 0;
                                                        // Update object dictionary flag
                                                        while (pComm->wCurIndex < pComm->wMaxIndex ) {
                                                                // Period = 0, just operate once; period > 0, operate periodicly, period < 0 , not operate
                                                                if (pComm->pTable[pComm->wCurIndex].nPeriod >= 0 &&
                                                                        Canopen_GetSysTick() - pComm->pTable[pComm->wCurIndex].dwTimeoutTick < CANOPEN_HALF_MAX_TIME) {
                                                                        lOffset = Canopen_OD_IndexToOffset(
                                                                                                        pLink->pObjdict,
                                                                                                        pComm->pTable[pComm->wCurIndex].wIndex,
                                                                                                        pComm->pTable[pComm->wCurIndex].bySubIndex);
                                                                        if (lOffset >= 0 && lOffset < pLink->pObjdict->wNumber) {
                                                                                pLink->pObjdict->pValue[lOffset].bUpdate  = CANOPEN_TRUE;
                                                                                pLink->pObjdict->pValue[lOffset].bDirty   = CANOPEN_TRUE;
                                                                                pLink->pObjdict->pValue[lOffset].byUpDown = pComm->pTable[pComm->wCurIndex].byUpDown;
                                                                        }
                                                                        pComm->pTable[pComm->wCurIndex].dwTimeoutTick = Canopen_GetSysTick() + pComm->pTable[pComm->wCurIndex].nPeriod;
                                                                }
                                                                pComm->wCurIndex++;
                                                        }
                                                }
                                        }

                                        while (pLink->wCurIndex < pLink->pObjdict->wNumber) {
                                                if (pLink->pObjdict->pValue[pLink->wCurIndex].bUpdate == CANOPEN_TRUE) {
                                                        // if (pLink->Sdo.byState == CANOPEN_SDO_STATE_FINISHED) {
                                                        //         pLink->pObjdict->pValue[pLink->wCurIndex].bUpdate = CANOPEN_FALSE;
                                                        // }

                                                        if ((pLink->pObjdict->pValue[pLink->wCurIndex].byUpDown & CANOPEN_SDO_MODE_DOWNLOAD) == CANOPEN_SDO_MODE_DOWNLOAD) {
                                                                Canopen_Sdo_DownLoad(
                                                                                pCanopen,
                                                                                pLink,
                                                                                (pLink->pObjdict->pIndex[pLink->wCurIndex].dwIndex >> 8) & 0xFFFF,
                                                                                pLink->pObjdict->pIndex[pLink->wCurIndex].dwIndex & 0xFF,
                                                                                CANOPEN_NULL,
                                                                                CANOPEN_FALSE);
                                                                pLink->pObjdict->pValue[pLink->wCurIndex].byUpDown &= ~(1<<0);
                                                        }
                                                        if ((pLink->pObjdict->pValue[pLink->wCurIndex].byUpDown & CANOPEN_SDO_MODE_UPLOAD) == CANOPEN_SDO_MODE_UPLOAD) {
                                                                Canopen_Sdo_UpLoad(
                                                                                pCanopen,
                                                                                pLink,
                                                                                (pLink->pObjdict->pIndex[pLink->wCurIndex].dwIndex >> 8) & 0xFFFF,
                                                                                pLink->pObjdict->pIndex[pLink->wCurIndex].dwIndex & 0xFF,
                                                                                CANOPEN_FALSE);
                                                                pLink->pObjdict->pValue[pLink->wCurIndex].byUpDown &= ~(1<<1);
                                                        }

                                                        pLink->pObjdict->pValue[pLink->wCurIndex].bUpdate = CANOPEN_FALSE;
                                                        pLink->wCurIndex++;
                                                        break;
                                                }
                                                pLink->wCurIndex++;
                                        }
                                }
                                break;
                        default:
                                break;
                        }
                }
        }
        Canopen_Timer_Run(pCanopen);
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
static void Canopen_Dispatch(CANOPEN_T *pCanopen, CANOPEN_DEV_FRAME_T *pFrame)
{
        CANOPEN_WORD_T wCombID = pFrame->Msg.Bit.wStdID >> 7;

        switch (wCombID) {
        case CANOPEN_FUNCID_NMT:
                if (pCanopen->byCliServ == CANOPEN_SERVER) {
                        CanSlave_Nmt_Recv(pCanopen, pFrame);
                }
                break;
        case CANOPEN_FUNCID_SYNCANDEMERG:
            break;
        case CANOPEN_FUNCID_TIMESTAMP:
            break;
        case CANOPEN_FUNCID_TXPDO1:
        case CANOPEN_FUNCID_RXPDO1:
        case CANOPEN_FUNCID_TXPDO2:
        case CANOPEN_FUNCID_RXPDO2:
        case CANOPEN_FUNCID_TXPDO3:
        case CANOPEN_FUNCID_RXPDO3:
        case CANOPEN_FUNCID_TXPDO4:
        case CANOPEN_FUNCID_RXPDO4:
                if (pCanopen->State.CommState.bPdo) {
                        Canopen_Pdo_Recv(pCanopen, pFrame);
                }
                break;
        case CANOPEN_FUNCID_SDO_S:
        case CANOPEN_FUNCID_SDO_C:
                if (pCanopen->State.CommState.bSdo) {
                        Canopen_Sdo_Recv(pCanopen, pFrame);
                }
                break;
        case CANOPEN_FUNCID_RESERVED1:
                break;
        case CANOPEN_FUNCID_NMTE:
                if ( pCanopen->State.CommState.bLifeGuard ) {
                        Canopen_LifeGrd_Recv(pCanopen, pFrame);
                }
                break;
        default:
            break;
        }
}
