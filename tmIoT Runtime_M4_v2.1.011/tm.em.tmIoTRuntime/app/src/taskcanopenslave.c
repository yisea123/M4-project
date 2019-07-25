/*===========================================================================+
|  Function : Can Slave Application                                          |
|  Task     : Can Slave Application Source File                              |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : YW                                                             |
|  Version  : V2.00                                                          |
|  Creation : 2018/01/19                                                     |
|  Revision :                                                                |
+===========================================================================*/
#include "driver/include/_timer.h"
#include "driver/include/_ecan.h"
#include "driver/include/_output.h"
#include "driver/include/_addr.h"
#include "kernel/include/task.h"
#include "comlib.h"
//#include "ledapp.h"
#include "database.h"
#include "taskcanopenslave.h"
#include "studioprotocol.h"

#include "CanFunctionBlock.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
HTASK	TASK_SLAVE=HTASK_NULL;
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
static void UpdateCanSIdList(BYTE port);
/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/
CANOPEN_T                g_CanSlave[HardDeviceNum];
CANOPEN_LINK_T           u_CanSlaveLink[HardDeviceNum];
CANOPEN_OD_VALUE_T * 	 CanValueStoreS[HardDeviceNum]={NULL,NULL};
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
//HANDLE_T    g_hOutput;
//extern HANDLE_T g_hRS485;
//static BYTE CAN_byPara_Buff[12] = {0};
BYTE_T                  g_CAN_SlavePara_Buff[4] = {0};
CANOPEN_OD_T            u_CanSlave_Od[HardDeviceNum];
//CANOPEN_T               g_CanSlave[HardDeviceNum];
//CANOPEN_LINK_T          u_CanSlaveLink[HardDeviceNum];
HANDLE_T                g_hCAN0;

CAN_PARAM_CONFIG CAN_Param_Config = {0};
CAN_PARAM_CONFIG *pParam_Config_CAN = &CAN_Param_Config;
HTASK         g_htask_CanopenSlave;

/*===========================================================================+
|           Helpers  Implement                                               |
+===========================================================================*/
//void CanSlave_SetLinkRxPdoPara(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);
//void CanSlave_SetLinkRxPdoMap (CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);
//void CanSlave_SetLinkTxPdoPara(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);
//void CanSlave_SetLinkTxPdoMap (CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);

void    Create_CanSlave(void);
static  void Run_TaskCanSlave(void);
static  void RunCanopenSlave(void *vpParam);
//static  CAN_PARAM_CONFIG *Can_Select_Param(CAN_PARAM_CONFIG *pParam_Config, BYTE *Read_Eeprombuffer);

//void CanSlave_SetDBData(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);

//void RunOutput(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);
/*===========================================================================+
|           Inner Global variable                                            |
+===========================================================================*/
long u_lCounter_CanLEDIndicator = 0;
//+++++
//  Can device
//+++++
CANOPEN_DEV_FRAME_T u_aSlaveTxCanBuf[HardDeviceNum][30];
CANOPEN_DEV_FRAME_T u_aSlaveRxCanBuf[HardDeviceNum][30];
CANOPEN_OD_INDEX_T	*u_aCanSlave_OdPara_TblP1;
CANOPEN_OD_INDEX_T	*u_aCanSlave_OdPara_TblP2;

CANOPEN_OD_DATA_T	*u_aCanSlave_OdValue_TblP1;
CANOPEN_OD_DATA_T	*u_aCanSlave_OdValue_TblP2;
/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
//void InitCanopenSlave()
//{
//    g_hCAN0 = Device_Create(&g_CAN0_Engine);
//    Create_CanSlave();
//}

void CreateTaskCanopenSlave()
{
    TASK    Task;
    Task.wType = TASK_WHILELOOP;
    Task.period.tCounter = 0;
    Task.period.tTimer = 1;
    Task.pRun = RunCanopenSlave;
    Task.pParam = 0;
    TASK_SLAVE = Create_Task(&Task, 0);
}

static  void RunCanopenSlave(void *vpParam)
{
    Run_TaskCanSlave();
}
/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
//void Create_CanSlave(void)
//{
//
//}

