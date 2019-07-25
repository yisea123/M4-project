/*===========================================================================+
|  Function : Canopen timer                                                  |
|  Task     : Canopen timer Header File                                      |
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
#ifndef D__CANOPEN_TIMER
#define D__CANOPEN_TIMER

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_node.h"
#include "canopen_def.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_HALF_MAX_TIME           0x7FFFFFFF

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tagCANOPEN_TIMER CANOPEN_TIMER_T;
typedef void(*CANOPEN_TIMERFUNC)(CANOPEN_TIMER_T *pTimer);

struct tagCANOPEN_TIMER {
        CANOPEN_NODE_T                  Node;
        CANOPEN_T                       *pCanopen;
        CANOPEN_LINK_T                  *pLink;
        CANOPEN_BYTE_T                  byID;
        CANOPEN_WORD_T                  wTime;
        CANOPEN_TIMERFUNC               TimerFunc;
        CANOPEN_DWORD_T                 dwTimeoutTick;
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
void Canopen_Timer_Init(CANOPEN_T *pCanopen);

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_Timer_Create(
        CANOPEN_TIMER_T         *pTimer,
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_BYTE_T          byID,
        CANOPEN_WORD_T          wTime,
        CANOPEN_TIMERFUNC       TimerFunc);

void Canopen_Timer_Delete(CANOPEN_TIMER_T *pTimer);
void Canopen_Timer_Reset(CANOPEN_TIMER_T *pTimer);
void Canopen_Timer_Run(CANOPEN_T *pCanopen);

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
