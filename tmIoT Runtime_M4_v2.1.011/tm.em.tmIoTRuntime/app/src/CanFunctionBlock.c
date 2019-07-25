/*
 * CanFunctionBlock.c
 *
 *  Created on: 2018年2月28日
 *      Author: wangyunlou
 */
#include "CanFunctionBlock.h"
#include  "studioprotocol.h"

extern BYTE	u_TaskCanFlag;

BYTE	g_InitCommunP1 =0;
BYTE	g_InitCommunP2 =0;
BYTE	g_PacketStamp = 0;

#define	   CanParmLen			16
#define	   Can1ParmOffset		0x0
#define    Can2ParmOffset		0X40013
BYTE	   g_StoreParmFlag=0;
BYTE	   g_StoreParmFlagAP=0;

WORD	mep=0;
void 	RunCanAppFunction(void *pParam);
void    RunCanMasterProtocol();
void 	SetFlashValueToCanopen(BYTE *addressS,WORD lencan1,BYTE *addressE,WORD lencan2);
/***********************type*********************************/
CanMsg      CanParm[HardDeviceNum];//8*2
CanIdParm	CanIdParmTab[HardDeviceNum];//3+12

StoreCanIdMsg	WATCH[2][300];

//StoreCanIdMsg   WP[100]={0};

BYTE	free_memory(BYTE *buf);
/*************************Function***************************/
void    Create_TaskCanFunction()
{
    TASK    Task;
    Task.wType = TASK_WHILELOOP;
    Task.period.tCounter = 0;
    Task.period.tTimer = 1;
    Task.pRun = RunCanAppFunction;
    Task.pParam = 0;
    Create_Task(&Task, 0);
}

BYTE CanDealRecvParm(BYTE parmtype, BYTE *buf)
{
	 BYTE_T  *Patch = NULL;
	 BYTE   i, port = 0;
	 static BYTE initFalg1 = 0, initFalg2 = 0;
	 static WORD w_recvOdNum = 0;

	if(parmtype==InitCanParm)
	{
		Patch = &g_byRecv_Buffer[CanDataPatchLen];

		for(i=0; i<HardDeviceNum; i++)
		{
			port = *Patch;
			if((port==HardPort1)||(port==HardPort2))
			{
				if(port==HardPort1)
				{
//					g_InitCommunP1=0;
					g_bCanFunctionFlag&=~CAN0_AS_Master;
					g_bCanFunctionFlag&=~CAN0_AS_Slave;
				}
				if(port==HardPort2)
				{
//					g_InitCommunP2=0;
					g_bCanFunctionFlag&=~CAN1_AS_Master;
				    g_bCanFunctionFlag&=~CAN1_AS_Slave;
				}
				CanParm[port-1].Configured=CanParmDwn;
				CanParm[port-1].HardPortNUM = HardPort1;
				Patch++;
				CanParm[port-1].CanMode = *Patch;
				Patch++;
				CanParm[port-1].CanID = *Patch;
				Patch++;
				CanParm[port-1].CanBuadRate= *Patch;
				Patch++;
				CanParm[port-1].CycTime = htons(*((WORD*)Patch));
				Patch = Patch+sizeof(WORD);
				CanParm[port-1].SlaveNum = *Patch;
				Patch++;
			}
		}
		initFalg1 = initFalg2 = 1;
	}
	else if(parmtype==CommunParm)
	{
		u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];
		Patch = &g_byRecv_Buffer[CanDataPatchLen];
		port = *Patch;
		if((port==HardPort1)||(port==HardPort2))
		{
			if(CanParm[port-1].Configured==CanParmDwn)
				CanParm[port-1].Configured = CanOdDwn;
			CanIdParmTab[port-1].HardPort = port;
			Patch+=sizeof(BYTE);//MODE
			Patch+=sizeof(BYTE);
			CanIdParmTab[port-1].IdNum = htons(*((WORD*) Patch));//total od length
			if(CanIdParmTab[port-1].IdNum==0)
			{
				CanParm[port-1].Configured = CanParmDwn;
				return 2;
			}
			if(initFalg1 || initFalg2)
			{
				mep = CanIdParmTab[port-1].IdNum*sizeof(StoreCanIdMsg);
				if(CanIdParmTab[port-1].IdListP!=NULL)
					free_memory((BYTE*)CanIdParmTab[port-1].IdListP);
				CanIdParmTab[port-1].IdListP=(StoreCanIdMsg *)malloc(mep);
				memset((BYTE*)CanIdParmTab[port-1].IdListP,0,mep);
				if(initFalg1) initFalg1 = 0;
				if(initFalg2) initFalg2 = 0;
				w_recvOdNum = 0;
			}
			Patch+=sizeof(WORD);//od length of this package
			BYTE currentIdNum = *Patch;
			Patch+=sizeof(BYTE);
			if(CanIdParmTab[port-1].IdListP!=NULL)
			{
				for(i=0; i<currentIdNum; i++)
				{
					CanIdParmTab[port-1].IdListP[i+w_recvOdNum].OffsetNum = htonl(*((DWORD*) Patch));
					Patch+=sizeof(DWORD);
					if(CanParm[port-1].CanMode==ModeMaster)
					{
						CanIdParmTab[port-1].IdListP[i+w_recvOdNum].CanNodeID = *Patch;
						Patch+=sizeof(BYTE);
					}
					CanIdParmTab[port-1].IdListP[i+w_recvOdNum].CanOD =  htonl(*((DWORD*) Patch));
					Patch+=sizeof(DWORD);
					CanIdParmTab[port-1].IdListP[i+w_recvOdNum].DataType = *Patch;
					Patch+=sizeof(BYTE);
					CanIdParmTab[port-1].IdListP[i+w_recvOdNum].Pemission = *Patch;
					Patch+=sizeof(BYTE);
				}
				w_recvOdNum += currentIdNum;
			}
//			if(port==1)
//				memcpy(&WATCH[0],CanIdParmTab[port-1].IdListP,CanIdParmTab[port-1].IdNum*sizeof(StoreCanIdMsg));
// 			if(port==2)
//				memcpy(&WATCH[1],CanIdParmTab[port-1].IdListP,CanIdParmTab[port-1].IdNum*sizeof(StoreCanIdMsg));
		}
	}
	return 1;
}



