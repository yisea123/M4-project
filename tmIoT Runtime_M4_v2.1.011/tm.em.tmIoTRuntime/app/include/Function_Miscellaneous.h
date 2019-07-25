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

#ifndef D__TASK_FUNCTION_MISCELLANEOUS_H_
#define	D__TASK_FUNCTION_MISCELLANEOUS_H_


#ifdef __cplusplus
extern "C" {
#endif


#include   "task_Runtime.h"

/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Type definition                                                   |
+============================================================================*/
typedef struct tagSquareWave
{
	DWORD_T	 SquareWave_Tick;
	DWORD_T	 SquareWave_Tick1;
}SQUAREWAVETYPE_T;
/*===========================================================================+
|           Macro definition                                                  |
+============================================================================*/
#define  SQUARWAVENUM     20;


/*===========================================================================+
|           External                                                         |
+===========================================================================*/
extern  BOOL_T 	Funcblock_Miscellaneous_RSLatching(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Miscellaneous_SRLatching(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Miscellaneous_SquareWaveGenertor(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Miscellaneous_Constant(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Miscellaneous_Random(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Miscellaneous_GetValueAddress(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T  Funcblock_High_Linker(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
