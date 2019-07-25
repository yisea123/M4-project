/*==============================================================================+
|  File     : Studiolwipudp.c                                                   |
|  Function :                                                                   |
|  Task     :                                                                   |
|-------------------------------------------------------------------------------|
|  Compile  :                                                                   |
|  Link     :                                                                   |
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : YW                                                              	|
|  Version  : V2.00                                                             |
|  Creation : 2018/01/19                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include  "studioprotocol.h"
#include  "userlib.h"
#include  "driver/include/_flash.h"
#include  "taskrs485.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
BYTE	u_TaskCanFlag = 0;
/*===========================================================================+
|           Constructor and destructor                                       |
+============================================================================*/
/*===========================================================================+
|           Operations                                                       |
+============================================================================*/
/*===========================================================================+
|         Inner Global variable                                              |
+===========================================================================*/
BYTE_T              u_bReProPackstamp_HeartBeat = 0x0;
BYTE_T              g_bReProPackstamp_StartFrame = 0x01;
BYTE_T              u_byPacketStamp_EndFrame;
BYTE_T              u_byPacketStamp = 0x0;
BYTE_T              u_byOldPacketStamp = 0x0;
BYTE_T              u_bReProSpecial_Flag = 0x0;
WORD_T              u_wReProLength = 0x0;

WORD_T              u_wPakageSize;
WORD_T              u_wComponentCount = 0;

BYTE_T              u_wSerialNo = 0;

BYTE_T				u_cLength	= 0;
BYTE_T              u_byTotal_RecvPakageNum;

BYTE_T              u_ServerIP_WriteToEeprom[4];
BYTE_T              u_ServerPort_WriteToEeprom[2];
//BYTE_T              u_ServerMAC_WriteToEeprom[6];

BYTE_T              u_TmIoTIP_WriteToEeprom[4];
BYTE_T              u_TmIoTMASK_WriteToEeprom[4];
BYTE_T              u_TmIoTGATEWAY_WriteToEeprom[4];
BYTE_T              u_TmIoTMAC_WriteToEeprom[6];

BYTE_T              u_ServerConfig_Flag = 0x77; //Config iNetServer IP Flag
BYTE_T              u_TmIoTConfig_Flag = 0x77; //Config tmIoT Device IP Falg
//BYTE_T              u_WriteOdToFlash_Flag = 0x77;//the flag of write canopen Od to flash
//BYTE_T              u_WriteOdParaToEE_Flag = 0x77;//the flag of write Od para to eeprom,such as slave_count,Od_count and node ID
//DWORD_T             u_WriteSlaveParaToEE_Flag = 0x77;
BYTE_T              u_ObjDataChange_Flag = 0x66;
//
//BYTE_T              g_byODCnts[CANOPEN_MAX_SLAVE] = {1};
//
//WORD_T              g_wTotal_ODcnts = 0;
//WORD_T              g_wRevTotal_ODcnts = 0;
//BYTE_T              g_byCanSlave_Count = 0;
//BYTE_T              g_byRcvCanSlave_Count = 0;
//BYTE_T              g_byDownLoadOd_Flag = 0;
//
//WORD_T              g_byRcvTotal_ODCnts = 0;

BYTE_T              SendProBuf[PACKLENGTH];

TASKDATA_T			TaskData[MaxTask];

BYTE_T				g_RcvPortflag  = 1;
BYTE_T				u_cConnectPort = 1;

static              BYTE_T                reply_modeflag = 0;

BYTE_T				g_cTaskTotalCount;
BYTE_T				g_cSendStartDebugFBNum;

int u_ModbusPort_Line[MAX_RS485] = {0, 0};

static BYTE u_ProtocolNum = 0;
/*===========================================================================+
|           Helpers  Implement                                               |
+===========================================================================*/

