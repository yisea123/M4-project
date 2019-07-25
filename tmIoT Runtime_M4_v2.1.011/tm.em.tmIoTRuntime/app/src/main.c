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
    //���ֻ��һ�����ڣ���ôiNet��modbus TCPʹ��ʱ������ڣ�������Ҫ����¼ģʽ���߱�����flashʱ��Ҫ�޸ĶԷ���ram��flash���flag
    //���iNet��modbus TCP���Թ��棬��ô�Ͳ���Ҫ�޸ĶԷ���flag
    CreateLwipTcp();
	CreateLwipUdp();
#endif
	Create_TaskIOAcquisition();
	Create_Idataconf();
}
