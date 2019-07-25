/*===========================================================================+
|  Function : Can Master Application                                         |
|  Task     : Can Master Application Header File                             |
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
#ifndef D__CANMASTERAPP
#define D__CANMASTERAPP


#ifdef __cplusplus
extern "C" {
#endif

#include "kernel/include/task.h"
#include "canopen/include/canopen.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_MAX_SLAVE               1

#define CONFIG_CARD1_NODEID             1
#define CONFIG_CARD2_NODEID             2
#define CONFIG_CARD3_NODEID             3
#define CONFIG_CARD4_NODEID             4
#define CONFIG_CARD5_NODEID             5


#define Switch_database_Num           18

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
#define     CANOPEN_ID_SWITCH_ELECTNAME_PN                                      0x300000
#define     CANOPEN_ID_SWITCH_ELECTNAME_SN                                      0x300001
#define     CANOPEN_ID_SWITCH_SOFTWARE_PN                                       0x300002
#define     CANOPEN_ID_SWITCH_SOFTWARE_SN                                       0x300003
#define     CANOPEN_ID_SWITCH_ADDRESS                               			0x300004
#define     CANOPEN_ID_SWITCH_STATUS_CHANNEL1                               	0x300005
#define     CANOPEN_ID_SWITCH_STATUS_CHANNEL2                               	0x300006
#define     CANOPEN_ID_SWITCH_STATUS_CHANNEL3                               	0x300007
#define     CANOPEN_ID_SWITCH_STATUS_CHANNEL4                               	0x300008
#define     CANOPEN_ID_SWITCH_LEDPWM_CHANNEL1                               	0x300009
#define     CANOPEN_ID_SWITCH_LEDPWM_CHANNEL2                               	0x30000A
#define     CANOPEN_ID_SWITCH_LEDPWM_CHANNEL3                               	0x30000B
#define     CANOPEN_ID_SWITCH_LEDPWM_CHANNEL4                               	0x30000C
#define     CANOPEN_ID_SWITCH_BEEP											    0x30000D
#define     CANOPEN_ID_SWITCH_ERROR_CHANNEL1									0x30000E
#define     CANOPEN_ID_SWITCH_ERROR_CHANNEL2									0x30000F
#define     CANOPEN_ID_SWITCH_ERROR_CHANNEL3									0x300010
#define     CANOPEN_ID_SWITCH_ERROR_CHANNEL4									0x300011
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
void InitCanopenMaster(void);
void CreateTaskCanopenMaster(void);

extern  const CANOPEN_OD_INDEX_T u_aCanMaster_OdPara_Tbl[];

void sendSDO(
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_DWORD_T         dwIndex,
        CANOPEN_BYTE_T          pbySize,
        void                    *pBuf);

void SDO_upload_set(BYTE port, CANOPEN_BYTE_T byNodeID, CANOPEN_DWORD_T dwIndex);
/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/
extern CANOPEN_OD_DATA_T	*u_aCanMaster_OdValue_TblP1;
extern CANOPEN_OD_DATA_T	*u_aCanMaster_OdValue_TblP2;
extern CANOPEN_OD_INDEX_T	*u_aCanMaster_OdPara_TblP1;
extern CANOPEN_OD_INDEX_T	*u_aCanMaster_OdPara_TblP2;

#ifdef __cplusplus
}
#endif


#endif
