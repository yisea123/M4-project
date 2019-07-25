/*==========================================================================+
|  File     : taskinet.h                                                    |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : ZZYuan                                                        |
|  Version  : v1.00                                                         |
|  Creation : 2018年10月25日                                                                                                                                                                               |
|  Revision :                                                               |
+==========================================================================*/
#ifndef TASKINET_H_
#define TASKINET_H_

#include "studioprotocol.h"
#include "tasklwiptcp.h"
#include "kernel/include/type.h"
#include "kernel/include/task.h"
#include "kernel/include/device.h"
#include "database.h"
/*****************************************************************************************************************************/
/***********************************************************tmIoT Studio******************************************************/
/*****************************************************************************************************************************/
/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Structure                                                           |
+------------------------------------------------------------------------------*/
#pragma pack(push)
#pragma pack(1)
typedef struct tagINETODPARA
{
	DWORD   dwTableID;
	DWORD   dwDeviceEntityAID;
    DWORD   dwiNetDataID;
    WORD    wiNetDataType;
    WORD    wiNetUom;
    BYTE    byiNetPermission;
    BYTE    byiNetPrecision;
    DBVALUE Value;
}iNetODPara;
#pragma pack(pop)

typedef struct tagINETODPARATAB
{
    WORD  wODNum;
    WORD  wCurrentODNum;
    iNetODPara *pODList;
}INETODParaTab;

#pragma pack(push)
#pragma pack(1)
typedef struct tagINETMODEMSG
{
    BYTE  byMode;
    BYTE  byProtocolType;
    BYTE  byProVer;
}iNetMODEMES;
#pragma pack(pop)

typedef struct TAGDEVICEBOOTLOADMARK
{
    BYTE   byNetVerMark;
    BYTE   byNetAddrMark;
    BYTE   byHeartBeatMark;
    BYTE   byDeviceSNMark;
    WORD   wDeviceNum;
}__attribute__((packed, aligned(1))) BOOTLOADMARK;
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#define         MAX_INETODLIST                      200

#define         IENTSetIPInfoFRAME                  1
#define         IENTGetIPInfoFRAME                  2
#define         IENTPARAFRAME                       3
#define         INETODFRAME                         4
#define         INETENDFRAME                        5
#define			INETSAVEFRAME                       6

#define			INETIPINFOBUF_OFFSET_INFLASH        0
#define			INETPACKETSTAMP_OFFSET_INFLASH      4
#define 		INETPARABUF_OFFSET_INFLASH          27
#define 		INETODBUF_OFFSET_INFLASH            29

#define 		INETCONFIGDATA_OFFSET               16

#define     	INETIPInfo_SIZE                     26
#define     	INETPARA_SIZE                       2

#define   		RW      						    1
#define  	 	RO                                  2
#define  		WO                                  3

#define 		INETMODENOTUSED                     0x00
#define 		INETMODECLIENT                      0x11
#define 		INETMODESERVER                      0x22
#define		 	INETPROTYPETCP                      0x33
#define 		INETPROTYPEUDP                      0x44

#define			 MAX_DEVICEAID                      50


#define 		STANDARD_PRO                        0
#define         ADDITIONAL_PRO                      1
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/

/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
extern BYTE iNetDealRecvParm(BYTE parmtype, BYTE *buf);
extern void Read_iNetMacAddr_FromEeprom(void);
extern void Read_iNetModeMeg_FromFlash(void);
extern iNetMODEMES    iNetModeMeg;
extern INETODParaTab  iNetODParaTab;
extern	HTASK 	g_htask_iNet;
/*****************************************************************************************************************************/
/***********************************************************IoT Sever*********************************************************/
/*****************************************************************************************************************************/

/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/
//+++++++
// Table define
//+++++++
typedef BYTE_T (* pFUNC_DEAL)(BYTE* pData,WORD wlen,BYTE byMode);
typedef struct tagDB_PROATTRIBUTE
{
     DWORD                   dwID;
     pFUNC_DEAL              DealFunc;
}DB_PROATTRIBUTE;

