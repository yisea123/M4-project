/*===========================================================================+
|  Function : Function_Timer.h	                                             |
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

#ifndef D__TASK_FUNCTION_DELAYTIME_H_
#define	D__TASK_FUNCTION_DELAYTIME_H_


#ifdef __cplusplus
extern "C" {
#endif


#include    "driver/include/_timer.h"
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

typedef struct tagTimer
{
//    BYTE_T   ID;
//  WORD_T   Value;
    DWORD_T  DelayTimeClock;
}TIMETYPE_T;

typedef struct tagTimeCount
{
//    BYTE_T   ID;
//  WORD_T   Value;
    DWORD_T   UpCount;
    DWORD_T   DownCount;
    DWORD_T   UpCounter_Count;
    DWORD_T   DownCounter_Count;
    DWORD_T   TimeTick;
}TIMECOUNTTYPE_T;


typedef struct tagPeriodTime
{
//    BYTE_T   ID;
//  WORD_T   Value;
    DWORD_T   UpCount;
    DWORD_T   DownCount;
    BYTE_T    InputChangeFlag[3];
    DBVALUE   InputsValue[3];
    DWORD_T   UpCounter_Count;
    DWORD_T   DownCounter_Count;
    DWORD_T   TimeTick;
}PERIODTIMETYPE_T;
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
|            External                                                        |
+===========================================================================*/
extern	BOOL_T 	Funcblock_Timer_DelayTime_On(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Timer_DelayTime_Off(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Timer_HighLevelTime(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Timer_LowLevelTime(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Timer_PulseTime(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);


/*===========================================================================+
|            External Veriable                                               |
+===========================================================================*/

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
