/*===========================================================================+
|  Function : Function_Modbus.h	                                             |
|  Task     : 			                                                     |
|----------------------------------------------------------------------------|
|  Compile  : 			                                                   	 |
|  Link     : 			                                                   	 |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : 			                                                     |
|  Version  : V2.00                                                          |
|  Creation : 			                                                     |
|  Revision :                                                          	     |
+===========================================================================*/

#ifndef D__TASK_FUNCTION_MODBUS_H_
#define	D__TASK_FUNCTION_MODBUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include	"driver/include/_timer.h"
#include    "task_Runtime.h"

/*===========================================================================+
|           Macro definition                                                  |
+============================================================================*/

/*===========================================================================+
|           Type definition                                                   |
+============================================================================*/

/*===========================================================================+
|            Operations                                                      |
+============================================================================*/
/*===========================================================================+
|            Attributes                                                      |
+============================================================================*/

/*===========================================================================+
|            Extern Veriable                                                 |
+===========================================================================*/

/*===========================================================================+
|            Extern                                                          |
+===========================================================================*/
BOOL_T Funcblock_ModbusRTU_M01(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_M02(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_M03(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_M04(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_M05(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_M06(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_M0F(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_M10(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S01(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S02(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S03(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S04(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S05(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S06(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S0F(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
BOOL_T Funcblock_ModbusRTU_S10(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
