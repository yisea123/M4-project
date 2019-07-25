/*===========================================================================+
|  Function : Can Device                                                     |
|  Task     : Can Device Source File                                         |
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
#include "driver/include/_timer.h"
#include "canopen/include/canopen.h"
#include "driver/include/_ecan.h"

#include "CanFunctionBlock.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
extern	CANOPEN_T       g_CanMaster[HardDeviceNum];
extern	CANOPEN_T       g_CanSlave[HardDeviceNum];
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

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_TICK_T Canopen_GetSysTick(void)
{
        return g_dwSystemTick;
}

CANOPEN_SIZE_T Canopen_ReadDev(
		CANOPEN_T 				*pCanopen,//wyl modify
        void                    *pBuf,
        CANOPEN_OFFSET_T        Offset,
        CANOPEN_SIZE_T          Size)
{
        CANOPEN_WORD_T wLen = 0;
        BYTE  Device = 0;
        //CANOPEN_T * test = pCanopen;
        if((pCanopen==&g_CanMaster[0])||(pCanopen==&g_CanSlave[0]))
        	Device = HardPort1;
        else if((pCanopen==&g_CanMaster[1])||(pCanopen==&g_CanSlave[1]))
        	Device = HardPort2;

        switch (Device) {
           case HardPort1:
                if (_eCan0_Read(pBuf, Size, &wLen) == CAN_SUCCESS)
                   return wLen;
                break;
           case HardPort2:
                if (_eCan1_Read(pBuf, Size, &wLen) == CAN_SUCCESS)
                  return wLen;
                break;
           default:
                break;
              }
//        switch (Dev) {
//        case CANOPEN_MASTER:
//                if (_eCan1_Read(pBuf, Size, &wLen) == CAN_SUCCESS)
//                        return wLen;
//                break;
//        case CANOPEN_SLAVE:
//                if (_eCan0_Read(pBuf, Size, &wLen) == CAN_SUCCESS)
//                        return wLen;
//                break;
//        default:
//                break;
//        }

        return 0;
}

CANOPEN_SIZE_T Canopen_WriteDev(
		CANOPEN_T  *pCanopen,
        void                    *pBuf,
        CANOPEN_OFFSET_T        Offset,
        CANOPEN_SIZE_T          Size)
{

    CANOPEN_WORD_T wLen = 0;
    BYTE  Device = 0;
    //CANOPEN_T * test = pCanopen;
    if((pCanopen==&g_CanMaster[0])||(pCanopen==&g_CanSlave[0]))
    	Device = HardPort1;
    else if((pCanopen==&g_CanMaster[1])||(pCanopen==&g_CanSlave[1]))
    	Device = HardPort2;

    switch (Device) {
       case HardPort1:
            if (_eCan0_Write(pBuf, Size) == CAN_SUCCESS)
               return wLen;
            break;
       case HardPort2:
            if (_eCan1_Write(pBuf, Size) == CAN_SUCCESS)
              return wLen;
            break;
       default:
            break;
          }
//        switch (Dev) {
//        case CANOPEN_MASTER:
//                if (_eCan1_Write(pBuf, Size) == CAN_SUCCESS)
//                        return Size;
//                break;
//        case CANOPEN_SLAVE:
//                if (_eCan0_Write(pBuf, Size) == CAN_SUCCESS)
//                        return Size;
//                break;
//        default:
//                break;
//        }
//
////       switch (Dev) {
////       case CANOPEN_MASTER:
////               if (_eCan0_Write(pBuf, Size) == CAN_SUCCESS)
////                       return Size;
////               break;
////               if (_eCan1_Write(pBuf, Size) == CAN_SUCCESS)
////                       return Size;
////
////       default:
////               break;
        return 0;
}

void Canopen_Control(
        CANOPEN_DEVICE_T        Dev,
        void                    *pPara,
        CANOPEN_BYTE_T          byFlag)
{

}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
