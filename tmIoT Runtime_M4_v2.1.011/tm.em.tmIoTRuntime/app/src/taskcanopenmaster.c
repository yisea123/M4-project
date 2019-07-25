/*===========================================================================+
|  Function : Can Master Application                                         |
|  Task     : Can Master Application Source File                             |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   :                                                                |
|  Version  : V1.00                                                          |
|  Creation : 2016.3.5                                                       |
|  Revision :                                                                |
+===========================================================================*/
#include "driver/include/_timer.h"
#include "driver/include/_ecan.h"
#include "taskcanopenmaster.h"
#include "taskcanopenslave.h"


#include "CanFunctionBlock.h"
NodeMsg  NodeMsgP;

HTASK	TASK_MASTER=HTASK_NULL;
HANDLE_T HandlePort=NULL;

BYTE	test_upload = 0;
//CANOPEN_OD_DATA_T watch_can_value[2][20];
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
HANDLE_T                g_hCAN0;
static void UpdateCanMIdList(BYTE port);
static void	FindOdNum(BYTE HardPort);
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
void sendSDO(
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_DWORD_T         dwIndex,
        CANOPEN_BYTE_T          pbySize,
        void                    *pBuf);
void SDO_upload_set(BYTE port, CANOPEN_BYTE_T byNodeID, CANOPEN_DWORD_T dwIndex);
void PDO_MTS(
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          pbySize,
        void                    *pBuf);
void PDO_STM(
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          pbySize,
        void                    *pBuf);

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/
CANOPEN_OD_VALUE_T * CanValueStoreM[HardDeviceNum]={NULL,NULL};
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
CANOPEN_OD_T            u_CanMaster_Od[HardDeviceNum][CanNumMax];
CANOPEN_T               g_CanMaster[HardDeviceNum];
CANOPEN_LINK_T          u_CanMasterLink[HardDeviceNum][CanNumMax];
HANDLE_T    g_hCAN1;
/*===========================================================================+
|           Helpers  Implement                                               |
+===========================================================================*/
void CanMaster_HeatbeatFunc   (   CANOPEN_T           *pCanopen ,
                                  CANOPEN_LINK_T      *pLink  );
void CanMaster_SetSlaveData    (CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);

//static	void Create_CanMaster(void);
static	void RunCanopenMaster(void *vpParam);
//static  void CanMaster_SetLinkPdo(CANOPEN_LINK_T *pLink, CANOPEN_SDO_COMM_TBL_T  *pTable, CANOPEN_WORD_T wNumber);
/*===========================================================================+
|           Inner Global variable                                            |
+===========================================================================*/
//+++++
//  Can device
//+++++
CANOPEN_DEV_FRAME_T u_aMasterTxCanBuf[HardDeviceNum][30];
CANOPEN_DEV_FRAME_T u_aMasterRxCanBuf[HardDeviceNum][30];
CANOPEN_OD_INDEX_T	*u_aCanMaster_OdPara_TblP1=NULL;
CANOPEN_OD_INDEX_T	*u_aCanMaster_OdPara_TblP2=NULL;
//CANOPEN_OD_INDEX_T  watch_p1[20];
//CANOPEN_OD_INDEX_T  watch_p2[20];
//const CANOPEN_OD_INDEX_T u_aCanMaster_OdPara_Tbl[] = {
//        {CANOPEN_ID_DEVICETYPE,         OD_DATATYPE_DWORD,      OD_ACCESS_RW,   CANOPEN_NULL},
//        {CANOPEN_ID_HEARTBEAT,          OD_DATATYPE_WORD,       OD_ACCESS_RW,   CANOPEN_NULL},
//};

CANOPEN_BYTE_T u_aCanMaster_Link_NodeID[] = {};

CANOPEN_SDO_COMM_TBL_T u_aCanMasterLink_Conf[] = {
        /* Producer Heartbeat Time */
        {0x1017, 0x0, CANOPEN_SDO_MODE_DOWNLOAD, 500,           500, 0},
};

CANOPEN_SDO_COMM_TBL_T      u_aCanMasterLink_SDO[] =
{
};