void CanFunctionInit(BYTE port)
{
	if(port==HardPort1)
	{
		g_hCAN0 = Device_Create(&g_CAN0_Engine);
		if(CanParm[port-1].CanMode==ModeMaster)//master
			Create_CanopenMaster(HardPort1);
		else if (CanParm[port-1].CanMode==ModeSlave)
			Create_CanopenSlave(HardPort1);
	//	free_memory((BYTE*)&CanIdParmTab[port-1].IdListP);
	}

	else if(port==HardPort2)
	{
		g_hCAN1 = Device_Create(&g_CAN1_Engine);
		if(CanParm[port-1].CanMode==ModeMaster)//master
			Create_CanopenMaster(HardPort2);
		else if (CanParm[port-1].CanMode==ModeSlave)
			Create_CanopenSlave(HardPort2);
		//free_memory((BYTE*)&CanIdParmTab[port-1].IdListP);
	}
}

void ResetMemory()
{
	free_memory((BYTE *)CanIdParmTab[0].IdListP);
	free_memory((BYTE *)CanIdParmTab[1].IdListP);
	free_memory((BYTE *)u_aCanMaster_OdValue_TblP1);
	free_memory((BYTE *)u_aCanMaster_OdValue_TblP2);
	free_memory((BYTE *)u_aCanMaster_OdPara_TblP1);
	free_memory((BYTE *)u_aCanMaster_OdPara_TblP2);
	free_memory((BYTE *)u_aCanSlave_OdValue_TblP1);
	free_memory((BYTE *)u_aCanSlave_OdValue_TblP2);
	free_memory((BYTE *)u_aCanSlave_OdPara_TblP1);
	free_memory((BYTE *)u_aCanSlave_OdPara_TblP2);
}

