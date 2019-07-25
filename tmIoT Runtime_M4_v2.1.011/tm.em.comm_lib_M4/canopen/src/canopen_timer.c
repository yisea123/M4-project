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
#ifndef HardDeviceNum
#define HardDeviceNum 2
#endif
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
extern	CANOPEN_T       g_CanMaster[HardDeviceNum];
extern	CANOPEN_T       g_CanSlave[HardDeviceNum];
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
CANOPEN_NODE_T u_MasterTimerList[HardDeviceNum];
CANOPEN_NODE_T u_SlaveTimerList[HardDeviceNum];

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_Timer_Init(CANOPEN_T *pCanopen)
{
//	CANOPEN_DEVICE_T  DEV = 0;
//        switch (pCanopen->CanDev) {
//        case CANOPEN_MASTER:
//                Canopen_Node_Init(&u_MasterTimerList);
//                break;
//        case CANOPEN_SLAVE:
//                Canopen_Node_Init(&u_SlaveTimerList);
//                break;
//        default:
//                break;
//        }
	  if(pCanopen==&g_CanMaster[0])
		  Canopen_Node_Init(&u_MasterTimerList[0]);

	  if(pCanopen==&g_CanMaster[1])
		  Canopen_Node_Init(&u_MasterTimerList[1]);

	  if(pCanopen==&g_CanSlave[0])
		  Canopen_Node_Init(&u_SlaveTimerList[0]);

	  if(pCanopen==&g_CanSlave[1])
		  Canopen_Node_Init(&u_SlaveTimerList[1]);

//      switch (pCanopen->CanDev) {
//      case CANOPEN_MASTER:
//              Canopen_Node_Init(&u_MasterTimerList);
//              break;
//      case CANOPEN_SLAVE:
//              Canopen_Node_Init(&u_SlaveTimerList);
//              break;
//      default:
//              break;
//      }
}

void Canopen_Timer_Create(
        CANOPEN_TIMER_T         *pTimer,
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_BYTE_T          byID,
        CANOPEN_WORD_T          wTime,
        CANOPEN_TIMERFUNC       TimerFunc)
{
        pTimer->byID            = byID;
        pTimer->pCanopen        = pCanopen;
        pTimer->pLink           = pLink;
        pTimer->wTime           = wTime;
        pTimer->TimerFunc       = TimerFunc;
        pTimer->dwTimeoutTick   = Canopen_GetSysTick() + pTimer->wTime;


        switch (pCanopen->CanDev) {
        case CANOPEN_MASTER:
                if (Canopen_Node_IsEmpty(&(pTimer->Node))) {
                		if(pCanopen==&g_CanMaster[0])
                			Canopen_Node_InsertBefore(&u_MasterTimerList[0], &(pTimer->Node));

                		else if(pCanopen==&g_CanMaster[1])
                			Canopen_Node_InsertBefore(&u_MasterTimerList[1], &(pTimer->Node));
                }
                break;
        case CANOPEN_SLAVE:
                if (Canopen_Node_IsEmpty(&(pTimer->Node))) {
                	if(pCanopen==&g_CanSlave[0])
                        Canopen_Node_InsertBefore(&u_SlaveTimerList[0], &(pTimer->Node));

                	if(pCanopen==&g_CanSlave[1])
                	                        Canopen_Node_InsertBefore(&u_SlaveTimerList[1], &(pTimer->Node));
                }
                break;
        default:
                break;
        }
}

void Canopen_Timer_Delete(CANOPEN_TIMER_T *pTimer)
{
        Canopen_Node_Remove(&(pTimer->Node));
}

void Canopen_Timer_Reset(CANOPEN_TIMER_T *pTimer)
{
        pTimer->dwTimeoutTick = Canopen_GetSysTick() + pTimer->wTime;
}

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_Timer_Run(CANOPEN_T *pCanopen)
{
        CANOPEN_TIMER_T *pTimer;
        CANOPEN_NODE_T  *pNode;
        CANOPEN_BYTE_T  DEV = 0;
        switch (pCanopen->CanDev) {
        case CANOPEN_MASTER:
        		if(pCanopen==&g_CanMaster[0])
        			DEV = 0;
        		else if(pCanopen==&g_CanMaster[1])
        			DEV = 1;
                for (pNode = u_MasterTimerList[DEV].pNext; pNode != &u_MasterTimerList[DEV]; pNode = pNode->pNext) {
                        pTimer = Canopen_Node_Entry(pNode, CANOPEN_TIMER_T, Node);
                        if (Canopen_GetSysTick() - pTimer->dwTimeoutTick  < CANOPEN_HALF_MAX_TIME) {
                                if (pTimer->TimerFunc)
                                        pTimer->TimerFunc(pTimer);
                                pTimer->dwTimeoutTick = Canopen_GetSysTick() + pTimer->wTime;
                        }
                }
                break;
        case CANOPEN_SLAVE:
        		if(pCanopen==&g_CanSlave[0])
        	        DEV = 0;
        	    else if(pCanopen==&g_CanSlave[1])
        	        DEV = 1;
                for (pNode = u_SlaveTimerList[DEV].pNext; pNode != &u_SlaveTimerList[DEV]; pNode = pNode->pNext) {
                        pTimer = Canopen_Node_Entry(pNode, CANOPEN_TIMER_T, Node);
                        if (Canopen_GetSysTick() - pTimer->dwTimeoutTick  < CANOPEN_HALF_MAX_TIME) {
                                if (pTimer->TimerFunc)
                                        pTimer->TimerFunc(pTimer);
                                pTimer->dwTimeoutTick = Canopen_GetSysTick() + pTimer->wTime;
                        }
                }
                break;
        default:
                break;
        }
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