//CANOPEN_OD_DATA_T   u_aCanMaster_OdValue_Tbl[HardDeviceNum][sizeof(u_aCanMaster_OdPara_Tbl)/sizeof(CANOPEN_OD_INDEX_T)];
CANOPEN_OD_DATA_T	*u_aCanMaster_OdValue_TblP1=NULL;
CANOPEN_OD_DATA_T	*u_aCanMaster_OdValue_TblP2=NULL;
/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
//void InitCanopenMaster()
//{
//	g_hCAN1 = Device_Create(&g_CAN1_Engine);
//    Create_CanMaster();
//}

void CreateTaskCanopenMaster()
{
    TASK    Task;
    Task.wType = TASK_WHILELOOP;
    Task.period.tCounter = 0;
    Task.period.tTimer = 1;
    Task.pRun = RunCanopenMaster;
    Task.pParam = 0;
    TASK_MASTER = Create_Task(&Task, 0);
}
/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
//static	void Create_CanMaster(void)
//{
//
//}

static	void Run_TaskCanMaster(void)
{
	static CANOPEN_DWORD_T dwTick1 = 0;
	static CANOPEN_DWORD_T dwTick2 = 0;

	if(g_bCanFunctionFlag&CAN0_AS_Master)
	{
		Device_Control(g_hCAN0, CAN_CMD_RX, NULL);
        Device_Control(g_hCAN0, CAN_CMD_TX, NULL);


        if (g_dwSystemTick - dwTick1 <= 1)
        	return;
        dwTick1 = g_dwSystemTick;
        Canopen_Run(&g_CanMaster[0]);
	}
	if(g_bCanFunctionFlag&CAN1_AS_Master)
	{
		Device_Control(g_hCAN1, CAN_CMD_RX, NULL);
		Device_Control(g_hCAN1, CAN_CMD_TX, NULL);

		if (g_dwSystemTick - dwTick2 <= 1)
			return;
		dwTick2 = g_dwSystemTick;
		Canopen_Run(&g_CanMaster[1]);
	}
}

static	void RunCanopenMaster(void *vpParam)
{
//	WORD    i;
	Run_TaskCanMaster();

//	if(test_upload)
//	{
//		for(i=0;i<CanIdParmTab[0].IdNum;i++)
//			SDO_upload_set(1,CanIdParmTab[0].IdListP[i].CanNodeID, CanIdParmTab[0].IdListP[i].CanOD);
//
//		for(i=0;i<CanIdParmTab[1].IdNum;i++)
//			SDO_upload_set(2,CanIdParmTab[1].IdListP[i].CanNodeID, CanIdParmTab[1].IdListP[i].CanOD);
////		memcpy(&watch_can_value[0],u_aCanMaster_OdValue_TblP1,CanIdParmTab[0].IdNum*sizeof(CANOPEN_OD_DATA_T));
////		memcpy(&watch_can_value[1],u_aCanMaster_OdValue_TblP2,CanIdParmTab[1].IdNum*sizeof(CANOPEN_OD_DATA_T));
//	}
}

/*===========================================================================+
|           Helpers                                                          |
+===========================================================================*/
//void CanMaster_SetSlaveData(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset)
//{
//
//}

void sendSDO(
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_DWORD_T         dwIndex,
        CANOPEN_BYTE_T          pbySize,
        void                    *pBuf)
{
	int                     i;
	int 					j;
	CANOPEN_LONG_T          lOffset;
	CANOPEN_WORD_T          wIndex;
	CANOPEN_BYTE_T          bySubIndex;


	wIndex = (dwIndex >> 8) & 0xFFFF;
	bySubIndex = (dwIndex & 0xFF);

	for(i=0; i<HardDeviceNum; i++)
	{
		for (j = 0; j < CanParm[0].SlaveNum; j++)
		{
			if(byNodeID==u_CanMasterLink[i][j].byNodeID)
			{
				lOffset =  Canopen_OD_IndexToOffset(u_CanMasterLink[i][j].pObjdict, wIndex, bySubIndex);
				if(memcmp(&(u_CanMasterLink[i][j].pObjdict->pValue[lOffset].DbValue), pBuf, pbySize) == 0) return;
        		Canopen_Memcpy( &(u_CanMasterLink[i][j].pObjdict->pValue[lOffset].DbValue), pBuf, pbySize);
        		u_CanMasterLink[i][j].pObjdict->pValue[lOffset].bUpdate = CANOPEN_TRUE;
        		if(u_CanMasterLink[i][j].pObjdict->pIndex[lOffset].byAccess != OD_ACCESS_RW)
        			u_CanMasterLink[i][j].pObjdict->pValue[lOffset].byUpDown = CANOPEN_SDO_MODE_DOWNLOAD;
        		else
        			u_CanMasterLink[i][j].pObjdict->pValue[lOffset].byUpDown |= CANOPEN_SDO_MODE_DOWNLOAD;
        		return;
        	}
        }
	}
}

