/*===========================================================================+
|  Class	: 			                                                     |
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

#include <string.h>
#include "kernel/include/task.h"
#include "driver/include/_timer.h"

/*===========================================================================+
|           Global variable                                                  |
+===========================================================================*/
/**
 * Initialize static class member
 */
static	HTASK		u_htaskCurrent = HTASK_NULL;
static	CTask		u_aTaskList[MAX_TASK];
static	int			u_aTasksCount[MAX_TASKTYPE];
CTask	*g_paTaskList;
/*===========================================================================+
|           Class implementation - Task                                      |
+===========================================================================*/
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void Init_Task(void)
{
	memset(u_aTaskList, 0, sizeof(u_aTaskList));
	memset(u_aTasksCount, 0, sizeof(u_aTasksCount));
	g_paTaskList = &u_aTaskList[0];
}

HTASK Create_Task(TASK *pTask, WORD wPriority)
{
	int		i;
	WORD	wType;
	CTask	*pCTaskList;
	TINT	 tCounter;
	
	if (pTask == NULL || pTask->pRun == NULL) 
		return HTASK_NULL;
	wType = pTask->wType;
	tCounter = pTask->period.tCounter; //add 2016/10/10
	if (wType >= MAX_TASKTYPE || u_aTasksCount[wType] >= MAXTASK_TYPE) 
		return HTASK_NULL;
	
	pCTaskList = &u_aTaskList[0];
	for (i=MAXTASK_TYPE*wType; i<MAXTASK_TYPE*(wType+1); i++)
	{
		if ((pCTaskList+i)->m_task.pRun == pTask->pRun && (pCTaskList+i)->m_task.pParam == pTask->pParam && \
			((pCTaskList+i)->m_task.period.tTimer == pTask->period.tTimer || (pCTaskList+i)->m_task.period.tCounter == pTask->period.tCounter))
			return (pCTaskList+i)->m_htaskme;
	}
	
	for (i=MAXTASK_TYPE*wType; i<MAXTASK_TYPE*(wType+1); i++)
	{
		if ((pCTaskList+i)->m_htaskme == HTASK_NULL) 
			break;
	}
	
	(pCTaskList+i)->m_htaskme	= i + 1;    /* Save the handle in global list : index to handal */
	(pCTaskList+i)->m_rtti		= wType;
	(pCTaskList+i)->m_wStatus	= TASK_NORMAL;
	(pCTaskList+i)->m_wPriority	= wPriority;
	(pCTaskList+i)->m_tCounter = tCounter;    //add 2016/10/10
	AddUpCounter(&(pCTaskList+i)->m_tCounter);
	memcpy(&(pCTaskList+i)->m_task, pTask, sizeof(TASK));
	u_aTasksCount[wType]++;

	return (pCTaskList+i)->m_htaskme;
}

void Destroy_Task(HTASK htask)
{
	WORD	wType;
	CTask	*pCTask;
	
	if (htask <= 0 || htask > MAX_TASK) 
		return;
	
	wType = (htask-1)/MAXTASK_TYPE;
	pCTask = &u_aTaskList[htask-1];
	
	if (pCTask->m_htaskme == htask)
	{
		DeleteUpCounter(&pCTask->m_tCounter);
		memset(pCTask, 0, sizeof(CTask));
		u_aTasksCount[wType]--;
	}
}

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
HTASK GetNext_Task()
{
	return u_htaskCurrent;
}

CTask* GetTask_Task(HTASK htask)
{
	CTask	*pCTask;
	
	if (htask <= 0 || htask > MAX_TASK) 
		return NULL;
	
	pCTask = &u_aTaskList[htask-1];
	
	if (pCTask->m_htaskme == htask && pCTask->m_wStatus == TASK_NORMAL) 
		return pCTask;
	else return NULL;
}

WORD GetStatus_Task(HTASK htask)
{
	CTask	*pCTask;
	
	if (htask <= 0 || htask > MAX_TASK) 
		return TASK_NONE;
	
	pCTask = &u_aTaskList[htask-1];
	
	if (pCTask->m_htaskme == htask) 
		return pCTask->m_wStatus;
	else return TASK_NONE;
}

void SetStatus_Task(HTASK htask, WORD wStatus)
{
	CTask	*pCTask;
	
	if (htask <= 0 || htask > MAX_TASK) 
		return;
	
	pCTask = &u_aTaskList[htask-1];
	
	if (pCTask->m_htaskme == htask) pCTask->m_wStatus = wStatus;
}

void SetPriority_Task(HTASK htask, WORD wPriority)
{
	CTask	*pCTask;
	
	if (htask <= 0 || htask > MAX_TASK) 
		return;
	
	pCTask = &u_aTaskList[htask-1];
	
	if (pCTask->m_htaskme == htask) pCTask->m_wPriority = wPriority;
}