void RunCanAppFunction(void *pParam)
{
	if(!g_InitCommunP1&&CanParm[HardPort1-1].Configured==CanOdDwn)
	{
		CanFunctionInit(HardPort1);
		if((CanParm[HardPort1-1].CanMode==ModeMaster)&&(TASK_MASTER==HTASK_NULL))
			CreateTaskCanopenMaster();

		if((CanParm[HardPort1-1].CanMode==ModeSlave)&&(TASK_SLAVE==HTASK_NULL))
			CreateTaskCanopenSlave();
		g_InitCommunP1=1;
	}

	if(!g_InitCommunP2&&CanParm[HardPort2-1].Configured==CanOdDwn)
	{
		CanFunctionInit(HardPort2);
		if((CanParm[HardPort2-1].CanMode==ModeMaster)&&(TASK_MASTER==HTASK_NULL))
			CreateTaskCanopenMaster();

		if((CanParm[HardPort2-1].CanMode==ModeSlave)&&(TASK_SLAVE==HTASK_NULL))
			CreateTaskCanopenSlave();
		g_InitCommunP2=1;
	}
	RunCanMasterProtocol();
}


BYTE	free_memory(BYTE *buf)
{
	if(buf!=NULL)
	{
		free(buf);
		return 1;
	}
	return 0;
}


void RunCanMasterProtocol()
{
	WORD i,count=0;

    if(g_bCanFunctionFlag&CAN0_AS_Master)
	{
		for(i=0; i<CanIdParmTab[HardPort1-1].IdNum; i++)
		{
			if(CanIdParmTab[HardPort1-1].IdListP[i].MasterCMD==MasterReadData)
			{
//				memcpy((BYTE*)&WP,(BYTE*)&CanIdParmTab[HardPort1-1].IdListP[0],CanIdParmTab[HardPort1-1].IdNum*sizeof(StoreCanIdMsg));
				SDO_upload_set(HardPort1,CanIdParmTab[HardPort1-1].IdListP[i].CanNodeID, CanIdParmTab[HardPort1-1].IdListP[i].CanOD);
				if(CanValueStoreM[0]!=NULL)
					CanValueStoreM[0][count++] = u_aCanMaster_OdValue_TblP1[i].DbValue;
			}
		}
	}

	if(g_bCanFunctionFlag&CAN1_AS_Master)
	{
		for(i=0; i<CanIdParmTab[HardPort2-1].IdNum; i++)
		{
			if(CanIdParmTab[HardPort2-1].IdListP[i].MasterCMD==MasterReadData)
			{
				SDO_upload_set(HardPort2,CanIdParmTab[HardPort2-1].IdListP[i].CanNodeID, CanIdParmTab[HardPort2-1].IdListP[i].CanOD);
				if(CanValueStoreM[1]!=NULL)
					CanValueStoreM[1][count++] = u_aCanMaster_OdValue_TblP2[i].DbValue;
			}
		}
	}
}

