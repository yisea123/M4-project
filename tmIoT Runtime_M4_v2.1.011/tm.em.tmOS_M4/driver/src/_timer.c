/*===========================================================================+
|  Class    : Timer                                                          |
|  Task     : Timer Operation File                                           |
|----------------------------------------------------------------------------|
|  Compile  : C320 V6.60 -                                                   |
|  Link     : C320 V6.60 -                                                   |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Version  : V1.00                                                          |
|  Creation : 03/16/2011                                                     |
|  Revision :                                                                |
+===========================================================================*/
#include    "driver/include/_timer.h"
#include 	"kernel/include/task.h"
#include    "studioprotocol.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tagROUTINE_
{
    TINT_T        	tCounter;
    ROUTINE     	routine;
} ROUTINE_;
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
DWORD_T       	g_dwSystemTick      =   0;
DWORD_T       	g_dwSystemClock     =   0;
DWORD_T       	g_dwSystemGet     	=   0;
WORD_T			g_wRunFlag 			= 	1;
WORD_T			g_wStepFlag 		= 	0;
WORD_T			g_wStepRunflag		= 	0;
/*===========================================================================+
|           Class implementation - Timer                                     |
+===========================================================================*/
/*---------------------------------------------------------------------------+
|           Helpers  Implement                                               |
+---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------+
|           Inner Global variable                                            |
+---------------------------------------------------------------------------*/
static      INT_T         	u_nRoutineCount         = 0;
static      ROUTINE_    	u_Routine[MAX_ROUTINE];

static      INT_T         	u_nDnCounter=0;
static      LONG_T*       	u_ptDnCounter[MAX_COUNTER];

static      INT_T           u_nUpCounter = 0;
static      LONG_T*         u_ptUpCounter[MAX_COUNTER];

