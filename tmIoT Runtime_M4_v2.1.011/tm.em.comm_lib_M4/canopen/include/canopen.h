/*===========================================================================+
|  Function : Canopen                                                        |
|  Task     : Canopen Header File                                            |
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
#ifndef D__CANOPEN
#define D__CANOPEN

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_def.h"
#include "canopen_node.h"
#include "canopen_timer.h"
#include "canopen_state.h"
#include "canopen_nmt.h"
#include "canopen_lifegrd.h"
#include "canopen_od.h"
#include "canopen_sdo.h"
#include "canopen_pdo.h"
#include "canopen_dev.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_LINK_STATUS_INTIAL      0
#define CANOPEN_LINK_STATUS_CONFIG      1
#define CANOPEN_LINK_STATUS_NORMAL      2

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
struct tagCANOPEN {
        CANOPEN_BYTE_T                  byCliServ;
        CANOPEN_BYTE_T                  byNodeID;
        CANOPEN_OD_T                    *pObjdict;
        CANOPEN_DEVICE_T                CanDev;
        CANOPEN_STATE_T                 State;
        CANOPEN_NODE_T                  LinkList;
};

struct tagCANOPEN_LINK {
        CANOPEN_NODE_T                  Node;
        CANOPEN_BYTE_T                  byStatus;
        CANOPEN_BYTE_T                  byNodeID;
        CANOPEN_WORD_T                  wCurIndex;
        CANOPEN_OD_T                    *pObjdict;
        CANOPEN_SDO_T                   Sdo;
        CANOPEN_PDO_T                   Pdo;
        CANOPEN_BYTE_T                  byNmtState;
        CANOPEN_NODEGUARD_T             NodeGrd;
        CANOPEN_HEARTBEAT_T             Heartbeat;
};

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/
#define		CANOPEN_EQUAL(a,b) ((a==b)?1:0)
/*===========================================================================+
|           Class declaration                                                |
+===========================================================================*/

/*===========================================================================+
|           Attributes                                                       |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void Canopen_Create(CANOPEN_T *pCanopen);
void Canopen_Link_Create(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink, CANOPEN_BYTE_T byNodeID, CANOPEN_OD_T *pObjDict);

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_Run(CANOPEN_T *pCanopen);
CANOPEN_LINK_T* Canopen_GetLink(CANOPEN_T *pCanopen, CANOPEN_BYTE_T byNodeID);

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
