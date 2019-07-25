/*===========================================================================+
|  Function : Canopen Nmt                                                    |
|  Task     : Canopen Nmt Header File                                        |
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
#ifndef D__CANOPEN_NMT
#define D__CANOPEN_NMT

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_dev.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_NMT_CMD_START_NODE                      0x1
#define CANOPEN_NMT_CMD_STOP_NODE                       0x2
#define CANOPEN_NMT_CMD_ENTER_PREOPERATIONAL            0x80
#define CANOPEN_NMT_CMD_RESET_NODE                      0x81
#define CANOPEN_NMT_CMD_RESET_COMM                      0x82

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/

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

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_SIZE_T CanMaster_Nmt_SendStateChange(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink, CANOPEN_BYTE_T byCs);
CANOPEN_SIZE_T CanMaster_Nmt_SendNodeGuard(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);
CANOPEN_SIZE_T CanMaster_Nmt_ReqAllNode(CANOPEN_T *pCanOpen);
CANOPEN_SIZE_T CanSalve_Nmt_SendBootup(CANOPEN_T *pCanOpen);
void CanSlave_Nmt_Recv(CANOPEN_T *pCanOpen, CANOPEN_DEV_FRAME_T *pFrame);

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
