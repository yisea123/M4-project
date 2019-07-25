/*===========================================================================+
|  Class    : Timer                                                          |
|  Task     : Timer Header File                                              |
|----------------------------------------------------------------------------|
|  Compile  : C320 V6.60 -                                                   |
|  Link     : C320 V6.60 -                                                   |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Wain.Wei                                                       |
|  Version  : V1.00                                                          |
|  Creation : 03/16/2011                                                     |
|  Revision :                                                                |
+===========================================================================*/
#ifndef     D___TIMER
#define     D___TIMER

#ifdef __cplusplus
extern "C" {
#endif

#include	"kernel/include/type.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define     MAX_COUNTER         64
#define     MAX_ROUTINE         32
#define     TIMER_SLICE			1	    // time tick = 1ms
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tagROUTINE
{
	TINT_T    	tSlice;                     // The period in 1/1024 second to execute the routine
	void    	(*pRoutine)();              // Pointer to user defined routine
} ROUTINE;

typedef     struct  tagCPULOAD
{
    BOOL_T        	bFlag;
    DWORD_T       	dwStartTick;
    DWORD_T       	dwEndTick;    
    DWORD_T       	dwRunTick;
    DWORD_T       	dwMaxRunTick;
} CPULOAD;
/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/


/*===========================================================================+
|           Class implementation - TIMER                                     |
+===========================================================================*/
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void        Init_Timer(void);
/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
void        AddRoutine   (ROUTINE* proutine);   // Can not execute AddRountine() and DeleteRoutine() inside the routine
void        DeleteRoutine(ROUTINE* proutine);   // Can not execute AddRountine() and DeleteRoutine() inside the routine

void 		AddDnCounter   (long* ptCounter);
void 		DeleteDnCounter(long* ptCounter);

void 		AddRoutine   (ROUTINE* proutine);   /* Can not execute AddRountine() and DeleteRoutine() inside the routine */
void 		DeleteRoutine(ROUTINE* proutine);   /* Can not execute AddRountine() and DeleteRoutine() inside the routine */

void 		AddUpCounter(long* ptCounter);
void 		DeleteUpCounter(long* ptCounter);

void	    AddDnCounter(LONG_T* ptCounter);
void	    DeleteDnCounter(LONG_T* ptCounter);

void        SysTick_ISR(void);
void 		Delay(long lDelayTime);

void        CalCpuLoadStart(CPULOAD  *pCpuLoad);
void        CalCpuLoadEnd(CPULOAD    *pCpuLoad);

void Delay (long lDelayTime);
/*===========================================================================+
|           Externals                                                        |
+===========================================================================*/
extern      DWORD_T	    	g_dwSystemTick;
extern      DWORD_T       g_dwSystemClock;
extern      DWORD_T	    	g_dwSystemTick;
extern      DWORD_T       	g_dwSystemClock;
extern      WORD_T			g_wRunFlag;
extern      WORD_T			g_wStepFlag;
extern      WORD_T			g_wStepRunflag;

#ifdef __cplusplus
}
#endif

#endif
