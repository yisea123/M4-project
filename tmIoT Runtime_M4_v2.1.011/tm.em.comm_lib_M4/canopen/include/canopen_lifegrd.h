/*===========================================================================+
|  Function : Canopen life guard                                             |
|  Task     : Canopen life guard Header File                                 |
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
#ifndef D__CANOPEN_LIFEGUARD
#define D__CANOPEN_LIFEGUARD

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_def.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef void (*CANOPEN_LIFEGRD_TIMERFUNC)(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);
typedef struct tagCANOPEN_NODEGUARD CANOPEN_NODEGUARD_T;
typedef struct tagCANOPEN_HEARTBEAT CANOPEN_HEARTBEAT_T;

struct tagCANOPEN_HEARTBEAT {
        CANOPEN_TIMER_T                 Timer;
        CANOPEN_WORD_T                  wTime;
        CANOPEN_LIFEGRD_TIMERFUNC       TimerFunc;
};

struct tagCANOPEN_NODEGUARD {
        CANOPEN_TIMER_T                 Timer;
        CANOPEN_WORD_T                  wTime;
        CANOPEN_BYTE_T                  byFactor;
        CANOPEN_LIFEGRD_TIMERFUNC       TimerFunc;
        CANOPEN_BYTE_T                  byToggle;
        CANOPEN_SHORT_T                 sRemainCount;
};

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/

/*===========================================================================+
|           Class declaration                                                |
+===========================================================================*/

/*===========================================================================+
|           Attributes                                                       |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_LifeGrd_Init(CANOPEN_T  *pCanopen, CANOPEN_LINK_T *pLink);

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_LifeGrd_Recv(CANOPEN_T  *pCanopen, CANOPEN_DEV_FRAME_T *pFrame);

/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/


#ifdef __cplusplus
}
#endif


#endif