void SDO_upload_set(BYTE port, CANOPEN_BYTE_T byNodeID, CANOPEN_DWORD_T dwIndex)
{
	int                     i;
	CANOPEN_LONG_T          lOffset;
	CANOPEN_WORD_T          wIndex;
	CANOPEN_BYTE_T          bySubIndex;

	wIndex = (dwIndex >> 8) & 0xFFFF;
	bySubIndex = (dwIndex & 0xFF);

	if(port==HardPort1)
	{
		for (i = 0; i < CanParm[0].SlaveNum; i++) {//i < CanIdParmTab[1].IdNum
			if (u_CanMasterLink[0][i].byNodeID == byNodeID) {
				lOffset = Canopen_OD_IndexToOffset(u_CanMasterLink[0][i].pObjdict, wIndex, bySubIndex);
				u_CanMasterLink[0][i].pObjdict->pValue[lOffset].bUpdate = CANOPEN_TRUE;
				if(u_CanMasterLink[0][i].pObjdict->pIndex[lOffset].byAccess != OD_ACCESS_RW)
					u_CanMasterLink[0][i].pObjdict->pValue[lOffset].byUpDown = CANOPEN_SDO_MODE_UPLOAD;
				else
					u_CanMasterLink[0][i].pObjdict->pValue[lOffset].byUpDown |= CANOPEN_SDO_MODE_UPLOAD;
			}
		}
	}
	else if(port==HardPort2)
	{
		for (i = 0; i < CanParm[0].SlaveNum; i++) {
			if (u_CanMasterLink[1][i].byNodeID == byNodeID) {
				lOffset = Canopen_OD_IndexToOffset(u_CanMasterLink[1][i].pObjdict, wIndex, bySubIndex);
				u_CanMasterLink[1][i].pObjdict->pValue[lOffset].bUpdate = CANOPEN_TRUE;
				if(u_CanMasterLink[1][i].pObjdict->pIndex[lOffset].byAccess != OD_ACCESS_RW)
					u_CanMasterLink[1][i].pObjdict->pValue[lOffset].byUpDown = CANOPEN_SDO_MODE_UPLOAD;
				else
					u_CanMasterLink[1][i].pObjdict->pValue[lOffset].byUpDown |= CANOPEN_SDO_MODE_UPLOAD;
			}
		}
	}
}

//
//设置接收Pdo和发送Pdo参数 add at 2016/04/13
//
DWORD 	GetCanBurate(BYTE offset)
{
	DWORD burate = 0;
	switch(offset)
	{
	case 1:
		burate = BAUDRATE_2K5BPS;
		break;
	case 2:
		burate = BAUDRATE_5KBPS;
		break;
	case 3:
		burate = BAUDRATE_10KBPS;
		break;
	case 4:
		burate = BAUDRATE_20KBPS;
		break;
	case 5:
		burate = BAUDRATE_25KBPS;
		break;
	case 6:
		burate = BAUDRATE_50KBPS;
		break;
	case 7:
		burate = BAUDRATE_100KBPS;
		break;
	case 8:
		burate = BAUDRATE_125KBPS;
		break;
	case 9:
		burate = BAUDRATE_250KBPS;
		break;
	case 10:
		burate = BAUDRATE_500KBPS;
		break;
	case 11:
		burate = BAUDRATE_1MBPS;
	    break;
	default:
		burate = BAUDRATE_125KBPS;
		break;
	}
	return burate;
}


