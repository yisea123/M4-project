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

#ifndef D__TASK
#define	D__TASK

#include "kernel/include/type.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define		MAXTASK_TYPE		16	// 64-->16

#define     HTASK_NULL          0
#define     NULL_HTASK          HTASK_NULL

#define     TASK_NONE	        0
#define     TASK_NORMAL         1
#define     TASK_IDLE           2
#define     TASK_WAITIO         3
#define     TASK_SUSPEND        4
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef	WORD 	HTASK;  /* Handle of task */
typedef void 	(* pRUN)(void *pParam);

enum TASK_TYPE
{
	TASK_ISR,
	TASK_TIMERINT,
	TASK_WHILELOOP,
	TASK_RSD,
	MAX_TASKTYPE
};

enum TASK_HANDLE
{
	HTASK_TASKISR				= HTASK_NULL+1,
	HTASK_TASKTIMERINT			= HTASK_TASKISR+MAXTASK_TYPE,
	HTASK_TASKWHILELOOP			= HTASK_TASKTIMERINT+MAXTASK_TYPE,
	HTASK_TASKRSD				= HTASK_TASKWHILELOOP+MAXTASK_TYPE
};

#define		MAX_TASK			(MAXTASK_TYPE * MAX_TASKTYPE)

typedef struct tagPERIOD       // union --> struct   2016/10/10
{
	TINT    tCounter;	/* Time slice counter */
	TINT	tTimer;		/* Unit: us */
} PERIOD;

typedef struct tagTASK
{
	WORD	wType;
	pRUN	pRun;	
	PERIOD	period;		/* Cyclic period */
	void	*pParam;
} TASK;

typedef struct tagCTask
{
	HTASK	m_htaskme;
	RTTI	m_rtti;		/* Task type: isr, timer int, while loop */
	WORD	m_wStatus;
    WORD	m_wPriority;
    TINT	m_tCounter;
    
	TASK	m_task;
} CTask;

/*===========================================================================+
|           External                                                         |
+===========================================================================*/
void	Init_Task(void);

HTASK	Create_Task(TASK *pTask, WORD wPriority);
void	Destroy_Task(HTASK htask);

HTASK   GetNext_Task(void);
CTask*  GetTask_Task(HTASK htask);
WORD    GetStatus_Task(HTASK htask);
void    SetStatus_Task(HTASK htask, WORD wStatus);
void	SetPriority_Task(HTASK htask, WORD wPriority);

CTask	*g_paTaskList;

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
