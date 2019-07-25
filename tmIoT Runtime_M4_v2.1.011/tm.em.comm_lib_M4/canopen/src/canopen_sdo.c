/*===========================================================================+
|  Function : Sdo                                                            |
|  Task     : Sdo Source File                                                |
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
typedef union  tagCANOPEN_SDOHEADER CANOPEN_SDOHEADER_T;
typedef struct tagCANOPEN_SDOFRAME CANOPEN_SDOFRAME_T;

union tagCANOPEN_SDOHEADER {
        struct {
                CANOPEN_WORD_T  s               : 1 ;       //whether size of Data in Data Set Size specified
                CANOPEN_WORD_T  e               : 1 ;       //whether Expedited transfer
                CANOPEN_WORD_T  n               : 2 ;       //Data Set Size
                CANOPEN_WORD_T  CompleteAccess  : 1 ;
                CANOPEN_WORD_T  Command         : 3 ;
                CANOPEN_WORD_T  IndexLow        : 8 ;
                CANOPEN_WORD_T  IndexHigh       : 8 ;
                CANOPEN_WORD_T  SubIndex        : 8 ;
        } Bits;

        CANOPEN_DWORD_T dwAll;
} ;

struct tagCANOPEN_SDOFRAME {
        CANOPEN_SDOHEADER_T     Header;
        CANOPEN_DWORD_T         dwValue;
};

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/
#define     Canopen_Sdo_GetToggle(byData)       ((byData>>4) & 1)
#define     Canopen_Sdo_GetN2(byData)           ((byData >> 2) & 3)
#define     Canopen_Sdo_GetN3(byDate)           ((byData >> 1) & 7)
#define     Canopen_Sdo_GetE(byData)            ((byData >> 1) & 1)
#define     Canopen_Sdo_GetS(byData)            (byData & 1)
#define     Canopen_Sdo_GetBlkSubCmd(byData)    (byData & 3)
/*===========================================================================+
|           Globle Variables                                                 |
+===========================================================================*/

/*===========================================================================+
|           Class declaration - Canopen sdo                                  |
+===========================================================================*/

/*===========================================================================+
|           Helpers Implementation                                           |
+===========================================================================*/
static void Canopen_Sdo_InitComm(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          byState);
static void Canopen_Sdo_Send(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_BYTE_T          bySdoMode,
        void                    *pData);
static void Canopen_Sdo_SendAbort(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_DWORD_T         dwAbortCode);
static void Canopen_Sdo_Fail(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_DWORD_T         dwAbortCode);
static void Canopen_Sdo_TimeOut(CANOPEN_TIMER_T *pTimer);