void            CanMaster_HeatbeatFunc   (   CANOPEN_T           *pCanopen ,
                                            CANOPEN_LINK_T      *pLink  )
{
	WORD		i;
    CanMaster_Nmt_SendStateChange   (   pCanopen,
                                        pLink,
                                        CANOPEN_NMT_CMD_ENTER_PREOPERATIONAL   );

    pLink->byStatus     =   CANOPEN_LINK_STATUS_INTIAL;
    pLink->Sdo.byState  =   CANOPEN_SDO_STATE_FINISHED;
    pLink->byNmtState   =   CANOPEN_STATE_PREOPERATIONAL;

    Canopen_Timer_Delete 	(	&(pLink->Sdo.Timer));
    for (i = 0; i < pLink->pObjdict->wNumber; i++)
    {
        if (pLink->pObjdict->pValue[i].bDirty == CANOPEN_TRUE)
        {
            pLink->pObjdict->pValue[i].bUpdate    =   CANOPEN_TRUE;
        }
    }
}




/****************add can function***************/
void Create_CanopenMaster(BYTE DeviceNUM)
{

     int     i;
     ECAN    ECan;
     BYTE	 offset = DeviceNUM-1;
     Canopen_Memset(u_aMasterRxCanBuf[offset], 0, sizeof(u_aMasterRxCanBuf[offset]));
     Canopen_Memset(u_aMasterTxCanBuf[offset], 0, sizeof(u_aMasterTxCanBuf[offset]));
     ECan.wRxBufferSize                      = sizeof(u_aMasterRxCanBuf[offset]);
     ECan.wTxBufferSize                      = sizeof(u_aMasterTxCanBuf[offset]);
     ECan.pRxBuffer                          = (BYTE *)(u_aMasterRxCanBuf[offset]);
     ECan.pTxBuffer                          = (BYTE *)(u_aMasterTxCanBuf[offset]);
     ECan.CanConf.wCanMode                   = CANMODE_STANDARD;
     ECan.CanConf.BaudrateConf.dwBaudrate     = GetCanBurate(CanParm[offset].CanBuadRate);//BAUDRATE_1MBPS;
     if(DeviceNUM==HardPort1)
     {
    	 Device_Open(g_hCAN0, &ECan);
    	 g_bCanFunctionFlag|=CAN0_AS_Master;
    	 g_bCanFunctionFlag&=~CAN0_AS_Slave;
    	 if(u_aCanMaster_OdValue_TblP1!=NULL)
    		 free_memory((BYTE *)u_aCanMaster_OdValue_TblP1);
    	 u_aCanMaster_OdValue_TblP1 = malloc(CanIdParmTab[offset].IdNum*sizeof(CANOPEN_OD_DATA_T));
     }
     if(DeviceNUM==HardPort2)
     {
    	 Device_Open(g_hCAN1, &ECan);
    	 g_bCanFunctionFlag|=CAN1_AS_Master;
    	 g_bCanFunctionFlag&=~CAN1_AS_Slave;
    	 if(u_aCanMaster_OdValue_TblP2!=NULL)
    	    free_memory((BYTE *)u_aCanMaster_OdValue_TblP2);
    	 u_aCanMaster_OdValue_TblP2 = malloc(CanIdParmTab[offset].IdNum*sizeof(CANOPEN_OD_DATA_T));
     }


     /*****接资料需在此之前进行******/
     //u_aCanMaster_OdPara_Tbl  CANOPEN_HEARTBEAT偏移量后需要接资料
     UpdateCanMIdList(DeviceNUM);
     for (i = 0; i < CanParm[offset].SlaveNum; i++) {
//             Canopen_Memset(u_aCanMaster_OdValue_Tbl, 0, sizeof(u_aCanMaster_OdValue_Tbl));
    	 FindOdNum(DeviceNUM);
    	 u_CanMaster_Od[offset][i].wNumber    = NodeMsgP.OffsetNum;//CanIdParmTab[offset].IdNum; //sizeof(u_aCanMaster_OdPara_Tbl)/sizeof(CANOPEN_OD_INDEX_T)+g_SlaveDataNum[i];

//             if(i==0)
//            	 NodeMsgP.AllTabOffset=0;
    	 if(DeviceNUM==HardPort1)
    	 {
    		 if(i==0)
    		 {
    			 u_CanMaster_Od[offset][i].pIndex = &u_aCanMaster_OdPara_TblP1[0];
    			 if(u_aCanMaster_OdValue_TblP1!=NULL)
    				 u_CanMaster_Od[offset][i].pValue = &u_aCanMaster_OdValue_TblP1[0];
    		 }
    		 else
    		 {
    			 u_CanMaster_Od[offset][i].pIndex = &u_aCanMaster_OdPara_TblP1[NodeMsgP.AllTabOffset];
    			 if(u_aCanMaster_OdValue_TblP1!=NULL)
    				 u_CanMaster_Od[offset][i].pValue = &u_aCanMaster_OdValue_TblP1[NodeMsgP.AllTabOffset];
    		 }
    	 }
    	 else if(DeviceNUM==HardPort2)
    	 {
    		 if(i==0)
    		 {
    			 u_CanMaster_Od[offset][i].pIndex = &u_aCanMaster_OdPara_TblP2[0];
    			 if(u_aCanMaster_OdValue_TblP2!=NULL)
    				 u_CanMaster_Od[offset][i].pValue = &u_aCanMaster_OdValue_TblP2[0];
    		 }
    		 else
    		 {
    			 u_CanMaster_Od[offset][i].pIndex = &u_aCanMaster_OdPara_TblP2[NodeMsgP.AllTabOffset];
    			 if(u_aCanMaster_OdValue_TblP2!=NULL)
    				 u_CanMaster_Od[offset][i].pValue = &u_aCanMaster_OdValue_TblP2[NodeMsgP.AllTabOffset];
    		 }
    	 }
	}

     // Create Canopen
     Canopen_Memset(&g_CanMaster[offset], 0, sizeof(g_CanMaster[offset]));
     g_CanMaster[offset].CanDev      = CANOPEN_MASTER;
     g_CanMaster[offset].byCliServ   = CANOPEN_CLIENT;
     g_CanMaster[offset].byNodeID    = 0;
     Canopen_Create(&g_CanMaster[offset]);
     // Create link
     memset(&NodeMsgP,0,sizeof(NodeMsgP));
     for (i = 0; i < CanParm[offset].SlaveNum; i++) {
         	 FindOdNum(DeviceNUM);
             Canopen_Memset(&u_CanMasterLink[offset][i], 0, sizeof(u_CanMasterLink[offset][i]));
             u_CanMasterLink[offset][i].Heartbeat.wTime      = 3000;//2000;
             u_CanMasterLink[offset][i].Heartbeat.TimerFunc     = CanMaster_HeatbeatFunc;//NULL;
             Canopen_Link_Create(&g_CanMaster[offset], &u_CanMasterLink[offset][i] , NodeMsgP.ID, &u_CanMaster_Od[offset][i]);
             Canopen_Sdo_AddConfComm(
                             &g_CanMaster[offset],
                             &u_CanMasterLink[offset][i],
                             u_aCanMasterLink_Conf,
                             sizeof(u_aCanMasterLink_Conf)/sizeof(CANOPEN_SDO_COMM_TBL_T));
     }
     memset(&NodeMsgP,0,sizeof(NodeMsgP));
}