static  void Run_TaskCanSlave(void)
{
        static CANOPEN_DWORD_T dwTick1 = 0;
        static CANOPEN_DWORD_T dwTick2 = 0;

        if(g_bCanFunctionFlag&CAN0_AS_Slave)
        {
            Device_Control(g_hCAN0, CAN_CMD_RX, NULL);
            Device_Control(g_hCAN0, CAN_CMD_TX, NULL);


            if (g_dwSystemTick - dwTick1 <= 1)
            	    return;
            dwTick1 = g_dwSystemTick;
            Canopen_Run(&g_CanSlave[0]);
        }

        if(g_bCanFunctionFlag&CAN1_AS_Slave)
        {
    		Device_Control(g_hCAN1, CAN_CMD_RX, NULL);
    		Device_Control(g_hCAN1, CAN_CMD_TX, NULL);

    	    if (g_dwSystemTick - dwTick2 <= 1)
    			return;
    	    dwTick2 = g_dwSystemTick;
    	    Canopen_Run(&g_CanSlave[1]);
        }
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
//void CanSlave_SetLinkRxPdoPara(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset)
//{
//
//}

//void CanSlave_SetLinkRxPdoMap(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset)
//{
//
//}

//void CanSlave_SetLinkTxPdoPara(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset)
//{
//
//}

//void CanSlave_SetLinkTxPdoMap(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset)
//{
//
//}

void Create_CanopenSlave(BYTE	DeviceNUM)
{
	   ECAN    ECan;
	   BYTE	 offset = DeviceNUM-1;
	   Canopen_Memset(u_aSlaveRxCanBuf[offset], 0, sizeof(u_aSlaveRxCanBuf[offset]));
	   Canopen_Memset(u_aSlaveTxCanBuf[offset], 0, sizeof(u_aSlaveTxCanBuf[offset]));
	   ECan.wRxBufferSize                      = sizeof(u_aSlaveRxCanBuf[offset]);
	   ECan.wTxBufferSize                      = sizeof(u_aSlaveTxCanBuf[offset]);
	   ECan.pRxBuffer                          = (BYTE *)u_aSlaveRxCanBuf[offset];
	   ECan.pTxBuffer                          = (BYTE *)u_aSlaveTxCanBuf[offset];
	   ECan.CanConf.wCanMode                   = CANMODE_STANDARD;
	   ECan.CanConf.BaudrateConf.dwBaudrate     = GetCanBurate(CanParm[offset].CanBuadRate);//BAUDRATE_1MBPS;//GetBuadRateValue(CanParm[offset].CanBuadRate);//BAUDRATE_1MBPS;
	   if(DeviceNUM==HardPort1)
	   {
	    	 Device_Open(g_hCAN0, &ECan);
	    	 g_bCanFunctionFlag|=CAN0_AS_Slave;
	    	 g_bCanFunctionFlag&=~CAN0_AS_Master;
	   }
	   else if(DeviceNUM==HardPort2)
	   {
		   	 Device_Open(g_hCAN1, &ECan);
		     g_bCanFunctionFlag|=CAN1_AS_Slave;
		     g_bCanFunctionFlag&=~CAN1_AS_Master;
	   }
	   // Create Canopen
//	   Canopen_Memset(u_aCanSlave_OdValue_Tbl, 0, sizeof(u_aCanSlave_OdValue_Tbl));
	   UpdateCanSIdList(DeviceNUM);
	   u_CanSlave_Od[offset].wNumber   = CanIdParmTab[offset].IdNum;//sizeof(u_aCanSlave_OdPara_Tbl)/sizeof(CANOPEN_OD_INDEX_T);
	   if(DeviceNUM==HardPort1)
	   {
		   u_CanSlave_Od[offset].pIndex    = u_aCanSlave_OdPara_TblP1;
		   if(u_aCanSlave_OdValue_TblP1!=NULL)
			   free_memory((BYTE *)u_aCanSlave_OdValue_TblP1);
		   u_aCanSlave_OdValue_TblP1 = malloc(CanIdParmTab[offset].IdNum*sizeof(CANOPEN_OD_DATA_T));
		   memset((BYTE*)u_aCanSlave_OdValue_TblP1,0,CanIdParmTab[offset].IdNum*sizeof(CANOPEN_OD_DATA_T));
		   if(u_aCanSlave_OdValue_TblP1!=NULL)
			   u_CanSlave_Od[offset].pValue    = u_aCanSlave_OdValue_TblP1;
	   }
	   else if(DeviceNUM==HardPort2)
	   {
		   u_CanSlave_Od[offset].pIndex    = u_aCanSlave_OdPara_TblP2;
		   if(u_aCanSlave_OdValue_TblP2!=NULL)
			   free_memory((BYTE *)u_aCanSlave_OdValue_TblP2);
		   u_aCanSlave_OdValue_TblP2 = malloc(CanIdParmTab[offset].IdNum*sizeof(CANOPEN_OD_DATA_T));
		   memset((BYTE*)u_aCanSlave_OdValue_TblP2,0,CanIdParmTab[offset].IdNum*sizeof(CANOPEN_OD_DATA_T));
		   if(u_aCanSlave_OdValue_TblP2!=NULL)
			   u_CanSlave_Od[offset].pValue    = u_aCanSlave_OdValue_TblP2;
	   }


	   Canopen_Memset(&g_CanSlave[offset], 0, sizeof(g_CanSlave[offset]));
	   g_CanSlave[offset].CanDev      = CANOPEN_SLAVE;
	   g_CanSlave[offset].pObjdict    = &u_CanSlave_Od[offset];
	   g_CanSlave[offset].byCliServ   = CANOPEN_SERVER;
	   g_CanSlave[offset].byNodeID    = CanParm[offset].CanID;//_Get_NodeID();
	   Canopen_Create(&g_CanSlave[offset]);

	   // Create link
	   Canopen_Memset(&u_CanSlaveLink[offset], 0, sizeof(u_CanSlaveLink[offset]));
	   u_CanSlaveLink[offset].Heartbeat.wTime = 500;
	   Canopen_Link_Create(&g_CanSlave[offset], &u_CanSlaveLink[offset], CanParm[offset].CanID, &u_CanSlave_Od[offset]);
}


static void UpdateCanSIdList(BYTE port)
{
			WORD i;
//			BYTE	*Patch = NULL;
//			CANOPEN_OD_INDEX_T *point=NULL;
			if(port==HardPort1)
			{
				if(u_aCanSlave_OdPara_TblP1)
					free_memory((BYTE *)u_aCanSlave_OdPara_TblP1);
				u_aCanSlave_OdPara_TblP1  = (CANOPEN_OD_INDEX_T *)(malloc(CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T)));

			    if(u_aCanSlave_OdPara_TblP1!=NULL)
			    {
			    	memset((BYTE*)u_aCanSlave_OdPara_TblP1,0,CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T));
					for(i=0; i<CanIdParmTab[port-1].IdNum; i++)
					{
						u_aCanSlave_OdPara_TblP1[i].dwIndex = CanIdParmTab[port-1].IdListP[i].CanOD;
						if(CanIdParmTab[port-1].IdListP[i].DataType==DWORD_SEVER)
						{
							CanIdParmTab[port-1].IdListP[i].DataType=OD_DATATYPE_DWORD;
							u_aCanSlave_OdPara_TblP1[i].byDataType = OD_DATATYPE_DWORD;//CanIdParmTab[i].IdListP->DataType;
						}//add other datatypoe at here
						else
							u_aCanSlave_OdPara_TblP1[i].byDataType = CanIdParmTab[port-1].IdListP[i].DataType;//read datatype from flash
						u_aCanSlave_OdPara_TblP1[i].byAccess = CanIdParmTab[port-1].IdListP[i].Pemission;//CanIdParmTab[i].IdListP->Pemission;
						u_aCanSlave_OdPara_TblP1[i].WriteFunc = CANOPEN_NULL;
					}
			    }
//			    memcpy(watch_p1,(BYTE*)&u_aCanSlave_OdPara_TblP1[0],CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T));
			}
			else if(port==HardPort2)
			{
				if(u_aCanSlave_OdPara_TblP2!=NULL)
					free_memory((BYTE *)u_aCanSlave_OdPara_TblP2);
				u_aCanSlave_OdPara_TblP2  = (CANOPEN_OD_INDEX_T *)(malloc(CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T)));

				if(u_aCanSlave_OdPara_TblP2!=NULL)
				{
					memset((BYTE*)u_aCanSlave_OdPara_TblP2,0,CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T));
					for(i=0; i<CanIdParmTab[port-1].IdNum; i++)
					{
						u_aCanSlave_OdPara_TblP2[i].dwIndex = CanIdParmTab[port-1].IdListP[i].CanOD;
						if(CanIdParmTab[port-1].IdListP[i].DataType==DWORD_SEVER)
						{
							CanIdParmTab[port-1].IdListP[i].DataType=OD_DATATYPE_DWORD;
							u_aCanSlave_OdPara_TblP2[i].byDataType = OD_DATATYPE_DWORD;//CanIdParmTab[i].IdListP->DataType;
					    }
						else
							u_aCanSlave_OdPara_TblP2[i].byDataType = CanIdParmTab[port-1].IdListP[i].DataType;//read datatype from flash
						u_aCanSlave_OdPara_TblP2[i].byAccess = CanIdParmTab[port-1].IdListP[i].Pemission;//CanIdParmTab[i].IdListP->Pemission;
						u_aCanSlave_OdPara_TblP2[i].WriteFunc = CANOPEN_NULL;
					}
				}
//				memcpy(watch_p2,(BYTE*)&u_aCanSlave_OdPara_TblP2[0],CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T));
			}
}
