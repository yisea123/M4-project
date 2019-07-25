/*===========================================================================+
|  Function : Canopen pdo                                                    |
|  Task     : Canopen pdo Header File                                        |
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
#ifndef D__CANOPEN_PDO
#define D__CANOPEN_PDO

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_type.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_MAXDATA_ONEPDO          4       // the data number one pdo support
#define CANOPEN_MAXPDO_ONENODE          4       // the pdo number one node support

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tagCANOPEN_PDO_PARA      CANOPEN_PDO_PARA_T;
typedef struct tagCANOPEN_PDO_MAP       CANOPEN_PDO_MAP_T;
typedef struct tagCANOPEN_PDO_COMM      CANOPEN_PDO_COMM_T;
typedef struct tagCANOPEN_PDO           CANOPEN_PDO_T;

struct  tagCANOPEN_PDO_PARA {
        CANOPEN_BYTE_T                  bySubIndex0;
        CANOPEN_DWORD_T                 dwCombID;
        CANOPEN_BYTE_T                  byTransType;
        CANOPEN_WORD_T                  wInhibitTime;
        CANOPEN_BYTE_T                  byReserved;
        CANOPEN_WORD_T                  wEventTime;
};

struct tagCANOPEN_PDO_MAP {
        CANOPEN_BYTE_T                  bySubIndex0;
        CANOPEN_DWORD_T                 adwEntries[CANOPEN_MAXDATA_ONEPDO];
};

struct  tagCANOPEN_PDO_COMM {
        CANOPEN_BOOL_T                  bUse;
        CANOPEN_PDO_PARA_T              Para;
        CANOPEN_PDO_MAP_T               Map;
        CANOPEN_TIMER_T                 Timer;
};

struct  tagCANOPEN_PDO {
        CANOPEN_PDO_COMM_T              aSendPdo[CANOPEN_MAXPDO_ONENODE];
        CANOPEN_PDO_COMM_T              aRecvPdo[CANOPEN_MAXPDO_ONENODE];
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
void Canopen_Pdo_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Canopen_Pdo_Start(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);
void Canopen_Pdo_Stop(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);
void Canopen_Pdo_Send(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink, CANOPEN_BYTE_T byID);
void Canopen_Pdo_Recv(CANOPEN_T *pCanopen, CANOPEN_DEV_FRAME_T *pFrame);

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
