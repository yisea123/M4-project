/*===========================================================================+
|  Function : Canopen device                                                 |
|  Task     : Canopen device Header File                                     |
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
#ifndef D__CANOPEN_DEV
#define D__CANOPEN_DEV

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_type.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef union tagCANOPEN_DEV_DATA       CANOPEN_DEV_DATA_T;
typedef union tagCANOPEN_DEV_MSG        CANOPEN_DEV_MSG_T;
typedef struct tagCANOPEN_DEV_FRAME     CANOPEN_DEV_FRAME_T;

union tagCANOPEN_DEV_MSG {
        CANOPEN_DWORD_T                 dwAll;
        struct {
                CANOPEN_WORD_T          wExtID_L        :       16;
                CANOPEN_WORD_T          wExtID_H        :       2;
                CANOPEN_WORD_T          wStdID          :       11;
                CANOPEN_WORD_T          wReserved       :       2;
                CANOPEN_WORD_T          wRTR            :       1;
        } Bit;
};

union tagCANOPEN_DEV_DATA {
        CANOPEN_BYTE_T                  abyData[8];
        CANOPEN_WORD_T                  awData[4];
        CANOPEN_DWORD_T                 adwData[2];
};

struct tagCANOPEN_DEV_FRAME {
        CANOPEN_DEV_MSG_T               Msg;
        CANOPEN_DEV_DATA_T              CanData;
        CANOPEN_WORD_T                  wLength;
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

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_TICK_T Canopen_GetSysTick(void);
CANOPEN_SIZE_T Canopen_ReadDev(CANOPEN_T  *pCanopen, void *pBuf, CANOPEN_SIZE_T Size, CANOPEN_OFFSET_T Offset);
CANOPEN_SIZE_T Canopen_WriteDev(CANOPEN_T  *pCanopen, void *pBuf, CANOPEN_SIZE_T Size, CANOPEN_OFFSET_T Offset);
void Canopen_Control(CANOPEN_DEVICE_T Dev, void *pPara, CANOPEN_BYTE_T byFlag);

/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/


#ifdef __cplusplus
}
#endif


#endif
