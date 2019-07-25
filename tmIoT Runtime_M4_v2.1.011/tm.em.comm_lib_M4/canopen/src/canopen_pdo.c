/*===========================================================================+
|  Function :                                                                |
|  Task     :  Source File                                                   |
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
static CANOPEN_BOOL_T Canopen_Pdo_Build(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_BYTE_T          byID,
        CANOPEN_BYTE_T          *pbyData,
        CANOPEN_BYTE_T          *pbySize);
static CANOPEN_LINK_T* Canopen_Pdo_FindLink(
        CANOPEN_T               *pCanopen,
        CANOPEN_DEV_FRAME_T     *pFrame,
        CANOPEN_BYTE_T          *pbyID);
static CANOPEN_BOOL_T Canopen_Pdo_FindSameCombID(CANOPEN_T *pCanopen, CANOPEN_DWORD_T dwCombID);
static void Canopen_Pdo_SendTimerFunc(CANOPEN_TIMER_T *pTimer);
static void Canopen_Pdo_RecvTimerFunc(CANOPEN_TIMER_T *pTimer);

/*===========================================================================+
|           Inner Globle Variable                                            |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_Pdo_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        CANOPEN_BYTE_T i;

        for (i = 0; i < CANOPEN_MAXPDO_ONENODE; i++) {
                Canopen_Node_Init(&(pLink->Pdo.aSendPdo[i].Timer.Node));
                Canopen_Node_Init(&(pLink->Pdo.aRecvPdo[i].Timer.Node));
        }
}

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_Pdo_Start(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        CANOPEN_BYTE_T i;

        if (!pCanopen->State.CommState.bPdo)
                return;

        for (i = 0; i < CANOPEN_MAXPDO_ONENODE; i++) {
                // Configure send pdo
                if (pLink->Pdo.aSendPdo[i].Map.bySubIndex0 != 0) {
                        if (!Canopen_Pdo_FindSameCombID( pCanopen, pLink->Pdo.aSendPdo[i].Para.dwCombID)) {
                                pLink->Pdo.aSendPdo[i].bUse = CANOPEN_TRUE;
                                if ( pLink->Pdo.aSendPdo[i].Para.byTransType == 0x00FE) {
                                        Canopen_Timer_Create(
                                                        &pLink->Pdo.aSendPdo[i].Timer,
                                                        pCanopen,
                                                        pLink,
                                                        i,
                                                        pLink->Pdo.aSendPdo[i].Para.wEventTime,
                                                        Canopen_Pdo_SendTimerFunc);
                                }
                        }
                }

                // Configure recive pdo
                if (pLink->Pdo.aRecvPdo[i].Map.bySubIndex0 != 0) {
                        pLink->Pdo.aRecvPdo[i].bUse = CANOPEN_TRUE;
                        Canopen_Timer_Create(
                                        &pLink->Pdo.aRecvPdo[i].Timer,
                                        pCanopen,
                                        pLink,
                                        i,
                                        pLink->Pdo.aRecvPdo[i].Para.wEventTime,
                                        Canopen_Pdo_RecvTimerFunc);
                }
        }
}

void Canopen_Pdo_Stop(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        CANOPEN_BYTE_T i;

        // Configure send pdo
        for (i = 0; i < CANOPEN_MAXPDO_ONENODE; i++) {
                Canopen_Timer_Delete(&pLink->Pdo.aSendPdo[i].Timer);
                Canopen_Timer_Delete(&pLink->Pdo.aRecvPdo[i].Timer);

                pLink->Pdo.aSendPdo[i].bUse = CANOPEN_FALSE;
                pLink->Pdo.aRecvPdo[i].bUse = CANOPEN_FALSE;
        }
}

void Canopen_Pdo_Send(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink, CANOPEN_BYTE_T byID)
{
        CANOPEN_BYTE_T bySize;
        CANOPEN_DEV_FRAME_T CanFrame;

        CANOPEN_ASSERT(pLink);
        CANOPEN_ASSERT(pCanopen);
        CANOPEN_ASSERT(byID < 4);

        if (pLink->Pdo.aSendPdo[byID].bUse != CANOPEN_TRUE)
                return;

        Canopen_Memset(&CanFrame, 0, sizeof(CanFrame));

        if (!Canopen_Pdo_Build(pCanopen, pLink, byID, (CANOPEN_BYTE_T *)&CanFrame.CanData, &bySize))
                return;

        CanFrame.Msg.Bit.wStdID = pLink->Pdo.aSendPdo[byID].Para.dwCombID;

        CanFrame.wLength = bySize;

        Canopen_WriteDev(pCanopen, (CANOPEN_BYTE_T *)&CanFrame, 0, sizeof(CANOPEN_DEV_FRAME_T));
}


void Canopen_Pdo_Recv(CANOPEN_T *pCanopen, CANOPEN_DEV_FRAME_T *pFrame)
{
        CANOPEN_BYTE_T i, bySize, byOffset = 0;
        CANOPEN_LINK_T *pLink;
        CANOPEN_BYTE_T byID;
        CANOPEN_BYTE_T *pbyData = (CANOPEN_BYTE_T *)&(pFrame->CanData);

        pLink = Canopen_Pdo_FindLink(pCanopen, pFrame, &byID);

        if (pLink == CANOPEN_NULL || pLink->Pdo.aRecvPdo[byID].bUse != CANOPEN_TRUE)
                return;

        for ( i = 0; i < pLink->Pdo.aRecvPdo[byID].Map.bySubIndex0; i++) {

                if (CANOPEN_ERR_NONE != Canopen_OD_Write(
                                                pCanopen,
                                                pLink->pObjdict,
                                                pLink->byNodeID,
                                                (pLink->Pdo.aRecvPdo[byID].Map.adwEntries[i] >> 16) & 0xFFFF,
                                                (pLink->Pdo.aRecvPdo[byID].Map.adwEntries[i] >> 8) & 0xFF,
                                                &bySize,
                                                &(pbyData[byOffset])))
                        return;
                byOffset += bySize;
        }
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
static CANOPEN_BOOL_T Canopen_Pdo_Build(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_BYTE_T          byID,
        CANOPEN_BYTE_T          *pbyData,
        CANOPEN_BYTE_T          *pbySize)
{
        CANOPEN_BYTE_T i;
        CANOPEN_WORD_T wIndex;
        CANOPEN_BYTE_T bySubIndex;
        CANOPEN_BYTE_T bySize;
        CANOPEN_BYTE_T byOffset = 0;

        for ( i = 0; i < pLink->Pdo.aSendPdo[byID].Map.bySubIndex0; i++) {
                wIndex      =   pLink->Pdo.aSendPdo[byID].Map.adwEntries[i] >> 16;
                bySubIndex  =   (pLink->Pdo.aSendPdo[byID].Map.adwEntries[i] >> 8) & 0xFF;


                if (CANOPEN_ERR_NONE != Canopen_OD_Read(
                                                pCanopen,
                                                pLink->pObjdict,
                                                wIndex,
                                                bySubIndex,
                                                &bySize,
                                                &pbyData[byOffset]))
                        return CANOPEN_FALSE;

                byOffset += bySize;
        }

        *pbySize = byOffset;

        return CANOPEN_TRUE;
}

static CANOPEN_LINK_T* Canopen_Pdo_FindLink(
        CANOPEN_T               *pCanopen,
        CANOPEN_DEV_FRAME_T     *pFrame,
        CANOPEN_BYTE_T          *pbyID)
{
        CANOPEN_BYTE_T i;
        CANOPEN_NODE_T *pNode;
        CANOPEN_LINK_T *pLink;

        for (pNode = pCanopen->LinkList.pNext; pNode != &(pCanopen->LinkList); pNode = pNode->pNext) {
                pLink = Canopen_Node_Entry(pNode, CANOPEN_LINK_T, Node);

                for (i = 0; i < CANOPEN_MAXPDO_ONENODE; i++) {
                        if (pLink->Pdo.aRecvPdo[i].Para.dwCombID == pFrame->Msg.Bit.wStdID) {
                                *pbyID = i;
                                return pLink;
                        }
                }
        }

        return  CANOPEN_NULL;
}

static CANOPEN_BOOL_T Canopen_Pdo_FindSameCombID(CANOPEN_T *pCanopen, CANOPEN_DWORD_T dwCombID)
{
        CANOPEN_BYTE_T i;
        CANOPEN_LINK_T *pLink;
        CANOPEN_NODE_T *pNode;

        for (pNode = pCanopen->LinkList.pNext; pNode != &(pCanopen->LinkList); pNode = pNode->pNext) {
                pLink = Canopen_Node_Entry(pNode, CANOPEN_LINK_T, Node);
                for (i = 0; i < 4; i++) {
                        if (pLink->Pdo.aSendPdo[i].Para.dwCombID == dwCombID && pLink->Pdo.aSendPdo[i].bUse == CANOPEN_TRUE)
                                return CANOPEN_TRUE;
                }
        }

        return CANOPEN_FALSE;
}

static void Canopen_Pdo_SendTimerFunc(CANOPEN_TIMER_T *pTimer)
{
        CANOPEN_ASSERT(pTimer);
        CANOPEN_ASSERT(pTimer->pCanopen);
        CANOPEN_ASSERT(pTimer->pLink);

        Canopen_Pdo_Send(pTimer->pCanopen, pTimer->pLink, pTimer->byID);
}

static void Canopen_Pdo_RecvTimerFunc(CANOPEN_TIMER_T *pTimer)
{

}
