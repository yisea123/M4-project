/*===========================================================================+
|  Function : Canopen state                                                  |
|  Task     : Canopen state Source File                                      |
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
static void Canopen_State_Switch(CANOPEN_T *pCanopen, CANOPEN_COMM_STATE_T *pState);

/*===========================================================================+
|           Inner Globle Variable                                            |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_State_Init(CANOPEN_T *pCanopen)
{
        pCanopen->State.wNodeState = CANOPEN_STATE_UNKNOWN;
        Canopen_State_Set(pCanopen, CANOPEN_STATE_INITIALISATION);
}

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_WORD_T Canopen_State_Set(CANOPEN_T *pCanopen, CANOPEN_WORD_T wNodeState)
{
        switch (wNodeState) {
                case CANOPEN_STATE_INITIALISATION:
                {
                        CANOPEN_COMM_STATE_T CommState = {
                                                CANOPEN_TRUE,
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE};
                        pCanopen->State.wNodeState = wNodeState;
                        Canopen_State_Switch(pCanopen, &CommState);
                        break;
                }
                case CANOPEN_STATE_STOPPED:
                {
                        CANOPEN_COMM_STATE_T CommState = {
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE,
                                                CANOPEN_FALSE,
                                                CANOPEN_TRUE,
                                                CANOPEN_FALSE,
                                                CANOPEN_TRUE};
                        if (pCanopen->State.wNodeState == CANOPEN_STATE_INITIALISATION)
                                return CANOPEN_STATE_UNKNOWN;
                        pCanopen->State.wNodeState = wNodeState;
                        Canopen_State_Switch(pCanopen, &CommState);
                        break;
                }
                case CANOPEN_STATE_OPERATIONAL:
                {
                        CANOPEN_COMM_STATE_T CommState = {
                                                CANOPEN_FALSE,
                                                CANOPEN_TRUE,
                                                CANOPEN_TRUE,
                                                CANOPEN_TRUE,
                                                CANOPEN_TRUE,
                                                CANOPEN_TRUE,
                                                CANOPEN_FALSE};
                        if (pCanopen->State.wNodeState == CANOPEN_STATE_INITIALISATION)
                                return CANOPEN_STATE_UNKNOWN;
                        pCanopen->State.wNodeState = wNodeState;
                        Canopen_State_Switch(pCanopen, &CommState);
                        break;
                }
                case CANOPEN_STATE_PREOPERATIONAL:
                {
                        CANOPEN_COMM_STATE_T CommState = {
                                        CANOPEN_FALSE,
                                        CANOPEN_TRUE,
                                        CANOPEN_TRUE,
                                        CANOPEN_TRUE,
                                        CANOPEN_TRUE,
                                        CANOPEN_FALSE,
                                        CANOPEN_TRUE};
                        pCanopen->State.wNodeState = wNodeState;
                        Canopen_State_Switch(pCanopen, &CommState);
                        break;
                }
                default:
                        return CANOPEN_STATE_UNKNOWN;
        }

        return pCanopen->State.wNodeState;
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
static void Canopen_State_Switch(CANOPEN_T *pCanopen, CANOPEN_COMM_STATE_T *pState)
{
#define FuncNone
#define Canopen_State_Deal(Type, FuncStart, FuncStop )                  \
        if (pState->Type != pCanopen->State.CommState.Type) {           \
                pCanopen->State.CommState.Type = pState->Type;          \
                if (pState->Type == CANOPEN_TRUE)                       \
                        FuncStop;                                       \
                else                                                    \
                        FuncStart;                                      \
        }

        Canopen_State_Deal(bBootUp    , FuncNone, FuncNone)
        Canopen_State_Deal(bSdo       , FuncNone, FuncNone)
        Canopen_State_Deal(bEmcy      , FuncNone, FuncNone)
        Canopen_State_Deal(bSync      , FuncNone, FuncNone)
        Canopen_State_Deal(bLifeGuard , FuncNone, FuncNone)
        Canopen_State_Deal(bPdo       , FuncNone, FuncNone)
}
