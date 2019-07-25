/*==========================================================================+
|  File     : Function_ModbusTCP.h                                           |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : CP                                                        |
|  Version  : v1.00                                                         |
|  Creation : 2018骞�1鏈�8鏃�                                                                                                                                                     |
|  Revision :                                                               |
+==========================================================================*/
#include "taskmodbustcp.h"
#include "comlib.h"
#include "database.h"
#include "taskiNet.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
void SetPackges(BYTE_T *pBuf, MODBUS *pModbus);
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/
BYTE ModbusTCPFlashData[1024*5] = {0};
MODBUSTCPOD_Table	ModbusTCPOD_Table[MAX_MODBUSTCPOD];
MODBUSTCPODVALUE_T	ModbusTCPODValue[MAX_MODBUSTCPOD];			// Master Value Table

MODBUSTCPREG_Table	ModbusTCPReg_Table[MAX_MODBUSTCPREG];
MODBUSTCPREGVALUE_Table	ModbusTCPRegValue_Table[MAX_MODBUSTCPREG];	// Slave Value Table

MODBUSTCPOFFSET_Table ModbusTCPOffset_Table;

BYTE 	ModbusTCPResBuf[MAX_MODBUSTCPREG];
WORD 	ModbusTCPRes_Length;

MODBUSTCPODOLD_T 				ModbusTCPOD_Table_Old[MAX_MODBUSTCPOD];

