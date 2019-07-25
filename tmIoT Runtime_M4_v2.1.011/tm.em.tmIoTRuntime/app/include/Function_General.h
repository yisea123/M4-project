/*===========================================================================+
|  Function : Function_General.h	                                         |
|  Task     : 			                                                     |
|----------------------------------------------------------------------------|
|  Compile  : 			                                                   	 |
|  Link     : 			                                                   	 |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : 			                                                     |
|  Version  : V1.00                                                          |
|  Creation : 			                                                     |
|  Revision :                                                          	     |
+===========================================================================*/

#ifndef D__TASK_FUNCTION_GENERAL_H_
#define	D__TASK_FUNCTION_GENERAL_H_


#ifdef __cplusplus
extern "C" {
#endif

#include   "task_Runtime.h"

/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|           External                                                         |
+===========================================================================*/
extern  BOOL_T 	Funcblock_InputModule_Digital(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_InputModule_Analog(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_InputModule_GlobalVariable(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_OutputModule_Digital(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_OutputModule_Analog(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_OutputModule_GlobalVariable(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
