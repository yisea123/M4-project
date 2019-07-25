/*===========================================================================+
|  Class    : Header		                                                 |
|  Task     : 									                             |
|----------------------------------------------------------------------------|
|  Compile  : 				                                                 |
|  Link     : 				                                                 |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : 			                                                     |
|  Version  : V1.00                                                          |
|  Creation : 			                                                     |
|  Revision :                                                                |
+===========================================================================*/

#ifndef     D__APP
#define     D__APP

#include "kernel/include/type.h"
#include "kernel/include/task.h"
#include "driver/include/_timer.h"
/**
 * This file is used for task schedule and message deal.
 */

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define     CMD_MAX_CHANNEL     4
#define     CMD_MAX_MESSAGE     8

#define     CMD_SYS             0x0000
#define     CMD_KEY             0x0001
#define     CMD_232             0x0002

#define     _SYS_COMMAND        0x0000
#define     _SYS_DELETE_TASK    0x0001
#define     _SYS_DELETE_ROUTINE 0x0002
#define     _SYS_RESET          0xFFFE
#define     _SYS_EXIT           0xFFFF

#define     _KEY_ADDHEAD        0x0001
#define     _KEY_ADDTAIL        0x0002

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tagMSG
{
    WORD    wMessage;
    union
    {
        DWORD       dwParam;
        WORD        wKey;
        WORD        wCmd;
        TINT        tCounter;
        ROUTINE*    proutine;
	} PARAM;
} MSG;

typedef struct tagMSGQUEUE
{
    WORD    wInput;
    WORD    wOutput;
    WORD    wQueueEnd;
    BOOL    bQueueFull;
    MSG     msgQueue[CMD_MAX_MESSAGE];
} MSGQUEUE;

/*===========================================================================+
|           Function and Class prototype                                     |
+===========================================================================*/

/*===========================================================================+
|           Class declaration - Application                                  |
+===========================================================================*/
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void Create_App(void); /* Insert Run_TimerintTask() into AddRoutine() */
void Destroy_App(void);
    
/*---------------------------------------------------------------------------+
|           Attributes                                                       |
+---------------------------------------------------------------------------*/
typedef struct tagCApp
{
	HTASK       m_htaskCurrent;
    MSGQUEUE    m_msg[CMD_MAX_CHANNEL];
} CApp;

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
void Run_App(void);
BOOL SetMessage_App(WORD wChannel, MSG* pmsg);
BOOL GetMessage_App(WORD wChannel, MSG* pmsg);

#endif
