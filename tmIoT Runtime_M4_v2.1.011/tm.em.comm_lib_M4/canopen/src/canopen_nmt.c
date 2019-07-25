/*===========================================================================+
|  Function : Canopen Nmt                                                    |
|  Task     : Canopen Nmt Source File                                        |
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

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_SIZE_T CanMaster_Nmt_SendStateChange(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink, CANOPEN_BYTE_T byCs)
{
        CANOPEN_DEV_FRAME_T Frame;

        Canopen_Memset(&Frame, 0, sizeof(Frame));

        Frame.Msg.dwAll         = 0;
        Frame.CanData.awData[0] = byCs | (pLink->byNodeID << 8);
        Frame.wLength           = 2;

        Canopen_WriteDev(pCanopen, (CANOPEN_BYTE_T *)&Frame, 0, sizeof(CANOPEN_DEV_FRAME_T));

        return      0;
}

CANOPEN_SIZE_T CanMaster_Nmt_SendNodeGuard(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        CANOPEN_DEV_FRAME_T Frame;

        Canopen_Memset (&Frame, 0, sizeof(Frame));

        Frame.Msg.Bit.wStdID    = CANOPEN_COMBID_NMTE | pLink->byNodeID;
        Frame.Msg.Bit.wRTR      = 1;
        Frame.wLength           = 0;

        Canopen_WriteDev(pCanopen, (CANOPEN_BYTE_T *)&Frame, 0, sizeof(CANOPEN_DEV_FRAME_T));
        return 0;
}

CANOPEN_SIZE_T CanMaster_Nmt_ReqAllNode(CANOPEN_T *pCanopen)
{
        return CanMaster_Nmt_SendNodeGuard(pCanopen, 0);
}

CANOPEN_SIZE_T CanSalve_Nmt_SendBootup(CANOPEN_T *pCanopen)
{
        CANOPEN_DEV_FRAME_T Frame;

        Canopen_Memset (&Frame, 0, sizeof(Frame));

        Frame.Msg.Bit.wStdID    = CANOPEN_COMBID_NMTE | pCanopen->byNodeID;
        Frame.CanData.awData[0] = 0;
        Frame.wLength           = 1;

        Canopen_WriteDev(pCanopen, (CANOPEN_BYTE_T *)&Frame, 0, sizeof(CANOPEN_DEV_FRAME_T));

        return 0;
}

void CanSlave_Nmt_Recv(CANOPEN_T *pCanOpen, CANOPEN_DEV_FRAME_T*pFrame)
{
        CANOPEN_BYTE_T byNodeID;
        CANOPEN_BYTE_T byCs;

        byNodeID  = (pFrame->CanData.awData[0] >> 8) & 0xFF;
        byCs      = pFrame->CanData.awData[0] & 0xFF;

        if (byNodeID == 0 || byNodeID == pCanOpen->byNodeID) {
                switch (byCs) {
                case CANOPEN_NMT_CMD_START_NODE:
                        if (pCanOpen->State.wNodeState == CANOPEN_STATE_STOPPED ||
                            pCanOpen->State.wNodeState == CANOPEN_STATE_PREOPERATIONAL) {
                                CANOPEN_LINK_T *pLink;
                                pLink = Canopen_GetLink(pCanOpen, byNodeID);
                                Canopen_State_Set(pCanOpen, CANOPEN_STATE_OPERATIONAL);
                                Canopen_Pdo_Start(pCanOpen, pLink);
                        }
                        break;
                case CANOPEN_NMT_CMD_STOP_NODE:
                        if (pCanOpen->State.wNodeState == CANOPEN_STATE_OPERATIONAL ||
                            pCanOpen->State.wNodeState == CANOPEN_STATE_PREOPERATIONAL) {
                                Canopen_State_Set(pCanOpen, CANOPEN_STATE_STOPPED);
                        }
                        break;
                case CANOPEN_NMT_CMD_ENTER_PREOPERATIONAL:
                        if (pCanOpen->State.wNodeState == CANOPEN_STATE_OPERATIONAL ||
                            pCanOpen->State.wNodeState == CANOPEN_STATE_STOPPED ||
                            pCanOpen->State.wNodeState == CANOPEN_STATE_INITIALISATION) {
                                Canopen_State_Set(pCanOpen, CANOPEN_STATE_PREOPERATIONAL);
                        }
                        break;
                case CANOPEN_NMT_CMD_RESET_NODE:
                        if (pCanOpen->State.ResetNode)
                                pCanOpen->State.ResetNode(pCanOpen);
                        CanSalve_Nmt_SendBootup(pCanOpen);
                        Canopen_State_Set(pCanOpen, CANOPEN_STATE_PREOPERATIONAL);
                        break;
                case CANOPEN_NMT_CMD_RESET_COMM:
                        if (pCanOpen->State.ResetComm)
                                pCanOpen->State.ResetComm(pCanOpen);
                        CanSalve_Nmt_SendBootup(pCanOpen);
                        Canopen_State_Set(pCanOpen, CANOPEN_STATE_PREOPERATIONAL);
                        break;
                }
        }
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