DWORD		u_dwRoutineCounter = 0;                   
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void        Init_Timer()
{

	#ifdef PART_TM4C129XNCZAD
	//======
	//	Set System Clock (For M4 Tiva system, system run from PLL at 120MHz)
	//======
	g_dwSystemClock = 	MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                         SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                         SYSCTL_CFG_VCO_480), 120000000);
	#endif

	#ifdef PART_TM4C1230H6PM

	SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	g_dwSystemClock = SysCtlClockGet();

	#endif

	#ifdef PART_TM4C1294KCPDT
	//======
	//	Set System Clock (For M4 Tiva system, system run from PLL at 120MHz)
	//======
	g_dwSystemClock = 	MAP_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                     SYSCTL_OSC_MAIN | SYSCTL_USE_PLL |
                                     SYSCTL_CFG_VCO_480), 120000000);
	#endif
    //+++++
    //  Set Systick
    //+++++
    SysTickPeriodSet(g_dwSystemClock/1000);
    SysTickIntEnable(); 
     
    SysTickEnable();
}
/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------------+
|  Function : SysTick_ISR()             q                                              |
|  Task     : Timer ISR                                                               |
+-------------------------------------------------------------------------------------+
|  Call     : SysTick_ISR()                                                           |
|                                                                                     |
|  Parameter: proutine锟紾which shoude be Added into the List                           |
|  Return   : None                                                                    |
+------------------------------------------------------------------------------------*/
void        SysTick_ISR(void)
{
	int		i;
	WORD	wType;
	CTask	*pCTaskList;

	pCTaskList = g_paTaskList;
	wType = (&TaskLwipUdp)->wType;

	if(g_wRunFlag || g_cStepNexttaskFalg)
	{
		g_dwSystemTick++;     //system time tick count up
		g_dwBKPTScanCnt++;
		g_cStepNexttaskFalg = 0; //单步调试时，若当前的task里所有元件step扫描完成，系统所有参数加1。该情况出现一次累加1次
		//+++++
		//  Timer Counter up & down base on Time Slice(default=1)
		//+++++
		for (i=0; i<u_nDnCounter; i++)
			*u_ptDnCounter[i] -= TIMER_SLICE;

		for (i=0; i<u_nUpCounter; i++)
			*u_ptUpCounter[i] += TIMER_SLICE;
		//+++++
		//  Critical Task Routine
		//Usage notice: do not add task in timer list unless it is timing critical task.
		//Combining down/up counter with "while" loop for periodical routine. Add task in
		//this timing list will extend timer interrupt service routine and may cause system
		//performance down or even worse to crash system.
		//+++++
		for (i = 0; i < u_nRoutineCount; i++)
		{
			u_Routine[i].tCounter--;
			if (u_Routine[i].tCounter < 0)
			{
				u_Routine[i].tCounter=u_Routine[i].routine.tSlice;	//reload time slice for critical task    20151211
				if (u_Routine[i].routine.pRoutine)
					u_Routine[i].routine.pRoutine();
			}
		}
	}

	if(g_wStepFlag)
	{
		g_wStepFlag = 0;

		//debug task m_tCounter++
		for (i=MAXTASK_TYPE*wType; i<MAXTASK_TYPE*(wType+1); i++)
		{
			if ((pCTaskList+i)->m_task.pRun == (&TaskDebug[g_cCurrentDebugTask])->pRun && \
				(pCTaskList+i)->m_task.pParam == (&TaskDebug[g_cCurrentDebugTask])->pParam && \
				(pCTaskList+i)->m_task.period.tTimer == (&TaskDebug[g_cCurrentDebugTask])->period.tTimer && \
				(pCTaskList+i)->m_task.period.tCounter == (&TaskDebug[g_cCurrentDebugTask])->period.tCounter)
			{
				(pCTaskList+i)->m_tCounter += (pCTaskList+i)->m_task.period.tTimer;
				break;
			}
		}
		g_wStepRunflag = 1;
		if(g_wNext_FunctionBlockNum != 0) return;

		g_dwSystemTick++;     //system time tick count up
		g_dwBKPTScanCnt++;

		//+++++
		//  Timer Counter up & down base on Time Slice(default=1)
		//+++++
		for (i=0; i<u_nDnCounter; i++)
			*u_ptDnCounter[i] -= TIMER_SLICE;

		for (i=0; i<u_nUpCounter; i++)
			*u_ptUpCounter[i] += TIMER_SLICE;
		//+++++
		//  Critical Task Routine
		//Usage notice: do not add task in timer list unless it is timing critical task.
		//Combining down/up counter with "while" loop for periodical routine. Add task in
		//this timing list will extend timer interrupt service routine and may cause system
		//performance down or even worse to crash system.
		//+++++
		for (i = 0; i < u_nRoutineCount; i++)
		{
			u_Routine[i].tCounter--;
			if (u_Routine[i].tCounter < 0)
			{
				u_Routine[i].tCounter=u_Routine[i].routine.tSlice;	//reload time slice for critical task    20151211
				if (u_Routine[i].routine.pRoutine)
					u_Routine[i].routine.pRoutine();
			}
		}
	}
}
/*------------------------------------------------------------------------------------+
|  Function : AddRoutine(ROUTINE* proutine)                                           |
|  Task     : Add proutine into the list                                              |
+-------------------------------------------------------------------------------------+
|  Call     : AddRoutine(proutine)                                                    |
|                                                                                     |
|  Parameter: proutine锟紾which shoude be Added into the List                           |
|  Return   : None                                                                    |
+------------------------------------------------------------------------------------*/
void AddUpCounter(long* ptCounter)
{
	if (u_nUpCounter < MAX_COUNTER)
    	u_ptUpCounter[u_nUpCounter++] = ptCounter;
}

void DeleteUpCounter(long* ptCounter)
{
	int		i;
	int		j;

	for (i=0; i<u_nUpCounter; i++)
    {
    	if (u_ptUpCounter[i] == ptCounter)
    	{
    		u_nUpCounter--;
    		for (j=i; j<u_nUpCounter; j++) u_ptUpCounter[j] = u_ptUpCounter[j+1];
        	break;
    	}
    }
}

