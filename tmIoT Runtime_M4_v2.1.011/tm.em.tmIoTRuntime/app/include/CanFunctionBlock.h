/*
 * CanFunctionBlock.h
 *
 *  Created on: 2018年2月28日
 *      Author: wangyunlou
 */

#ifndef APP_CANFUNCTIONBLOCK_H_
#define APP_CANFUNCTIONBLOCK_H_
#include "kernel/include/type.h"
#include "driver/include/_ecan.h"
#include "kernel/include/task.h"
#include "canopen/include/canopen.h"
/************************define********************/
#define		CanDataPatchLen		16
#define		InitCanParm			0x01
#define		CommunParm			0x02
#define		HardDeviceNum		0x02

#define		HardPort1			0x01
#define		HardPort2			0x02
#define		CanFBMaster			0X01
#define		CanFBSlave			0X02

#define     CanParmDwn			0x01
#define     CanOdDwn			0x02
#define		CanNumMax			10

#define		CAN0_AS_Master		0x01
#define		CAN0_AS_Slave		0x02
#define		CAN1_AS_Master		0x04
#define		CAN1_AS_Slave		0x08

#define		ModeSlave			0x01
#define		ModeMaster			0x02

#define		DWORD_SEVER			0x09


#define		SDO_UPLOAD			0x01


#define		MasterReadData		0x01

/************************type********************/
#pragma pack(push)
#pragma pack(1)
/*****************************
    5k-----1
   10k-----2
   20k-----3
   50k-----4
  100k-----5
  125k-----6
  250k-----7
  500k-----8
  800k-----9
 1000k-----10
 *****************************/
typedef struct tagCanMsg
{
	BYTE HardPortNUM;
	BYTE CanMode;
	BYTE CanID;
	BYTE CanBuadRate;
	WORD CycTime;
	BYTE SlaveNum;
	BYTE Configured;
}CanMsg;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
typedef struct tagStoreCanIdMsg
{
	DWORD	OffsetNum;
	BYTE	CanNodeID;
	DWORD	CanOD;
	BYTE    DataType;
	BYTE    Pemission;
	BYTE	MasterCMD;
}StoreCanIdMsg;
#pragma pack(pop)

#pragma pack(push)
#pragma pack(1)
typedef struct tagCanIdParm
{
	BYTE HardPort;
	WORD IdNum;//od 数
	StoreCanIdMsg *IdListP;
}CanIdParm;
#pragma pack(pop)

typedef struct tagNodeMsg
{
	BYTE ID;
	WORD OffsetNum;
	WORD AllTabOffset;
}NodeMsg;
/*********************datatype**************/
BYTE					g_bCanFunctionFlag;

/*********************extern*************/
extern BYTE_T    u_byPacketStamp;
extern CanMsg    CanParm[HardDeviceNum];
extern CanMsg    CanParm[HardDeviceNum];
extern CanIdParm CanIdParmTab[HardDeviceNum];
extern BYTE	   g_StoreParmFlag;
extern BYTE	   g_StoreParmFlagAP;

void   Create_TaskCanFunction();
extern HTASK	TASK_MASTER;
extern HTASK	TASK_SLAVE;
extern	BYTE	g_InitCommunP1;
extern	BYTE	g_InitCommunP2;
extern CANOPEN_OD_VALUE_T * CanValueStoreM[HardDeviceNum];
extern CANOPEN_OD_VALUE_T * CanValueStoreS[HardDeviceNum];
extern CANOPEN_OD_DATA_T	*u_aCanMaster_OdValue_TblP1;
extern CANOPEN_OD_DATA_T	*u_aCanMaster_OdValue_TblP2;

extern void Create_CanopenMaster(BYTE DeviceNUM);
extern BYTE	free_memory(BYTE *buf);
extern BYTE CanDealRecvParm(BYTE parmtype, BYTE *buf);
extern void ReadCanDataFromErprom(void);
extern void WriteCanDataToEeprom(void);
extern BOOL_T 	Funcblock_Canopen_MasterIn(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern BOOL_T 	Funcblock_Canopen_MasterOut(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern BOOL_T 	Funcblock_Canopen_SlaveIn(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern BOOL_T 	Funcblock_Canopen_SlaveOut(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern DWORD 	GetCanBurate(BYTE offset);

#endif /* APP_CANFUNCTIONBLOCK_H_ */
