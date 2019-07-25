/*===========================================================================+
|  Function : Function_Analog.h	                                                     |
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

#ifndef D__TASK_FUNCTION_ANALOG_H_
#define	D__TASK_FUNCTION_ANALOG_H_


#ifdef __cplusplus
extern "C" {
#endif
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/


#include	"driver/include/_timer.h"
#include    "task_Runtime.h"
/*===========================================================================+
|           Macro definition                                                  |
+============================================================================*/
#define     ADVALUECOUNT       10

/*===========================================================================+
|           Type definition                                                   |
+============================================================================*/
typedef  struct   AdCollectorTag
{
    DWORD_T   UpCount;
    DWORD_T   DownCount;
    DWORD_T   UpCounter_Count;
    DWORD_T   DownCounter_Count;
    DWORD_T   AdCollectorTick;
}ADCOLLECTORTICKTYPE_T;
/*===========================================================================+
|            Operations                                                      |
+============================================================================*/
/*===========================================================================+
|            Attributes                                                      |
+============================================================================*/

/*===========================================================================+
|            Extern Veriable                                                 |
+===========================================================================*/

extern  DWORD_T     AdValue[30][ADVALUECOUNT];
/*===========================================================================+
|            Extern                                                          |
+===========================================================================*/
extern  BOOL_T 	Funcblock_InputModule_AnalogSpecialCollector(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_InputModule_AnalogCollector(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Comparator_lagerEqual(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Sort_Max(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Sort_Min(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Comparator_Large(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Comparator_Equal(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