void WriteCanDataToEeprom(void)
{
	WORD  count = 0;
	WORD  countod = 0;
	BYTE  buffer[10000]={0};

//	if(g_StoreParmFlag)
	{
		memcpy(&buffer[count],&CanParm[0],sizeof(CanParm[0]));
		count+=sizeof(CanParm[0]);
		memcpy(&buffer[count],&CanParm[1],sizeof(CanParm[1]));
		count+=sizeof(CanParm[1]);

		*((WORD*)(&buffer[count])) = sizeof(BYTE)+sizeof(WORD)+CanIdParmTab[0].IdNum*sizeof(StoreCanIdMsg);
		count+=sizeof(WORD);
		*((WORD*)(&buffer[count])) = sizeof(BYTE)+sizeof(WORD)+CanIdParmTab[1].IdNum*sizeof(StoreCanIdMsg);
		count+=sizeof(WORD);//20 byte

		if(CanIdParmTab[0].HardPort == 1)
		{
			buffer[count] = CanIdParmTab[0].HardPort;
			count+=sizeof(CanIdParmTab[0].HardPort);
			*((WORD*)(&buffer[count])) = CanIdParmTab[0].IdNum;
			count+=sizeof(CanIdParmTab[0].IdNum);
			if(CanIdParmTab[0].IdListP!=NULL)
			{
				for(countod=0; countod<CanIdParmTab[0].IdNum; countod++)
				{
					*((DWORD*)(&buffer[count])) = CanIdParmTab[0].IdListP[countod].OffsetNum;
					count+=sizeof(CanIdParmTab[0].IdListP[countod].OffsetNum);
//					if(CanParm[0].CanMode==ModeMaster)
					{
						buffer[count] = CanIdParmTab[0].IdListP[countod].CanNodeID;
						count+=sizeof(CanIdParmTab[0].IdListP[countod].CanNodeID);
					}
					*((DWORD*)(&buffer[count])) = CanIdParmTab[0].IdListP[countod].CanOD;
					count+=sizeof(CanIdParmTab[0].IdListP[countod].CanOD);
					buffer[count] = CanIdParmTab[0].IdListP[countod].DataType;
					count+=sizeof(CanIdParmTab[0].IdListP[countod].DataType);
					buffer[count] = CanIdParmTab[0].IdListP[countod].Pemission;
					count+=sizeof(CanIdParmTab[0].IdListP[countod].Pemission);
					buffer[count] = CanIdParmTab[0].IdListP[countod].MasterCMD;
					count+=sizeof(CanIdParmTab[0].IdListP[countod].MasterCMD);
				 }
			}
//		   Device_Write(g_hEepromAt24c16,buffer,count,Can1ParmOffset);
			Flash_Write(buffer,count,Can1OD_Block,0);
			free_memory((BYTE*)CanIdParmTab[0].IdListP);
	   }

		if(CanIdParmTab[1].HardPort == 2)
		{
			count = 0;
			memset(buffer,0,sizeof(buffer));
			buffer[count++] = CanIdParmTab[1].HardPort;
			*((WORD*)(&buffer[count])) = CanIdParmTab[1].IdNum;
			count+=sizeof(CanIdParmTab[1].IdNum);
			if(CanIdParmTab[1].IdListP!=NULL)
			{
				for(countod=0; countod<CanIdParmTab[1].IdNum; countod++)
				{
					*((DWORD*)(&buffer[count])) = CanIdParmTab[1].IdListP[countod].OffsetNum;
					count+=sizeof(CanIdParmTab[1].IdListP[countod].OffsetNum);
//					if(CanParm[1].CanMode==ModeMaster)
					{
						buffer[count] = CanIdParmTab[1].IdListP[countod].CanNodeID;
						count+=sizeof(CanIdParmTab[1].IdListP[countod].CanNodeID);
					}
					*((DWORD*)(&buffer[count])) = CanIdParmTab[1].IdListP[countod].CanOD;
					count+=sizeof(CanIdParmTab[1].IdListP[countod].CanOD);
					buffer[count] = CanIdParmTab[1].IdListP[countod].DataType;
					count+=sizeof(CanIdParmTab[1].IdListP[countod].DataType);
					buffer[count] = CanIdParmTab[1].IdListP[countod].Pemission;
					count+=sizeof(CanIdParmTab[1].IdListP[countod].Pemission);
					buffer[count] = CanIdParmTab[1].IdListP[countod].MasterCMD;
					count+=sizeof(CanIdParmTab[1].IdListP[countod].MasterCMD);
				 }
			}
//			Device_Write(g_hEepromAt24c16,buffer,count,Can2ParmOffset);
			Flash_Write(buffer,count,Can2OD_Block,0);
			free_memory((BYTE*)CanIdParmTab[1].IdListP);
		}
			g_StoreParmFlag = 0;
	 }
}
void ReadCanDataFromErprom(void)
{
	BYTE  buffer1[5000]={0};
	BYTE  buffer2[5000]={0};
//	BYTE  buffer3[200]={0};
	BYTE  *patch = NULL;
	WORD  len_port1=0,len_port2=0;

	if(g_StoreParmFlagAP == 1)
	{
		ResetMemory();
		Flash_Read(buffer1,sizeof(CanParm[0])+sizeof(CanParm[1])+sizeof(WORD)*2,Can1OD_Block,0);
//		Device_Read(g_hEepromAt24c16,buffer1,sizeof(CanParm[0])+sizeof(CanParm[1])+sizeof(WORD)*2,Can1ParmOffset);
		patch = &buffer1[CanParmLen];
		len_port1 = (*(WORD*)patch);//39
		patch+=sizeof(WORD);
		len_port2 = (*(WORD*)patch);//3

		Flash_Read(&buffer1[20],len_port1,Can1OD_Block,20);
		Flash_Read(buffer2,len_port2,Can2OD_Block,0);
		SetFlashValueToCanopen(buffer1,len_port1,buffer2,len_port2);
		if(!u_TaskCanFlag)
		{
			 Create_TaskCanFunction();
			 u_TaskCanFlag = 1;
		}
		//Device_Read(g_hEepromAt24c16,&buffer1[20],len_port1,20);
//		Device_Read(g_hEepromAt24c16,buffer3,300,0);
//		Device_Read(g_hEepromAt24c16,buffer2,len_port2,Can2ParmOffset);
	//	Device_Read(g_hEepromAt24c16,buffer2,300,CanOffsetAddress);
//		g_StoreParmFlagAP = 0;
	}
}
//void  Test_Store(void)
//{
//	WriteCanDataToEeprom();
//	ReadCanDataFromErprom();
//}

