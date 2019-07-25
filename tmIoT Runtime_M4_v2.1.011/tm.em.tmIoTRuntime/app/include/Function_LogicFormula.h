/*===========================================================================+
|  Function : Header	                                                     |
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

#ifndef D__TASK_FUNCTION_LOGICAFORMULA_H_
#define	D__TASK_FUNCTION_LOGICAFORMULA_H_


#ifdef __cplusplus
extern "C" {
#endif


#include   "task_Runtime.h"

/*===========================================================================+
|           Content                                                         |
+===========================================================================*/

/*===========================================================================+
|           External                                                         |
+===========================================================================*/
extern	BOOL_T 	Funcblock_Logic_And(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Logic_Or(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Logic_Not(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Logic_Xor(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_Add(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_Minus(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_Multiply(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_Devide(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_And(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_Or(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_ShiftLeft(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Arithmetic_ShiftRight(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
