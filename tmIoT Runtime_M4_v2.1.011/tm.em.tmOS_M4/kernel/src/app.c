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
#include "kernel/include/app.h"
/*===========================================================================+
|           Function and Class prototype                                     |
+===========================================================================*/
void Run_TimerIntApp(void);

/*===========================================================================+
|           Global variable                                                  |
+===========================================================================*/
static	CApp		g_capp;
static	ROUTINE		u_routine = { TIMER_SLICE, &Run_TimerIntApp };

/*===========================================================================+
|           Class implementation - Application                               |
+===========================================================================*/
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void Create_App()
{
	int	i;
	
	memset(&g_capp, 0, sizeof(g_capp));
	for (i=0; i<CMD_MAX_CHANNEL; i++)
	{
		g_capp.m_msg[i].wInput = g_capp.m_msg[i].wOutput = 0;
		g_capp.m_msg[i].wQueueEnd = CMD_MAX_MESSAGE;
		g_capp.m_msg[i].bQueueFull = FALSE;
	}
	
	AddRoutine(&u_routine);
}

/*
void        Destroy_App()
{
}
*/

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------+
|  Function : CApp::Run()                                                    |
|  Task     : Application processor and task scheduler                       |
+----------------------------------------------------------------------------+
|  Call     : CApp::Run()                                                    |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void Run_App()
{
	HTASK	htask;
	CTask	*pctask;
	
 	for (htask=HTASK_TASKWHILELOOP; htask<HTASK_TASKRSD; htask++)
	{
		pctask = GetTask_Task(htask);
		if (pctask != NULL)
		{
			if (pctask->m_tCounter >= pctask->m_task.period.tTimer)
			{
				(*(pctask->m_task.pRun))(pctask->m_task.pParam);
				//pctask->m_tCounter = pctask->m_task.period.tTimer;
				pctask->m_tCounter = pctask->m_task.period.tCounter;
			}
		}
	}
}
/*---------------------------------------------------------------------------+
|  Function : CApp::Run()                                                    |
|  Task     : Application processor and task scheduler                       |
+----------------------------------------------------------------------------+
|  Call     : CApp::Run()                                                    |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void Run_TimerIntApp()
{
	HTASK	htask;
	CTask	*pctask;
	
	for (htask=HTASK_TASKTIMERINT; htask<HTASK_TASKWHILELOOP; htask++)
	{
		pctask = GetTask_Task(htask);
		if (pctask != NULL)
		{
			if (pctask->m_tCounter >= pctask->m_task.period.tTimer)
			{
				(*(pctask->m_task.pRun))(pctask->m_task.pParam);
				//pctask->m_tCounter = pctask->m_task.period.tTimer;
				pctask->m_tCounter = pctask->m_task.period.tCounter;     // Modfiy 2016/04/05
			}
		}
	}
}

/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
BOOL SetMessage_App(WORD wChannel, MSG* pmsg)
{
	MSGQUEUE *pMSG;
	
	pMSG = &g_capp.m_msg[wChannel];
	if (!pMSG->bQueueFull)
	{
		pMSG->msgQueue[pMSG->wInput++] = *pmsg;
		if (pMSG->wInput >= pMSG->wQueueEnd)
		    pMSG->wInput = 0;
		if (pMSG->wInput == pMSG->wOutput)
		    pMSG->bQueueFull = TRUE;
		return TRUE;
	}
	
	return FALSE;
}

BOOL GetMessage_App(WORD wChannel, MSG* pmsg)
{
	MSGQUEUE *pMSG;
	
	pMSG = &g_capp.m_msg[wChannel];
	if ((pMSG->wInput != pMSG->wOutput) || (pMSG->bQueueFull))
	{
		*(pmsg) = pMSG->msgQueue[pMSG->wOutput++];
		if (pMSG->wOutput >= pMSG->wQueueEnd) pMSG->wOutput = 0;
		return TRUE;
	}
	
	return FALSE;
}
/*---------------------------------------------------------------------------+
|           Helpers                                                          |
+---------------------------------------------------------------------------*/