void SetFlashValueToCanopen(BYTE *addressS,WORD lencan1,BYTE *addressE,WORD lencan2)
{
//	CanParm[0].
	BYTE *parmport1 = NULL,*parmport2 = NULL;
	parmport1 = addressS;

	memcpy(&CanParm[0],addressS,sizeof(CanParm[0])+sizeof(CanParm[1]));
	parmport1+=sizeof(CanParm[0])+sizeof(CanParm[1])+sizeof(WORD)*2;


	CanIdParmTab[0].HardPort = *parmport1;
	parmport1+=sizeof(CanIdParmTab[0].HardPort);
	CanIdParmTab[0].IdNum = *((WORD*)(parmport1));
	parmport1+=sizeof(CanIdParmTab[0].IdNum);
	free_memory((BYTE*)CanIdParmTab[0].IdListP);
	CanIdParmTab[0].IdListP=(StoreCanIdMsg *)malloc(CanIdParmTab[0].IdNum*sizeof(StoreCanIdMsg));
	if(CanIdParmTab[0].IdListP!=NULL)
		memcpy((BYTE*)CanIdParmTab[0].IdListP,parmport1,lencan1-3);
//	memcpy(&WATCH[0],CanIdParmTab[0].IdListP,CanIdParmTab[0].IdNum*sizeof(StoreCanIdMsg));

	parmport2 = addressE;
	CanIdParmTab[1].HardPort = *parmport2;
	parmport2+=sizeof(CanIdParmTab[1].HardPort);
	CanIdParmTab[1].IdNum = *((WORD*)(parmport2));
	parmport2+=sizeof(CanIdParmTab[1].IdNum);
	free_memory((BYTE*)CanIdParmTab[1].IdListP);
	CanIdParmTab[1].IdListP=(StoreCanIdMsg *)malloc(CanIdParmTab[1].IdNum*sizeof(StoreCanIdMsg));
	if(CanIdParmTab[1].IdListP!=NULL)
		memcpy((BYTE*)CanIdParmTab[1].IdListP,parmport2,lencan2-3);
//	memcpy(&WATCH[1],CanIdParmTab[1].IdListP,CanIdParmTab[1].IdNum*sizeof(StoreCanIdMsg));
}

BOOL_T 	Funcblock_Canopen_MasterIn(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	WORD i;
//	static BYTE port=0;
	WORD Offset = 0;
	RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	WORD StartOffset=DesigntimeData->PrivateParam&0xFFFF;
	WORD EndOffset = StartOffset+(DesigntimeData->PrivateParam>>16)-1;

	if(((g_bCanFunctionFlag&CAN0_AS_Master)==0)&&((g_bCanFunctionFlag&CAN1_AS_Master)==0)) return FALSE;

	if(g_bCanFunctionFlag&CAN0_AS_Master)
	{
//		if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[0].IdListP[CanIdParmTab[0].IdNum-1].OffsetNum)
		{
			for(Offset=StartOffset; Offset<=EndOffset; Offset++)
			{
				for(i=0; i<CanIdParmTab[0].IdNum; i++)
				{
					if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[0].IdListP[i].OffsetNum)
					{
//						port = 1;
						CanIdParmTab[0].IdListP[i].MasterCMD = MasterReadData;
						g_ALLCommDataList[Offset].Value.dwData = u_aCanMaster_OdValue_TblP1[i].DbValue.dwValue;
						break;
					}
				}
			}
		}
	 }
	 if(g_bCanFunctionFlag&CAN1_AS_Master)
	 {
//		 if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[1].IdListP[CanIdParmTab[1].IdNum-1].OffsetNum)
		 {
			 for(Offset=StartOffset; Offset<=EndOffset; Offset++)
			 {
				for(i=0; i<CanIdParmTab[1].IdNum; i++)
				{
					if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[1].IdListP[i].OffsetNum)
					{
//						port=2;
						CanIdParmTab[1].IdListP[i].MasterCMD =MasterReadData;
						g_ALLCommDataList[Offset].Value.dwData = u_aCanMaster_OdValue_TblP2[i].DbValue.dwValue;
						break;
					}

				}
			 }
		 }
	 }
	 RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[StartOffset];

	 RuntimeData[DesigntimeData->serialNo].DataType = 8;

	 return TRUE;
}

