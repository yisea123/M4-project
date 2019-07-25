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
|  Creation : 2018年11月08日                                                                                                                                                                               |
|  Revision :                                                               |
+==========================================================================*/
#ifndef _TASKMODBUSTCP_H_
#define _TASKMODBUSTCP_H_
#include  "studioprotocol.h"
#include "tasklwiptcp.h"
#include "kernel/include/type.h"
#include "kernel/include/task.h"
#include "database.h"
/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Structure                                                           |
+------------------------------------------------------------------------------*/
#pragma pack(push)					// Master Value Table
#pragma pack(1)
typedef struct tagMODBUSTCPODTABLE
{
	DWORD 	dataid;
	BYTE 	id;
	BYTE 	cmd;
	WORD 	addr;
}MODBUSTCPOD_Table;
#pragma pack(pop)
typedef struct tag_MODBUSTCPODVALUE {
	DWORD 	dataid;
	DBVALUE readvalue;
	DBVALUE writevalue;
} MODBUSTCPODVALUE_T;

#pragma pack(push)					// Slave Value Table
#pragma pack(1)
typedef struct tagMODBUSTCPREG
{
	DWORD regdataid;
	WORD 	regaddr;
}MODBUSTCPREG_Table;
#pragma pack(pop)

typedef 	struct tag_MODBUSTCPREGVALUE {
	WORD addr;
	WORD value;
} MODBUSTCPREGVALUE_Table;

typedef struct tag_MODBUSTCPOFFSET {
	WORD start;
	WORD end;
} MODBUSTCPOFFSET_Table;

typedef struct tagMODBUS { 		// Modbus 数据
		WORD  wTransmitFlag;
		WORD  wProtocolFlag;
		WORD  wLenth;
		BYTE  UnitID;
		BYTE  byCMD;
		WORD  wAddr;
		WORD  wCount;
		WORD  wValue;
} MODBUS;

#pragma pack(push)
#pragma pack(1)
typedef struct tag_MODBUSTCPCMD {
	WORD  wTransmitFlag;
	WORD  wProtocolFlag;
	WORD  wLenth;
	BYTE  UnitID;
	BYTE  byCMD;
	WORD  DataLenth;
	WORD  wAddr;
	WORD  Value;
} MODBUSTCPCMD_T;
#pragma pack(pop)

typedef struct tag_MODBUSTCPODOLD {
	BYTE 	data[12];
} MODBUSTCPODOLD_T;
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#define 	MAX_MODBUSTCPOD 		400
#define 	MAX_MODBUSTCPREG 		300
#define ModbusTCPSetMODEInfoFRAME                  1
#define ModbusTCPODFRAME                  		   2
#define ModbusTCPENDFRAME                  		   3
#define ModbusTCPSAVEFRAME                  	   4
#define ModbusTCPModeINFOBUF_OFFSET_INFLASH        0
#define MODBUSTCP_CMD_POSITION 					   7


#define	  ModbusTCPCONFIGDATA_OFFSET               16
#define	  ModbusTCPOD_OFFSET               		   19
#define   ModbusTCPModeInfo_SIZE                            1

#define ModbusTCPODBUF_OFFSET_INFLASH            	3

#define 	M2CTCP_SENT_PERIOD 						1000  //发送周期
#define		MosbudTCP_MaxPack						10   //最大请求寄存器数 最大127

#define		State_Null		 					 	0
#define		State_Down		  						1
#define 	M2CTCP_TX								3
#define 	M2CTCP_RX 								4

#define 	MODBUSTCP_SENT_LEN 						12
#define 	MODBUSTCP_OD_OFFSET 					3
#define 	TOTAL_PACKET_MODE_POSITION 				0
#define 	TOTAL_PACKET_COUNT_POSITION 			1

/* Modbus function codes */
#define MODBUSTCP_READ_COILS                    0x01
#define MODBUSTCP_READ_DISCRETE_INPUTS          0x02
#define MODBUSTCP_READ_HOLDING_REGISTERS        0x03
#define MODBUSTCP_READ_INPUT_REGISTERS          0x04
#define MODBUSTCP_WRITE_SINGLE_COIL             0x05
#define MODBUSTCP_WRITE_SINGLE_REGISTER         0x06
#define MODBUSTCP_READ_EXCEPTION_STATUS         0x07
#define MODBUSTCP_WRITE_MULTIPLE_COILS          0x0F
#define MODBUSTCP_WRITE_MULTIPLE_REGISTERS      0x10
#define MODBUSTCP_REPORT_SLAVE_ID               0x11
#define MODBUSTCP_MASK_WRITE_REGISTER           0x16
#define MODBUSTCP_WRITE_AND_READ_REGISTERS      0x17

#define		MODBUSTCP_TRANSMITFILG_POSITION 		0
#define 	MODBUSTCP_PROTOCOLFILG_POSITION 		2
#define 	MODBUSTCP_LENGTH_POSITION 		 		4
#define 	MODBUSTCP_ID_POSITION 					6
#define 	MODBUSTCP_CMD_POSITION 					7
#define 	MODBUSTCP_ADDR_POSITION 				8
#define 	MODBUSTCP_RegData_POSITION 				9
#define 	MODBUSTCP_COUNT_POSITION 			    10

#define 	MODBUSTRANSMITFILG_LEN					2
#define 	MODBUSPROTOCOLFILG_LEN					2
#define 	MODBUSLENGTH_LEN						2
#define 	MODBUSID_LEN							1
#define 	MODBUSCMD_LEN							1
#define 	MODBUSADDR_LEN							2
#define 	MODBUSCOUNT_LEN							2
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#define 	MODBUSTCP_NotUsed 			0x00
#define 	MODBUSTCP_MASTER 		    0x11
#define 	MODBUSTCP_SLAVE 		    0x22

#define 	MODBUSTCPOD_OFFSET  			0x3

#define 	PROTOCOL_MODBUSTCPMASTER_LEN 				8
#define 	PROTOCOL_MODBUSTCPSLAVE_LEN					6
#define 	MODBUSTCPOD_LINE 				0x1
#define		ModbusTCP_RECV_LEN				16
#define		ModbusTCP_SENT_LEN				13
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
extern long SendTime;
extern long ReConnect;
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
BYTE ModbusTCPDealRecvParm(BYTE parmtype, BYTE *buf);
WORD Fetchword(BYTE *data, int offset);
void 	Read_ModbusTCPData_FromFlash(void);
void 	ModebusTCP_Send(struct tcp_pcb *pcb);
void 	ModebusTCPSlave_Send(struct tcp_pcb *pcb);
void 	HandleModbusTCPData(BYTE *pBuf);
void ModbusTCPSlave_Recv(struct tcp_pcb *pcb, struct pbuf *p);
void Get_ModbusTCPODValue_ByDataID_2(DWORD_T id, DBVALUE *dbValue);
void Set_ModbusTCPODValue_ByDataID_2(DWORD_T id, DWORD_T value);
void Set_ModbusTCPODValueByDataID_2(DWORD_T id, DWORD_T value);
void Set_ModbusTCPRegValue_byDataID_2(DWORD_T id, WORD_T value);
void  Get_ModbusTCPRegValue_byDataID_2(DWORD_T id, DBVALUE *dbValue);
void CreateTaskModbusTCP(void);
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
extern BYTE		pTotalMode;
extern WORD 	pTotalCount;
extern BYTE 	ResponseFlag ;
extern BYTE 	ModbusTCPResBuf[MAX_MODBUSTCPREG];
extern WORD 	ModbusTCPRes_Length;
extern HTASK 	g_htask_ModbusTCP;

#endif
