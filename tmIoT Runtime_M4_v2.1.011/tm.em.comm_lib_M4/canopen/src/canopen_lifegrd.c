/*===========================================================================+
|  Function : Life guard                                                     |
|  Task     : Life guard Source File                                         |
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
static void Canopen_Heartbeat_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);
static void Canopen_NodeGuard_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);
static void Canopen_Heartbeat_Producer_TimerFunc(CANOPEN_TIMER_T *pTimer);
static void Canopen_Heartbeat_Consumer_TimerFunc(CANOPEN_TIMER_T *pTimer);
static void Canopen_NodeGuard_TimerFunc(CANOPEN_TIMER_T *pTimer);

/*===========================================================================+
|           Inner Globle Variable                                            |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_LifeGrd_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        Canopen_Node_Init(&(pLink->Heartbeat.Timer.Node));
        Canopen_Node_Init(&(pLink->NodeGrd.Timer.Node));

        Canopen_Heartbeat_Init(pCanopen, pLink);
        Canopen_NodeGuard_Init(pCanopen, pLink);
}

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_LifeGrd_Recv(CANOPEN_T *pCanopen, CANOPEN_DEV_FRAME_T *pFrame)
{
        CANOPEN_DEV_FRAME_T     Frame;
        CANOPEN_BYTE_T          byNodeID  = pFrame->Msg.Bit.wStdID&0x7F;
        CANOPEN_LINK_T          *pLink    = Canopen_GetLink(pCanopen, byNodeID);

        if (pLink == CANOPEN_NULL)
                return;

        // Master to slave
        if (pFrame->Msg.Bit.wRTR == 1) {
                if ( pLink != CANOPEN_NULL) {
                        Canopen_Memset (&Frame, 0, sizeof(Frame));
                        Frame.Msg.Bit.wStdID    = CANOPEN_COMBID_NMTE | byNodeID;
                        Frame.wLength           = 1;
                        if (pLink->NodeGrd.byToggle) {
                                Frame.CanData.awData[0] = byNodeID;
                                pLink->NodeGrd.byToggle = 0;
                        } else {
                                Frame.CanData.awData[0] = 0x80 | byNodeID;
                                pLink->NodeGrd.byToggle = 0x1;
                        }
                        Canopen_WriteDev(pCanopen, (CANOPEN_BYTE_T *)&Frame, 0, sizeof(CANOPEN_DEV_FRAME_T));
                }
        } else {       // Slave to Master
                CANOPEN_BYTE_T byNewState;
                byNewState = pFrame->CanData.awData[0] & 0x7F;
                pLink->NodeGrd.sRemainCount = pLink->NodeGrd.byFactor;
                if (pLink->byNmtState != byNewState) {
                        pLink->byNmtState = byNewState;
                }
                if (pLink->byNmtState != CANOPEN_STATE_UNKNOWN) {
                        Canopen_Timer_Reset(&(pLink->Heartbeat.Timer));
                }
        }
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
static void Canopen_Heartbeat_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        if (pCanopen->byCliServ == CANOPEN_SERVER) {
                Canopen_Timer_Delete(&(pLink->Heartbeat.Timer));
                Canopen_Timer_Create(
                                &(pLink->Heartbeat.Timer),
                                pCanopen,
                                pLink,
                                0,
                                pLink->Heartbeat.wTime,
                                Canopen_Heartbeat_Producer_TimerFunc
                        );
        } else {
                Canopen_Timer_Delete(&(pLink->Heartbeat.Timer));
                Canopen_Timer_Create(
                                &(pLink->Heartbeat.Timer),
                                pCanopen,
                                pLink,
                                0,
                                pLink->Heartbeat.wTime,
                                Canopen_Heartbeat_Consumer_TimerFunc
                        );
        }
}

static void Canopen_NodeGuard_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink)
{
        if (pLink->Heartbeat.wTime      != 0 ||
            pLink->NodeGrd.wTime        == 0 ||
            pLink->NodeGrd.byFactor     == 0)
                return;

        pLink->NodeGrd.byToggle = 0;

        Canopen_Timer_Delete(&(pLink->NodeGrd.Timer));
        Canopen_Timer_Create(
                        &(pLink->NodeGrd.Timer),
                        pCanopen,
                        pLink,
                        0,
                        pLink->NodeGrd.wTime,
                        Canopen_NodeGuard_TimerFunc
                );
}

static void Canopen_Heartbeat_Producer_TimerFunc(CANOPEN_TIMER_T *pTimer)
{
        CANOPEN_DEV_FRAME_T Frame;

        Canopen_Memset(&Frame, 0, sizeof(Frame));

        Frame.Msg.Bit.wStdID    = CANOPEN_COMBID_NMTE + pTimer->pCanopen->byNodeID;
        Frame.CanData.awData[0] = pTimer->pCanopen->State.wNodeState;
        Frame.wLength           = 1;

        Canopen_WriteDev(pTimer->pCanopen, (CANOPEN_BYTE_T *)&Frame, 0, sizeof(CANOPEN_DEV_FRAME_T));
}

static void Canopen_Heartbeat_Consumer_TimerFunc(CANOPEN_TIMER_T *pTimer)
{
        pTimer->pLink->byNmtState = CANOPEN_STATE_PREOPERATIONAL;

        CanMaster_Nmt_SendStateChange(
                                pTimer->pCanopen,
                                pTimer->pLink,
                                CANOPEN_NMT_CMD_RESET_COMM
                        );

        pTimer->pLink->byStatus     = CANOPEN_LINK_STATUS_INTIAL;
        pTimer->pLink->Sdo.byState  = CANOPEN_SDO_STATE_FINISHED;
        pTimer->pLink->byNmtState   = CANOPEN_STATE_DISCONNECTED;

        Canopen_Timer_Delete(&(pTimer->pLink->Sdo.Timer));

        if (pTimer->pLink->Heartbeat.TimerFunc)
                pTimer->pLink->Heartbeat.TimerFunc(pTimer->pCanopen, pTimer->pLink);
}

static void Canopen_NodeGuard_TimerFunc(CANOPEN_TIMER_T *pTimer)
{
        if (pTimer->pLink->NodeGrd.sRemainCount <= 0) {
                pTimer->pCanopen->State.wNodeState = CANOPEN_STATE_DISCONNECTED;
                if (pTimer->pLink->NodeGrd.TimerFunc)
                        pTimer->pLink->NodeGrd.TimerFunc(pTimer->pCanopen, pTimer->pLink);
        }
        pTimer->pLink->NodeGrd.sRemainCount--;
        CanMaster_Nmt_SendNodeGuard ( pTimer->pCanopen, pTimer->pLink );
}