BOOL_T 	Funcblock_Canopen_MasterOut(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	 RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	 DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	 WORD Offset = 0;
	 WORD StartOffset = 0;
	 WORD EndOffset = 0;
	 WORD i= 0;
	 WORD_T   *p;
	 DWORD InValue;
	 StartOffset=DesigntimeData->PrivateParam&0xFFFF;
	 EndOffset = StartOffset+(DesigntimeData->PrivateParam>>16)-1;

	 if(((g_bCanFunctionFlag&CAN0_AS_Master)==0)&&((g_bCanFunctionFlag&CAN1_AS_Master)==0)) return FALSE;

	 p = &(DesigntimeData->InputsID[0]);
	 COMMDATATYPE_T *set = NULL;
	 set = (COMMDATATYPE_T*)(RuntimeData[*p].outputValue.dwData);

	 RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	 if(g_bCanFunctionFlag&CAN0_AS_Master)
	 {
//		 if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[0].IdListP[CanIdParmTab[0].IdNum-1].OffsetNum)
		 {
			 for(Offset=StartOffset;Offset<=EndOffset;Offset++)
			 {
				for(i=0; i<=CanIdParmTab[0].IdNum; i++)
				{
					if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[0].IdListP[i].OffsetNum)
					{
						if(RuntimeData[*p].DataType!=8)//single value
						{
							InValue = RuntimeData[*p].outputValue.dwData;
//							break;
						}
						else if(RuntimeData[*p].DataType==8)
							InValue = (set++)->Value.dwData;
						sendSDO(CanIdParmTab[0].IdListP[i].CanNodeID,CanIdParmTab[0].IdListP[i].CanOD,sizeof(DWORD),(void*)&InValue);
						break;
					}
//					if(RuntimeData[*p].DataType!=8)
//						break;
				}
			 }
		 }
	 }
	  if(g_bCanFunctionFlag&CAN1_AS_Master)
	 {
//		 if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[1].IdListP[CanIdParmTab[1].IdNum-1].OffsetNum)
		 {
			 for(Offset=StartOffset;Offset<=EndOffset;Offset++)
			 {
				for(i=0; i<=CanIdParmTab[1].IdNum; i++)
				{
					if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[1].IdListP[i].OffsetNum)
					{
						if(RuntimeData[*p].DataType!=8)//single value
						{
							InValue = RuntimeData[*p].outputValue.dwData;
//							break;
						}
						else if(RuntimeData[*p].DataType==8)
							InValue = (set++)->Value.dwData;//*((DWORD*)RuntimeData[*p].outputValue.dwData++);
						sendSDO(CanIdParmTab[1].IdListP[i].CanNodeID,CanIdParmTab[1].IdListP[i].CanOD,sizeof(DWORD),(void*)&InValue);
						break;
					}
				}
//				if(RuntimeData[*p].DataType!=8)
//					break;
			 }
		 }
	 }
	 return TRUE;
}