//++++++++
// iNet Error ID Definetion
//+++++++
#define     INET_OK                             0x0
#define     INET_ERROR_ID_MAX_SIZE              0x1
#define     INET_ERROR_ID_LENGTH                0x2
#define     INET_ERROR_ID_VERSION               0x3
#define     INET_ERROR_ID_CRC                   0x4
#define     INET_ERROR_ID_STATICVERSION         0x5
#define     INET_ERROR_ID_MACERROR              0x6
#define     INET_ERROR_ID_PROFLAG               0x7
#define     INET_ERROR_ID_NewLocalIP            0x8
#define     INET_ERROR_ID_PROTOCOLID            0x9
#define     INET_ERROR_ID_DATAIDNUM             0xA
/*------------------------------------------------------------------------------+
|           Structure                                                           |
+------------------------------------------------------------------------------*/
//++++++++
// iNet frame header
//++++++++
typedef struct tagHeader
{
    WORD       wLength;     //帧长度
    WORD       wVersion;      //协议版本
    BYTE       byPacketStamp; //时间戳
    BYTE       byFlag;        //标志位
    DWORD      dwProtocolID;  //协议ID
    BYTE       byControllerID[6]; //控制器ID
}__attribute__((packed, aligned(1))) FRAMEHEADER;

typedef struct TAGPROCONTENT
{
    BYTE  byFlag;           //标记
    WORD  wDataIDCount;     //Data ID个数
    DWORD dwDataID;         //Data ID
    WORD  wDataType;        //标准数据类型
    WORD  wUOM;             //单位
    BYTE  byPrecision;      //精度
    WORD  wRowCount;        //资料笔数
    WORD  wRowDataLen;      //数据资料长度
    DBVALUE Value;
}PROCONTENT;//__attribute__((packed, aligned(1)))
typedef struct TAGREADDATA
{
    WORD   DataIDCount;
    DWORD *DataIDList;
}PROREADDATA;
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
//++++++++++++++
// iNet Protocol ID Definetion
//+++++++++++++++
//standard protocol
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_ProAssignedAddr           0x02000001
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_ProTimingCheck            0x02000002
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_ProTimingAck              0x02010002
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_ProHeartBeat              0x02000003
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_ProVersion                0x02000004
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_ProReadData               0x02000006
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_ProWriteData              0x02000007
#define     COMMUNICATION_PROTOCOLS_ID_APPLY_DateDictionary            0x02000008

#define     COMMUNICATION_PROTOCOLS_ID_REPLY_ProAssignedAddr           0x01000001
#define     COMMUNICATION_PROTOCOLS_ID_REPLY_ProTimingCheck            0x01000002
#define     COMMUNICATION_PROTOCOLS_ID_REPLY_ProTimingAck              0x01010002
#define     COMMUNICATION_PROTOCOLS_ID_REPLY_proHeartBeat              0x01000003
#define     COMMUNICATION_PROTOCOLS_ID_REPLY_ProVersion                0x01000004
#define     COMMUNICATION_PROTOCOLS_ID_REPLY_ProReadData               0x01000006
#define     COMMUNICATION_PROTOCOLS_ID_REPLY_ProWriteData              0x01000007
#define     COMMUNICATION_PROTOCOLS_ID_REPLY_DataDictionary            0x01000008

//additional protocol
#define     COMMUNICATION_ADDPROTOCOLS_ID_APPLY_ProHeartBeat          0x02000006
#define     COMMUNICATION_ADDPROTOCOLS_ID_APPLY_SendDeviceSN          0x02000010
#define     COMMUNICATION_ADDPROTOCOLS_ID_APPLY_SendSensorData        0x02000011
#define     COMMUNICATION_ADDPROTOCOLS_ID_APPLY_WriteDataToSensor     0x0200000C
#define     COMMUNICATION_ADDPROTOCOLS_ID_APPLY_AlarmData             0x02000012

