/*===========================================================================+
|  Function : Canopen state                                                  |
|  Task     : Canopen state Header File                                      |
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
#ifndef D__CANOPEN_STATE
#define D__CANOPEN_STATE

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_def.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
//+++++
//  Node state
//+++++
#define CANOPEN_STATE_INITIALISATION    0x0
#define CANOPEN_STATE_DISCONNECTED      0x1
#define CANOPEN_STATE_PREPARED          0x2
#define CANOPEN_STATE_STOPPED           0x4
#define CANOPEN_STATE_OPERATIONAL       0x5
#define CANOPEN_STATE_PREOPERATIONAL    0x7F
#define CANOPEN_STATE_UNKNOWN           0xFF

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef CANOPEN_BOOL_T                  (*CANOPEN_STATE_CHECK_FUNC)();
typedef void                            (*CANOPEN_STATE_FUNC)(CANOPEN_T *pCanopen);
typedef struct tagCANOPEN_COMM_STATE    CANOPEN_COMM_STATE_T;
typedef struct tagCANOPEN_STATE         CANOPEN_STATE_T;

struct tagCANOPEN_COMM_STATE {
        CANOPEN_BOOL_T                  bBootUp;
        CANOPEN_BOOL_T                  bSdo;
        CANOPEN_BOOL_T                  bEmcy;
        CANOPEN_BOOL_T                  bSync;
        CANOPEN_BOOL_T                  bLifeGuard;
        CANOPEN_BOOL_T                  bPdo;
        CANOPEN_BOOL_T                  bLss;
};

struct tagCANOPEN_STATE {
        CANOPEN_WORD_T                  wNodeState;
        CANOPEN_COMM_STATE_T            CommState;
        CANOPEN_STATE_FUNC              ResetNode;
        CANOPEN_STATE_FUNC              ResetComm;
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
void Canopen_State_Init(CANOPEN_T *pCanopen);

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_WORD_T Canopen_State_Set(CANOPEN_T *pCanopen, CANOPEN_WORD_T wNodeState);

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