BOOL_T 	Funcblock_Canopen_SlaveIn(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{

	 	 WORD i;
//		 static BYTE port,init_flag1=0,init_flag2 = 0;
		 WORD Offset = 0;
		 RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
		 DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
		 WORD StartOffset=DesigntimeData->PrivateParam&0xFFFF;
		 WORD EndOffset = StartOffset+(DesigntimeData->PrivateParam>>16)-1;

		 if(((g_bCanFunctionFlag&CAN0_AS_Slave)==0)&&((g_bCanFunctionFlag&CAN1_AS_Slave)==0)) return FALSE;
		 if(g_bCanFunctionFlag&CAN0_AS_Slave)
		 {
		 	if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[0].IdListP[CanIdParmTab[0].IdNum-1].OffsetNum)
		 	{
				for(Offset=StartOffset; Offset<=EndOffset; Offset++)
				{
					for(i=0; i<CanIdParmTab[0].IdNum; i++)
					{
						if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[0].IdListP[i].OffsetNum)
						{
							 g_ALLCommDataList[Offset].Value.dwData = u_aCanSlave_OdValue_TblP1[i].DbValue.dwValue;
							 break;
						}
					}
				}
		   }
		 }
		 if(g_bCanFunctionFlag&CAN1_AS_Slave)
		 {
			 if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[1].IdListP[CanIdParmTab[1].IdNum-1].OffsetNum)
			 {
				 for(Offset=StartOffset; Offset<=EndOffset; Offset++)
				 {
					for(i=0; i<CanIdParmTab[1].IdNum; i++)
					{
						if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[1].IdListP[i].OffsetNum)
						{
							g_ALLCommDataList[Offset].Value.dwData = u_aCanSlave_OdValue_TblP2[i].DbValue.dwValue;
							break;
						}
					}
				}
			 }
		 }

		 RuntimeData[DesigntimeData->serialNo].DataType = 8;

		 RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)&g_ALLCommDataList[StartOffset];
		 return true;
}

BOOL_T 	Funcblock_Canopen_SlaveOut(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	 WORD i;
//	 static BYTE port;
//	 WORD *CanIDOffset = NULL;
	 WORD Offset = 0;
	 WORD_T   *p;
	 RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	 DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;

	 WORD StartOffset=DesigntimeData->PrivateParam&0xFFFF;
	 WORD EndOffset = StartOffset+(DesigntimeData->PrivateParam>>16)-1;

	    if(((g_bCanFunctionFlag&CAN0_AS_Slave)==0)&&((g_bCanFunctionFlag&CAN1_AS_Slave)==0)) return FALSE;
	    p = &(DesigntimeData->InputsID[0]);
	    COMMDATATYPE_T *set = NULL;
	    set = (COMMDATATYPE_T*)(RuntimeData[*p].outputValue.dwData);

	    RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;

	 	if(g_bCanFunctionFlag&CAN0_AS_Slave)
	 	{
	 		if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[0].IdListP[CanIdParmTab[0].IdNum-1].OffsetNum)
	 		{
				for(Offset=StartOffset; Offset<=EndOffset; Offset++)
				{
					for(i=0; i<CanIdParmTab[0].IdNum; i++)
					{
						if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[0].IdListP[i].OffsetNum)
						{
							if(RuntimeData[*p].DataType!=8)
							{
								u_aCanSlave_OdValue_TblP1[i].DbValue.dwValue = RuntimeData[*p].outputValue.dwData;
								break;
							}
							else
								u_aCanSlave_OdValue_TblP1[i].DbValue.dwValue = (set++)->Value.dwData;//*((DWORD*)RuntimeData[*p].outputValue.dwData++);
							break;
						}
					}
					if(RuntimeData[*p].DataType!=8)
						break;
				}
		 }
     }
	  if(g_bCanFunctionFlag&CAN1_AS_Slave)
	 {
		 if(g_ALLCommDataList[EndOffset].DataID<=CanIdParmTab[1].IdListP[CanIdParmTab[1].IdNum-1].OffsetNum)
		 {
			 for(Offset=StartOffset; Offset<=EndOffset; Offset++)
			 {
				for(i=0; i<CanIdParmTab[1].IdNum; i++)
				{
					if(g_ALLCommDataList[Offset].DataID==CanIdParmTab[1].IdListP[i].OffsetNum)
					{
						if(RuntimeData[*p].DataType!=8)
						{
							u_aCanSlave_OdValue_TblP2[i].DbValue.dwValue = RuntimeData[*p].outputValue.dwData;
							break;
						}
						else
							u_aCanSlave_OdValue_TblP2[i].DbValue.dwValue = (set++)->Value.dwData;//*((DWORD*)RuntimeData[*p].outputValue.dwData++);;
						break;
					}
				}
				if(RuntimeData[*p].DataType!=8)
					break;
			}
		 }
	 }
	 return true;
}