WORD      M2C_OFFSET = 0;
BYTE	 	M2CDownloadFlag = State_Down;
BYTE	 	Downflag = State_Null;
WORD 		pTotalCount = 0;
BYTE		pTotalMode = 0;
BYTE 	  ResponseFlag = 0;
BYTE 	  ResponseSendFlag = 0;
static WORD 		All_Line = 0;
static DWORD   	 	ModbusTCPFlash_lOffset = 0;
static int 				Setp = 0;
char 	g_ModbusTCPFalg = 0;
char 	Write_Flag = 0;
long SendTime 		= 0;
long ReConnect 		= 5000;
HTASK 	g_htask_ModbusTCP;
/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
DBVALUE Get_ModbusTCPODValue_ByDataID(WORD_T Index);
static void RunLwipTcp(void *pParam);
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
void CreateTaskModbusTCP(void)
{
	TASK    Task;
	Task.wType = TASK_WHILELOOP;
	Task.period.tCounter = 0;
	Task.period.tTimer = 1;
	Task.pRun = RunLwipTcp;
	Task.pParam = 0;
	g_htask_ModbusTCP = Create_Task(&Task, 0);
	if(pTotalMode == MODBUSTCP_MASTER)
	{
		AddDnCounter(&SendTime);
		AddDnCounter(&ReConnect);
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
/*===========================================================================+
| Name:        RunLwip                                                       |
| Description:                                                               |
+===========================================================================*/
void RunLwipTcp(void *pParam)
{
	if(pTotalMode == MODBUSTCP_MASTER)
	{
		if (g_dwSystemTick - SendTime >= M2CTCP_SENT_PERIOD)
		{
			if(u_Tcp_pcb->state == ESTABLISHED)
			{
					SendTime = g_dwSystemTick;
					ModebusTCP_Send(u_Tcp_pcb);
			}
		}
		if(u_Tcp_pcb->state != ESTABLISHED)
		{
			if (ReConnect < 0)
			{
				ReConnect = 5000;
				Lwip_TcpClose(u_Tcp_pcb);
				u_Tcp_pcb = Create_LwipTcp(&g_sAddRemoteMesg, LWIP_CLIENT);
			}
		}
	}
	else if(pTotalMode == MODBUSTCP_SLAVE)
	{
		if(ResponseFlag == 1)
		{
			ModebusTCPSlave_Send(u_Tcp_pcb);
			ResponseFlag = 0;
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
WORD Fetchword(BYTE *data, int offset)
{
	WORD wData;

	wData = (((data[offset] & 0xFF) << 8) | (data[offset+1] & 0xFF));

	return wData;
}
/*===========================================================================+
|Name:         Reorder                                    					 |
|Description:  去锟截猴拷锟斤拷锟斤拷锟斤拷					                         		     |
+===========================================================================*/
void ModbusTCPReorder(WORD_T *dest)
{
	int i,j;
	int index = 1;
	WORD_T src[MAX_MODBUSTCPREG];
	WORD_T tmp;

	memset(&src,  0x0, sizeof(src));

	for (i = 0; i < pTotalCount; i++)
	{
		memcpy(&src[i], &ModbusTCPReg_Table[i].regaddr, sizeof(WORD_T));
	}
	for(i = 0; i < pTotalCount-1; i++)
	{
		for(j = 0; j < pTotalCount-1-i; j++)
		{
			if(src[j] > src[j+1])
			{
				tmp = src[j];
				src[j] = src[j+1];
				src[j+1] = tmp;
			}
		}
	}

	dest[0] = src[0];

	for(i = 1; i < MAX_MODBUSTCPREG; i++)
	{
		if(src[i] != src[i-1])
		{
			dest[index++] = src[i];
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
void   StorageOD_DataTable_FromFlash(void)
{
	WORD i = 0;
	WORD dwValue = 0;
	WORD BufFlash[MAX_MODBUSTCPREG];
	DWORD_T dwIDValue = 0;
	memcpy(&pTotalMode, &ModbusTCPFlashData[0], sizeof(BYTE));
	if(pTotalMode == 0xFF)//未存储使用信息
		pTotalMode = MODBUSTCP_NotUsed;
	memcpy(&pTotalCount, &ModbusTCPFlashData[1], sizeof(WORD));
	if(pTotalCount == 0xFFFF)
		pTotalCount = 0;
	switch (pTotalMode)
	{
		case MODBUSTCP_MASTER :
			memcpy(&ModbusTCPOD_Table[0], &ModbusTCPFlashData[ModbusTCPODBUF_OFFSET_INFLASH], sizeof(MODBUSTCPOD_Table)*pTotalCount);
			for (i = 0; i < pTotalCount ; i++)
			{
				dwValue = htons(ModbusTCPOD_Table[i].addr);
				memcpy(&ModbusTCPOD_Table[i].addr, &dwValue, sizeof(WORD));
				dwIDValue = htonl(ModbusTCPOD_Table[i].dataid);
				memcpy(&ModbusTCPOD_Table[i].dataid, &dwIDValue, sizeof(DWORD_T));
				memcpy(&ModbusTCPODValue[i].dataid, &dwIDValue, sizeof(DWORD_T));
			}
			break;
		case MODBUSTCP_SLAVE :
			memcpy(&ModbusTCPReg_Table[0], &ModbusTCPFlashData[ModbusTCPODBUF_OFFSET_INFLASH], sizeof(MODBUSTCPREG_Table)*pTotalCount);
			for (i = 0; i < pTotalCount ; i++)
			{
				dwValue = htons(ModbusTCPReg_Table[i].regaddr);
				memcpy(&ModbusTCPReg_Table[i].regaddr, &dwValue, sizeof(WORD));
				dwIDValue = htonl(ModbusTCPReg_Table[i].regdataid);
				memcpy(&ModbusTCPReg_Table[i].regdataid, &dwIDValue, sizeof(DWORD_T));
			}
			ModbusTCPReorder(BufFlash);
			for (i = 0; i < sizeof(ModbusTCPRegValue_Table)/sizeof(ModbusTCPRegValue_Table[0]); i++)
			{
				memcpy(&ModbusTCPRegValue_Table[i].addr, &BufFlash[i], sizeof(WORD_T));
			}
			break;
		default:
			break;
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
void   StorageOD_DataTable_FromStudio(void)
{
	WORD i = 0;
	WORD dwValue = 0;
	WORD BufStudio[MAX_MODBUSTCPREG];
	DWORD_T dwIDValue = 0;

	memset(BufStudio, 0, sizeof(BufStudio));
	memcpy(&pTotalMode, &ModbusTCPFlashData[0], sizeof(BYTE));
	memcpy(&pTotalCount, &ModbusTCPFlashData[1], sizeof(WORD));
	switch (pTotalMode)
	{
		case MODBUSTCP_MASTER :
			memcpy(&ModbusTCPOD_Table[0], &ModbusTCPFlashData[ModbusTCPODBUF_OFFSET_INFLASH], sizeof(MODBUSTCPOD_Table)*pTotalCount);
			for (i = 0; i < pTotalCount ; i++)
			{
				dwValue = htons(ModbusTCPOD_Table[i].addr);
				memcpy(&ModbusTCPOD_Table[i].addr, &dwValue, sizeof(WORD));
				dwIDValue = htonl(ModbusTCPOD_Table[i].dataid);
				memcpy(&ModbusTCPOD_Table[i].dataid, &dwIDValue, sizeof(DWORD_T));
				memcpy(&ModbusTCPODValue[i].dataid, &dwIDValue, sizeof(DWORD_T));
			}
			break;
		case MODBUSTCP_SLAVE :
			memcpy(&ModbusTCPReg_Table[0], &ModbusTCPFlashData[ModbusTCPODBUF_OFFSET_INFLASH], sizeof(MODBUSTCPREG_Table)*pTotalCount);
			for (i = 0; i < pTotalCount ; i++)
			{
				dwValue = htons(ModbusTCPReg_Table[i].regaddr);
				memcpy(&ModbusTCPReg_Table[i].regaddr, &dwValue, sizeof(WORD));
				dwIDValue = htonl(ModbusTCPReg_Table[i].regdataid);
				memcpy(&ModbusTCPReg_Table[i].regdataid, &dwIDValue, sizeof(DWORD_T));
			}
			ModbusTCPReorder(BufStudio);
			for (i = 0; i < sizeof(ModbusTCPRegValue_Table)/sizeof(ModbusTCPRegValue_Table[0]); i++)
			{
				memcpy(&ModbusTCPRegValue_Table[i].addr, &BufStudio[i], sizeof(WORD_T));
			}
			break;
		default:
			break;
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
void 	Read_ModbusTCPData_FromFlash(void)
{
	Flash_Read(ModbusTCPFlashData, sizeof(ModbusTCPFlashData), ModbusTcpData_Block, 0);
	StorageOD_DataTable_FromFlash();
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
void 	Read_ModbusTCPData_FromStudio(void)
{
	StorageOD_DataTable_FromStudio();
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
BYTE ModbusTCPDealRecvParm(BYTE parmtype, BYTE *buf)
{
	BYTE Mode = 0;
	WORD LineValue=0;

    if(parmtype == ModbusTCPSetMODEInfoFRAME)
    {
    	memset(ModbusTCPFlashData, 0, sizeof(ModbusTCPFlashData));
        memcpy(&ModbusTCPFlashData[ModbusTCPModeINFOBUF_OFFSET_INFLASH],
                  &g_byRecv_Buffer[ModbusTCPCONFIGDATA_OFFSET], ModbusTCPModeInfo_SIZE);
        Flash_Write(ModbusTCPFlashData, 1, ModbusTcpData_Block, 0);       //锟斤拷锟斤拷ModbusTCP mode
    }
    else if(parmtype == ModbusTCPODFRAME)
    {
		memcpy(&Mode, &g_byRecv_Buffer[16], sizeof(BYTE));
		memcpy(&LineValue, &g_byRecv_Buffer[17], sizeof(WORD));
		LineValue = htons(LineValue);
		switch(Mode)
		{
			case MODBUSTCP_MASTER :
			{
				memcpy(&ModbusTCPFlashData[ModbusTCPODBUF_OFFSET_INFLASH+ModbusTCPFlash_lOffset], &g_byRecv_Buffer[ModbusTCPOD_OFFSET], LineValue*PROTOCOL_MODBUSTCPMASTER_LEN);
				ModbusTCPFlash_lOffset += LineValue*PROTOCOL_MODBUSTCPMASTER_LEN;
				All_Line += LineValue;
				break;
			}
			case MODBUSTCP_SLAVE :
			{
				memcpy(&ModbusTCPFlashData[ModbusTCPODBUF_OFFSET_INFLASH+ModbusTCPFlash_lOffset], &g_byRecv_Buffer[ModbusTCPOD_OFFSET], LineValue*PROTOCOL_MODBUSTCPSLAVE_LEN);
				ModbusTCPFlash_lOffset += LineValue*PROTOCOL_MODBUSTCPSLAVE_LEN;
				All_Line += LineValue;
				break;
			}
		}
    }
    else if(parmtype == ModbusTCPENDFRAME)
    {
    	memcpy(&ModbusTCPFlashData[MODBUSTCPOD_LINE], &All_Line, sizeof(WORD));
    	Read_ModbusTCPData_FromStudio();
    	if((pTotalMode == MODBUSTCP_MASTER) ||(pTotalMode == MODBUSTCP_SLAVE))
    	{
    		g_byiNetTaskFlag = 0x55;
    		CreateLwipTcp();
    	}

    	//clear iNet info
    	iNetModeMeg.byMode = INETMODENOTUSED;
    	iNetModeMeg.byProtocolType = INETMODENOTUSED;
    	iNetODParaTab.wODNum = 0;
    	iNetODParaTab.wCurrentODNum = 0;
    	if(iNetODParaTab.pODList != NULL)
    		free(iNetODParaTab.pODList);
    	Destroy_Task(g_htask_iNet);

    	ModbusTCPFlash_lOffset = 0;
    	All_Line = 0;
    }
    else if(parmtype == ModbusTCPSAVEFRAME)
    {
    	Flash_Write(0,sizeof(ModbusTCPFlashData), ModbusTcpData_Block, 0);
        Flash_Write(ModbusTCPFlashData,sizeof(ModbusTCPFlashData), ModbusTcpData_Block, 0);
        memset(ModbusTCPFlashData, 0, sizeof(ModbusTCPFlashData));

        //clear iNet mode and protocol type flag in flash
    	Flash_Write(0, INETPARA_SIZE, iNetData_Block, iNetPara_ofs);

  //      Read_ModbusTCPData_FromFlash();

    }
    return 1;
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
void Setp_ModbusTCP(void)
{
	int HeadIndex, TailIndex = 0;
	MODBUS					ChecksModbus;
	MODBUS 					*Modbus = &ChecksModbus;
	DBVALUE 				dbValue;
	HeadIndex = Setp;
	ModbusTCPOffset_Table.start = HeadIndex;
	BYTE		buf[12] = {0};
	BYTE SetPackCount = 0;

	for (; HeadIndex < pTotalCount; HeadIndex++)
	{
		TailIndex = HeadIndex + 1;
	//	TailIndex = HeadIndex ;

		if (TailIndex == pTotalCount)
		{
			ModbusTCPOffset_Table.end = HeadIndex;
			SetPackCount = 0;
			Setp = 0;
			return;
		}
		if ((ModbusTCPOD_Table[HeadIndex].cmd == MODBUSTCP_READ_HOLDING_REGISTERS) || (ModbusTCPOD_Table[HeadIndex].cmd == MODBUSTCP_READ_INPUT_REGISTERS))
		{
			SetPackCount++;
		}
		if (ModbusTCPOD_Table[HeadIndex].cmd != ModbusTCPOD_Table[TailIndex].cmd || ModbusTCPOD_Table[HeadIndex].id != ModbusTCPOD_Table[TailIndex].id || SetPackCount==MosbudTCP_MaxPack )
		{
			SetPackCount = 0;
			break;
		}
		if (ModbusTCPOD_Table[HeadIndex].cmd == MODBUSTCP_WRITE_SINGLE_REGISTER)
		{
//			Modbus->wTransmitFlag = 0x0;
//			Modbus->wProtocolFlag = 0x0;
//			Modbus->wLenth = 0x0006;
//			Modbus->UnitID = ModbusTCPOD_Table[HeadIndex].id;
//			Modbus->byCMD = ModbusTCPOD_Table[HeadIndex].cmd;
//			Modbus->wAddr = ModbusTCPOD_Table[HeadIndex].addr;
//			dbValue = Get_ModbusTCPODValue_ByDataID(HeadIndex);
//			memcpy(&Modbus->wValue, &dbValue, sizeof(WORD));
//			Modbus->wLenth = htons(Modbus->wLenth);
//			Modbus->wAddr = htons(Modbus->wAddr);
//			Modbus->wValue = htons(Modbus->wValue);
//			memcpy(&buf[0], &Modbus->wTransmitFlag, sizeof(WORD));
//			memcpy(&buf[2], &Modbus->wProtocolFlag, sizeof(WORD));
//			memcpy(&buf[4], &Modbus->wLenth, sizeof(WORD));
//			memcpy(&buf[6], &Modbus->UnitID, sizeof(BYTE));
//			memcpy(&buf[7], &Modbus->byCMD, sizeof(BYTE));
//			memcpy(&buf[8], &Modbus->wAddr, sizeof(WORD));
//			memcpy(&buf[10], &Modbus->wValue, sizeof(WORD));
//			if(memcmp(&buf,ModbusTCPOD_Table_Old[HeadIndex].data, sizeof(BYTE)*12))
//			{
//				Write_Flag = 1;
//			}
//			else
//			{
//				Write_Flag = 0;
//			}
//			if(Write_Flag == 1)
				break;
		}
	}
	ModbusTCPOffset_Table.end = HeadIndex;
	Setp = HeadIndex + 1;

	return;
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
DBVALUE Get_ModbusTCPODValue_ByDataID(WORD_T Index)
{
	int 		i;
	DBVALUE 	dbValue;

	memset(&dbValue, 0, sizeof(dbValue));

	for (i = 0; i < MAX_MODBUSTCPOD; i++)
	{
		if (ModbusTCPOD_Table[Index].dataid == ModbusTCPODValue[i].dataid)
		{
			memcpy(&dbValue, &ModbusTCPODValue[i].writevalue, sizeof(DBVALUE));
			break;
		}
	}
	return dbValue;
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
void Get_ModbusTCPODValue_ByDataID_2(DWORD_T id, DBVALUE *dbValue)
{
	int j = 0;

	for (j = 0; j < MAX_MODBUSTCPOD; j++)
	{
		if (ModbusTCPODValue[j].dataid == id)
		{
			memcpy(dbValue, &ModbusTCPODValue[j].readvalue, sizeof(DBVALUE));
			return;
		}
	}
	return;
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
void Set_ModbusTCPODValue_ByDataID_2(DWORD_T id, DWORD_T value)
{
	int  j = 0;

	for (j = 0; j < MAX_MODBUSTCPOD; j++)
	{
		if (ModbusTCPODValue[j].dataid == id)
		{
			memcpy(&ModbusTCPODValue[j].writevalue, &value, sizeof(DWORD_T));
			return;
		}
	}
	return;
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
void Set_OldTCPSenbuf(BYTE *Buf)
{
	BYTE_T 		TempCmd;
	WORD_T 		HeadIndex;

	HeadIndex = ModbusTCPOffset_Table.start;
	TempCmd = ModbusTCPOD_Table[HeadIndex].cmd;
	if(TempCmd == MODBUSTCP_WRITE_SINGLE_REGISTER)
	{
		memcpy( &ModbusTCPOD_Table_Old[HeadIndex].data, Buf, sizeof(BYTE)*12 );
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
void SetModbusTCPPackges(MODBUS *pModbus)
{
	WORD 		HeadIndex = 0;
	WORD 		TailIndex = 0;
	static WORD		ReadInputCounts = 0;
	BYTE 		TempCmd = 0;
	DBVALUE 	dbValue;
	BYTE		buf[12] = {0};

	HeadIndex = ModbusTCPOffset_Table.start;
	TailIndex = ModbusTCPOffset_Table.end;
	TempCmd = ModbusTCPOD_Table[HeadIndex].cmd;
	switch (TempCmd)
	{
		case MODBUSTCP_READ_HOLDING_REGISTERS:
				pModbus->wTransmitFlag = 0x0;
				pModbus->wProtocolFlag = 0x0;
				pModbus->wLenth = 0x0006;
				pModbus->UnitID = ModbusTCPOD_Table[HeadIndex].id;
				pModbus->byCMD = ModbusTCPOD_Table[HeadIndex].cmd;
				pModbus->wAddr = ModbusTCPOD_Table[HeadIndex].addr;
				dbValue.wData = ModbusTCPOD_Table[TailIndex].addr - ModbusTCPOD_Table[HeadIndex].addr + 0x1;
				memcpy(&pModbus->wCount, &dbValue.wData, sizeof(WORD_T));
				pModbus->wLenth = htons(pModbus->wLenth);
				pModbus->wAddr = htons(pModbus->wAddr);
				pModbus->wCount = htons(pModbus->wCount);
			break;
		case MODBUSTCP_READ_INPUT_REGISTERS:
				pModbus->wTransmitFlag = 0x0;
				pModbus->wProtocolFlag = 0x0;
				pModbus->wLenth = 0x0006;
				pModbus->UnitID = ModbusTCPOD_Table[HeadIndex].id;
				pModbus->byCMD = ModbusTCPOD_Table[HeadIndex].cmd;
				pModbus->wAddr = ModbusTCPOD_Table[HeadIndex].addr;
				dbValue.wData = ModbusTCPOD_Table[TailIndex].addr - ModbusTCPOD_Table[HeadIndex].addr + 0x1;
				memcpy(&pModbus->wCount, &dbValue.wData, sizeof(WORD_T));
				pModbus->wLenth = htons(pModbus->wLenth);
				pModbus->wAddr = htons(pModbus->wAddr);
				pModbus->wCount = htons(pModbus->wCount);
//				pModbus->wTransmitFlag = 0x0;
//				pModbus->wProtocolFlag = 0x0;
//				pModbus->wLenth = 0x0006;
//				pModbus->UnitID = ModbusTCPOD_Table[HeadIndex].id;
//				pModbus->byCMD = ModbusTCPOD_Table[HeadIndex].cmd;
//				pModbus->wAddr = ModbusTCPOD_Table[HeadIndex].addr + ReadInputCounts;
//				dbValue.wData =  0x1;
//				memcpy(&pModbus->wCount, &dbValue.wData, sizeof(WORD_T));
//				pModbus->wLenth = htons(pModbus->wLenth);
//				pModbus->wAddr = htons(pModbus->wAddr);
//				pModbus->wCount = htons(pModbus->wCount);
//				if(ReadInputCounts < ( ModbusTCPOD_Table[TailIndex].addr - ModbusTCPOD_Table[HeadIndex].addr))
//								ReadInputCounts ++ ;
//				else
//					ReadInputCounts = 0;
			break;
		case MODBUSTCP_WRITE_SINGLE_REGISTER:
				pModbus->wTransmitFlag = 0x0;
				pModbus->wProtocolFlag = 0x0;
				pModbus->wLenth = 0x0006;
				pModbus->UnitID = ModbusTCPOD_Table[HeadIndex].id;
				pModbus->byCMD = ModbusTCPOD_Table[HeadIndex].cmd;
				pModbus->wAddr = ModbusTCPOD_Table[HeadIndex].addr;
				dbValue = Get_ModbusTCPODValue_ByDataID(HeadIndex);
				memcpy(&pModbus->wValue, &dbValue, sizeof(WORD));
				pModbus->wLenth = htons(pModbus->wLenth);
				pModbus->wAddr = htons(pModbus->wAddr);
				pModbus->wValue = htons(pModbus->wValue);
				memcpy(&buf[0], &pModbus->wTransmitFlag, sizeof(WORD));
				memcpy(&buf[2], &pModbus->wProtocolFlag, sizeof(WORD));
				memcpy(&buf[4], &pModbus->wLenth, sizeof(WORD));
				memcpy(&buf[6], &pModbus->UnitID, sizeof(BYTE));
				memcpy(&buf[7], &pModbus->byCMD, sizeof(BYTE));
				memcpy(&buf[8], &pModbus->wAddr, sizeof(WORD));
				memcpy(&buf[10], &pModbus->wValue, sizeof(WORD));
				if(memcmp(&buf,ModbusTCPOD_Table_Old[HeadIndex].data, sizeof(BYTE)*12))
				{
					g_ModbusTCPFalg = 1;
				}
				else
				{
					g_ModbusTCPFalg = 0;
				}
			break;
		default:
			break;
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
void ModbusTCPTx(BYTE *pBuf, MODBUS *pModbus)
{
	memcpy(&pBuf[MODBUSTCP_TRANSMITFILG_POSITION], &(pModbus->wTransmitFlag), MODBUSTRANSMITFILG_LEN);
	memcpy(&pBuf[MODBUSTCP_PROTOCOLFILG_POSITION], &(pModbus->wProtocolFlag), MODBUSPROTOCOLFILG_LEN);
	memcpy(&pBuf[MODBUSTCP_LENGTH_POSITION], &(pModbus->wLenth), MODBUSLENGTH_LEN);
	memcpy(&pBuf[MODBUSTCP_ID_POSITION], &(pModbus->UnitID), MODBUSID_LEN);
	memcpy(&pBuf[MODBUSTCP_CMD_POSITION], &(pModbus->byCMD), MODBUSCMD_LEN);
	memcpy(&pBuf[MODBUSTCP_ADDR_POSITION], &(pModbus->wAddr), MODBUSADDR_LEN);
	if(pModbus->byCMD == MODBUSTCP_WRITE_SINGLE_REGISTER)
		memcpy(&pBuf[MODBUSTCP_COUNT_POSITION], &(pModbus->wValue), MODBUSCOUNT_LEN);
	else
		memcpy(&pBuf[MODBUSTCP_COUNT_POSITION], &(pModbus->wCount), MODBUSCOUNT_LEN);
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
void ModebusTCP_Send(struct tcp_pcb *pcb)
{
	MODBUS					Modbus;
	MODBUS 					*pModbus = &Modbus;
	BYTE_T 					TCP_Send_Buf[MODBUSTCP_SENT_LEN] = {0};
	if(ResponseSendFlag ==0)
	{
		Setp_ModbusTCP();
	}
	SetModbusTCPPackges(pModbus);
	ModbusTCPTx(TCP_Send_Buf,pModbus);
	if((TCP_Send_Buf[MODBUSTCP_CMD_POSITION] == MODBUSTCP_READ_HOLDING_REGISTERS) || (TCP_Send_Buf[MODBUSTCP_CMD_POSITION] == MODBUSTCP_READ_INPUT_REGISTERS))
	{
		ResponseSendFlag = 1;
		Lwip_TcpWrite(pcb, TCP_Send_Buf, sizeof(TCP_Send_Buf));
	}
	else if(TCP_Send_Buf[MODBUSTCP_CMD_POSITION] == MODBUSTCP_WRITE_SINGLE_REGISTER)
	{
		if(g_ModbusTCPFalg == 0)
		{
			return ;
		}
		else
		{
			Lwip_TcpWrite(pcb, TCP_Send_Buf, sizeof(TCP_Send_Buf));
			Set_OldTCPSenbuf(TCP_Send_Buf);
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
void ModebusTCPSlave_Send(struct tcp_pcb *pcb)
{
	Lwip_TcpWrite(pcb, ModbusTCPResBuf, ModbusTCPRes_Length);
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
void HandleModbusTCPData(BYTE *pBuf)
{
		WORD HeadAddr, HeadIndex, TailIndex = 0;
		BYTE i = 1;
		HeadIndex 	= ModbusTCPOffset_Table.start;
		TailIndex 	= ModbusTCPOffset_Table.end +1;
//		HeadAddr 	= ModbusTCPOD_Table[HeadIndex].addr;
		if (pBuf[MODBUSTCP_CMD_POSITION] == MODBUSTCP_READ_HOLDING_REGISTERS || pBuf[MODBUSTCP_CMD_POSITION] == MODBUSTCP_READ_INPUT_REGISTERS)
		{
			ResponseSendFlag = 0;
			memcpy(&ModbusTCPODValue[HeadIndex].readvalue.wData, &pBuf[MODBUSTCP_RegData_POSITION], sizeof(WORD));
			ModbusTCPODValue[HeadIndex].readvalue.wData = htons(ModbusTCPODValue[HeadIndex].readvalue.wData);
			HeadIndex++;
			for(; HeadIndex <= TailIndex; HeadIndex++)
			{
		//		memcpy(&ModbusTCPODValue[HeadIndex].readvalue, &pBuf[((htons(ModbusTCPOD_Table[HeadIndex].addr) - HeadAddr) * sizeof(WORD) + MODBUSTCP_RegData_POSITION)], sizeof(WORD));
				memcpy(&ModbusTCPODValue[HeadIndex].readvalue.wData, &pBuf[(i * sizeof(WORD) + MODBUSTCP_RegData_POSITION)], sizeof(WORD));
				ModbusTCPODValue[HeadIndex].readvalue.wData = htons(ModbusTCPODValue[HeadIndex].readvalue.wData);
				i++;
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
int FindModbusTCPRegAddr(WORD_T addr)
{
	int i;

	for (i = 0; i < MAX_MODBUSTCPREG; i++)
	{
		if (ModbusTCPRegValue_Table[i].addr == addr)
		{
			return i;
		}
	}
	return -1;
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
void Get_ModbusTCPRegValues_byAddr(MODBUSTCPCMD_T ModbusTCPCmd)
{
	 WORD HeadAddr , Counts ,  n , Value = 0;
	 int offset = 0;
	memset(&ModbusTCPResBuf, 0, sizeof(ModbusTCPResBuf));
	memset(&ModbusTCPRes_Length, 0, sizeof(ModbusTCPRes_Length));
	if ((ModbusTCPCmd.byCMD == MODBUSTCP_READ_HOLDING_REGISTERS) || (ModbusTCPCmd.byCMD == MODBUSTCP_READ_INPUT_REGISTERS))
	{
		ModbusTCPRes_Length = ModbusTCPCmd.wLenth + 6;
		ModbusTCPCmd.wLenth = htons(ModbusTCPCmd.wLenth);
		memcpy(&ModbusTCPResBuf[MODBUSTCP_TRANSMITFILG_POSITION], &ModbusTCPCmd.wTransmitFlag, sizeof(ModbusTCPCmd.wTransmitFlag));
		memcpy(&ModbusTCPResBuf[MODBUSTCP_PROTOCOLFILG_POSITION], &ModbusTCPCmd.wProtocolFlag, sizeof(ModbusTCPCmd.wProtocolFlag));
		memcpy(&ModbusTCPResBuf[MODBUSTCP_ID_POSITION], &ModbusTCPCmd.UnitID, sizeof(ModbusTCPCmd.UnitID));
		memcpy(&ModbusTCPResBuf[MODBUSTCP_CMD_POSITION], &ModbusTCPCmd.byCMD, sizeof(ModbusTCPCmd.byCMD));
		memcpy(&ModbusTCPResBuf[4], &ModbusTCPCmd.wLenth, sizeof(ModbusTCPCmd.wLenth)); //锟斤拷锟斤拷
		memcpy(&ModbusTCPResBuf[8], &ModbusTCPCmd.DataLenth, sizeof(ModbusTCPCmd.DataLenth)); //锟斤拷莩锟斤拷锟�
		Counts = ModbusTCPCmd.DataLenth / 2;
		HeadAddr = ModbusTCPCmd.wAddr;
		n = 0;
		while (Counts--)
		{
			offset = FindModbusTCPRegAddr(HeadAddr);
			if (offset == -1)
			{
				memcpy(&ModbusTCPResBuf[10 + n], &Value, sizeof(WORD));
				n += 2;
			}
			else
			{
				memcpy(&ModbusTCPResBuf[10 + n], &ModbusTCPRegValue_Table[offset].value, sizeof(WORD));
				n += 2;
			}
			HeadAddr++;
		}
		ResponseFlag = 1;
	}
	return ;
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
void Set_ModbusTCPRegValue_byAddr(MODBUSTCPCMD_T ModbusTCPCmd)
{
	int Offset = 0;
	WORD length = 0;
	memset(&ModbusTCPResBuf, 0, sizeof(ModbusTCPResBuf));
	if (ModbusTCPCmd.byCMD == MODBUSTCP_WRITE_SINGLE_REGISTER)
	{
		Offset = FindModbusTCPRegAddr(ModbusTCPCmd.wAddr);
		if(Offset == -1)
		{
			return;
		}
		ModbusTCPRes_Length = 12;
		length = ModbusTCPRes_Length / 2;
		memcpy(&ModbusTCPRegValue_Table[Offset].value, &ModbusTCPCmd.Value, sizeof(WORD));
		memcpy(&ModbusTCPResBuf[MODBUSTCP_TRANSMITFILG_POSITION], &ModbusTCPCmd.wTransmitFlag, sizeof(ModbusTCPCmd.wTransmitFlag));
		memcpy(&ModbusTCPResBuf[MODBUSTCP_PROTOCOLFILG_POSITION], &ModbusTCPCmd.wProtocolFlag, sizeof(ModbusTCPCmd.wProtocolFlag));
		memcpy(&ModbusTCPResBuf[MODBUSTCP_ID_POSITION], &ModbusTCPCmd.UnitID, sizeof(ModbusTCPCmd.UnitID));
		memcpy(&ModbusTCPResBuf[MODBUSTCP_CMD_POSITION], &ModbusTCPCmd.byCMD, sizeof(ModbusTCPCmd.byCMD));
		ModbusTCPCmd.wAddr = htons(ModbusTCPCmd.wAddr);
		length = htons(length);
		memcpy(&ModbusTCPResBuf[4], &length, sizeof(WORD)); //锟斤拷锟斤拷
		memcpy(&ModbusTCPResBuf[8], &ModbusTCPCmd.wAddr, sizeof(ModbusTCPCmd.wAddr)); //锟斤拷锟斤拷
		ModbusTCPResBuf[10] = 0;
		ModbusTCPResBuf[11] = 1;
		ResponseFlag = 1;
	}
	return;
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
void Set_ModbusTCPODValueByDataID_2(DWORD_T id, DWORD_T value)
{
	int j = 0;

	for (j = 0; j < MAX_MODBUSTCPOD; j++)
	{
		if (ModbusTCPODValue[j].dataid == id)
		{
			memcpy(&ModbusTCPODValue[j].writevalue, &value, sizeof(DWORD));
			return;
		}
	}
	return;
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
void Set_ModbusTCPRegValue_byDataID_2(DWORD_T id, WORD_T value)
{
	int j = 0;
	int Offset;

	for (j = 0; j < MAX_MODBUSTCPOD; j++)
	{
		if(ModbusTCPReg_Table[j].regdataid == id)
		{
			Offset = FindModbusTCPRegAddr(ModbusTCPReg_Table[j].regaddr);
			if (Offset == -1)
			{
				return;
			}
			memcpy(&ModbusTCPRegValue_Table[Offset].value, &value, sizeof(WORD));
			return;
		}
	}
	return;
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
void  Get_ModbusTCPRegValue_byDataID_2(DWORD_T id, DBVALUE *dbValue)
{
	int j = 0;
	int Offset;

	for (j = 0; j < MAX_MODBUSTCPREG; j++)
	{
		if (ModbusTCPReg_Table[j].regdataid == id)
		{
			Offset = FindModbusTCPRegAddr(ModbusTCPReg_Table[j].regaddr);
			if (Offset == -1)
			{
				return;
			}
			memcpy(dbValue, &ModbusTCPRegValue_Table[Offset].value, sizeof(DBVALUE));
			return;
		}
	}
	return;
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
void ModbusTCPSlave_Recv(struct tcp_pcb *pcb, struct pbuf *p)
{
	MODBUSTCPCMD_T ModbusTCPCmd;
	BYTE_T  recv_buf[ModbusTCP_RECV_LEN] = {0};

	memcpy(recv_buf, p->payload, p->tot_len);
	memset(&ModbusTCPCmd, 	0, 	sizeof(ModbusTCPCmd));
	memcpy(&ModbusTCPCmd.wTransmitFlag, &recv_buf[MODBUSTCP_TRANSMITFILG_POSITION], sizeof(WORD));
	memcpy(&ModbusTCPCmd.wProtocolFlag, &recv_buf[MODBUSTCP_PROTOCOLFILG_POSITION], sizeof(WORD));
	memcpy(&ModbusTCPCmd.UnitID, &recv_buf[6], sizeof(BYTE));
	memcpy(&ModbusTCPCmd.byCMD, &recv_buf[7], sizeof(BYTE));
	memcpy(&ModbusTCPCmd.wAddr, &recv_buf[8], sizeof(WORD));
	memcpy(&ModbusTCPCmd.Value, &recv_buf[10], sizeof(WORD));
	memcpy(&ModbusTCPCmd.DataLenth, &recv_buf[10], sizeof(WORD));

	ModbusTCPCmd.wTransmitFlag = htons(ModbusTCPCmd.wTransmitFlag);
	ModbusTCPCmd.wProtocolFlag = htons(ModbusTCPCmd.wProtocolFlag);
	ModbusTCPCmd.wAddr = htons(ModbusTCPCmd.wAddr);
	ModbusTCPCmd.Value = htons(ModbusTCPCmd.Value);
	ModbusTCPCmd.DataLenth = htons(ModbusTCPCmd.DataLenth);

	ModbusTCPCmd.DataLenth = ModbusTCPCmd.DataLenth*2;
	ModbusTCPCmd.wLenth = ModbusTCPCmd.DataLenth + 3;
	switch(ModbusTCPCmd.byCMD)
	{
		case MODBUSTCP_READ_HOLDING_REGISTERS:
			Get_ModbusTCPRegValues_byAddr(ModbusTCPCmd);
			break;
		case MODBUSTCP_READ_INPUT_REGISTERS:
			Get_ModbusTCPRegValues_byAddr(ModbusTCPCmd);
			break;
		case MODBUSTCP_WRITE_SINGLE_REGISTER:
			Set_ModbusTCPRegValue_byAddr(ModbusTCPCmd);
			break;
		default:
			break;
	}
}