#define     COMMUNICATION_ADDPROTOCOLS_ID_REPLY_ProHeartBeat          0x01000006
#define     COMMUNICATION_ADDPROTOCOLS_ID_REPLY_SendDeviceSN          0x01000010
#define     COMMUNICATION_ADDPROTOCOLS_ID_REPLY_SendSensorData        0x01000011
#define     COMMUNICATION_ADDPROTOCOLS_ID_REPLY_WriteDataToSensor     0x0100000C
#define     COMMUNICATION_ADDPROTOCOLS_ID_REPLY_AlarmData             0x01000012


//++++++++++++++
// iNet Data Type
//+++++++++++++++
//+++++++
// iNet Data Type Definetion
//+++++++
#define DATATYPE_NULL                0x0100
#define DATATYPE_Variant             0x0200
#define DATATYPE_ByteBool            0x0300
#define DATATYPE_WordBool            0x0400
#define DATATYPE_Byte                0x0500
#define DATATYPE_WORD                0x0600
#define DATATYPE_DWORD               0x0700
#define DATATYPE_Int16               0x0900
#define DATATYPE_Int32               0x0A00
#define DATATYPE_Int64               0x0B00
#define DATATYPE_Float               0x0C00



#define     Family_Protocol_Version            0x07E0      //协议版本
#define     Static_Protocol_Version            0           //静态协议版本
#define     Dynamic_Protocol_Version           0           //动态协议版本
#define     Need_FeedBack                      0x1         //需要反馈
#define     NoNeed_FeedBack                    0           //不需要反馈
#define     CRC32                              0xEDB88320
#define     iNet_HBTimeout_Timer                2000
#define     iNet_FlowLink_Timer                 1000
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
enum        tagProID_INDEX
{
    PROTOCOLID_ApplyAssignedAddr ,  //0
    PROTOCOLID_ApplyTimingCheck ,
    PROTOCOLID_ApplyTimingAck ,
    PROTOCOLID_ApplyHeartBeat ,
    PROTOCOLID_ApplyProtocolVer ,
    PROTOCOLID_ApplyReadData ,
    PROTOCOLID_ApplyWriteData ,

    PROTOCOLID_ReplyAssignedAddr ,  //7
    PROTOCOLID_ReplyTimingCheck,
    PROTOCOLID_ReplyTimingAck ,
    PROTOCOLID_ReplyHeartBeat ,
    PROTOCOLID_ReplyProtocolVer ,
    PROTOCOLID_ReplyReadData ,
    PROTOCOLID_ReplyWriteData ,

    PROTOCOLID_DATA_NUM ,          //14
};

enum      tagAddProID_INDEX
{
	ADDPROTOCOLID_ApplyHeartBeat,
	ADDPROTOCOLID_APPLYSendDeviceSN,
	ADDPROTOCOLID_APPLYSendSensorData,
	ADDPROTOCOLID_APPLYWriteDataToSensor,
	ADDPROTOCOLID_APPLYAlarmData,

	ADDPROTOCOLID_ReplyHeartBeat,
	ADDPROTOCOLID_ReplySendDeviceSN,
	ADDPROTOCOLID_ReplySendSensorData,
	ADDPROTOCOLID_RePLYWriteDataToSensor,
	ADDPROTOCOLID_RePLYAlarmData,

	ADDPROTOCOLID_DATA_NUM,
};

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
static void CreateCRC32Table();
static unsigned int GetCRC32(char* pData, unsigned  int Length);
static unsigned int GetCRC32B(unsigned int CRCCode, char* pData, unsigned int Length);
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
extern BYTE iNetParseRecvData(BYTE *pData, int nlength, BYTE byMode);
extern void CreateTaskiNet();
#endif /* TASKINET_H_ */