static void UpdateCanMIdList(BYTE port)
{
	WORD i;
//	BYTE	*Patch = NULL;
//	CANOPEN_OD_INDEX_T *point = NULL;
	if(port==HardPort1)
	{
		if(u_aCanMaster_OdPara_TblP1!=NULL)
			free_memory((BYTE *)u_aCanMaster_OdPara_TblP1);
		u_aCanMaster_OdPara_TblP1  = (CANOPEN_OD_INDEX_T *)(malloc(CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T)));
	    if(u_aCanMaster_OdPara_TblP1!=NULL)
	    {
			for(i=0; i<CanIdParmTab[port-1].IdNum; i++)
			{
				u_aCanMaster_OdPara_TblP1[i].dwIndex = CanIdParmTab[port-1].IdListP[i].CanOD;
				if(CanIdParmTab[port-1].IdListP[i].DataType==DWORD_SEVER)
				{
					CanIdParmTab[port-1].IdListP[i].DataType=OD_DATATYPE_DWORD;
					u_aCanMaster_OdPara_TblP1[i].byDataType = OD_DATATYPE_DWORD;//CanIdParmTab[i].IdListP->DataType;
				}
				else
					u_aCanMaster_OdPara_TblP1[i].byDataType = CanIdParmTab[port-1].IdListP[i].DataType;//read datatype from flash
				u_aCanMaster_OdPara_TblP1[i].byAccess = CanIdParmTab[port-1].IdListP[i].Pemission;//CanIdParmTab[i].IdListP->Pemission;
				u_aCanMaster_OdPara_TblP1[i].WriteFunc = CANOPEN_NULL;
			}
	    }
//	    memcpy(watch_p1,(BYTE*)&u_aCanMaster_OdPara_TblP1[0],CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T));
	}
	else if(port==HardPort2)
	{
		if(u_aCanMaster_OdPara_TblP2!=NULL)
			free_memory((BYTE *)u_aCanMaster_OdPara_TblP2);
		u_aCanMaster_OdPara_TblP2  = (CANOPEN_OD_INDEX_T *)(malloc(CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T)));
		if(u_aCanMaster_OdPara_TblP2!=NULL)
		{
			for(i=0; i<CanIdParmTab[port-1].IdNum; i++)
			{
				u_aCanMaster_OdPara_TblP2[i].dwIndex = CanIdParmTab[port-1].IdListP[i].CanOD;
				if(CanIdParmTab[port-1].IdListP[i].DataType==DWORD_SEVER)
				{
					CanIdParmTab[port-1].IdListP[i].DataType=OD_DATATYPE_DWORD;
					u_aCanMaster_OdPara_TblP2[i].byDataType = OD_DATATYPE_DWORD;//CanIdParmTab[i].IdListP->DataType;
			    }
				else
					u_aCanMaster_OdPara_TblP2[i].byDataType = CanIdParmTab[port-1].IdListP[i].DataType;//read datatype from flash

				u_aCanMaster_OdPara_TblP2[i].byAccess = CanIdParmTab[port-1].IdListP[i].Pemission;//CanIdParmTab[i].IdListP->Pemission;
				u_aCanMaster_OdPara_TblP2[i].WriteFunc = CANOPEN_NULL;
			}
		}