BOOL_T  ApplyHandshake( BYTE_T FlagRecvOrSend);
BOOL_T  ReplyHandshake(  BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyStartFrame( BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyStartFrame( BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyDataFrame(  BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyDataFrame(  BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyCommDataFrame(  BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyCommDataFrame(  BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyGlobalVariableFrame(  BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyGlobalVariableFrame(  BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyEndFrame(   BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyEndFrame(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplySaveConfigData(   BYTE_T  FlagRecvOrSend);
BOOL_T  ReplySaveConfigData(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyRealtimeDataAttr(  BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyRealtimeDataAttr(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyConfigServerIP(    BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyConfigServerIP(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyConfigTmIoTIP(   BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyConfigTmIoTIP(  BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyDebugBkpt( BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyDebugBkpt(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyDebugRun(   BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyDebugRun(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyDebugPause(   BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyDebugPause(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyDebugStep(   BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyDebugStep(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyDebugMode(    BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyDebugMode(    BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyCanSlavePara(BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyCanSlavePara(BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyDownloadOd(    BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyDownloadOd(    BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyTaskData(    BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyTaskData(    BYTE_T  FlagRecvOrSend);


BOOL_T  ApplyRealtimeData(  BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyRealtimeData(   BYTE_T  FlagRecvOrSend);

BOOL_T  ApplyComponentScanCnt(  BYTE_T  FlagRecvOrSend);
BOOL_T  ReplyComponentScanCnt(   BYTE_T  FlagRecvOrSend);

BOOL_T	ApplyCanEndFrame(BYTE_T  FlagRecvOrSend);
BOOL_T	ReplyCanEndFrame(BYTE_T  FlagRecvOrSend);

BOOL_T	ApplyCanSaveConfigData(BYTE_T  FlagRecvOrSend);
BOOL_T	ReplyCanSaveConfigData(BYTE_T  FlagRecvOrSend);

BOOL_T  SaveObjectList( WORD_T   wComponentcount, BYTE_T  *bypData );
BOOL_T  SaveDevicePara( BYTE_T  byParaCount ,  BYTE_T *bypData);

void    SaveOdPara(BYTE_T g_byCanSlave_Count);
BOOL_T  ReadOdPara(void);

BOOL_T	ApplyModbusRTUPara(BYTE_T  FlagRecvOrSend);
BOOL_T	ReplyModbusRTUPara(BYTE_T  FlagRecvOrSend);

BOOL_T	ApplyModbusRTUOD(BYTE_T  FlagRecvOrSend);
BOOL_T	ReplyModbusRTUOD(BYTE_T  FlagRecvOrSend);

BOOL_T	ApplyModbusRTUEndFrame(BYTE_T  FlagRecvOrSend);
BOOL_T	ReplyModbusRTUEndFrame(BYTE_T  FlagRecvOrSend);

BOOL_T	ApplyModbusRTUSaveConfigData(BYTE_T  FlagRecvOrSend);
BOOL_T	ReplyModbusRTUSaveConfigData(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyiNetSetIPInfo(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyiNetSetIPInfo(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyiNetGetIPInfo(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyiNetGetIPInfo(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyiNetPARA(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyiNetPara(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyiNetODData(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyiNetODData(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyiNetEndFrame(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyiNetEndFrame(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyiNetSaveConfigData(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyiNetSaveConfigData(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyModbusTCPSETMODEInfo(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyModbusTCPSetMODEInfo(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyModbusTCPOD(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyModbusTCPOD(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyModbusTCPENDFRAME(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyMODBUSTCPENDFRAM(BYTE_T  FlagRecvOrSend);

BOOL_T ApplyModbusTCPSaveConfigData(BYTE_T  FlagRecvOrSend);
BOOL_T ReplyMODBUSTCPSAVEFRAM(BYTE_T  FlagRecvOrSend);
/*===========================================================================+
|          Global variable                                                   |
+===========================================================================*/
BYTE_T                g_byRecv_Buffer[MAX_RECVNUM];
BYTE_T                g_OBJDATACHANGE_FLAG_OFFSET = 0;//0X18;// The offset for save the object List data flag
BYTE_T                g_OBJLISTNUM_OFFSET = 0x01;//0X19; // The offset for save the object list count
BYTE_T				  g_CANFLAG_OFFSET = 0x3;
BYTE_T				  g_TASKTOTALNUM_OFFSET = 0x4;

BYTE_T              g_SERVERCONFIGFLAG_OFFSET = 0x1C;// The offset for config server para Flag(just for eeprom)
BYTE_T              g_SERVERIP_OFFSET = 0x1D;//The offset for config server ip
BYTE_T              g_SERVERPORT_OFFSET = 0x21;//The offset for config server port RS485

BYTE_T              g_TMIOTCONFIGFLAG_OFFSET = 0x23;// The offset for config device para flag
BYTE_T              g_TMIOTIP_OFFSET = 0x24; // The offset for config device ip
BYTE_T              g_TMIOTMASK_OFFSET = 0x28; // The offset for config device Mask
BYTE_T              g_TMIOTGATEWAY_OFFSET = 0x2C; // The offset for config device gateway

DWORD_T             g_RealtimeData[64];
BYTE_T              g_byObJlistFlag = 0;

const    PROTOMAP_T    ProtoMap[] =
{
    {PROTOCOL_ID_APPLY_HANDSHAKE                             ,  ApplyHandshake                }, //01000006       tmIoT Studio发送握手协议
    {PROTOCOL_ID_APPLY_STARTFRAME                            ,  ApplyStartFrame               }, //01000007       tmIoT Studio发送开始协议
    {PROTOCOL_ID_APPLY_DATAFRAME                             ,  ApplyDataFrame                }, //01000008       tmIoT Studio发送数据协议
    {PROTOCOL_ID_APPLY_ENDFRAME                              ,  ApplyEndFrame                 }, //01000009       tmIoT Studio发送结束协议
    {PROTOCOL_ID_APPLY_SAVECONFIGDATA                        ,  ApplySaveConfigData           }, //0100000A       tmIoT Studio发送保存协议
    {PROTOCOL_ID_APPLY_REALTIMEDATAATTR                      ,  ApplyRealtimeDataAttr         }, //0100000B       tmIoT Studio请求实时数据属性
    {PROTOCOL_ID_APPLY_CONFIGSERVERIP                        ,  ApplyConfigServerIP           }, //0100000C       tmIoT Studio发送服务器信息
    {PROTOCOL_ID_APPLY_CONFIGTMIOTIP                         ,  ApplyConfigTmIoTIP            }, //0100000D       tmIoT Studio发送设备信息
    {PROTOCOL_ID_APPLY_DEBUGBKPT                             ,  ApplyDebugBkpt                }, //0100000E       tmIoT Studio发送断点信息
    {PROTOCOL_ID_APPLY_COMMDATAFRAME                         ,  ApplyCommDataFrame            }, //01000010       tmIoT Studio发送逻辑缓存数据协议
	{PROTOCOL_ID_APPLY_GLOBALVARIABLEFRAME                   ,  ApplyGlobalVariableFrame      }, //01000011       tmIoT Studio发送全局变量数据协议
    {PROTOCOL_ID_APPLY_DEBUGRUN                              ,  ApplyDebugRun                 }, //01000020       tmIoT Studio发送运行协议
    {PROTOCOL_ID_APPLY_DEBUGPAUSE                            ,  ApplyDebugPause               }, //01000021       tmIoT Studio发送暂停协议
    {PROTOCOL_ID_APPLY_DEBUGSTEP                             ,  ApplyDebugStep                }, //01000022       tmIoT Studio发送单步协议
    {PROTOCOL_ID_APPLY_DEBUGMODE                             ,  ApplyDebugMode                }, //01000023       tmIoT Studio发送调试协议
    {PROTOCOL_ID_APPLY_TASKDATA                              ,  ApplyTaskData                 }, //01000028       tmIoT Studio发送Task数据
    {PROTOCOL_ID_APPLY_REALTIMEDATA                          ,  ApplyRealtimeData             }, //0100002A       tmIoT Studio请求实时数据
    {PROTOCOL_ID_APPLY_COMPONENTSCANCNT                      ,  ApplyComponentScanCnt         }, //0100002B       tmIoT Studio请求元件运行次数
	{PROTOCOL_ID_APPLY_MODBUSRTUPARA						 ,	ApplyModbusRTUPara			  }, //01000031		  tmIoT Studio发送Modbus参数
	{PROTOCOL_ID_APPLY_MODBUSRTUOD							 , 	ApplyModbusRTUOD			  }, //01000032		  tmIoT Studio发送Modbus OD数据协议
	{PROTOCOL_ID_APPLY_MODBUSRTUENDFRAM 					 ,  ApplyModbusRTUEndFrame		  }, //01000033		  tmIoT Studio发送Modbus结束协议
	{PROTOCOL_ID_APPLY_MODBUSRTUSAVECONFIGDATA 				 ,  ApplyModbusRTUSaveConfigData  }, //01000034		  tmIoT Studio发送Modbus保存协议
	{PROTOCOL_ID_APPLY_CANSLAVEPARA                          ,  ApplyCanSlavePara             }, //01000041       tmIoT Studio发送CAN参数
	{PROTOCOL_ID_APPLY_DOWNLOADOD                            ,  ApplyDownloadOd               }, //01000042       tmIoT Studio发送CAN OD数据协议
	{PROTOCOL_ID_APPLY_CANENDFRAME                           ,  ApplyCanEndFrame              }, //01000043       tmIoT Studio发送CAN结束协议
	{PROTOCOL_ID_APPLY_CANSAVECONFIGDATA                     ,  ApplyCanSaveConfigData        }, //01000044       tmIoT Studio发送CAN保存协议
    {PROTOCOL_ID_APPLY_INETSETIPINFO                         ,  ApplyiNetSetIPInfo            }, //01000051       tmIoT Studio发送iNet 下载IP信息
    {PROTOCOL_ID_APPLY_INETGETIPINFO                         ,  ApplyiNetGetIPInfo            }, //01000052       tmIoT Studio发送iNet 读取IP信息
    {PROTOCOL_ID_APPLY_INETPARA                              ,  ApplyiNetPARA                 }, //01000053       tmIoT Studio发送iNet 下载参数
    {PROTOCOL_ID_APPLY_INETODData                            ,  ApplyiNetODData               }, //01000054       tmIoT Studio发送iNet OD数据协议
    {PROTOCOL_ID_APPLY_INETENDFRAME                          ,  ApplyiNetEndFrame             }, //01000055       tmIoT Studio发送iNet 结束协议
    {PROTOCOL_ID_APPLY_INETCONFIGDATA                        ,  ApplyiNetSaveConfigData       }, //01000056       tmIoT Studio发送iNet 保存协议
	{PROTOCOL_ID_APPLY_MODBUSTCPPARA						 ,	ApplyModbusTCPSETMODEInfo	  }, //01000057		  tmIoT Studio发送ModbusTCP参数
	{PROTOCOL_ID_APPLY_MODBUSTCPOD							 ,	ApplyModbusTCPOD			  }, //01000058		  tmIoT Studio发送ModbusTCP  OD数据协议
	{PROTOCOL_ID_APPLY_MODBUSTCPENDFRAME					 ,	ApplyModbusTCPENDFRAME		  }, //01000059		  tmIoT Studio发送ModbusTCP   结束协议
	{PROTOCOL_ID_APPLY_MODBUSTCPSAVECONFIGDATA				 ,	ApplyModbusTCPSaveConfigData  }, //01000060		  tmIoT Studio发送ModbusTCP   保存协议


    {PROTOCOL_ID_REPLY_HANDSHAKE                             ,  ReplyHandshake                }, //02000006       tmIoT 回复握手协议
    {PROTOCOL_ID_REPLY_STARTFRAME                            ,  ReplyStartFrame               }, //02000007       tmIoT 回复开始协议
    {PROTOCOL_ID_REPLY_DATAFRAME                             ,  ReplyDataFrame                }, //02000008       tmIoT 回复数据协议
    {PROTOCOL_ID_REPLY_ENDFRAME                              ,  ReplyEndFrame                 }, //02000009       tmIoT 回复结束协议
    {PROTOCOL_ID_REPLY_SAVECONFIGDATA                        ,  ReplySaveConfigData           }, //0200000A       tmIoT 回复保存协议
    {PROTOCOL_ID_REPLY_REALTIMEDATAATTR                      ,  ReplyRealtimeDataAttr         }, //0200000B       tmIoT 回复实时数据属性
    {PROTOCOL_ID_REPLY_CONFIGSERVERIP                        ,  ReplyConfigServerIP           }, //0200000C       tmIoT 回复服务器信息
    {PROTOCOL_ID_REPLY_CONFIGTMIOTIP                         ,  ReplyConfigTmIoTIP            }, //0200000D       tmIoT 回复设备信息
    {PROTOCOL_ID_REPLY_DEBUGBKPT                             ,  ReplyDebugBkpt                }, //0200000E       tmIoT 回复断点协议
    {PROTOCOL_ID_REPLY_COMMDATAFRAME                         ,  ReplyCommDataFrame            }, //02000010       tmIoT 回复逻辑缓存数据协议
	{PROTOCOL_ID_REPLY_GLOBALVARIABLEFRAME                   ,  ReplyGlobalVariableFrame      }, //02000011       tmIoT 回复全局变量数据协议
    {PROTOCOL_ID_REPLY_DEBUGRUN                              ,  ReplyDebugRun                 }, //02000020       tmIoT 回复运行协议
    {PROTOCOL_ID_REPLY_DEBUGPAUSE                            ,  ReplyDebugPause               }, //02000021       tmIoT 回复暂停协议
    {PROTOCOL_ID_REPLY_DEBUGSTEP                             ,  ReplyDebugStep                }, //02000022       tmIoT 回复单步协议
    {PROTOCOL_ID_REPLY_DEBUGMODE                             ,  ReplyDebugMode                }, //02000023       tmIoT 回复调试协议
    {PROTOCOL_ID_REPLY_TASKDATA                              ,  ReplyTaskData                 }, //02000028       tmIoT 回复Task协议
    {PROTOCOL_ID_REPLY_REALTIMEDATA                          ,  ReplyRealtimeData             }, //0200002A       tmIoT 回复请求实时数据
    {PROTOCOL_ID_REPLY_COMPONENTSCANCNT                      ,  ReplyComponentScanCnt         }, //0200002B       tmIoT 回复请求元件运行次数
	{PROTOCOL_ID_REPLY_MODBUSRTUPARA						 ,	ReplyModbusRTUPara			  }, //02000031		  tmIoT 回复Modbus参数
	{PROTOCOL_ID_REPLY_MODBUSRTUOD 							 , 	ReplyModbusRTUOD			  }, //02000032		  tmIoT 回复Modbus OD数据协议
	{PROTOCOL_ID_REPLY_MODBUSRTUENDFRAM 					 ,  ReplyModbusRTUEndFrame		  }, //02000033		  tmIoT 回复Modbus结束协议
	{PROTOCOL_ID_REPLY_MODBUSRTUSAVECONFIGDATA 				 ,  ReplyModbusRTUSaveConfigData  }, //02000034	      tmIoT 回复Modbus保存协议
	{PROTOCOL_ID_REPLY_CANSLAVEPARA                          ,  ReplyCanSlavePara             }, //02000041       tmIoT 回复CAN参数
	{PROTOCOL_ID_REPLY_DOWNLOADOD                            ,  ReplyDownloadOd               }, //02000042       tmIoT 回复CAN OD数据协议
	{PROTOCOL_ID_REPLY_CANENDFRAME                           ,  ReplyCanEndFrame              }, //02000043       tmIoT 回复CAN结束协议
	{PROTOCOL_ID_REPLY_CANSAVECONFIGDATA                     ,  ReplyCanSaveConfigData        }, //02000044       tmIoT 回复CAN保存协议
    {PROTOCOL_ID_REPLY_INETSETIPINFO                         ,  ReplyiNetSetIPInfo            }, //02000051       tmIoT 回复iNet 下载IP信息
    {PROTOCOL_ID_REPLY_INETGETIPINFO                         ,  ReplyiNetGetIPInfo            }, //02000052       tmIoT 回复iNet 读取IP信息
    {PROTOCOL_ID_REPLY_INETPARA                              ,  ReplyiNetPara                 }, //02000053       tmIoT 回复iNet 下载参数
    {PROTOCOL_ID_REPLY_INETODData                            ,  ReplyiNetODData               }, //02000054       tmIoT 回复iNet OD数据协议
    {PROTOCOL_ID_REPLY_INETENDFRAME                          ,  ReplyiNetEndFrame             }, //02000055       tmIoT 回复iNet 结束协议
    {PROTOCOL_ID_REPLY_INETCONFIGDATA                        ,  ReplyiNetSaveConfigData       }, //02000056       tmIoT 回复iNet 保存协议
	{PROTOCOL_ID_REPLY_ModbusTCPSETMODEINFO					 ,	ReplyModbusTCPSetMODEInfo	  }, //02000057	  	  tmIoT 回复ModbusTCP 下载Mode信息
	{PROTOCOL_ID_REPLY_MODBUSTCPOD							 ,	ReplyModbusTCPOD			  }, //02000058	      tmIoT 回复ModbusTCP OD信息
	{PROTOCOL_ID_REPLY_MODBUSTCPENDFRAM						 ,	ReplyMODBUSTCPENDFRAM	      }, //02000059	      tmIoT 回复ModbusTCP 结束协议
	{PROTOCOL_ID_REPLY_MODBUSTCPSAVEFRAM					 ,	ReplyMODBUSTCPSAVEFRAM	      }, //02000060	      tmIoT 回复ModbusTCP 保存协议

};
/*===========================================================================+
|Name:         Param_Init                                                    |
|Description:  		                                                         |
+===========================================================================*/
void     Param_Init()
{
//	BYTE_T i;
//	WORD i;
	g_wTotal_FunctionBlockNum = 0;
	g_wTotal_CommDataNum = 0;
	g_wTotal_GlobalVariableNum = 0;
    u_byTotal_RecvPakageNum = 0;
    memset(g_ALLCommDataList, 0, sizeof(COMMDATATYPE_T)*MaxCommData);
    memset(g_ALLObjectListData, 0, sizeof(DESIGNTIMEDATATYPE_T)*MaxTask*MAXNUM);

//    static BYTE Param_Init = 0;
//
//    if(Param_Init) {
//		for(i=0;i<CanIdParmTab[0].IdNum;i++)
//			CanIdParmTab[0].IdListP[i].MasterCMD = 0;
//		for(i=0;i<CanIdParmTab[1].IdNum;i++)
//			CanIdParmTab[1].IdListP[i].MasterCMD = 0;
//    }
//    Param_Init = 1;
//	for(i = 0; i < MaxTask; i++)
//	{
//		if(GetStatus_Task(g_htask_Calculations[i]) != TASK_NONE)
//			Destroy_Task(g_htask_Calculations[i]);
//	}
//	memset(g_DesignTime_ObjectListData, 0, sizeof(DESIGNTIMEDATATYPE_T)*MaxTask*MAXNUM);
//	memset(g_DesignTime_ObjectParam, 0, sizeof(WORD_T)*MaxTask);
////	memset(TaskData, 0, sizeof(TASKDATA_T)*MaxTask);
//	memset(g_RunTime_ObjectData, 0, sizeof(RUNTIMEDATATYPE_T)*MaxTask*MAXNUM);
}
/*===========================================================================+
|Name:         DealDeceiveProtocol                                           |
|Description:  According to the ProtocolID, Executive the diffrent function  |
|              of tmIoT Studio Protocol(just for Receive NetData)            |
+===========================================================================*/
BOOL_T   DealRcvProtocol(WORD_T  wlen)
{
	BYTE_T   i;
    DWORD_T  dwRecProID;
    if(wlen <= 0)
	{
    	return 0;
	}
	memcpy(&dwRecProID, &g_byRecv_Buffer[6], sizeof(DWORD_T));
	dwRecProID = htonl(dwRecProID);
	for(i = 0; i < PRO_NUM; i++)
	{
		if(ProtoMap[i].dwProId == dwRecProID)
		{
			if(g_RcvPortflag) u_cConnectPort =1;
			else	u_cConnectPort = 0;
			ProtoMap[i].pFun(FLAG_RECEIVE);
		}
	}
	return 1;
}

/*===========================================================================+
|Name:         SendScanSerialNo                                              |
|Description:  tmIoT Send serial number of function block which is running   |
+===========================================================================*/
BOOL_T   SendScanSerialNo (WORD_T Number)
{
	DWORD_T    dwProtocol_Reply_SendScanSerialNoID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    WORD_T     wReplayProtocol_Number;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_SendScanSerialNoID = htonl(PROTOCOL_ID_REPLY_SENDSCANSERIALNO);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_SendScanSerialNoID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    wReplayProtocol_Number =  htons(Number);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Number,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         Read_ObjectListData_FromFlash                                 |
|Description:  Read FunctionBlock Info and Count from the flash              |
+===========================================================================*/
/****************************READ THE DATA FROM THE INTERNAL EEPROM***************/
void    Read_ObjectListData_FromFlash( void )
{
	WORD_T  i;
	DWORD_T	dwDataID[MaxCommData];
	WORD_T	wGVID[MaxGlobalVariable];
	BYTE_T	wGVType[MaxGlobalVariable];
	DWORD_T	dwGVofs;

	Flash_Read(&g_byObJlistFlag, sizeof(BYTE_T), para_Block, g_OBJDATACHANGE_FLAG_OFFSET);
	Flash_Read( (BYTE_T*)&g_StoreParmFlagAP, sizeof(BYTE_T), para_Block, g_CANFLAG_OFFSET );
	if( g_byObJlistFlag == u_ObjDataChange_Flag )
	{
		Flash_Read( (BYTE_T*)&g_wTotal_FunctionBlockNum, sizeof(WORD_T), para_Block, g_OBJLISTNUM_OFFSET );
		Flash_Read( (BYTE_T*)&g_cTaskTotalCount, sizeof(BYTE_T), para_Block, g_TASKTOTALNUM_OFFSET );
		Flash_Read( (BYTE_T*)g_ALLObjectListData, ((sizeof(DESIGNTIMEDATATYPE_T))*g_wTotal_FunctionBlockNum),Runtime_Block, ALLObjectListData_ofs );
		Flash_Read( (BYTE_T*)TaskData, ((sizeof(TASKDATA_T))*MaxTask), Runtime_Block, TaskData_ofs );
		Flash_Read( (BYTE_T*)&g_wTotal_CommDataNum, sizeof(WORD_T), Runtime_Block, CommData_ofs );
		Flash_Read( (BYTE_T*)dwDataID, sizeof(DWORD_T) * g_wTotal_CommDataNum, Runtime_Block, CommData_ofs + sizeof(WORD_T) );
		dwGVofs = CommData_ofs + sizeof(WORD_T) + sizeof(DWORD_T) * g_wTotal_CommDataNum;
		Flash_Read( (BYTE_T*)&g_wTotal_GlobalVariableNum, sizeof(WORD_T), Runtime_Block, dwGVofs );
		Flash_Read( (BYTE_T*)wGVID, sizeof(WORD_T) * g_wTotal_GlobalVariableNum, Runtime_Block, dwGVofs + sizeof(WORD_T) );
		Flash_Read( (BYTE_T*)wGVType, sizeof(BYTE_T) * g_wTotal_GlobalVariableNum, Runtime_Block, dwGVofs + sizeof(WORD_T) + sizeof(WORD_T) * g_wTotal_GlobalVariableNum );
	}

//	for( i = 0; i < g_cTaskTotalCount; i++ )
//	{
//		Temp = TaskData[i].EndComponentNo - TaskData[i].StartComponentNo + 1;
//		for( j = 0; j < Temp; j++ )
//		{
//			memcpy(&g_DesignTime_ObjectListData[i][j], &g_ALLObjectListData[TaskData[i].StartComponentNo + i], sizeof(DESIGNTIMEDATATYPE_T));
//		}
//	}
	for( i = 0; i < g_wTotal_CommDataNum; i++ )
	{
		g_ALLCommDataList[i].DataID = dwDataID[i];
		memset(&g_ALLCommDataList[i].Value, 0, sizeof(DBVALUE));
	}
	for( i = 0; i < g_wTotal_GlobalVariableNum; i++ )
	{
		g_ALLGlobalVariableList[i].GVID = wGVID[i];
		g_ALLGlobalVariableList[i].GVType = wGVType[i];
		memset(&g_ALLGlobalVariableList[i].Value, 0, sizeof(DBVALUE));
	}
	ComponenttoTaskData();
	InitObjectListData();
}
/*===========================================================================+
|Name:         ApplyHandshake                                                |
|Description:  Deal the Heartbeat Frame                                      |
+===========================================================================*/
BOOL_T  ApplyHandshake( BYTE_T FlagRecvOrSend)
{
    BYTE_T   Recvbuf[20];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;
    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
        if(FlagRecvOrSend == 0)
        {
             ProtoMap[ REPLY_HANDSHAKE ].pFun( FLAG_SEND);
        }
    }

    return TRUE;
}

/*===========================================================================+
|Name:         ApplyStartFrame                                               |
|Description:  tmIoT Studio Send Start Frame,The package include how many the|
|              package and the component Count                               |
+===========================================================================*/
BOOL_T  ApplyStartFrame( BYTE_T FlagRecvOrSend)
{
    Param_Init();
    BYTE_T   Recvbuf[30];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    u_wPakageSize =  ((((BYTE *)g_byRecv_Buffer)[PAKAGESIZE1_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[PAKAGESIZE2_POSITION]<<SHIFT00);
    u_wComponentCount =  ((((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT1_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT2_POSITION]<<SHIFT00);

    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
        if(FlagRecvOrSend == 0)
        {
          ProtoMap[ REPLY_STARTFRAME ].pFun( FLAG_SEND);
        }
    }

    return TRUE;
}

/*===========================================================================+
|Name:         ApplyDataFrame                                                |
|Description:  tmIoT Studio Send Data to tmIoT device,The package include    |
|              the current package stamp and the serialno of this data frame |
+===========================================================================*/
BOOL_T  ApplyDataFrame(BYTE_T  FlagRecvOrSend)
{
	WORD_T     wComponentcount;
	BYTE_T*    pTemBuf = NULL;

	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;

	pTemBuf = g_byRecv_Buffer;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(g_byRecv_Buffer,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	u_byPacketStamp =  (((BYTE *)g_byRecv_Buffer)[PACKETSTAMP_POSITION]);
	u_wSerialNo =  ((((BYTE *)g_byRecv_Buffer)[SERIALNO1_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[SERIALNO2_POSITION]<<SHIFT00);
	wComponentcount =  ((((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT1_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT2_POSITION]<<SHIFT00);
	pTemBuf = ((BYTE_T *)g_byRecv_Buffer) + DATA_POSITION_POINTSKIP_14;

	if((Recvcrch == g_byRecv_Buffer[byProLength])&&(Recvcrcl == g_byRecv_Buffer[byProLength+1]))
	{
		if((SaveObjectList(wComponentcount,pTemBuf) == TRUE) ) //&& (u_byOldPacketStamp != u_byPacketStamp)
		{
			//u_byOldPacketStamp = u_byPacketStamp;
			if(FlagRecvOrSend == 0)
			{
				ProtoMap[ REPLY_DATAFRAME ].pFun(FLAG_SEND );
				return TRUE;
			}
		}
	}

	return TRUE;
}

/*===========================================================================+
|Name:         ApplyEndFrame                                                 |
|Description:  tmIoT Studio send end frame to tmIoT device,the sign of tmIoT |
|              Studio and tmIoT Device connection is end, and tell the tmIoT |
|              Device the package number and FunctionBlock Count to ensure   |
|              the receive data is correct and completely                    |
+===========================================================================*/
BOOL_T  ApplyEndFrame(BYTE_T  FlagRecvOrSend)
{
	BYTE_T   byRecvPakageNum;
	WORD_T   wRecvComponentCount;

	BYTE_T   Recvbuf[30];
	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);
	memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(Recvbuf,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	u_byPacketStamp_EndFrame =  (((BYTE *)g_byRecv_Buffer)[PACKETSTAMP_POSITION]);
	byRecvPakageNum = ((((BYTE *)g_byRecv_Buffer)[PAKAGESIZE1_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[PAKAGESIZE2_POSITION]<<SHIFT00);
	wRecvComponentCount = ((((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT1_POSITION])<<SHIFT08)|
                           (((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT2_POSITION]<<SHIFT00);

	if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
	{
		u_byOldPacketStamp = 0;
		if((g_wTotal_FunctionBlockNum == wRecvComponentCount)&&(u_byTotal_RecvPakageNum == byRecvPakageNum))
		{
			ProtoMap[ REPLY_ENDFRAME ].pFun(RECEIVE_RIGHT_FLAG );
		}
		else
		{
			ProtoMap[ REPLY_ENDFRAME ].pFun(RECEIVE_ERROR_FLAG );
		}
	}

	return TRUE;
}

/*===========================================================================+
|Name:         ApplyCommDataFrame                                            |
|Description:  tmIoT Studio Send Data to tmIoT device,The package include    |
|              the current package stamp and the serialno of this data frame |
+===========================================================================*/
BOOL_T  ApplyCommDataFrame(BYTE_T  FlagRecvOrSend)
{
	BYTE_T*    pTemBuf = NULL;

	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;
	DWORD_T dwDataID;

	pTemBuf = g_byRecv_Buffer;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(g_byRecv_Buffer,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	u_byPacketStamp =  (((BYTE *)g_byRecv_Buffer)[PACKETSTAMP_POSITION]);
	u_cLength =  (((BYTE *)g_byRecv_Buffer)[COMMLEN_POSITION]);
	pTemBuf = ((BYTE_T *)g_byRecv_Buffer) + COMMDATA_POSITION;

	if((Recvcrch == g_byRecv_Buffer[byProLength])&&(Recvcrcl == g_byRecv_Buffer[byProLength+1]))
	{
		WORD_T     i = 0;
		BYTE_T*    bypTemBuf = pTemBuf;

		for(i = g_wTotal_CommDataNum; i < (g_wTotal_CommDataNum + u_cLength); i++)
		{
			memcpy( &dwDataID, bypTemBuf, sizeof(DWORD_T) );
//			g_ALLCommDataList[i].DataID = htonl(dwDataID);
			g_ALLCommDataList[i].DataID = dwDataID;
			memset(&g_ALLCommDataList[i].Value, 0, sizeof(DBVALUE));
			bypTemBuf =(BYTE_T *) (((BYTE_T *)bypTemBuf) + sizeof(DWORD_T));
		}
		g_wTotal_CommDataNum += u_cLength;

		if(FlagRecvOrSend == 0)
		{
			ProtoMap[ REPLY_COMMDATAFRAME ].pFun(FLAG_SEND );
			return TRUE;
		}
	}

	return TRUE;
}
/*===========================================================================+
|Name:         ApplyGlobalVariableFrame                                      |
|Description:  tmIoT Studio Send Data to tmIoT device,The package include    |
|              the current package stamp and the serialno of this data frame |
+===========================================================================*/
BOOL_T  ApplyGlobalVariableFrame(BYTE_T  FlagRecvOrSend)
{
	BYTE_T*    pTemBuf = NULL;

	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;
	WORD_T wDataID;
	BYTE_T cType;

	pTemBuf = g_byRecv_Buffer;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(g_byRecv_Buffer,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	u_byPacketStamp =  (((BYTE *)g_byRecv_Buffer)[PACKETSTAMP_POSITION]);
	u_cLength =  (((BYTE *)g_byRecv_Buffer)[COMMLEN_POSITION]);
	pTemBuf = ((BYTE_T *)g_byRecv_Buffer) + COMMDATA_POSITION;

	if((Recvcrch == g_byRecv_Buffer[byProLength])&&(Recvcrcl == g_byRecv_Buffer[byProLength+1]))
	{
		WORD_T     i = 0;
		BYTE_T*    bypTemBuf = pTemBuf;

		for(i = g_wTotal_GlobalVariableNum; i < (g_wTotal_GlobalVariableNum + u_cLength); i++)
		{
			memcpy( &wDataID, bypTemBuf, sizeof(WORD_T) );
			g_ALLGlobalVariableList[i].GVID = htons(wDataID);
			bypTemBuf =(BYTE_T *) (((BYTE_T *)bypTemBuf) + sizeof(WORD_T));
			memcpy( &cType, bypTemBuf, sizeof(BYTE_T) );
			g_ALLGlobalVariableList[i].GVType = cType;
			bypTemBuf =(BYTE_T *) (((BYTE_T *)bypTemBuf) + sizeof(BYTE_T));
			memset(&g_ALLGlobalVariableList[i].Value, 0, sizeof(DBVALUE));
		}
		g_wTotal_GlobalVariableNum += u_cLength;

		if(FlagRecvOrSend == 0)
		{
			ProtoMap[ REPLY_GLOBALVARIABLEFRAME ].pFun(FLAG_SEND );
			return TRUE;
		}
	}

	return TRUE;
}
/*===========================================================================+
|Name:         ApplyCanEndFrame                                              |
|Description:  tmIoT Studio Send Data to tmIoT device,The package include    |
|              the current package stamp and the serialno of this data frame |
+===========================================================================*/
BOOL_T  ApplyCanEndFrame(BYTE_T  FlagRecvOrSend)
{
	BYTE_T   byRecvPakageNum;
	WORD_T   wRecvComponentCount;

	BYTE_T   Recvbuf[30];
	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);
	memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(Recvbuf,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	u_byPacketStamp_EndFrame =  (((BYTE *)g_byRecv_Buffer)[PACKETSTAMP_POSITION]);
	byRecvPakageNum = ((((BYTE *)g_byRecv_Buffer)[PAKAGESIZE1_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[PAKAGESIZE2_POSITION]<<SHIFT00);
	wRecvComponentCount = ((((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT1_POSITION])<<SHIFT08)|
                           (((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT2_POSITION]<<SHIFT00);

	if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
	{
		if((g_wTotal_FunctionBlockNum == wRecvComponentCount)&&(u_byTotal_RecvPakageNum == byRecvPakageNum))
		{
			ProtoMap[ REPLY_CANENDFRAME ].pFun(RECEIVE_RIGHT_FLAG );
		}
		else
		{
			ProtoMap[ REPLY_CANENDFRAME ].pFun(RECEIVE_ERROR_FLAG );
		}
		if(CanIdParmTab[0].HardPort) g_InitCommunP1=0;
		if(CanIdParmTab[1].HardPort) g_InitCommunP2=0;
		if(!u_TaskCanFlag)
		{
			Create_TaskCanFunction();
			u_TaskCanFlag = 1;
		}
	}

	return TRUE;
}
/*===========================================================================+
|Name:         SaveObjectList                                                |
|Description:  Save FunctionBlock Info to the Local Array and the Total Of   |
|              FunctionBlock Count                                           |
+===========================================================================*/
BOOL_T  SaveObjectList( WORD_T   wComponentcount, BYTE_T  *bypData )
{
	WORD_T     i = 0;
	BYTE_T*    bypTemBuf = bypData;

	for(i = g_wTotal_FunctionBlockNum; i < (g_wTotal_FunctionBlockNum + wComponentcount); i++)
	{
		memcpy( &g_ALLObjectListData[i], bypTemBuf, sizeof(DESIGNTIMEDATATYPE_T) );
		bypTemBuf =(BYTE_T *) (((BYTE_T *)bypTemBuf) + sizeof(DESIGNTIMEDATATYPE_T));
	}
	g_wTotal_FunctionBlockNum += wComponentcount;
	u_byTotal_RecvPakageNum++;

	return  TRUE;
}
/*===========================================================================+
|Name:         ApplySaveConfigData                                           |
|Description:  tmIoT Studio send Save ConfigData command, tell tmIoT device  |
|              Save The FunctionBlock Info to Eeprom                         |
+===========================================================================*/
/***********************THE DATA IS SAVED INTO FLASH*********************/
BOOL_T  ApplySaveConfigData( BYTE_T FlagRecvOrSend)
{
	BYTE_T   Recvbuf[30];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    WORD_T WriteFBNum;
    WORD_T WriteTaskDataNum;

    WORD_T  i;
    DWORD_T	dwDataID[MaxCommData];
    WORD_T	wGVID[MaxGlobalVariable];
    BYTE_T	wGVType[MaxGlobalVariable];
    DWORD_T dwGVofs;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
    	_Led_RunFast();  //Run指示灯快速闪烁提醒
    	WriteFBNum = (sizeof(DESIGNTIMEDATATYPE_T))*g_wTotal_FunctionBlockNum;

        Flash_Write( (BYTE_T*)&u_ObjDataChange_Flag, sizeof(BYTE_T), para_Block, g_OBJDATACHANGE_FLAG_OFFSET );
        Flash_Write( (BYTE_T*)&g_wTotal_FunctionBlockNum, sizeof(WORD_T), para_Block, g_OBJLISTNUM_OFFSET );
        Flash_Write( (BYTE_T*)&g_cTaskTotalCount, sizeof(BYTE_T), para_Block, g_TASKTOTALNUM_OFFSET );

        WriteTaskDataNum = (sizeof(TASKDATA_T))*MaxTask;
        Flash_Write( (BYTE_T*)TaskData, WriteTaskDataNum, Runtime_Block, TaskData_ofs );
        Flash_Write( (BYTE_T*)g_ALLObjectListData, WriteFBNum, Runtime_Block, ALLObjectListData_ofs );

 		Flash_Write( (BYTE_T*)&g_wTotal_CommDataNum, sizeof(WORD_T), Runtime_Block, CommData_ofs );
        for(i = 0; i < g_wTotal_CommDataNum; i++)
        	dwDataID[i] = g_ALLCommDataList[i].DataID;
        Flash_Write( (BYTE_T*)dwDataID, sizeof(DWORD_T) * g_wTotal_CommDataNum, Runtime_Block, CommData_ofs + sizeof(WORD_T) );

        dwGVofs = CommData_ofs + sizeof(WORD_T) + sizeof(DWORD_T) * g_wTotal_CommDataNum;
        Flash_Write( (BYTE_T*)&g_wTotal_GlobalVariableNum, sizeof(WORD_T), Runtime_Block, dwGVofs );
        for(i = 0; i < g_wTotal_GlobalVariableNum; i++)
        {
        	wGVID[i] = g_ALLGlobalVariableList[i].GVID;
        	wGVType[i] = g_ALLGlobalVariableList[i].GVType;
        }
        Flash_Write( (BYTE_T*)wGVID, sizeof(WORD_T) * g_wTotal_GlobalVariableNum, Runtime_Block, dwGVofs + sizeof(WORD_T) );
        Flash_Write( (BYTE_T*)wGVType, sizeof(BYTE_T) * g_wTotal_GlobalVariableNum, Runtime_Block, dwGVofs + sizeof(WORD_T) + sizeof(WORD_T) * g_wTotal_GlobalVariableNum );
		Flash_Read( (BYTE_T*)wGVID, sizeof(WORD_T) * g_wTotal_GlobalVariableNum, Runtime_Block, dwGVofs + sizeof(WORD_T) );
		Flash_Read( (BYTE_T*)wGVType, sizeof(BYTE_T) * g_wTotal_GlobalVariableNum, Runtime_Block, dwGVofs + sizeof(WORD_T) + sizeof(WORD_T) * g_wTotal_GlobalVariableNum );

        ProtoMap[ REPLY_SAVECONFIGDATA ].pFun( RECEIVE_ERROR_FLAG );
    }

	return TRUE;
}
/*===========================================================================+
|Name:         ApplyRealtimeDataAttr                                         |
|Description:  tmIoT Studio apply the Real-time data from the tmIoT device   |
+===========================================================================*/
BOOL_T  ApplyRealtimeDataAttr(BYTE_T FlagRecvOrSend)
{
	BYTE_T   Recvbuf[30];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
    	if(FlagRecvOrSend == 0)
        {
    		ProtoMap[ REPLY_REALTIMEDATAATTR ].pFun( FLAG_SEND );
        }
    }

    return TRUE;
}

/*===========================================================================+
|Name:         ApplyConfigServerIP                                           |
|Description:  tmIoT Studio send the server IP to tmIoT Device, Then Save    |
|              this Info to Eeprom the server IP from users set              |
+===========================================================================*/
BOOL_T  ApplyConfigServerIP( BYTE_T FlagRecvOrSend)
{
//    BYTE_T   Recvbuf[50];
//    BYTE_T   buff[8];
//    WORD_T CRC,byProLength = 0;
//    BYTE_T Recvcrcl,Recvcrch;
//
//    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
//
//    byProLength = htons(byProLength);
//    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);
//
//    byProLength = byProLength - 2;
//    CRC = CRC_check(Recvbuf,byProLength);
//    Recvcrcl = CRC&0x00FF;
//    Recvcrch = (CRC&0xFF00)>>8;
//
//    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
//    {
//        if(FlagRecvOrSend == 0)
//        {
//        	Device_Read(g_hEeprom, buff, sizeof(buff), g_SERVERCONFIGFLAG_OFFSET);
//        	u_ServerIP_WriteToEeprom[0] = ((BYTE_T *)g_byRecv_Buffer)[IPV4IP_POSITION1];
//        	u_ServerIP_WriteToEeprom[1] = ((BYTE_T *)g_byRecv_Buffer)[IPV4IP_POSITION2];
//        	u_ServerIP_WriteToEeprom[2] = ((BYTE_T *)g_byRecv_Buffer)[IPV4IP_POSITION3];
//        	u_ServerIP_WriteToEeprom[3] = ((BYTE_T *)g_byRecv_Buffer)[IPV4IP_POSITION4];
//        	u_ServerPort_WriteToEeprom[0] = ((BYTE_T *)g_byRecv_Buffer)[IPV4PORT_POSITION1];
//        	u_ServerPort_WriteToEeprom[1] = ((BYTE_T *)g_byRecv_Buffer)[IPV4PORT_POSITION2];
//        	buff[0] = u_ServerConfig_Flag;
//        	buff[1] = u_ServerIP_WriteToEeprom[0];
//        	buff[2] = u_ServerIP_WriteToEeprom[1];
//        	buff[3] = u_ServerIP_WriteToEeprom[2];
//        	buff[4] = u_ServerIP_WriteToEeprom[3];
//        	buff[5] = u_ServerPort_WriteToEeprom[0];
//        	buff[6] = u_ServerPort_WriteToEeprom[1];
//            Device_Write(g_hEeprom, buff, sizeof(buff), 0);
//            ProtoMap[ REPLY_CONFIGSERVERIP ].pFun(FLAG_SEND );
//        }
//    }

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyConfigTmIoTIP                                            |
|Description:  tmIoT Studio send the tmIoT Device IP and Mac, Then Save this |
|              Info to Eeprom .the server IP from users set                  |
+===========================================================================*/
BOOL_T  ApplyConfigTmIoTIP( BYTE_T FlagRecvOrSend)
{
//    BYTE_T   Recvbuf[50];
//    BYTE_T   buff[16];
//    WORD_T CRC,byProLength = 0;
//    BYTE_T Recvcrcl,Recvcrch;
//
//    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
//
//    byProLength = htons(byProLength);
//    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);
//
//    byProLength = byProLength - 2;
//    CRC = CRC_check(Recvbuf,byProLength);
//    Recvcrcl = CRC&0x00FF;
//    Recvcrch = (CRC&0xFF00)>>8;
//
//    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
//    {
//        if(FlagRecvOrSend == 0)
//        {
//        	Device_Read(g_hEeprom, buff, sizeof(buff), g_TMIOTCONFIGFLAG_OFFSET);
//            u_TmIoTIP_WriteToEeprom[0] = Recvbuf[IPV4IP_POSITION1];
//            u_TmIoTIP_WriteToEeprom[1] = Recvbuf[IPV4IP_POSITION2];
//            u_TmIoTIP_WriteToEeprom[2] = Recvbuf[IPV4IP_POSITION3];
//            u_TmIoTIP_WriteToEeprom[3] = Recvbuf[IPV4IP_POSITION4];
//
//            u_TmIoTMASK_WriteToEeprom[0] = Recvbuf[IPV4MASK_POSITION1];
//            u_TmIoTMASK_WriteToEeprom[1] = Recvbuf[IPV4MASK_POSITION2];
//            u_TmIoTMASK_WriteToEeprom[2] = Recvbuf[IPV4MASK_POSITION3];
//            u_TmIoTMASK_WriteToEeprom[3] = Recvbuf[IPV4MASK_POSITION4];
//
//            u_TmIoTGATEWAY_WriteToEeprom[0] = Recvbuf[IPV4GATEWAY_POSITION1];
//            u_TmIoTGATEWAY_WriteToEeprom[1] = Recvbuf[IPV4GATEWAY_POSITION2];
//            u_TmIoTGATEWAY_WriteToEeprom[2] = Recvbuf[IPV4GATEWAY_POSITION3];
//            u_TmIoTGATEWAY_WriteToEeprom[3] = Recvbuf[IPV4GATEWAY_POSITION4];
//
//        	buff[0] = u_TmIoTConfig_Flag;
//        	buff[1] = u_TmIoTIP_WriteToEeprom[0];
//        	buff[2] = u_TmIoTIP_WriteToEeprom[1];
//        	buff[3] = u_TmIoTIP_WriteToEeprom[2];
//        	buff[4] = u_TmIoTIP_WriteToEeprom[3];
//        	buff[5] = u_TmIoTMASK_WriteToEeprom[0];
//        	buff[6] = u_TmIoTMASK_WriteToEeprom[1];
//        	buff[7] = u_TmIoTMASK_WriteToEeprom[2];
//        	buff[8] = u_TmIoTMASK_WriteToEeprom[3];
//        	buff[9] = u_TmIoTGATEWAY_WriteToEeprom[0];
//        	buff[10] = u_TmIoTGATEWAY_WriteToEeprom[1];
//        	buff[11] = u_TmIoTGATEWAY_WriteToEeprom[2];
//        	buff[12] = u_TmIoTGATEWAY_WriteToEeprom[3];
//        	Device_Write(g_hEeprom, buff, sizeof(buff), 0);
//            ProtoMap[ REPLY_CONFIGTMIOTIP ].pFun(FLAG_SEND );
//        }
//    }

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyDebugBkpt                                                |
|Description:  tmIoT Studio send break point command in debug mode, tell     |
|              tmIoT device save information of break point                  |
+===========================================================================*/
BOOL_T  ApplyDebugBkpt(BYTE_T FlagRecvOrSend)
{
    WORD_T     wBkptSerialNo;
    WORD_T     wBkptScanCnt;
    BYTE_T     cBkptFlag;
    BYTE_T     i,j;

    BYTE_T   Recvbuf[21];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != Recvbuf[byProLength]) || (Recvcrcl != Recvbuf[byProLength+1]))  return 0;

    wBkptSerialNo =  ((((BYTE *)g_byRecv_Buffer)[SERIALNO1_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[SERIALNO2_POSITION]<<SHIFT00);
    cBkptFlag = ((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT1_POSITION];
    wBkptScanCnt = ((((BYTE *)g_byRecv_Buffer)[COMPONENTCOUNT2_POSITION]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[DATA_POSITION_POINTSKIP_14]<<SHIFT00);

    if(cBkptFlag)	//cBkptFlag=1表示设置断点，直接在g_tBkptSerialNo后面添加断点信息，PC端控制最多发送5个断点故g_cBKPTCnt<=5
    {
    	g_cBKPTCnt++;
    	g_tBkptSerialNo[ g_cBKPTCnt - 1].SerialNo = wBkptSerialNo;
    	g_tBkptSerialNo[ g_cBKPTCnt - 1].ScanCntOffset = wBkptScanCnt;
    	g_tBkptSerialNo[ g_cBKPTCnt - 1].ScanCnt = 0;
    }
    if(cBkptFlag == 0)//cBkptFlag=1表示取消断点，g_tBkptSerialNo数组里的对应断点，删除并将后面数据前移一位
    {
    	for(i = 0; i < g_cBKPTCnt; i++)
    	{
    		if(wBkptSerialNo == g_tBkptSerialNo[i].SerialNo)
    		{
    			for(j = i; j < (g_cBKPTCnt-1); j++)
    			{
    				g_tBkptSerialNo[j] = g_tBkptSerialNo[j + 1];
    			}
    			g_tBkptSerialNo[g_cBKPTCnt - 1].SerialNo = 0;
    			g_tBkptSerialNo[g_cBKPTCnt - 1].ScanCntOffset = 0;
    			g_tBkptSerialNo[g_cBKPTCnt - 1].ScanCnt = 0;
    			g_cBKPTCnt--;
        		break;
    		}
    	}
    }
    ProtoMap[ REPLY_DEBUGBKPT ].pFun( RECEIVE_ERROR_FLAG );

    return TRUE;
}

/*===========================================================================+
|Name:         ApplyDebugRun                                                 |
|Description:  tmIoT Studio send run command in debug mode, tell tmIoT       |
|              device set command flag                                       |
+===========================================================================*/
BOOL_T  ApplyDebugRun(BYTE_T FlagRecvOrSend)
{
	WORD_T	   wTaskFBNum;
	BYTE_T	   i,j,k;

    BYTE_T	   Recvbuf[20];
    WORD_T     CRC,byProLength = 0;
    BYTE_T     Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != Recvbuf[byProLength]) || (Recvcrcl != Recvbuf[byProLength+1]))  return 0;

	g_wStepFlag = 0;
	g_wRunFlag = 1;
	g_wStepRunflag = 0;

	for(i = 0; i < g_cBKPTCnt; i++)
	{
		for(j = 0; j < g_cTaskTotalCount; j++)
		{
			wTaskFBNum = TaskData[j].EndComponentNo - TaskData[j].StartComponentNo + 1;
			for(k = 0; k < wTaskFBNum; k++)
			{
				if(g_tBkptSerialNo[i].SerialNo == (TaskData[j].StartComponentNo + k))
				{
					g_tBkptSerialNo[i].ScanCnt = g_dwRunFBsCnt[j][k];
				}
			}
		}
	}
    ProtoMap[ REPLY_DEBUGRUN ].pFun(RECEIVE_ERROR_FLAG );

    return TRUE;
}

/*===========================================================================+
|Name:         ApplyDebugPause                                               |
|Description:  tmIoT Studio send pause command in debug mode, tell tmIoT     |
|              device set command flag                                       |
+===========================================================================*/
BOOL_T  ApplyDebugPause(BYTE_T FlagRecvOrSend)
{
    BYTE_T	   Recvbuf[20];
    WORD_T     CRC,byProLength = 0;
    BYTE_T     Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != Recvbuf[byProLength]) || (Recvcrcl != Recvbuf[byProLength+1]))  return 0;

    g_wStepFlag = 0;
    g_wRunFlag = 0;
    ProtoMap[ REPLY_DEBUGPAUSE ].pFun(RECEIVE_ERROR_FLAG );

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyDebugStep                                                |
|Description:  tmIoT Studio send step command in debug mode, tell tmIoT      |
|              device set command flag                                       |
+===========================================================================*/
BOOL_T  ApplyDebugStep( BYTE_T FlagRecvOrSend)
{
    BYTE_T	   Recvbuf[20];
    WORD_T     CRC,byProLength = 0;
    BYTE_T     Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != Recvbuf[byProLength]) || (Recvcrcl != Recvbuf[byProLength+1]))  return 0;

	g_wStepFlag = 1;
    g_wRunFlag = 0;
    SendScanSerialNo(TaskData[g_cCurrentDebugTask].StartComponentNo + g_wNext_FunctionBlockNum[g_cCurrentDebugTask]);
    ProtoMap[ REPLY_DEBUGSTEP ].pFun( RECEIVE_ERROR_FLAG );

    return TRUE;
}

/*===========================================================================+
|Name:         ApplyDebugMode                                                |
|Description:  tmIoT Studio send break point command in debug mode, tell     |
|              tmIoT device save information of break point                  |
+===========================================================================*/
BOOL_T  ApplyDebugMode( BYTE_T FlagRecvOrSend)
{
	//解析协议 调试模式信息
	BYTE_T	cModeFlag;
	BYTE_T	i;
	BYTE_T	Recvbuf[21];
	WORD_T	CRC,byProLength = 0;
	BYTE_T	Recvcrcl,Recvcrch;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);
	memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(Recvbuf,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	if((Recvcrch != Recvbuf[byProLength]) || (Recvcrcl != Recvbuf[byProLength+1]))	return 0;

	cModeFlag = ((BYTE *)g_byRecv_Buffer)[SERIALNO1_POSITION];

	if(cModeFlag)
	{
		//点击开始调试，进入Debug模式，杀死正常的runtime task，创建debug的runtime task，Debug状态为暂停
		for(i = 0; i < MaxTask; i++)
		{
			if(GetStatus_Task(g_htask_Calculations[i]) != TASK_NONE)
			{
				Destroy_Task(g_htask_Calculations[i]);
			}
		}
		Create_Idataconf_Debug();
		g_wStepFlag = 0;
		g_wRunFlag = 0;
		reply_modeflag = 1;
		g_cSendStartDebugFBNum = 0;
	}

	else
	{
		//点击停止调试，退出Debug模式,杀死debug的runtime task，创建正常的runtime task，Debug状态为运行，删除全部断点
		for(i = 0; i < MaxTask; i++)
		{
			if(GetStatus_Task(g_htask_Calculations_Debug[i]) != TASK_NONE)
			{
				Destroy_Task(g_htask_Calculations_Debug[i]);
			}
			g_wNext_FunctionBlockNum[i] = 0;
		}
		Create_Idataconf();
		memset(g_tBkptSerialNo, 0, sizeof(g_tBkptSerialNo));
		g_cBKPTCnt = 0;
		g_wStepFlag = 0;
		g_wStepRunflag = 0;
		g_wRunFlag = 1;
		reply_modeflag = 0;
	}
	g_dwSystemTick = 0;
	g_dwBKPTScanCnt = 0;
	ProtoMap[ REPLY_DEBUGMODE ].pFun( RECEIVE_ERROR_FLAG );

	return TRUE;
}

/*===========================================================================+
|Name:         ParaOd_Init                                               	 |
|Description:  Initialization parameter, Reset g_wTotal_ODcnts and 		 |
|              g_byCanSlave_Count											 |
+===========================================================================*/
//void   ParaOd_Init()
//{
//    g_wTotal_ODcnts = 0;
//    g_byCanSlave_Count = 0;
//}

/*===========================================================================+
|Name:         ApplyDownloadOd                                               |
|Description:  tmIoT Studio send break point command in debug mode, tell     |
|              tmIoT device save information of break point                  |
+===========================================================================*/
BOOL_T  ApplyDownloadOd( BYTE_T FlagRecvOrSend)
{
	BYTE 	Recvcrcl;
	BYTE 	Recvcrch;
	WORD 	CRC;
	WORD_T byProLength;
	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
	byProLength = htons(byProLength);
	byProLength = byProLength - 2;
	CRC = CRC_check(g_byRecv_Buffer,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
	if(CanDealRecvParm(2,g_byRecv_Buffer))
	{
		if(FlagRecvOrSend == 0)
			ProtoMap[ REPLY_DOWNLOADOD ].pFun( FLAG_SEND );
	}
    return TRUE;
}
/*===========================================================================+
|Name:         ApplyCanSlavePara                                             |
|Description:  tmIoT Device respond HandShake from tmIoI Studio              |
+===========================================================================*/
BOOL_T   ApplyCanSlavePara(BYTE_T  FlagRecvOrSend)
{
	BYTE 	Recvcrcl;
	BYTE 	Recvcrch;
	WORD 	CRC;
	WORD_T byProLength;
	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
	byProLength = htons(byProLength);
	byProLength = byProLength - 2;
	CRC = CRC_check(g_byRecv_Buffer,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
	_Led_RunFast();  //Run指示灯快速闪烁提醒
	if(CanDealRecvParm(1,g_byRecv_Buffer))
	{
		if(FlagRecvOrSend == 0)
	    	ProtoMap[ REPLY_CANSLAVEPARA ].pFun( FLAG_SEND );
	}
//	if(!u_TaskCanFlag)
//	{
//		Create_TaskCanFunction();
//		u_TaskCanFlag = 1;
//	}
	return TRUE;
}

/*===========================================================================+
|Name:         ApplyCanSaveConfigData                                  |
|Description:                												 |
+===========================================================================*/
BOOL_T ApplyCanSaveConfigData(BYTE_T  FlagRecvOrSend)
{
	BYTE_T Recvbuf[18];
	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);
	memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(Recvbuf,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
	{
		g_StoreParmFlagAP = 1;
		Flash_Write( (BYTE_T*)&g_StoreParmFlagAP, sizeof(BYTE_T), para_Block, g_CANFLAG_OFFSET );
		WriteCanDataToEeprom();

        if(FlagRecvOrSend == 0)
        {
        	ProtoMap[ REPLY_CANSAVECONFIGDATA ].pFun( FLAG_SEND);
        }
	}

	return TRUE;
}
/*===========================================================================+
|Name:         ApplyTaskData                                             	 |
|Description:  tmIoT Device respond TASK data from tmIoI Studio              |
+===========================================================================*/
BOOL_T  ApplyTaskData(BYTE_T  FlagRecvOrSend)
{
    BYTE_T  i;
	WORD_T  CRC,byProLength = 0;
    BYTE_T  Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    _Led_RunFast();  //Run指示灯快速闪烁提醒

    u_byPacketStamp =  (((BYTE *)g_byRecv_Buffer)[PACKETSTAMP_POSITION]);
    g_cTaskTotalCount = (((BYTE *)g_byRecv_Buffer)[TASKCOUNT_POSITION]);

//    memset(TaskData, 0, sizeof(TASKDATA_T)*MaxTask);

    for(i = 0; i < g_cTaskTotalCount; i++)
	{
    	TaskData[i].Number = (((BYTE *)g_byRecv_Buffer)[TASKNUMBER_POSITION + i*8]);
    	TaskData[i].Class = (((BYTE *)g_byRecv_Buffer)[TASKCLASS_POSITION + i*8]);
    	TaskData[i].Cycle = ((((BYTE *)g_byRecv_Buffer)[TASKCYCLE1_POSITION + i*8]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[TASKCYCLE2_POSITION + i*8]<<SHIFT00);
    	TaskData[i].StartComponentNo = ((((BYTE *)g_byRecv_Buffer)[TASKSTART1_POSITION + i*8]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[TASKSTART2_POSITION + i*8]<<SHIFT00);
    	TaskData[i].EndComponentNo = ((((BYTE *)g_byRecv_Buffer)[TASKEND1_POSITION + i*8]<<SHIFT08)|((BYTE *)g_byRecv_Buffer)[TASKEND2_POSITION + i*8]<<SHIFT00);
	}

    if(FlagRecvOrSend == 0)
    {

    	ProtoMap[ REPLY_TASKDATA ].pFun( FLAG_SEND );
    }

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyRealtimeData                                             |
|Description:  tmIoT Studio apply the Real-time data from the tmIoT device   |
+===========================================================================*/
BOOL_T  ApplyRealtimeData(BYTE_T FlagRecvOrSend)
{
    BYTE_T   Recvbuf[30];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
    	if(FlagRecvOrSend == 0)
        {
             ProtoMap[ REPLY_REALTIMEDATA ].pFun( FLAG_SEND );
        }
    }

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyComponentScanCnt                                         |
|Description:  tmIoT Studio apply the Real-time data from the tmIoT device   |
+===========================================================================*/
BOOL_T  ApplyComponentScanCnt(BYTE_T FlagRecvOrSend)
{
    BYTE_T   Recvbuf[30];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
    	if(FlagRecvOrSend == 0)
        {
             ProtoMap[ REPLY_COMPONENTSCANCNT ].pFun( FLAG_SEND );
        }
    }

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyModbusRTUPara                                         	 |
|Description:  tmIoT Studio apply ModbusRTU para 						     |
+===========================================================================*/
BOOL_T ApplyModbusRTUPara(BYTE_T FlagRecvOrSend)
{
    BYTE_T Recvbuf[APPLY_MODBUSRTUPARA_FRAME_SIZE];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memset(Recvbuf, 0, sizeof(Recvbuf));

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;
    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
    	_Led_RunFast();  //Run指示灯快速闪烁提醒
//    	memcpy(&ModbusFlashData[MODBUSPARA_OFFSET], &g_byRecv_Buffer[PROTOCOL_DATACONFIG_POSITION], APPLY_MODBUSRTUPARA_FRAME_SIZE-PROTOCOL_BASIC_SIZE);  //  参数资料 (1-7 bytes端口1参数设置，8-14bytes端口2参数设置)
    	SetRs485Para(&g_byRecv_Buffer[PROTOCOL_DATACONFIG_POSITION], APPLY_MODBUSRTUPARA_FRAME_SIZE-PROTOCOL_BASIC_SIZE); //485端口参数设置,波特率等
    	if(FlagRecvOrSend == 0)
        {
             ProtoMap[ REPLY_MODBUSRTUPARA ].pFun( FLAG_SEND);
        }
        memset(u_ModbusPort_Line, 0, sizeof(u_ModbusPort_Line));
    	memset(ModbusOD_Table, 0, sizeof(ModbusOD_Table));
    	memset(ModbusReg_Table, 0, sizeof(ModbusReg_Table));
    }

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyModbusRTUOD                                         	 |
|Description:  tmIoT Studio apply ModbusRTU OD						     	 |
+===========================================================================*/
BOOL_T ApplyModbusRTUOD(BYTE_T  FlagRecvOrSend)
{
    BYTE_T Line, Mode, Port;
    BYTE_T Recvbuf[APPLY_MODBUSRTUOD_FRAME_SIZE];
    WORD_T CRC,byProLength = 0;
    BYTE_T Recvcrcl,Recvcrch;

    memset(Recvbuf, 0, sizeof(Recvbuf));

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

    byProLength = htons(byProLength);
    memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

    byProLength = byProLength - 2;
    CRC = CRC_check(Recvbuf,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;
    if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
    {
    	memcpy(&u_byPacketStamp, &Recvbuf[4], sizeof(BYTE_T));
        memcpy(&Port, &Recvbuf[16], sizeof(BYTE_T));
        memcpy(&Mode, &Recvbuf[17], sizeof(BYTE_T));
        memcpy(&Line, &Recvbuf[18], sizeof(BYTE_T));

        if(u_byPacketStamp == 1)
        {
            memset(u_ModbusPort_Line, 0, sizeof(u_ModbusPort_Line));
        	memset(ModbusOD_Table, 0, sizeof(ModbusOD_Table));
        	memset(ModbusReg_Table, 0, sizeof(ModbusReg_Table));
        }

        if(Port >= MAX_RS485) return FALSE;
        if(u_ModbusPort_Line[Port] + Line >= MAX_MODBUSOD ) return FALSE;
        switch (Mode) {
            case MODBUS_SLAVE:
                memcpy(&ModbusReg_Table[Port][u_ModbusPort_Line[Port]], &Recvbuf[PROTOCOL_DATAOD_POSITION], Line*PROTOCOL_MODBUSSLAVE_LEN);
                u_ModbusPort_Line[Port] += Line;
                break;
            case MODBUS_MASTER:
                memcpy(&ModbusOD_Table[Port][u_ModbusPort_Line[Port]], &Recvbuf[PROTOCOL_DATAOD_POSITION], Line*PROTOCOL_MODBUSMASTER_LEN);
                u_ModbusPort_Line[Port] += Line;
                break;
            default:
                break;
            }


        if(FlagRecvOrSend == 0)
        {
             ProtoMap[ REPLY_MODBUSRTUOD ].pFun( FLAG_SEND);
        }

    }

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyModbusRTUEndFrame                                        |
|Description:                												 |
+===========================================================================*/
BOOL_T ApplyModbusRTUEndFrame(BYTE_T  FlagRecvOrSend)
{
	int i = 0;
	BYTE_T   Recvbuf[18];
	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);
	memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(Recvbuf,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
	{
        if(FlagRecvOrSend == 0)
        {
             ProtoMap[ REPLY_MODBUSRTUENDFRAM ].pFun( FLAG_SEND);

             for(i = 0; i < MAX_RS485; i++){
            	 g_ModbusPara.wRegNum[i]= u_ModbusPort_Line[i];
             }
             g_ModbusPara.byEnable = 1;

             Read_ModbusData_FromStudio();
             InitMulRS485();
             DestroyMulTaskRS485();
             CreateMulTaskRS485();
        }
	}

	return TRUE;
}
/*===========================================================================+
|Name:         ApplyModbusRTUSaveConfigData                                  |
|Description:                												 |
+===========================================================================*/
BOOL_T ApplyModbusRTUSaveConfigData(BYTE_T  FlagRecvOrSend)
{
	BYTE_T Recvbuf[18];
	WORD_T CRC,byProLength = 0;
	BYTE_T Recvcrcl,Recvcrch;

	memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));

	byProLength = htons(byProLength);
	memcpy((BYTE_T*)Recvbuf,(BYTE_T*)g_byRecv_Buffer,byProLength);

	byProLength = byProLength - 2;
	CRC = CRC_check(Recvbuf,byProLength);
	Recvcrcl = CRC&0x00FF;
	Recvcrch = (CRC&0xFF00)>>8;

	if((Recvcrch == Recvbuf[byProLength])&&(Recvcrcl == Recvbuf[byProLength+1]))
	{
		Flash_Write((BYTE*)&g_ModbusPara, sizeof(g_ModbusPara), ModbusOD_Block, MODBUSDATA_OFFSET);
		Flash_Write((BYTE*)RS485StartPara,sizeof(RS485StartPara), ModbusOD_Block, MODBUSPARA_OFFSET);
		Flash_Write((BYTE*)ModbusOD_Table,sizeof(ModbusOD_Table), ModbusOD_Block, MODBUSOD1_OFFSET);
		Flash_Write((BYTE*)ModbusReg_Table,sizeof(ModbusReg_Table), ModbusOD_Block, MODBUSOD2_OFFSET);

        if(FlagRecvOrSend == 0)
        {
        	ProtoMap[ REPLY_MODBUSRTUSAVECONFIGDATA ].pFun( FLAG_SEND);
        }
	}

	return TRUE;
}
//Basic  Frame
// Length  ProtocolVersion   PacketStamp   Flag    ProtocolID  ControllerID  ProtocolContent  CRC
// WORD         WORD           BYTE       BYTE       DWORD         6 BYTE           …        WORD

/*==================================================================================================+
|Name:         ApplyiNetSetIPInfo                                                                   |
|Description:  tmIoT Studio Download IP To tmIoT Device                                             |
|Protocol:  帧头  + DeviceIP + DevicePort + GateWay + Mask + MacAddr + SeverIP + SeverPort + CRC       |
|                  DWORD      WORD        DWORD    DWORD   6BYTE     DWORD       WORD               |
+===================================================================================================*/
BOOL_T ApplyiNetSetIPInfo(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T byProLength;
    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;

    if(iNetDealRecvParm(1,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_INETSETIPINFO ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*===========================================================================+
|Name:         ApplyiNetGetIPInfo                                            |
|Description:  tmIoT Studio Get IP Info                                      |
|Protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ApplyiNetGetIPInfo(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T  byProLength;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch == g_byRecv_Buffer[byProLength]) || (Recvcrcl == g_byRecv_Buffer[byProLength+1]));
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_INETGETIPINFO ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*==================================================================================================+
|Name:         ApplyiNetWriteConfigPara                                                             |
|Description:  tmIoT Studio Download Config Data To tmIoT Device                                    |
|Protocol:     Mode    ProtocolType                                                                 |
|              BYTE       BYTE                                                                      |
+===================================================================================================*/
BOOL_T ApplyiNetPARA(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T byProLength;
    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];
    u_ProtocolNum = u_byPacketStamp;

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    _Led_RunFast();  //Run指示灯快速闪烁提醒
    if(iNetDealRecvParm(3,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_INETPARA ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*===============================================================================================================+
|Name:         ApplyiNetODData                                                                                   |
|Description:  tmIoT Studio Download OD Data To tmIoT Device                                                     |
|Protocol:帧头  +  ODNum CurrentODNum(NumOffset NodeID DataType Permission  Uom   Precision) + CRC                  |
|                WORD     WORD    ( WORD       BYTE   BYTE     BYTE      WORD    BYTE)括号中循环                                                                         |
+================================================================================================================*/
BOOL_T ApplyiNetODData(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T byProLength;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;

    if(iNetDealRecvParm(4,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_INETODData ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*===========================================================================+
|Name:          ApplyiNetEndFrame                                            |
|Description: tnIoT Studio Data Sent Over                                    |
|Protocol:    Protocol Num (从发送Write Para Frame开始到发送End Frame之前Frame的数量) |
|                  BYTE                                                      |
+===========================================================================*/
BOOL_T ApplyiNetEndFrame(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    BYTE    byProNum = 0;
    WORD    CRC;
    WORD_T  byProLength;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];
    u_ProtocolNum = u_byPacketStamp - u_ProtocolNum;     //从发送Write Para Frame开始到发送End Frame之前Frame的数量

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    memcpy(&byProNum, &g_byRecv_Buffer[INETCONFIGDATA_OFFSET], sizeof(BYTE));

	if(iNetDealRecvParm(5,g_byRecv_Buffer))
	{
		if(FlagRecvOrSend == 0)
			ProtoMap[ REPLY_INETENDFRAME ].pFun( FLAG_SEND );
	}

    return TRUE;
}
/*===========================================================================+
|Name:         ApplyiNetConfigData                                           |
|Description:  tmIoT Studio Download Config Data                             |
|Protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ApplyiNetSaveConfigData(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T  byProLength;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    if(iNetDealRecvParm(6,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_INETCONFIGDATA ].pFun( FLAG_SEND );
    }
    return TRUE;
}

/**************************************************************ModbusTCP********************************************************************/
//Basic  Frame
// Length  ProtocolVersion   PacketStamp   Flag    ProtocolID  ControllerID  ProtocolContent  CRC
// WORD         WORD           BYTE       BYTE       DWORD         6 BYTE           …        WORD

/*==================================================================================================+
|Name:         ApplyModbusTCPPara                                                                   |
|Description:  tmIoT Studio Download ModbusTCP Mode To tmIoT Device                                 |
|Protocol:  帧头  + Mode  + CRC                                                                        |
|                BYTE    WORD                                                                       |
+===================================================================================================*/
BOOL_T ApplyModbusTCPSETMODEInfo(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T byProLength;
    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD));

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    _Led_RunFast();  //Run指示灯快速闪烁提醒
    if(ModbusTCPDealRecvParm(1,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
           ProtoMap[ REPLY_MODBUSTCPSETMODEINFO ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*===========================================================================+
|Name:         ApplyModbusTCPOD                                              |
|Description:  tmIoT Studio Download OD Data                                 |
|Protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ApplyModbusTCPOD(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T  byProLength;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    if(ModbusTCPDealRecvParm(2,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_MODBUSTCPODDATA ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*===========================================================================+
|Name:         ApplyModbusTCPENDFRAME                                        |
|Description:  tmIoT Studio END FRAME                                        |
|Protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ApplyModbusTCPENDFRAME(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T  byProLength;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    if(ModbusTCPDealRecvParm(3,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_MODBUSTCPENDFRAM ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*===========================================================================+
|Name:         ApplyModbusTCPSaveConfigData                                  |
|Description:  tmIoT Studio END FRAME                                        |
|Protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ApplyModbusTCPSaveConfigData(BYTE_T  FlagRecvOrSend)
{
    BYTE    Recvcrcl;
    BYTE    Recvcrch;
    WORD    CRC;
    WORD_T  byProLength;

    memcpy(&byProLength,g_byRecv_Buffer,sizeof(WORD_T));
    u_byPacketStamp = g_byRecv_Buffer[sizeof(WORD)*2+sizeof(BYTE)-1];

    byProLength = htons(byProLength);
    byProLength = byProLength - 2;
    CRC = CRC_check(g_byRecv_Buffer,byProLength);
    Recvcrcl = CRC&0x00FF;
    Recvcrch = (CRC&0xFF00)>>8;

    if((Recvcrch != g_byRecv_Buffer[byProLength]) || (Recvcrcl != g_byRecv_Buffer[byProLength+1]))  return 0;
    if(ModbusTCPDealRecvParm(4,g_byRecv_Buffer))
    {
        if(FlagRecvOrSend == 0)
            ProtoMap[ REPLY_MODBUSTCPSAVEFRAM ].pFun( FLAG_SEND );
    }
    return TRUE;
}
/*===========================================================================+
|Name:         ReplyHandshake                                                |
|Description:  tmIoT Device respond HandShake from tmIoI Studio              |
+===========================================================================*/

BOOL_T   ReplyHandshake(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_HandShakeID;
	BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
	WORD_T     wProtocol_Reply_CRC  = 0x0;
	WORD_T     wReProVersion;
	BYTE_T*    pTemBuf = SendProBuf;
	BYTE_T*    pTemp=NULL;

	pTemp = pTemBuf;

	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	wReProVersion =  htons(PROVESION);
	memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	dwProtocol_Reply_HandShakeID = htonl(PROTOCOL_ID_REPLY_HANDSHAKE);
	memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_HandShakeID,sizeof(DWORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

	u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
	u_wReProLength = htons(u_wReProLength);
	memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

	u_wReProLength = htons(u_wReProLength);
	wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
	wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
	memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

	if(u_cConnectPort)
		Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

	return TRUE;
}

/*===========================================================================+
|Name:         ReplyStartFrame                                               |
|Description:  tmIoT Device respond Start Frame From tmIoT Studio            |
+===========================================================================*/
BOOL_T ReplyStartFrame( BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_StartID ;
	BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
	WORD_T     wReProVersion;
	WORD_T     wProtocol_Reply_CRC  = 0x0;
	BYTE_T*    pTemBuf = SendProBuf;
	BYTE_T*    pTemp=NULL;

	pTemp = pTemBuf;

	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	wReProVersion = htons(PROVESION);

	memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf,&g_bReProPackstamp_StartFrame,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	dwProtocol_Reply_StartID = htonl(PROTOCOL_ID_REPLY_STARTFRAME);
	memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_StartID,sizeof(DWORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

	u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
	u_wReProLength = htons(u_wReProLength);
	memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

	u_wReProLength = htons(u_wReProLength);

	wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
	wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
	memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

	if(u_cConnectPort)
		Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

	memset(g_RuntimeFlag, 0, MaxTask);

	return TRUE;
}

/*===========================================================================+
|Name:         ReplyDataFrame                                                |
|Description:  tmIoT Device respond Data Frame From tmIoT Studio if the Data |
|              is no Error                                                   |
+===========================================================================*/
BOOL_T ReplyDataFrame(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_DataFrameID;
	BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
	WORD_T     wProtocol_Reply_CRC  = 0x0;
	WORD_T     wReProVersion;
	BYTE_T*    pTemBuf = SendProBuf;
	BYTE_T*    pTemp=NULL;
	WORD_T     wSerialNo;

	pTemp = pTemBuf;

	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	wReProVersion = htons(PROVESION);
	memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	dwProtocol_Reply_DataFrameID = htonl(PROTOCOL_ID_REPLY_DATAFRAME);
	memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DataFrameID,sizeof(DWORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

	wSerialNo = htons(u_wSerialNo);
	memcpy((BYTE_T *)pTemBuf,&wSerialNo,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
	u_wReProLength = htons(u_wReProLength);
	memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

	u_wReProLength = htons(u_wReProLength);

	wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
	wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
	memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

	if(u_cConnectPort)
		Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

     return TRUE;
}

/*===========================================================================+
|Name:         ReplyEndFrame                                                 |
|Description:  tmIoT Device respond End Frame From tmIoT Studio,Tell tmIoT   |
|              Studio the Data is all Right or not                           |
+===========================================================================*/
BOOL_T ReplyEndFrame(BYTE_T  FlagRecvOrSend)
{
	WORD_T i;
    DWORD_T    dwProtocol_Reply_EndFrameID ;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;
    BYTE_T     byRight = 1,byError = 2;
    WORD_T     wErrorID  = 0x0;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp_EndFrame,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_EndFrameID = htonl(PROTOCOL_ID_REPLY_ENDFRAME);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_EndFrameID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    if( FlagRecvOrSend == 1 )
    {
        memcpy((BYTE_T *)pTemBuf,&byRight,sizeof(BYTE_T));
        pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
    }
    else
    {
        memcpy((BYTE_T *)pTemBuf,&byError,sizeof(BYTE_T));
        pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
    }

	memcpy((BYTE_T *)pTemBuf,&wErrorID,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
	u_wReProLength = htons(u_wReProLength);
	memcpy(SendProBuf,&u_wReProLength, sizeof(WORD_T));

	u_wReProLength = htons(u_wReProLength);

	wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength - 2));
	wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
	memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

	if(u_cConnectPort)
		Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

     //若数据接收成功，开始解析数据包
	if( FlagRecvOrSend == 1 )
	{
		for(i = 0; i < MaxTask; i++)
		{
			if(GetStatus_Task(g_htask_Calculations[i]) != TASK_NONE)
				Destroy_Task(g_htask_Calculations[i]);
		}
		ComponenttoTaskData();
		InitObjectListData();
		Create_Idataconf();
	}

	return TRUE;
}

/*===========================================================================+
|Name:         ReplyCommDataFrame                                            |
|Description:  tmIoT Device respond Data Frame From tmIoT Studio if the Data |
|              is no Error                                                   |
+===========================================================================*/
BOOL_T ReplyCommDataFrame(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_CommDataFrameID;
	BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
	WORD_T     wProtocol_Reply_CRC  = 0x0;
	WORD_T     wReProVersion;
	BYTE_T*    pTemBuf = SendProBuf;
	BYTE_T*    pTemp=NULL;
	BYTE_T     cLength;

	pTemp = pTemBuf;

	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	wReProVersion = htons(PROVESION);
	memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	dwProtocol_Reply_CommDataFrameID = htonl(PROTOCOL_ID_REPLY_COMMDATAFRAME);
	memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_CommDataFrameID,sizeof(DWORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

	cLength = htons(u_cLength);
	memcpy((BYTE_T *)pTemBuf, &cLength,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
	u_wReProLength = htons(u_wReProLength);
	memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

	u_wReProLength = htons(u_wReProLength);

	wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
	wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
	memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

	if(u_cConnectPort)
		Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

     return TRUE;
}
/*===========================================================================+
|Name:         ReplyGlobalVariableFrame                                      |
|Description:  tmIoT Device respond Data Frame From tmIoT Studio if the Data |
|              is no Error                                                   |
+===========================================================================*/
BOOL_T ReplyGlobalVariableFrame(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_GlobalVariableFrameID;
	BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
	WORD_T     wProtocol_Reply_CRC  = 0x0;
	WORD_T     wReProVersion;
	BYTE_T*    pTemBuf = SendProBuf;
	BYTE_T*    pTemp=NULL;
	BYTE_T     cLength;

	pTemp = pTemBuf;

	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	wReProVersion = htons(PROVESION);
	memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	dwProtocol_Reply_GlobalVariableFrameID = htonl(PROTOCOL_ID_REPLY_GLOBALVARIABLEFRAME);
	memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_GlobalVariableFrameID,sizeof(DWORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

	cLength = htons(u_cLength);
	memcpy((BYTE_T *)pTemBuf, &cLength,sizeof(BYTE_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

	u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
	u_wReProLength = htons(u_wReProLength);
	memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

	u_wReProLength = htons(u_wReProLength);

	wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
	wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
	memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

	if(u_cConnectPort)
		Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

     return TRUE;
}

/*===========================================================================+
|Name:         ReplySaveConfigData                                           |
|Description:  tmIoT Device respond Save ConfigData From tmIoT Studio        |
|              Tell tmIoT Studio ConfigData has saved successfully           |
+===========================================================================*/
BOOL_T ReplySaveConfigData(BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_ConfigID ;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;
    WORD_T     wReProVersion;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_ConfigID = htonl(PROTOCOL_ID_REPLY_SAVECONFIGDATA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_ConfigID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength - 2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}

/*===========================================================================+
|Name:         ReplyRealtimeDataAttr                                         |
|Description:  tmIoT Device Respond Real-time Data Frame that include the    |
|              current Data From every FunctionBlock,DataID,DataType,        |
|              UOM and so on                                                 |
+===========================================================================*/
BOOL_T ReplyRealtimeDataAttr( BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_RealtimeDataAttrID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    BYTE_T     bProtocol_DataFlag = 0x01;
    WORD_T     wCanonicial_DataType = 0x0;
    WORD_T     bProtocol_DataUom = 0x0;
    BYTE_T     bProtocol_DataPrecision = 0x0;

    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     i,j;
    BYTE_T*    pTemBuf_Send = SendProBuf;
    BYTE_T*    pTemp=NULL;
    BYTE_T     RealdataTaskNo = 0;
    WORD_T	   RealdataComponentCnt;
    WORD_T     DataIDCount_Reverse;
    DWORD_T    DataID;
    WORD_T     wReProVersion;
    DWORD_T	   IOMapping_Value;

    memcpy(&RealdataTaskNo,(BYTE_T*)&g_byRecv_Buffer[DATAIDCOUNT_POSITION],sizeof(BYTE_T));
    RealdataComponentCnt = TaskData[RealdataTaskNo].EndComponentNo - TaskData[RealdataTaskNo].StartComponentNo +1;

	pTemp = pTemBuf_Send;

	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	wReProVersion = htons(PROVESION);

	memcpy((BYTE_T *)pTemBuf_Send,&wReProVersion,sizeof(WORD_T));// Protocol Version
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf_Send,&u_byPacketStamp,sizeof(BYTE_T)); //Protocol Packet Stamp
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf_Send,&u_bReProSpecial_Flag,sizeof(BYTE_T));// Protocol Special Flag
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	dwProtocol_Reply_RealtimeDataAttrID = htonl(PROTOCOL_ID_REPLY_REALTIMEDATAATTR);  // Protocol ID
	memcpy((BYTE_T *)pTemBuf_Send,&dwProtocol_Reply_RealtimeDataAttrID,sizeof(DWORD_T));
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID)); // Controller ID
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(bProtocol_Reply_ControlID));

	memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_DataFlag,sizeof(BYTE_T)); //Data Flag,this is Protocol content
                                                                             //(The Following code belong to "protocol content" in this Function)
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	DataIDCount_Reverse = htons(RealdataComponentCnt);
	memcpy((BYTE_T *)pTemBuf_Send,&DataIDCount_Reverse,sizeof(WORD_T)); // DataID Counts
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf_Send,&RealdataTaskNo,sizeof(BYTE_T));	//respone task number to be verified by studio
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	for(j = 0; j < RealdataComponentCnt; j++)  //Data Frame (include all the DataID and Data in this Protocol frame)
	{
		if(g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].DataType == 1)
		{
			wCanonicial_DataType = 1;     //Digital Data;
		}
		else if(g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].DataType == 2)
		{
            wCanonicial_DataType = 2;     //Analog Data
        }
        else if(g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].DataType == 8)
        {
            wCanonicial_DataType = 8;     //Address Data;
        }
        else
        	wCanonicial_DataType = 3;     //Other;

        DataID = (DWORD_T)TaskData[RealdataTaskNo].StartComponentNo + j;
        DataID = htonl(DataID);
        memcpy((BYTE_T *)pTemBuf_Send,&DataID,sizeof(DWORD_T));              //Send DataID
        pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));

        wCanonicial_DataType = htons(wCanonicial_DataType);
        memcpy((BYTE_T *)pTemBuf_Send,&wCanonicial_DataType,sizeof(WORD_T));    //Send Data Type
        pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

        memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_DataUom,sizeof(WORD_T));        //Send Data UOM
        pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

        memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_DataPrecision,sizeof(BYTE_T));    //Send Data Precision
        pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

        WORD_T bProtocol_DataRowCount = 1;
        bProtocol_DataRowCount = htons(bProtocol_DataRowCount);
        memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_DataRowCount,sizeof(WORD_T));    //Send Data Row Count
        pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

		WORD_T bProtocol_Datalength = 4;
		bProtocol_Datalength = htons(bProtocol_Datalength);
		memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_Datalength,sizeof(WORD_T));    //Send Data length
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

		DWORD_T bProtocol_DataValue = g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].outputValue.dwData;
		bProtocol_DataValue = htonl(bProtocol_DataValue);
		memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_DataValue,sizeof(DWORD_T));    //Send Data length
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
    }

	//IO Mapping
#if (defined IO2C1111) || (defined IO2C1112)
	for( i = 0; i < 5; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_INPUTG1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T)); 			 //5DI
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
	for( i = 0; i < 5; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_OUTPUTG1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T));              //5DO
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
#elif (defined M2C2132)
	for( i = 0; i < 2; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_INPUTG1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T));              //2DI
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
	for( i = 0; i < 2; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_OUTPUTG1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T));              //2DO
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
#elif (defined IO2C1131)
	for( i = 0; i < 8; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_INPUTG1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T));              //8DI
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
	for( i = 0; i < 6; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_OUTPUTG1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T));               //6DO
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
#elif (defined S2C4111)
	for( i = 0; i < 4; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_AD1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T));               //4DI
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
	for( i = 0; i < 4; i++)
	{
		IOMapping_Value = g_DBData[ENUM_CARD_0_INTERFACE_IO_DA1+i].dbValue.dwData;
		IOMapping_Value = htonl(IOMapping_Value);
		memcpy((BYTE_T *)pTemBuf_Send,&IOMapping_Value,sizeof(DWORD_T));			  //4DO
		pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));
	}
#endif

    u_wReProLength = pTemBuf_Send - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength - 2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf_Send,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyConfigServerIP                                           |
|Description:  tmIoT Device respond Config Server IP Frame,Tell tmIoT Studio |
|              Server IP Config successfully                                 |
+===========================================================================*/
BOOL_T ReplyConfigServerIP( BYTE_T FlagRecvOrSend)
{
//	DWORD_T    dwProtocol_Reply_ConfigServerIP;
//    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
//    WORD_T     wProtocol_Reply_CRC  = 0x0;
//    BYTE_T*    pTemBuf = SendProBuf;
//    BYTE_T*    pTemp=NULL;
//    WORD_T     wReProVersion;
//
//    pTemp = pTemBuf;
//
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));
//
//    wReProVersion =  htons(PROVESION);
//    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));
//
//    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp_EndFrame,sizeof(BYTE_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
//
//    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
//
//    dwProtocol_Reply_ConfigServerIP = htonl(PROTOCOL_ID_REPLY_CONFIGSERVERIP);
//    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_ConfigServerIP,sizeof(DWORD_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));
//
//    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));
//
//    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
//    u_wReProLength = htons(u_wReProLength);
//    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));
//
//    u_wReProLength = htons(u_wReProLength);
//
//    wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength-2)) ;
//    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
//    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));
//
//    if(u_cConnectPort)
//    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
//	else
//		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);
//
//    del_list(0);
////    del_list(1);
//    SetLwipPara();
//    Init_RemoteIP();
//    u_Udp_pcb = Create_LwipUdp(&g_sRemoteMesg, LWIP_SERVER);

    return TRUE;
}

/*===========================================================================+
|Name:         ReplyConfigTmIoTIP                                            |
|Description:  tmIoT Device respond Config tmIoT Device IP Frame from tmIoT  |
|              Studio,Tell tmIoT That tmIoT Device IP config successfully    |
+===========================================================================*/
BOOL_T ReplyConfigTmIoTIP( BYTE_T FlagRecvOrSend)
{
//	DWORD_T    dwProtocol_Reply_ConfigTmIoTIP ;
//    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
//    WORD_T     wProtocol_Reply_CRC  = 0x0;
//    BYTE_T*    pTemBuf = SendProBuf;
//    BYTE_T*    pTemp=NULL;
//    WORD_T     wReplayProtocol_Version;
//
//    pTemp = pTemBuf;
//
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));
//
//    wReplayProtocol_Version =  htons(PROVESION);
//    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));
//
//    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp_EndFrame,sizeof(BYTE_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
//
//    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
//
//    dwProtocol_Reply_ConfigTmIoTIP = htonl(PROTOCOL_ID_REPLY_CONFIGTMIOTIP);
//    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_ConfigTmIoTIP,sizeof(DWORD_T));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));
//
//    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
//    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));
//
//    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
//    u_wReProLength = htons(u_wReProLength);
//    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));
//
//    u_wReProLength = htons(u_wReProLength);
//
//    wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength - 2));
//    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
//    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));
//
//    if(u_cConnectPort)
//    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
//	else
//		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);
//
//    del_list(0);
//    SetLwipUdpIp();
//    CreateLwipUdp();
//    u_Udp_pcb = Create_LwipUdp(&g_sRemoteMesg, LWIP_SERVER);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyDataFrame                                                |
|Description:  c                                                             |
+===========================================================================*/
BOOL_T ReplyDebugBkpt( BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_DebugBkptID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_DebugBkptID = htonl(PROTOCOL_ID_REPLY_DEBUGBKPT);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DebugBkptID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyDebugRun                                                 |
|Description:  tmIoT Device respond the Debug Run Frame from tmIoT Studio    |
+===========================================================================*/
BOOL_T ReplyDebugRun(BYTE_T FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_DebugRunID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_DebugRunID = htonl(PROTOCOL_ID_REPLY_DEBUGRUN);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DebugRunID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyDebugPause                                               |
|Description:  tmIoT Device respond the Debug Pause Frame from tmIoT Studio  |
+===========================================================================*/
BOOL_T ReplyDebugPause( BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_DebugPauseID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_DebugPauseID = htonl(PROTOCOL_ID_REPLY_DEBUGPAUSE);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DebugPauseID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplydebugStep                                                |
|Description:  tmIoT Device respond the Debug Step Frame from tmIoT Studio   |
+===========================================================================*/
BOOL_T ReplyDebugStep( BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_DebugStepID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_DebugStepID = htonl(PROTOCOL_ID_REPLY_DEBUGSTEP);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DebugStepID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyDebugMode                                                |
|Description:  tmIoT Device respond the Debug Mode Frame from tmIoT Studio   |
+===========================================================================*/
BOOL_T ReplyDebugMode( BYTE_T FlagRecvOrSend)
{
	BYTE_T     SendProBuf[30];
	DWORD_T    dwProtocol_Reply_DebugModeID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProPackstamp_HeartBeat,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_DebugModeID = htonl(PROTOCOL_ID_REPLY_DEBUGMODE);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DebugModeID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    memcpy((BYTE_T *)pTemBuf,&reply_modeflag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyDownloadOd                                               |
|Description:  tmIoT Studio send break point command in debug mode, tell     |
|              tmIoT device save information of break point                  |
+===========================================================================*/
BOOL_T  ReplyDownloadOd( BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_DebugModeID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_DebugModeID = htonl(PROTOCOL_ID_REPLY_DOWNLOADOD);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DebugModeID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyCanSlavePara                                             |
|Description:  tmIoT Studio send Object Dictionary parameter,include the Can |
|              Slave Count                                                   |
+===========================================================================*/
BOOL_T   ReplyCanSlavePara(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_DebugModeID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReplayProtocol_Version;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReplayProtocol_Version =  htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReplayProtocol_Version,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_DebugModeID = htonl(PROTOCOL_ID_REPLY_CANSLAVEPARA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_DebugModeID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);
    wProtocol_Reply_CRC = CRC_check((BYTE_T*)SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyCanSlavePara                                             |
|Description:  tmIoT Studio send Object Dictionary parameter,include the Can |
|              Slave Count                                                   |
+===========================================================================*/
BOOL_T  ReplyTaskData(    BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_TaskDataID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;
    BYTE_T     cTaskCount;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_TaskDataID = htonl(PROTOCOL_ID_REPLY_TASKDATA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_TaskDataID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    cTaskCount = g_cTaskTotalCount;
    memcpy((BYTE_T *)pTemBuf,&cTaskCount,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}

/*===========================================================================+
|Name:         ReplyRealtimeData                                             |
|Description:  tmIoT Device Respond Real-time Data Frame that include the    |
|              current Data From every FunctionBlock,DataID,DataType,        |
|              UOM and so on                                                 |
+===========================================================================*/
BOOL_T ReplyRealtimeData( BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_RealtimeDataID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     j;
    BYTE_T*    pTemBuf_Send = SendProBuf;
    BYTE_T*    pTemp=NULL;
    WORD_T     RealdataTaskNo = 0;
    WORD_T	   RealdataComponentCnt;
    WORD_T     DataIDCount_Reverse;
    DWORD_T    DataID;
    DWORD      DataValue;
    WORD_T     wReProVersion;

    memcpy(&RealdataTaskNo,(BYTE_T*)&g_byRecv_Buffer[DATAIDCOUNT_POSITION],sizeof(BYTE_T));
    RealdataComponentCnt = TaskData[RealdataTaskNo].EndComponentNo - TaskData[RealdataTaskNo].StartComponentNo +1;

	pTemp = pTemBuf_Send;

	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	wReProVersion = htons(PROVESION);
	memcpy((BYTE_T *)pTemBuf_Send,&wReProVersion,sizeof(WORD_T));// Protocol Version
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf_Send,&u_byPacketStamp,sizeof(BYTE_T)); //Protocol Packet Stamp
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf_Send,&u_bReProSpecial_Flag,sizeof(BYTE_T));// Protocol Special Flag
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	dwProtocol_Reply_RealtimeDataID = htonl(PROTOCOL_ID_REPLY_REALTIMEDATA);  // Protocol ID
	memcpy((BYTE_T *)pTemBuf_Send,&dwProtocol_Reply_RealtimeDataID,sizeof(DWORD_T));
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID)); // Controller ID
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(bProtocol_Reply_ControlID));

	DataIDCount_Reverse = htons(RealdataComponentCnt);
	memcpy((BYTE_T *)pTemBuf_Send,&DataIDCount_Reverse,sizeof(WORD_T)); // DataID Counts
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	for(j = 0; j < RealdataComponentCnt; j++)  //Data Frame (include all the DataID and Data in this Protocol frame)
	{
		if(g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].DataType == 0x0002)
		{
			DataValue = g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].outputValue.dwData;
		}
		else if(g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].DataType == 0x0003)
		{
            DataValue = g_RealtimeData[DataID];
        }
        else
        {
            DataValue = g_RunTime_ObjectData[TaskData[RealdataTaskNo].StartComponentNo+j].outputValue.dwData;
        }

        DataValue = htonl(DataValue);
        memcpy((BYTE_T *)pTemBuf_Send,&DataValue,sizeof(DWORD_T));
        pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));  //Send Data
	}

    u_wReProLength = pTemBuf_Send - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength - 2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf_Send,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}

/*===========================================================================+
|Name:         ReplyComponentScanCnt                                         |
|Description:  tmIoT Device Respond Real-time Data Frame that include the    |
|              current Data From every FunctionBlock,DataID,DataType,        |
|              UOM and so on                                                 |
+===========================================================================*/
BOOL_T ReplyComponentScanCnt( BYTE_T FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_ComponentScanCntID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     j;
    BYTE_T*    pTemBuf_Send = SendProBuf;
    BYTE_T*    pTemp=NULL;
    WORD_T     RealdataTaskNo = 0;
    WORD_T	   RealdataComponentCnt;
    DWORD_T	   ComponentScanCnt;
    WORD_T     DataIDCount_Reverse;
    WORD_T     wReProVersion;

    memcpy(&RealdataTaskNo,(BYTE_T*)&g_byRecv_Buffer[DATAIDCOUNT_POSITION],sizeof(BYTE_T));
    RealdataComponentCnt = TaskData[RealdataTaskNo].EndComponentNo - TaskData[RealdataTaskNo].StartComponentNo +1;

	pTemp = pTemBuf_Send;

	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	wReProVersion = htons(PROVESION);
	memcpy((BYTE_T *)pTemBuf_Send,&wReProVersion,sizeof(WORD_T));// Protocol Version
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	memcpy((BYTE_T *)pTemBuf_Send,&u_byPacketStamp,sizeof(BYTE_T)); //Protocol Packet Stamp
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	memcpy((BYTE_T *)pTemBuf_Send,&u_bReProSpecial_Flag,sizeof(BYTE_T));// Protocol Special Flag
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(BYTE_T));

	dwProtocol_Reply_ComponentScanCntID = htonl(PROTOCOL_ID_REPLY_COMPONENTSCANCNT);  // Protocol ID
	memcpy((BYTE_T *)pTemBuf_Send,&dwProtocol_Reply_ComponentScanCntID,sizeof(DWORD_T));
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));

	memcpy((BYTE_T *)pTemBuf_Send,&bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID)); // Controller ID
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(bProtocol_Reply_ControlID));

	DataIDCount_Reverse = htons(RealdataComponentCnt);
	memcpy((BYTE_T *)pTemBuf_Send,&DataIDCount_Reverse,sizeof(WORD_T)); // DataID Counts
	pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(WORD_T));

	for(j = 0; j < RealdataComponentCnt; j++)  //Data Frame (include all the DataID and Data in this Protocol frame)
	{
        ComponentScanCnt = htonl(g_dwRunFBsCnt[RealdataTaskNo][j]);
        memcpy((BYTE_T *)pTemBuf_Send,&ComponentScanCnt,sizeof(DWORD_T));
        pTemBuf_Send = (BYTE_T *)((BYTE_T *)pTemBuf_Send + sizeof(DWORD_T));  //Send Component Scan Count
	}

    u_wReProLength = pTemBuf_Send - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf,&u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength - 2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf_Send,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyCanEndFrame                                              |
|Description:  tmIoT Device Respond Real-time Data Frame that include the    |
|              current Data From every FunctionBlock,DataID,DataType,        |
|              UOM and so on                                                 |
+===========================================================================*/
BOOL_T ReplyCanEndFrame(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_EndFrameID ;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;
    BYTE_T     byRight = 1,byError = 2;
    WORD_T     wErrorID  = 0x0;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp_EndFrame,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_EndFrameID = htonl(PROTOCOL_ID_REPLY_CANENDFRAME);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_EndFrameID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    if( FlagRecvOrSend == 1 )
    {
        memcpy((BYTE_T *)pTemBuf,&byRight,sizeof(BYTE_T));
        pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
    }
    else
    {
        memcpy((BYTE_T *)pTemBuf,&byError,sizeof(BYTE_T));
        pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));
    }

	memcpy((BYTE_T *)pTemBuf,&wErrorID,sizeof(WORD_T));
	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

	u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
	u_wReProLength = htons(u_wReProLength);
	memcpy(SendProBuf,&u_wReProLength, sizeof(WORD_T));

	u_wReProLength = htons(u_wReProLength);

	wProtocol_Reply_CRC = CRC_check(SendProBuf, (u_wReProLength - 2));
	wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
	memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

	if(u_cConnectPort)
		Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

	return TRUE;
}

/*===========================================================================+
|Name:         ReplyCanSaveConfigData                                       |
|Description:                                                   			|
+===========================================================================*/
BOOL_T ReplyCanSaveConfigData(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_CANSAVECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_CANSAVECONFIGDATA = htonl(PROTOCOL_ID_REPLY_CANSAVECONFIGDATA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_CANSAVECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         Reply_ModbusRTUPara                                         	|
|Description:  			                                                   	|
+===========================================================================*/
BOOL_T ReplyModbusRTUPara(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_MODBUSRTUPARAID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_MODBUSRTUPARAID = htonl(PROTOCOL_ID_REPLY_MODBUSRTUPARA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_MODBUSRTUPARAID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         Reply_ModbusRTUOD                                         	|
|Description:                                                   			|
+===========================================================================*/
BOOL_T ReplyModbusRTUOD(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_MODBUSRTUODID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_MODBUSRTUODID = htonl(PROTOCOL_ID_REPLY_MODBUSRTUOD);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_MODBUSRTUODID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyModbusRTUEndFrame                                       |
|Description:                                                   			|
+===========================================================================*/
BOOL_T ReplyModbusRTUEndFrame(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_MODBUSRTUENDFRAMEID;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_MODBUSRTUENDFRAMEID = htonl(PROTOCOL_ID_REPLY_MODBUSRTUENDFRAM);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_MODBUSRTUENDFRAMEID,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyModbusRTUSaveConfigData                                 |
|Description:                                                   			|
+===========================================================================*/
BOOL_T ReplyModbusRTUSaveConfigData(BYTE_T  FlagRecvOrSend)
{
	DWORD_T    dwProtocol_Reply_MODBUSRTUSAVECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_MODBUSRTUSAVECONFIGDATA = htonl(PROTOCOL_ID_REPLY_MODBUSRTUSAVECONFIGDATA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_MODBUSRTUSAVECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyiNetSetIPInfo                                            |
|Description:  tmIoT Device Respond Config Para Frame To tmIoT Studio        |
|protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyiNetSetIPInfo(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETWERITECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETWERITECONFIGDATA = htonl(PROTOCOL_ID_REPLY_INETSETIPINFO);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETWERITECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
        Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*=======================================================================================================+
|Name:         ReplyiNetGetIPInfo                                                                        |
|Description:  tmIoT Device Respond Read Config Data Frame To tmIoT Studio                               |
|Protocol:  帧头  + DeviceIP + DevicePort + GateWay + Mask + MacAddr + SeverIP + SeverPort + CRC            |
|                  DWORD      WORD        DWORD    DWORD   6BYTE     DWORD       WORD                    |
+========================================================================================================*/
BOOL_T ReplyiNetGetIPInfo(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETSAVECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[8]  = {0x0};
    BYTE_T     byiNetParaBuf[26];
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    Flash_Read(byiNetParaBuf, INETIPInfo_SIZE, iNetData_Block, iNetIPInfo_ofs);

    byiNetParaBuf[14] = g_byMacAdderBuf[0];
    byiNetParaBuf[15] = g_byMacAdderBuf[1];
    byiNetParaBuf[16] = g_byMacAdderBuf[2];
    byiNetParaBuf[17] = g_byMacAdderBuf[3];
    byiNetParaBuf[18] = g_byMacAdderBuf[4];
    byiNetParaBuf[19] = g_byMacAdderBuf[5];

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETSAVECONFIGDATA = htonl(PROTOCOL_ID_REPLY_INETGETIPINFO);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETSAVECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID)-2);
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID)-2);

    memcpy((BYTE *)pTemBuf, byiNetParaBuf, sizeof(byiNetParaBuf));
    pTemBuf = (BYTE *) (((BYTE *)pTemBuf) + sizeof(byiNetParaBuf));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
        Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyiNetSetIPInfo                                            |
|Description:  tmIoT Device Respond Config Para Frame To tmIoT Studio        |
|Prtotovol:    Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyiNetPara(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETODTABDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETODTABDATA = htonl(PROTOCOL_ID_REPLY_INETPARA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETODTABDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
        Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyiNetODData                                               |
|Description:  tmIoT Device Respond OD Data Frame To tmIoT Studio            |
|Protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyiNetODData(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETODTABDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETODTABDATA = htonl(PROTOCOL_ID_REPLY_INETODData);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETODTABDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
        Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyiNetEndFrame                                             |
|Description:  tmIoT Device Respond iNet End Frame To tmIoT Studio           |
|Protocol:     Protocol Num                                                  |
|                 BYTE                                                       |
+===========================================================================*/
BOOL_T ReplyiNetEndFrame(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETENDFRAME;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETENDFRAME = htonl(PROTOCOL_ID_REPLY_INETENDFRAME);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETENDFRAME,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    memcpy((BYTE_T *)pTemBuf,&u_ProtocolNum,sizeof(u_ProtocolNum));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(u_ProtocolNum));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
        Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyiNetSaveConfigData                                       |
|Description:  tmIoT Device Respond Config Data Frame To tmIoT Studio        |
|Protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyiNetSaveConfigData(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETSAVECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETSAVECONFIGDATA = htonl(PROTOCOL_ID_REPLY_INETCONFIGDATA);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETSAVECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
        Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}

/*===========================================================================+
|Name:         ReplyModbusTCPSetMODEInfo                                     |
|Description:  tmIoT Device Respond Config Para Frame To tmIoT Studio        |
|protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyModbusTCPSetMODEInfo(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETWERITECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETWERITECONFIGDATA = htonl(PROTOCOL_ID_REPLY_ModbusTCPSETMODEINFO);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETWERITECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
	else
		USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyModbusTCPOD                                              |
|Description:  tmIoT Device Respond Config Para Frame To tmIoT Studio        |
|protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyModbusTCPOD(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETWERITECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETWERITECONFIGDATA = htonl(PROTOCOL_ID_REPLY_MODBUSTCPOD);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETWERITECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
    else
    	USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyMODBUSTCPENDFRAM                                         |
|Description:  tmIoT Device Respond Config Para Frame To tmIoT Studio        |
|protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyMODBUSTCPENDFRAM(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETWERITECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETWERITECONFIGDATA = htonl(PROTOCOL_ID_REPLY_MODBUSTCPENDFRAM);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETWERITECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
    else
    	USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);

    return TRUE;
}
/*===========================================================================+
|Name:         ReplyMODBUSTCPSAVEFRAM                                        |
|Description:  tmIoT Device Respond Config Para Frame To tmIoT Studio        |
|protocol:     Basic Frame                                                   |
+===========================================================================*/
BOOL_T ReplyMODBUSTCPSAVEFRAM(BYTE_T  FlagRecvOrSend)
{
    DWORD_T    dwProtocol_Reply_INETWERITECONFIGDATA;
    BYTE_T     bProtocol_Reply_ControlID[6]  = {0x0};
    WORD_T     wProtocol_Reply_CRC  = 0x0;
    WORD_T     wReProVersion;
    BYTE_T*    pTemBuf = SendProBuf;
    BYTE_T*    pTemp=NULL;

    pTemp = pTemBuf;

    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    wReProVersion = htons(PROVESION);
    memcpy((BYTE_T *)pTemBuf,&wReProVersion,sizeof(WORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD_T));

    memcpy((BYTE_T *)pTemBuf,&u_byPacketStamp,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    memcpy((BYTE_T *)pTemBuf,&u_bReProSpecial_Flag,sizeof(BYTE_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE_T));

    dwProtocol_Reply_INETWERITECONFIGDATA = htonl(PROTOCOL_ID_REPLY_MODBUSTCPSAVEFRAM);
    memcpy((BYTE_T *)pTemBuf,&dwProtocol_Reply_INETWERITECONFIGDATA,sizeof(DWORD_T));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD_T));

    memcpy((BYTE_T *)pTemBuf,bProtocol_Reply_ControlID,sizeof(bProtocol_Reply_ControlID));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(bProtocol_Reply_ControlID));

    u_wReProLength = pTemBuf - pTemp + sizeof(WORD_T);
    u_wReProLength = htons(u_wReProLength);
    memcpy(SendProBuf, &u_wReProLength,sizeof(WORD_T));

    u_wReProLength = htons(u_wReProLength);

    wProtocol_Reply_CRC = CRC_check(SendProBuf,(u_wReProLength-2));
    wProtocol_Reply_CRC = htons(wProtocol_Reply_CRC);
    memcpy((BYTE_T *)pTemBuf,&wProtocol_Reply_CRC,sizeof(WORD_T));

    if(u_cConnectPort)
    	Lwip_UdpWrite_Studio((BYTE_T *)SendProBuf, u_wReProLength);
    else
    	USB_WriteToHost_Studio(g_psDevice, (BYTE_T *)SendProBuf, u_wReProLength);
 //   Read_ModbusTCPData_FromFlash();
//	CreateLwipTcp();
    return TRUE;
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                            |
+---------------------------------------------------------------------------------------------*/
