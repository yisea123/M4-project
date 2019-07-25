/*==========================================================================+
|  File     : main.c                                                        |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : YW                                                          	|
|  Version  : V2.2                                                          |
|  Creation : 2019/02/26                                                    |
|  Revision :                                                               |
+==========================================================================*/
#include "driver/include/_timer.h"
#include "driver/include/_addr.h"
#include "kernel/include/app.h"
#include "kernel/include/type.h"
#include "kernel/include/device.h"
#include "kernel/include/task.h"
#include "tasklwiptcp.h"
#include "tasklwipudp.h"
#include "InitDevice.h"
#include "task_Runtime.h"
#include "task_IO_Acquisition.h"
#include "taskcanopenmaster.h"
#include "taskcanopenslave.h"
#include "userlib.h"
#include "config.h"
#include "CanFunctionBlock.h"
#include "taskrs485.h"
#include "taskiNet.h"
/*==============================================================================+
|           Content                                                             |
+==============================================================================*/
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
/*==============================================================================+
|           Global Variables                                                    |
+==============================================================================*/
/*=============================================================================+
|           Helpers  Implement                                                 |
+=============================================================================*/
static void InitDevice(void);
static void CreateTask(void);
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
int main()
{
    Init_Timer();
    Init_Task();

    Read_ModbusData_FromFlash();
    Read_iNetModeMeg_FromFlash();
    Read_ModbusTCPData_FromFlash();
	InitDevice();

	Read_ObjectListData_FromFlash();
	ReadCanDataFromErprom();

	CreateTask();

	while (1)
	{
		Run_App();
	}
}
/*---------------------------------------------------------------------------+
|  Function : Initial Device                                                 |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
static void InitDevice()
{
#ifdef USB
	Init_USB();
#endif
#ifdef RS485
	InitMulRS485();
#endif
	Init_tmIoT();
    Read_MacAddr_FromEeprom();
#if (!defined IO2C1111) && (!defined IO2C1112) && (!defined S2C4111)
	InitLwipUdp();
#endif
}
/*---------------------------------------------------------------------------+
|  Function :  Create task                                                   |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
static void CreateTask()
{
#ifdef USB
    CreateTaskUSB();
#endif
#ifdef RS485
    CreateMulTaskRS485();
#endif
#if (!defined IO2C1111) && (!defined IO2C1112) && (!defined S2C4111)
    //如果只有一个网口，那么iNet和modbus TCP使用时互斥存在，所以需要在烧录模式或者保存至flash时需要修改对方的ram或flash里的flag
    //如果iNet和modbus TCP可以共存，那么就不需要修改对方的flag
    CreateLwipTcp();
	CreateLwipUdp();
#endif
	Create_TaskIOAcquisition();
	Create_Idataconf();
}