//		memcpy(watch_p2,(BYTE*)&u_aCanMaster_OdPara_TblP2[0],CanIdParmTab[port-1].IdNum*sizeof(CANOPEN_OD_INDEX_T));
	}
}



static void	FindOdNum(BYTE HardPort)
{
	WORD	i;
	BYTE    cpflag=0;
	BYTE	SearchFlag=1;
	//static  CntFlag=0;
	NodeMsgP.OffsetNum = 0;
	for(i=0; i<CanIdParmTab[HardPort-1].IdNum; i++)
	{
		if(SearchFlag)
		{
			if(NodeMsgP.ID>=CanIdParmTab[HardPort-1].IdListP[i].CanNodeID) continue;
			if(NodeMsgP.ID!= CanIdParmTab[HardPort-1].IdListP[i].CanNodeID)
			{
				NodeMsgP.ID = CanIdParmTab[HardPort-1].IdListP[i].CanNodeID;
				SearchFlag = 0;
				cpflag = 1;
			}
		}

		if(cpflag)
		{
			if(NodeMsgP.ID==CanIdParmTab[HardPort-1].IdListP[i].CanNodeID)
				NodeMsgP.OffsetNum++;
			if(NodeMsgP.ID!=CanIdParmTab[HardPort-1].IdListP[i].CanNodeID)
			{
				NodeMsgP.AllTabOffset = i-2;
				cpflag = 0;
				break;
			}
			if(i==CanIdParmTab[HardPort-1].IdNum-1)
			{
				NodeMsgP.AllTabOffset = i;
				cpflag = 0;
				break;
			}
		}
	}
}