void	    AddRoutine(ROUTINE* proutine)
{
//    INT_T       nLock;
//    nLock   =   Hw_Int_Disable();
	if (u_nRoutineCount < MAX_ROUTINE)   //limit for MAX. 5 critical tasks
    {
    	u_Routine[u_nRoutineCount].routine    = *proutine;
    	u_Routine[u_nRoutineCount++].tCounter = proutine->tSlice;
    }
    
//    Hw_Int_Enable(nLock);
}
/*------------------------------------------------------------------------------------+
|  Function : DeleteRoutine(ROUTINE* proutine)                                        |
|  Task     : Delete proutine from the list                                           |
+-------------------------------------------------------------------------------------+
|  Call     : DeleteRoutine(proutine)                                                 |
|                                                                                     |
|  Parameter: proutine锟紾which shoude be deleted from the List                         |
|  Return   : None                                                                    |
+------------------------------------------------------------------------------------*/
void	    DeleteRoutine(ROUTINE* proutine)
{
	INT_T		i;
	INT_T		j;
 //   INT_T       nLock;
//    nLock   =   Hw_Int_Disable();
    
	for (i=0; i<u_nRoutineCount; i++)
    {
    	if (u_Routine[i].routine.pRoutine == proutine->pRoutine)
    	{
    		u_nRoutineCount--;
    		for (j=i; j<u_nRoutineCount; j++) u_Routine[j] = u_Routine[j+1];
        	break;
    	}
    }
//    Hw_Int_Enable(nLock);
}
/*------------------------------------------------------------------------------------+
|  Function : _AddDnCounter(long* ptCounter)                                          |
|  Task     : Add Down Timer                                                          |
+-------------------------------------------------------------------------------------+
|  Call     : _AddDnCounter(ptCounter)                                                |
|                                                                                     |
|  Parameter:                                                                         |
|  Return   : None                                                                    |
+------------------------------------------------------------------------------------*/
void	    AddDnCounter(long* ptCounter)
{
//    INT_T       nLock;
 //   nLock   =   Hw_Int_Disable();
    
	if (u_nDnCounter < MAX_COUNTER)
    	u_ptDnCounter[u_nDnCounter++] = ptCounter;
    	
//    Hw_Int_Enable(nLock);
}
/*------------------------------------------------------------------------------------+
|  Function : _DeleteDnCounter(long* ptCounter)                                       |
|  Task     : Delete Down Timer                                                       |
+-------------------------------------------------------------------------------------+
|  Call     : _DeleteDnCounter(ptCounter)                                             |
|                                                                                     |
|  Parameter:                                                                         |
|  Return   : None                                                                    |
+------------------------------------------------------------------------------------*/
void	    DeleteDnCounter(long* ptCounter)
{
	INT_T		i;
	INT_T		j;

//	INT_T       nLock;
 //   nLock   =   Hw_Int_Disable();

	for (i=0; i<u_nDnCounter; i++)
    {
    	if (u_ptDnCounter[i] == ptCounter)
    	{
    		u_nDnCounter--;
    		for (j=i; j<u_nDnCounter; j++) u_ptDnCounter[j] = u_ptDnCounter[j+1];
        	break;
    	}
    }
//    Hw_Int_Enable(nLock);
}



void        CalCpuLoadStart(CPULOAD    *pCpuLoad)
{
    if (pCpuLoad->bFlag)        return;
        
    pCpuLoad->dwStartTick   =   SysTickValueGet();
    pCpuLoad->bFlag         =   1;
}


void        CalCpuLoadEnd(CPULOAD    *pCpuLoad)
{
    if (!pCpuLoad->bFlag)        return;
    pCpuLoad->bFlag         =   0;
    
    pCpuLoad->dwEndTick     =   SysTickValueGet();
    
    if (pCpuLoad->dwEndTick > pCpuLoad->dwStartTick)
        pCpuLoad->dwRunTick = SysTickPeriodGet() + pCpuLoad->dwStartTick - pCpuLoad->dwEndTick;
    else
        pCpuLoad->dwRunTick = pCpuLoad->dwStartTick - pCpuLoad->dwEndTick;
        
    if (pCpuLoad->dwRunTick > pCpuLoad->dwMaxRunTick) 
        pCpuLoad->dwMaxRunTick  =  pCpuLoad->dwRunTick;   
        
}

void Delay (long lDelayTime)
{
	unsigned int i;                                     
	while( --lDelayTime != 0)
	{
		for(i = 0; i < 600; i++);                      
	}
}
/*---------------------------------------------------------------------------+
|           Helpers                                                          |
+---------------------------------------------------------------------------*/