/*===========================================================================+
|           Inner Globle Variable                                            |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_Sdo_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        Canopen_Node_Init(&(pLink->Sdo.Timer.Node));

        Canopen_Sdo_InitComm(pCanopen, pLink, 0, 0, CANOPEN_SDO_STATE_FINISHED);
}

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_Sdo_AddConfComm(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_SDO_COMM_TBL_T  *pTable,
        CANOPEN_WORD_T          wNumber)
{
        pLink->Sdo.ConfComm.wMaxIndex   = wNumber;
        pLink->Sdo.ConfComm.wCurIndex   = 0;
        pLink->Sdo.ConfComm.pTable      = pTable;
}

void Canopen_Sdo_AddNormalComm(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_SDO_COMM_TBL_T  *pTable,
        CANOPEN_WORD_T          wNumber)
{
    pLink->Sdo.NormalComm.wMaxIndex     = wNumber;
    pLink->Sdo.NormalComm.wCurIndex     = 0;
    pLink->Sdo.NormalComm.pTable        = pTable;
}

CANOPEN_ERR_T Canopen_Sdo_DownLoad(
        CANOPEN_T       *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        void                    *pBuf,
        CANOPEN_BOOL_T          bUseBlock)
{
        CANOPEN_SDOFRAME_T      TxSdoFrame;
        CANOPEN_BYTE_T          bySize;

        //+++++
        //+++++
        //  Debug
        CANOPEN_ASSERT(pCanopen);

        if (bUseBlock)
                Canopen_Sdo_InitComm(
                                pCanopen,
                                pLink,
                                wIndex,
                                bySubIndex,
                                CANOPEN_SDO_STATE_BLOCK_DOWNLOAD_IN_PROGRESS);
        else
                Canopen_Sdo_InitComm(
                                pCanopen,
                                pLink,
                                wIndex,
                                bySubIndex,
                                CANOPEN_SDO_STATE_DOWNLOAD_IN_PROGRESS);
        //+++++
        //  if pbuf is not Null, Use this data to send and save to object dictionary, else use object dictionary data to send
        //+++++
        if (pBuf) {
                Canopen_OD_Write(pCanopen, pLink->pObjdict, pLink->byNodeID, wIndex, bySubIndex, &bySize, pBuf);
                if (bySize == 1)
                        *(CANOPEN_WORD_T*)pLink->Sdo.abyData = (*(CANOPEN_WORD_T*)pLink->Sdo.abyData & 0xFF);
                if (bySize == 2)
                        *(CANOPEN_WORD_T*)pLink->Sdo.abyData = (*(CANOPEN_WORD_T*)pLink->Sdo.abyData & 0xFFFF);
                Canopen_Memcpy(pLink->Sdo.abyData, pBuf, bySize);
        } else {
                Canopen_OD_Read(pCanopen, pLink->pObjdict, wIndex, bySubIndex, &bySize, pLink->Sdo.abyData);
        }
        //+++++
        //  Send first data
        //+++++
        if (bUseBlock) {        // Block mode
                return CANOPEN_ERR_SDO_GENERAL_ERROR;
        } else {                // Expedited download
                // CCS = 1, n = 4-wSize, e=1, s=1
        		TxSdoFrame.Header.dwAll = ((CANOPEN_SDO_INITIATE_DOWNLOAD_REQUEST << 5) |
                                          ((4-bySize)<<2) | 3) |
                                          ((CANOPEN_DWORD_T)wIndex << 8) |
                                          ((CANOPEN_DWORD_T)bySubIndex << 24);
                Canopen_Memcpy(&TxSdoFrame.dwValue, pLink->Sdo.abyData, sizeof(CANOPEN_DWORD_T));
        }
        Canopen_Sdo_Send(pCanopen, pLink, CANOPEN_CLIENT, (CANOPEN_BYTE_T *)&TxSdoFrame);

        return  CANOPEN_ERR_NONE;
}

void Canopen_Sdo_UpLoad(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BOOL_T          bUseBlock)
{
        CANOPEN_SDOFRAME_T      TxSdoFrame;

        //+++++
        //  Debug
        //+++++
        CANOPEN_ASSERT(pCanopen);

        //+++++
        //  Init line
        //+++++
        if (bUseBlock)
                Canopen_Sdo_InitComm(
                                pCanopen,
                                pLink,
                                wIndex,
                                bySubIndex,
                                CANOPEN_SDO_STATE_BLOCK_UPLOAD_IN_PROGRESS);
        else
                Canopen_Sdo_InitComm(
                                pCanopen,
                                pLink,
                                wIndex,
                                bySubIndex,
                                CANOPEN_SDO_STATE_UPLOAD_IN_PROGRESS);

        //+++++
        //  Send first data
        //+++++
        if (bUseBlock) {        // Block mode
            return;
        } else {
                TxSdoFrame.Header.dwAll = (CANOPEN_SDO_INITIATE_UPLOAD_REQUEST << 5) |
                                          ((CANOPEN_DWORD_T)wIndex << 8) |
                                          ((CANOPEN_DWORD_T)bySubIndex << 24);
                TxSdoFrame.dwValue = 0;
        }

        Canopen_Sdo_Send(pCanopen, pLink, CANOPEN_CLIENT, &TxSdoFrame);
}

void Canopen_Sdo_Recv(CANOPEN_T *pCanopen, CANOPEN_DEV_FRAME_T *pFrame)
{
        CANOPEN_LINK_T          *pLink;

        CANOPEN_BYTE_T          bySize          = CANOPEN_SDO_MAX_TRANSFERDATA;
        CANOPEN_SDOFRAME_T      TxSdoFrame;
        CANOPEN_SDOFRAME_T      *pRxSdoFrame    = (CANOPEN_SDOFRAME_T *)&pFrame->CanData;
        CANOPEN_DWORD_T         dwErrCode;
        CANOPEN_WORD_T          wIndex;
        CANOPEN_BYTE_T          bySubIndex;
        CANOPEN_SDO_T           *pSdo;
        CANOPEN_BYTE_T          byCs            = pRxSdoFrame->Header.Bits.Command;

        pLink = Canopen_GetLink(pCanopen, pFrame->Msg.Bit.wStdID & 0x7F);

        if (pLink == CANOPEN_NULL)
                return;

        pSdo = &(pLink->Sdo);

        if (pCanopen->byCliServ == CANOPEN_CLIENT) {
                switch (byCs) {
                case CANOPEN_SDO_INITIATE_UPLOAD_RESPONSE:
                        // The Sdo must be in upload operation
                        if (pSdo->byState != CANOPEN_SDO_STATE_UPLOAD_IN_PROGRESS) {
                        		Canopen_Sdo_Fail(pCanopen, pLink, 0, 0, CANOPEN_ERR_SDO_GENERAL_ERROR);
                                return;
                        }
                        // Expedited transfer
                        if (pRxSdoFrame->Header.Bits.e) {
                                //  Delete count
                                Canopen_Timer_Delete(&(pSdo->Timer));
                                //  Copy data to sdo
                                if (((pSdo->dwHeader >> 8) & 0xFFFFFF) == ((pRxSdoFrame->Header.dwAll >> 8) & 0xFFFFFF)) {
                                        Canopen_Memcpy(pSdo->abyData, &(pRxSdoFrame->dwValue), 4/CANOPEN_BYTE_SIZE);
                                        pSdo->byState = CANOPEN_SDO_STATE_FINISHED;
                                        //  write to object dictinary
                                        dwErrCode = Canopen_OD_Write(
                                                                pCanopen,
                                                                pLink->pObjdict,
                                                                pLink->byNodeID,
                                                                pSdo->wIndex,
                                                                pSdo->bySubIndex,
                                                                &bySize,
                                                                &(pRxSdoFrame->dwValue));
                                        if (dwErrCode != CANOPEN_ERR_NONE) {
                                                Canopen_Sdo_Fail(
                                                        pCanopen,
                                                        pLink,
                                                        pSdo->wIndex,
                                                        pSdo->bySubIndex,
                                                        dwErrCode);
                                        }
                                }
                        }
                        break;
                case CANOPEN_SDO_INITIATE_DOWNLOAD_RESPONSE:
                        // The Sdo must be in upload operation
                        if (pSdo->byState != CANOPEN_SDO_STATE_DOWNLOAD_IN_PROGRESS) {
                        		Canopen_Sdo_Fail(pCanopen, pLink, 0, 0, CANOPEN_ERR_SDO_GENERAL_ERROR);
                                return;
                        }
                        if (((pSdo->dwHeader >> 8) & 0xFFFFFF) == ((pRxSdoFrame->Header.dwAll >> 8) & 0xFFFFFF)) {
                                Canopen_Timer_Delete(&(pSdo->Timer));
                                pSdo->byState = CANOPEN_SDO_STATE_FINISHED;
                        }
                        break;
                case CANOPEN_SDO_ABORT_TRANSFER_RESPONSE:
                        Canopen_Memcpy (&dwErrCode, &(pFrame->CanData.adwData[1]), sizeof(CANOPEN_DWORD_T));
                        // Canopen_Timer_Delete(&(pSdo->Timer));
                        // pSdo->byState = CANOPEN_SDO_STATE_ABORTED_RCV;
                        pSdo->dwAbortCode = dwErrCode;
                        break;
                default:
                        break;
                }
        } else {
                switch (byCs) {
                case CANOPEN_SDO_INITIATE_DOWNLOAD_REQUEST:
                        // Get index and sub index
                        wIndex     = pRxSdoFrame->Header.Bits.IndexLow | (pRxSdoFrame->Header.Bits.IndexHigh << 8);
                        bySubIndex = pRxSdoFrame->Header.Bits.SubIndex;
                        Canopen_Sdo_InitComm(
                                        pCanopen,
                                        pLink,
                                        wIndex,
                                        bySubIndex,
                                        CANOPEN_SDO_STATE_DOWNLOAD_IN_PROGRESS);
                        //  Copy data to sdo
                        Canopen_Memcpy(pSdo->abyData, &(pRxSdoFrame->dwValue), sizeof(CANOPEN_DWORD_T));
                        dwErrCode = Canopen_OD_Write(
                                                pCanopen,
                                                pLink->pObjdict,
                                                pLink->byNodeID,
                                                wIndex,
                                                bySubIndex,
                                                &bySize,
                                                pSdo->abyData);
                        if (dwErrCode != CANOPEN_ERR_NONE) {
                                Canopen_Sdo_Fail(
                                        pCanopen,
                                        pLink,
                                        wIndex,
                                        bySubIndex,
                                        dwErrCode);
                                return;
                        }
                        Canopen_Sdo_InitComm(pCanopen, pLink, 0, 0, CANOPEN_SDO_STATE_RESET);
                        // Send resp sdo
                        TxSdoFrame.Header.dwAll = (CANOPEN_SDO_INITIATE_DOWNLOAD_RESPONSE << 5) |
                                                  ((CANOPEN_DWORD_T)wIndex << 8) |
                                                  ((CANOPEN_DWORD_T)bySubIndex << 24);
                         TxSdoFrame.dwValue = 0;
                        Canopen_Sdo_Send(pCanopen, pLink, CANOPEN_SERVER, &TxSdoFrame);
                        break;
                case CANOPEN_SDO_INITIATE_UPLOAD_REQUEST:
                        // Get index and sub index
                        wIndex      = pRxSdoFrame->Header.Bits.IndexLow | (pRxSdoFrame->Header.Bits.IndexHigh << 8);
                        bySubIndex  = pRxSdoFrame->Header.Bits.SubIndex;
                        Canopen_Sdo_InitComm(
                                        pCanopen,
                                        pLink,
                                        wIndex,
                                        bySubIndex,
                                        CANOPEN_SDO_STATE_UPLOAD_IN_PROGRESS);
                        Canopen_Memset(pSdo->abyData, 0, CANOPEN_SDO_MAX_TRANSFERDATA);
                        // Read objdict to sdo
                        dwErrCode = Canopen_OD_Read(
                                                pCanopen,
                                                pLink->pObjdict,
                                                wIndex,
                                                bySubIndex,
                                                &bySize,
                                                pSdo->abyData);
                        if (dwErrCode != CANOPEN_ERR_NONE) {
                                Canopen_Sdo_Fail(
                                        pCanopen,
                                        pLink,
                                        wIndex,
                                        bySubIndex,
                                        dwErrCode);
                                return;
                        }
                        TxSdoFrame.Header.dwAll = ((CANOPEN_SDO_INITIATE_UPLOAD_RESPONSE << 5) |
                                                  ((4 - bySize) << 2) | 3) |
                                                  ((CANOPEN_DWORD_T)wIndex << 8) |
                                                  ((CANOPEN_DWORD_T)bySubIndex << 24);
                        Canopen_Memcpy(&TxSdoFrame.dwValue, pSdo->abyData, sizeof(CANOPEN_DWORD_T));
                        Canopen_Sdo_Send(pCanopen, pLink, CANOPEN_SERVER, &TxSdoFrame);
                        Canopen_Sdo_InitComm(pCanopen, pLink, 0, 0, CANOPEN_SDO_STATE_RESET);
                        break;
                case CANOPEN_SDO_ABORT_TRANSFER_REQUEST:
                        dwErrCode = pRxSdoFrame->dwValue;
                        Canopen_Sdo_InitComm(pCanopen, pLink, 0, 0, CANOPEN_SDO_STATE_RESET);
                        break;
                default:
                        break;
                }
        }
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
static void Canopen_Sdo_InitComm(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          byState)
{
        CANOPEN_SDO_T *pSdo = &(pLink->Sdo);

        if (byState == CANOPEN_SDO_STATE_DOWNLOAD_IN_PROGRESS ||
            byState == CANOPEN_SDO_STATE_UPLOAD_IN_PROGRESS ||
            byState == CANOPEN_SDO_STATE_BLOCK_DOWNLOAD_IN_PROGRESS ||
            byState == CANOPEN_SDO_STATE_BLOCK_UPLOAD_IN_PROGRESS) {
                Canopen_Timer_Create(
                                &(pSdo->Timer),
                                pCanopen,
                                pLink,
                                0,
                                CANOPEN_SDO_RESEND_TIMER,
                                Canopen_Sdo_TimeOut);
        }
        pSdo->wIndex      = wIndex;
        pSdo->bySubIndex  = bySubIndex;
        pSdo->dwAbortCode = CANOPEN_ERR_NONE;
        pSdo->byState     = byState;
}

static void Canopen_Sdo_Send(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_BYTE_T          bySdoMode,
        void                    *pData)
{
        CANOPEN_DEV_FRAME_T CanFrame;
        Canopen_Memset(&CanFrame, 0, sizeof(CanFrame));
        if (bySdoMode == CANOPEN_CLIENT) {
                CanFrame.Msg.Bit.wStdID = 0x600 + pLink->byNodeID;
                Canopen_Memcpy (&(pLink->Sdo.dwHeader), pData, sizeof(CANOPEN_DWORD_T));
        } else if (bySdoMode == CANOPEN_SERVER) {
                CanFrame.Msg.Bit.wStdID = 0x580 + pLink->byNodeID;
        } else {
                return;
        }
        Canopen_Memcpy(&CanFrame.CanData, pData, CANOPEN_SDO_FRAME_SIZE);
        CanFrame.wLength = 8;
        Canopen_WriteDev(
                pCanopen,
                (CANOPEN_BYTE_T *)&CanFrame,
                0,
                sizeof(CANOPEN_DEV_FRAME_T));
}

static void Canopen_Sdo_SendAbort(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_DWORD_T         dwAbortCode)
{
        CANOPEN_DEV_FRAME_T     CanFrame;
        CANOPEN_SDOFRAME_T      *pTxSdoFrame = (CANOPEN_SDOFRAME_T *)&CanFrame.CanData;

        pTxSdoFrame->Header.dwAll = CANOPEN_SDO_SEG_ABORT |
                                    ((CANOPEN_DWORD_T) wIndex << 8) |
                                    ((CANOPEN_DWORD_T) bySubIndex << 24);
        pTxSdoFrame->dwValue = dwAbortCode;
        CanFrame.wLength = 8;

        if (pCanopen->byCliServ == CANOPEN_CLIENT) {
                CanFrame.Msg.Bit.wStdID = 0x600 + pLink->byNodeID;
        } else if (pCanopen->byCliServ == CANOPEN_SERVER) {
                CanFrame.Msg.Bit.wStdID = 0x580 + pLink->byNodeID;
        } else {
                return;
        }

        Canopen_WriteDev(
                pCanopen,
                (CANOPEN_BYTE_T *)&CanFrame,
                0,
                sizeof(CANOPEN_DEV_FRAME_T));
}

static void Canopen_Sdo_Fail(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_DWORD_T         dwAbortCode)
{
        if (pCanopen->byCliServ == CANOPEN_SERVER) {
                Canopen_Sdo_InitComm(pCanopen, pLink , 0, 0, CANOPEN_SDO_STATE_RESET);
        } else {
                Canopen_Timer_Delete(&(pLink->Sdo.Timer));
                pLink->Sdo.byState      = CANOPEN_SDO_STATE_ABORTED_INTERNAL;
                pLink->Sdo.dwAbortCode  = dwAbortCode;
        }

        Canopen_Sdo_SendAbort(pCanopen, pLink, wIndex, bySubIndex, dwAbortCode);
}

static void Canopen_Sdo_TimeOut(CANOPEN_TIMER_T *pTimer)
{
        if (pTimer->pCanopen->byCliServ == CANOPEN_CLIENT) {
                CANOPEN_SDOFRAME_T TxSdoFrame;
                Canopen_Timer_Reset(pTimer);
                Canopen_Memcpy(&TxSdoFrame.Header, &(pTimer->pLink->Sdo.dwHeader), sizeof(CANOPEN_DWORD_T));
                Canopen_Memcpy(&TxSdoFrame.dwValue, pTimer->pLink->Sdo.abyData, sizeof(CANOPEN_DWORD_T));
                Canopen_Sdo_Send(pTimer->pCanopen, pTimer->pLink, CANOPEN_CLIENT, &TxSdoFrame);
        }
}
