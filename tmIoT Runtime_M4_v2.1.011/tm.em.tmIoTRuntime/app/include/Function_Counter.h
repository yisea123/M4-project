/*===========================================================================+
|  Function : Function_Counter.h	                                         |
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

#ifndef D__TASK_FUNCTION_COUNTER_H_
#define	D__TASK_FUNCTION_COUNTER_H_


#ifdef __cplusplus
extern "C" {
#endif


#include    "task_Runtime.h"

/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Type definition                                                   |
+============================================================================*/
typedef  struct tagCounter
{
//  WORD_T    ID;
//  WORD_T    Value;
    DWORD_T   UpCount;
    DWORD_T   DownCount;
    DWORD_T   UpCounter_Count;
    DWORD_T   DownCounter_Count;
}COUTERTYPE_T;

typedef  struct  tagPulseCounter
{
    DWORD_T   UpCount;
    DWORD_T   DownCount;
    BYTE_T    InputChangeFlag[3];
    DBVALUE   InputsValue[3];
    DWORD_T   UpCounter_Count;
    DWORD_T   DownCounter_Count;
    DWORD_T   PulseCounter_Count;
}PULSECOUNTERTYPE_T;
/*===========================================================================+
|           Macro definition                                                  |
+============================================================================*/
/*===========================================================================+
|            Operations                                                      |
+============================================================================*/
/*===========================================================================+
|            Attributes                                                      |
+============================================================================*/

/*===========================================================================+
|            External Veriable                                               |
+===========================================================================*/


/*===========================================================================+
|            External                                                        |
+===========================================================================*/
extern 	BOOL_T 	Funcblock_Counter_UpSlopeCounter(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern 	BOOL_T 	Funcblock_Counter_DownSlopeCounter(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern 	BOOL_T 	Funcblock_Counter_PulseCounter(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
