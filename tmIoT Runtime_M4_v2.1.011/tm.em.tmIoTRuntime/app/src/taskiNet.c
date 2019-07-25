/*==========================================================================+
|  File     : taskinet.c                                                    |
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
#include "taskiNet.h"
#include "taskmodbustcp.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
extern   BYTE_T    g_byRecv_Buffer[MAX_RECVNUM];

static   BYTE      u_iNetSendBuf[1500];
static   DWORD     dwDeviceEntityAID[MAX_DEVICEAID];

static   DWORD     g_dwMap = 0;
/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
static BYTE SendProtocolVersionToManagementSystem    (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE RecvProtocolVersionFromManagementSystem  (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE SendLocalAddrToManagementSystem          (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE RecvLocalAddrFromManagementSystem        (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE RecvHeartBeatDataFromManagementSystem    (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE SendHeartBeatDataToManagementSystem      (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE RecvReadDataCommandFromManagementSystem  (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE SendReadDataToManagementSystem           (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE RecvWriteDataCommandFromManagementSystem (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE SendWriteDataToManagementSystem          (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE RecvTimingCheckFromManagementSystem      (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE SendTimingCheckToManagementSystem        (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE RecvTimingAckFromManagementSystem        (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE SendTimingAckToManagementSystem          (BYTE *pData, WORD wlen,BYTE byMode);

static BYTE AddPro_RecvHeartBeatDataFromManagementSystem    (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE AddPro_SendHeartBeatDataToManagementSystem      (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE AddPro_RecvDeviceSNDataFromManagementSystem     (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE AddPro_SendDeviceSNDataToManagementSystem       (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE AddPro_SendSensorDataToManagementSystem         (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE AddPro_RecvWriteDataCommandFromManagementSystem (BYTE *pData, WORD wlen,BYTE byMode);
static BYTE AddPro_RecvWriteDataCommandToManagementSystem   (BYTE *pData, WORD wlen,BYTE byMode);
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/
INETODParaTab  iNetODParaTab;
PROREADDATA    ReadDataTab;

const DB_PROATTRIBUTE  u_aNetCommMap[] =
{
//APPLY 申请
    {COMMUNICATION_PROTOCOLS_ID_APPLY_ProAssignedAddr,            RecvLocalAddrFromManagementSystem       }, //7.2.2.1 初始帧
    {COMMUNICATION_PROTOCOLS_ID_APPLY_ProTimingCheck,             RecvTimingCheckFromManagementSystem     }, //7.2.2.3 对时发起帧
    {COMMUNICATION_PROTOCOLS_ID_APPLY_ProTimingAck,               RecvTimingAckFromManagementSystem       }, //7.2.2.3 对时确认帧
    {COMMUNICATION_PROTOCOLS_ID_APPLY_ProHeartBeat,               RecvHeartBeatDataFromManagementSystem   }, //7.2.2.4 连线保持协议帧
    {COMMUNICATION_PROTOCOLS_ID_APPLY_ProVersion,                 RecvProtocolVersionFromManagementSystem }, //7.2.2.5 协议族版本校验帧
    {COMMUNICATION_PROTOCOLS_ID_APPLY_ProReadData,                RecvReadDataCommandFromManagementSystem }, //7.2.2.7 读数据帧
    {COMMUNICATION_PROTOCOLS_ID_APPLY_ProWriteData,               RecvWriteDataCommandFromManagementSystem}, //7.2.2.8 写数据帧
//REPLY 回复
    {COMMUNICATION_PROTOCOLS_ID_REPLY_ProAssignedAddr,            SendLocalAddrToManagementSystem         }, //7.2.2.1 初始帧
    {COMMUNICATION_PROTOCOLS_ID_REPLY_ProTimingCheck,             SendTimingCheckToManagementSystem       }, //7.2.2.3 对时回复帧
    {COMMUNICATION_PROTOCOLS_ID_REPLY_ProTimingAck,               SendTimingAckToManagementSystem         }, //7.2.2.3 对时确认帧
    {COMMUNICATION_PROTOCOLS_ID_REPLY_proHeartBeat,               SendHeartBeatDataToManagementSystem     }, //7.2.2.4 连线保持协议帧
    {COMMUNICATION_PROTOCOLS_ID_REPLY_ProVersion,                 SendProtocolVersionToManagementSystem   }, //7.2.2.5 协议版本校验帧
    {COMMUNICATION_PROTOCOLS_ID_REPLY_ProReadData,                SendReadDataToManagementSystem          }, //7.2.2.7 读数据帧
    {COMMUNICATION_PROTOCOLS_ID_REPLY_ProWriteData,               SendWriteDataToManagementSystem         }, //7.2.2.8 写数据帧
};

const DB_PROATTRIBUTE  u_aNetCommMap_Add[] =
{
//APPLY 申请
	{COMMUNICATION_ADDPROTOCOLS_ID_APPLY_ProHeartBeat,           AddPro_RecvHeartBeatDataFromManagementSystem    },   //06
	{COMMUNICATION_ADDPROTOCOLS_ID_APPLY_SendDeviceSN,           AddPro_RecvDeviceSNDataFromManagementSystem     },   //10
	{COMMUNICATION_ADDPROTOCOLS_ID_APPLY_SendSensorData,         NULL                                            },   //11
	{COMMUNICATION_ADDPROTOCOLS_ID_APPLY_WriteDataToSensor,      AddPro_RecvWriteDataCommandFromManagementSystem },   //0C
	{COMMUNICATION_ADDPROTOCOLS_ID_APPLY_AlarmData,              NULL                                            },   //12
//REPLY 回复
	{COMMUNICATION_ADDPROTOCOLS_ID_REPLY_ProHeartBeat,           AddPro_SendHeartBeatDataToManagementSystem      },   //06
	{COMMUNICATION_ADDPROTOCOLS_ID_REPLY_SendDeviceSN,           AddPro_SendDeviceSNDataToManagementSystem       },   //10
	{COMMUNICATION_ADDPROTOCOLS_ID_REPLY_SendSensorData,         AddPro_SendSensorDataToManagementSystem         },   //11
	{COMMUNICATION_ADDPROTOCOLS_ID_REPLY_WriteDataToSensor,      AddPro_RecvWriteDataCommandToManagementSystem   },   //0C
	{COMMUNICATION_ADDPROTOCOLS_ID_APPLY_AlarmData,              NULL                                            },   //12
};
/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
static BYTE  iNetFlashData[1024*5];
static DWORD u_ProVerBuf[3] = {Family_Protocol_Version,
                               Static_Protocol_Version,
                               Dynamic_Protocol_Version};

static DWORD u_dwTimePeriod  = 0;
static DWORD u_dwTimeoutTick = 0;
static DWORD u_dwLinkTick    = 0;
static BYTE  u_byWriteState  = 0;

iNetMODEMES    iNetModeMeg;
BOOTLOADMARK   BootloadMark;

static unsigned int _CRC32_Talbe[256];

HTASK 	g_htask_iNet;
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
static void  RuniNetAppFunctionType();
static void RuniNetAppIoTSever(void *vpParam);

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
void    CreateTaskiNet()
{
    TASK    Task;
    Task.wType = TASK_WHILELOOP;
    Task.period.tCounter = 0;
    Task.period.tTimer = 1;
    Task.pRun = RuniNetAppIoTSever;
    Task.pParam = 0;
    g_htask_iNet = Create_Task(&Task, 0);
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
static void RuniNetAppIoTSever(void *vpParam)
{
	WORD  wAIDNum = 0;

    if(iNetModeMeg.byMode == INETMODECLIENT) //设备作为客户端                 UDP Client      TCP Client
    {
        if(g_dwSystemTick - u_dwLinkTick > iNet_FlowLink_Timer)
        {
        	if(iNetModeMeg.byProVer == STANDARD_PRO)      //standby agreement
        	{
				if(!(BootloadMark.byNetVerMark)) {

					u_aNetCommMap[PROTOCOLID_ReplyProtocolVer].DealFunc(NULL, NULL, iNetModeMeg.byProtocolType);   //Send standard protocol 04 packets
				}

				if( (BootloadMark.byNetAddrMark == 0) && (BootloadMark.byNetVerMark == 0x55) ){

					u_aNetCommMap[PROTOCOLID_ReplyAssignedAddr].DealFunc(NULL, NULL, iNetModeMeg.byProtocolType);  //Send standard protocol 01 packets
				}
        	}
        	else if(iNetModeMeg.byProVer == ADDITIONAL_PRO)  //Additional agreement
        	{
        		u_aNetCommMap_Add[ADDPROTOCOLID_ReplyHeartBeat].DealFunc(NULL, NULL, iNetModeMeg.byProtocolType);  //Send heartbeat packet

				if((BootloadMark.byHeartBeatMark == 0x55) && (BootloadMark.byDeviceSNMark == 0))
				{
					u_aNetCommMap_Add[ADDPROTOCOLID_ReplySendDeviceSN].DealFunc(NULL, NULL,iNetModeMeg.byProtocolType);            //Send additional protocol 10 packet
				}

				if(BootloadMark.byDeviceSNMark == 0x55)
				{
					for(wAIDNum = 0; wAIDNum < BootloadMark.wDeviceNum; wAIDNum++)
						u_aNetCommMap_Add[ADDPROTOCOLID_ReplySendSensorData].DealFunc(NULL, wAIDNum, iNetModeMeg.byProtocolType);   //Send additional protocol 11 packets
				}
        	}
            u_dwLinkTick = g_dwSystemTick;
        }
        //掉线重新连接
        if(g_dwSystemTick - u_dwTimeoutTick > iNet_HBTimeout_Timer)
        {
            memset(&BootloadMark, 0, sizeof(BootloadMark));

            u_dwTimeoutTick = g_dwSystemTick;
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
void Read_iNetModeMeg_FromFlash()
{
	WORD wODNum;
	DWORD iNetODListSize = 0;

	memset(&iNetFlashData, 0, sizeof(iNetFlashData));

	Flash_Read((BYTE *)&wODNum, sizeof(wODNum), iNetData_Block, iNetODData_ofs);

	if(wODNum <= MAX_INETODLIST)
		iNetODListSize = wODNum* sizeof(iNetODPara) + sizeof(WORD)*2;

	Flash_Read((BYTE*)&iNetMessage,   sizeof(iNetMessage), iNetData_Block, iNetIPInfo_ofs );
	Flash_Read((BYTE*)&iNetModeMeg,   sizeof(iNetModeMeg), iNetData_Block, iNetPara_ofs   );
	Flash_Read((BYTE*)&iNetODParaTab, iNetODListSize,      iNetData_Block, iNetODData_ofs );

    if(iNetModeMeg.byMode == 0xFF)//未存储使用信息
    {
    	iNetModeMeg.byMode = INETMODENOTUSED;
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
void iNet_demo_senddata(BYTE* pData, WORD wLen, BYTE byMode)
{
    if(wLen <= 0)   return;

    if((u_Tcp_pcb != NULL) || (u_Udp_pcb_Add != NULL))
    {
        if(byMode == INETPROTYPETCP)
            Lwip_TcpWrite(u_Tcp_pcb, pData, wLen);
        if(byMode == INETPROTYPEUDP)
            Lwip_UdpWrite(u_Udp_pcb_Add,pData,wLen);
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
BYTE iNetParseRecvData(BYTE *pData, int nlength, BYTE byMode)
{
    int i;
    BYTE byNET_Back;
    FRAMEHEADER FrameHeader;

    if(nlength < sizeof(FrameHeader)+4)   return 1;
    memcpy(&FrameHeader, pData, sizeof(FrameHeader));

    if(FrameHeader.byControllerID[0] ==  g_byMacAdderBuf[0] &&
       FrameHeader.byControllerID[1] ==  g_byMacAdderBuf[1] &&
       FrameHeader.byControllerID[2] ==  g_byMacAdderBuf[2] &&
       FrameHeader.byControllerID[3] ==  g_byMacAdderBuf[3] &&
       FrameHeader.byControllerID[4] ==  g_byMacAdderBuf[4] &&
       FrameHeader.byControllerID[5] ==  g_byMacAdderBuf[5]){
        for(i = 0; i < PROTOCOLID_DATA_NUM; i++){
        	if(iNetModeMeg.byProVer == STANDARD_PRO)      //parsing  standby agreement
        	{
				if(u_aNetCommMap[i].dwID == FrameHeader.dwProtocolID){
					if(FrameHeader.dwProtocolID == COMMUNICATION_PROTOCOLS_ID_APPLY_ProVersion){
						u_aNetCommMap[i].DealFunc(pData, nlength, byMode);
					}
					else if(FrameHeader.dwProtocolID == COMMUNICATION_PROTOCOLS_ID_APPLY_ProAssignedAddr)
						u_aNetCommMap[i].DealFunc(pData, nlength, byMode);
					else{
						byNET_Back  = u_aNetCommMap[i].DealFunc(pData, nlength, byMode);
						return byNET_Back;
					}
				}
        	}
        	if(iNetModeMeg.byProVer == ADDITIONAL_PRO)      //parsing  Additional  agreement
        	{
				if(u_aNetCommMap_Add[i].dwID == FrameHeader.dwProtocolID){
						byNET_Back  = u_aNetCommMap_Add[i].DealFunc(pData, nlength, byMode);
					return byNET_Back;
				}
			}
        }
    }
    return 1;
}

/*---------------------------------------------------------------------------+
|  Function :    InitStaticProtrolHeader                                     |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
static FRAMEHEADER InitStaticProtrolHeader(
                              WORD   wProtocolLen,
                              BYTE   byProtocolflag,
                              DWORD  dwProtocolID)
{
    int i;
    FRAMEHEADER FrameHeader;
    static  DWORD dwHeartStamp = 0;

    memset(u_iNetSendBuf, 0, sizeof(u_iNetSendBuf));

    FrameHeader.wLength = wProtocolLen;
    FrameHeader.wVersion = u_ProVerBuf[0];
    FrameHeader.byPacketStamp = dwHeartStamp++;
    if(byProtocolflag) FrameHeader.byFlag = 0x40;  //need feedback
    else FrameHeader.byFlag = 0;                   //no need feedback
    FrameHeader.dwProtocolID = dwProtocolID;
    for(i = 0; i < 6; i++)
        FrameHeader.byControllerID[i] = g_byMacAdderBuf[i];
    return FrameHeader;
}

/*************************************************************************************************************************************************/
/************************************************************standby agreement *******************************************************************/
/*************************************************************************************************************************************************/
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
static BYTE RecvProtocolVersionFromManagementSystem(BYTE * pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    DWORD Recv_Sta_Ver,Recv_Dyna_Ver,Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);

    memcpy(&FrameHeader,   pData, sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(&Recv_Sta_Ver,  pData, sizeof(DWORD));
    pData += sizeof(DWORD);
    memcpy(&Recv_Dyna_Ver, pData, sizeof(DWORD));

    if(Recv_CRC_Val == Calc_CRC_Val){
        if(FrameHeader.wVersion != htons(Family_Protocol_Version)){
            if(Recv_Sta_Ver != htonl(Static_Protocol_Version)){
                u_ProVerBuf[1] = Recv_Sta_Ver;
                return INET_ERROR_ID_VERSION;
            }
            u_ProVerBuf[0] = FrameHeader.wVersion;
            return INET_ERROR_ID_VERSION;
        }
        BootloadMark.byNetVerMark = 0x55;
        return INET_OK;
    }
    return INET_ERROR_ID_CRC;
}
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                            -                                   |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
static BYTE RecvLocalAddrFromManagementSystem(BYTE * pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    BYTE byNewLocalIP[4];
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);

    memcpy(&FrameHeader,   pData, sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(byNewLocalIP,   pData, sizeof(DWORD));

    if(Calc_CRC_Val == Recv_CRC_Val);  //可在此修改IP,做重启操作
    	BootloadMark.byNetAddrMark = 0x55;
    return INET_OK;
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
static BYTE RecvTimingCheckFromManagementSystem  (BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    BYTE byTimingBuf[8];  //年WORD 月BYTE 日BYTE 时BYTE 分BYTE 毫秒WORD
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);

    memcpy(&FrameHeader,  pData, sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(byTimingBuf,   pData, sizeof(byTimingBuf));

    if(Calc_CRC_Val == Recv_CRC_Val)
    {
        if((FrameHeader.byFlag & 0xFF) >> 6){
            u_aNetCommMap[PROTOCOLID_ReplyTimingCheck].DealFunc(pData, wlen, byMode);
            return INET_OK;
        }
        return INET_ERROR_ID_PROFLAG;
    }
    return INET_OK;
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
static BYTE RecvTimingAckFromManagementSystem  (BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    BYTE byTimingBuf[8];  //年WORD 月BYTE 日BYTE 时BYTE 分BYTE 毫秒WORD
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);

    memcpy(&FrameHeader,  pData, sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(byTimingBuf,   pData, sizeof(byTimingBuf));

    if(Calc_CRC_Val == Recv_CRC_Val)
    {
        if((FrameHeader.byFlag & 0xFF) >> 6){
            u_aNetCommMap[PROTOCOLID_ReplyTimingAck].DealFunc(pData, wlen, byMode);
            return INET_OK;
        }
        return INET_ERROR_ID_PROFLAG;
    }
    return INET_OK;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE RecvHeartBeatDataFromManagementSystem(BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);

    memcpy(&FrameHeader,    pData,  sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(&u_dwTimePeriod, pData,  sizeof(DWORD));

    if(Calc_CRC_Val == Recv_CRC_Val){
        u_dwTimeoutTick = g_dwSystemTick;//心跳包计时再此复位
        if((FrameHeader.byFlag & 0xFF) >> 6){
        		u_aNetCommMap[PROTOCOLID_ReplyHeartBeat].DealFunc(pData, wlen, byMode);
            return INET_OK;
        }
        return INET_ERROR_ID_PROFLAG;
    }
    return INET_ERROR_ID_CRC;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE RecvReadDataCommandFromManagementSystem(BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);


    if(Calc_CRC_Val == Recv_CRC_Val)
    {
        memcpy(&FrameHeader,   pData,  sizeof(FrameHeader));
        if((FrameHeader.byFlag & 0xFF) >> 6){
            u_aNetCommMap[PROTOCOLID_ReplyReadData].DealFunc(pData, wlen, byMode);
            return INET_OK;
        }
        return INET_ERROR_ID_PROFLAG;
    }
    return INET_OK;
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
static BYTE RecvWriteDataCommandFromManagementSystem  (BYTE *pData, WORD wlen, BYTE byMode)
{
    int i,j;
    FRAMEHEADER FrameHeader;
    WORD  wCount;
    DWORD Data_ID;
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);

    memcpy(&FrameHeader,    pData,  sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(&wCount,         pData,  sizeof(WORD));
    pData += sizeof(WORD);

    if(Calc_CRC_Val == Recv_CRC_Val){
        if((wCount > 0) && (wCount < iNetODParaTab.wODNum))
        {
            for(i = 0; i< wCount; i++)
            {
                memcpy(&Data_ID,  pData,  sizeof(WORD));
                for(j = 0; j < iNetODParaTab.wODNum; j++)
                {
                    if(iNetODParaTab.pODList[j].dwiNetDataID = Data_ID)
                    {
                        if((iNetODParaTab.pODList[j].byiNetPermission == RW) ||
                                        (iNetODParaTab.pODList[j].byiNetPermission == WO))
                        {
                            pData += sizeof(WORD);
                            memcpy(&iNetODParaTab.pODList[j].wiNetDataType, pData, sizeof(WORD));
                            pData += sizeof(WORD);
                            memcpy(&iNetODParaTab.pODList[j].wiNetUom, pData, sizeof(WORD));
                            pData += sizeof(WORD);
                            pData += sizeof(WORD);

                            switch(iNetODParaTab.pODList[j].wiNetDataType)
                            {
                                case DATATYPE_Byte:
                                    memcpy(&iNetODParaTab.pODList[j].Value.byData, pData, sizeof(BYTE));
                                    pData += sizeof(BYTE);
                                    break;
                                case DATATYPE_WORD:
                                    memcpy(&iNetODParaTab.pODList[j].Value.wData,  pData, sizeof(WORD));
                                    pData += sizeof(WORD);
                                    break;
                                case DATATYPE_DWORD:
                                    memcpy(&iNetODParaTab.pODList[j].Value.dwData, pData, sizeof(DWORD));
                                    pData += sizeof(DWORD);
                                    break;
                                case DATATYPE_Int16:
                                    memcpy(&iNetODParaTab.pODList[j].Value.sData,  pData, sizeof(short int));
                                    pData += sizeof(long int);
                                    break;
                                case DATATYPE_Int32:
                                    memcpy(&iNetODParaTab.pODList[j].Value.lData,  pData, sizeof(long int));
                                    pData += sizeof(long);
                                    break;
                                case DATATYPE_Float:
                                    memcpy(&iNetODParaTab.pODList[j].Value.fData,  pData, sizeof(float));
                                    pData += sizeof(float);
                                    break;
                                default:
                                    break;
                            }
                        }
                        else  u_byWriteState = 4;//无操作权限   State = 4

                    }
                    else  u_byWriteState = 4;//无操作权限   State = 4
                }
            }
            if((FrameHeader.byFlag & 0xFF) >> 6){
                u_byWriteState = 0;//成功 State = 0
                u_aNetCommMap[PROTOCOLID_ReplyWriteData].DealFunc(pData, wlen, byMode);
                return INET_OK;
            }
        }
        else
        {
            if(wCount == 0) u_byWriteState = 2;  //超过下限 State = 2
            else if(wCount > iNetODParaTab.wODNum) u_byWriteState = 1; //超过下限 State = 1
            u_aNetCommMap[PROTOCOLID_ReplyWriteData].DealFunc(pData, wlen, byMode);
        }
    }
    else   //数据在操作中，需要监控/管理系统再次下达  State = 16
    {
        u_byWriteState = 16;
        u_aNetCommMap[PROTOCOLID_ReplyWriteData].DealFunc(pData, wlen, byMode);
    }

    return 0;
}
/*---------------------------------------------------------------------------+
|  Function : RsCreateTaskLwipTcp()                                          |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
static BYTE SendProtocolVersionToManagementSystem(BYTE* pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    DWORD  CRC_Check = 0;
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;
    u_ProVerBuf[1] = htonl(Static_Protocol_Version);
    u_ProVerBuf[2] = htonl(Dynamic_Protocol_Version);

    pTemp = pTempBuf;
//Fill Protocol Content + CRC
    pTempBuf += sizeof(FrameHeader);

    memcpy(pTempBuf, &u_ProVerBuf[1], sizeof(DWORD));
    pTempBuf += sizeof(DWORD);

    memcpy(pTempBuf, &u_ProVerBuf[2], sizeof(DWORD));
    pTempBuf += sizeof(DWORD);
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                      Need_FeedBack,
                                      COMMUNICATION_PROTOCOLS_ID_REPLY_ProVersion);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf- pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);

//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
}
/*---------------------------------------------------------------------------+
|  Function : RsCreateTaskLwipTcp()                                          |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
static BYTE SendLocalAddrToManagementSystem(BYTE* pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER  FrameHeader;
    DWORD  CRC_Check = 0;
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;

    pTemp = pTempBuf;
//Fill Protocol Content + CRC
    pTempBuf += sizeof(FrameHeader);

    memcpy(pTempBuf, 0, sizeof(BYTE));   //Assign Address
    pTempBuf += sizeof(BYTE);

    memcpy(pTempBuf, 0, sizeof(DWORD));  //Address
    pTempBuf += sizeof(DWORD);

    memcpy(pTempBuf, 0, sizeof(BYTE));   //Character Set Encoding
    pTempBuf += sizeof(BYTE);
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                Need_FeedBack,
                                COMMUNICATION_PROTOCOLS_ID_REPLY_ProAssignedAddr);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);
//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
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
static BYTE SendTimingCheckToManagementSystem  (BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER  FrameHeader;
    DWORD  CRC_Check = 0;
    BYTE byTimingBuf[8] = {0};
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;

    pTemp = pTempBuf;
//Fill Protocol Content + CRC
    pTempBuf += sizeof(FrameHeader);

    memcpy(pTempBuf, byTimingBuf, sizeof(byTimingBuf));   //年WORD 月BYTE 日BYTE 时BYTE 分BYTE 毫秒WORD
    pTempBuf += sizeof(byTimingBuf);
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                Need_FeedBack,
                                COMMUNICATION_PROTOCOLS_ID_REPLY_ProTimingCheck);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);

//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
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
static BYTE SendTimingAckToManagementSystem  (BYTE *pData, WORD wlen,BYTE byMode)
{
    FRAMEHEADER  FrameHeader;
    DWORD  CRC_Check = 0;
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;

    pTemp = pTempBuf;
//Fill Protocol Content + CRC
    pTempBuf += sizeof(FrameHeader);

    memcpy(pTempBuf, 0, sizeof(BYTE));   //Synchroize Flag
    pTempBuf += sizeof(BYTE);
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                NoNeed_FeedBack,
                                COMMUNICATION_PROTOCOLS_ID_REPLY_ProTimingCheck);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);

//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE SendHeartBeatDataToManagementSystem(BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER  FrameHeader;
    DWORD  CRC_Check = 0;
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;

    pTemp = pTempBuf;
//Fill Protocol Content
    pTempBuf += sizeof(FrameHeader);

    memcpy(pTempBuf, &u_dwTimePeriod, sizeof(DWORD));   //Time
    pTempBuf += sizeof(DWORD);
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                NoNeed_FeedBack,
                                COMMUNICATION_PROTOCOLS_ID_REPLY_proHeartBeat);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);
//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
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
static BYTE SendReadDataToManagementSystem  (BYTE *pData, WORD wlen, BYTE byMode)
{
    int i,j;

    FRAMEHEADER  FrameHeader;
    FRAMEHEADER  RecvFrameHeader;
    PROCONTENT   ProContent;
    DWORD  CRC_Check = 0,dwDataID;
    WORD   wDataIDCount;
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;
    BYTE *pDataMark = NULL;

    memcpy(&RecvFrameHeader, pData, sizeof(RecvFrameHeader));  //接收帧帧头
    pData += sizeof(RecvFrameHeader);

    memcpy(&wDataIDCount, pData, sizeof(WORD));   //接收到DataID个数
    pData += sizeof(WORD);
    pDataMark = pData;   //标记DataID初始位置

    pTemp = pTempBuf;    //标记发送Buf初始位置
//Fill Protocol Content
    pTempBuf += sizeof(FrameHeader); //跳过回复帧帧头赋值

    ProContent.byFlag = 0;
    memcpy(pTempBuf, &ProContent.byFlag, sizeof(BYTE));
    pTempBuf += sizeof(BYTE);

    ProContent.wDataIDCount = wDataIDCount;
    memcpy(pTempBuf, &ProContent.wDataIDCount, sizeof(WORD));
    pTempBuf += sizeof(WORD);

    for(i = 0; i < wDataIDCount; i++)
    {
        memcpy(&dwDataID, pData, sizeof(DWORD));   //接收到的DataID,每循环一次，后移一个DWORD
        pData += sizeof(DWORD);
        ProContent.dwDataID = dwDataID;
        for(j = 0; j < iNetODParaTab.wODNum; j++)
        {
            if(iNetODParaTab.pODList[j].dwiNetDataID = dwDataID)
            {
                if((iNetODParaTab.pODList[j].byiNetPermission == RW) ||
                                    (iNetODParaTab.pODList[j].byiNetPermission == RO))
                {
                    ProContent.wDataType = iNetODParaTab.pODList[j].wiNetDataType;
                    memcpy(pTempBuf, &ProContent.wDataType, sizeof(WORD));
                    pTempBuf += sizeof(WORD);

                    ProContent.wUOM = iNetODParaTab.pODList[j].wiNetUom;
                    memcpy(pTempBuf, &ProContent.wUOM, sizeof(WORD));
                    pTempBuf += sizeof(WORD);

                    ProContent.byPrecision = iNetODParaTab.pODList[j].byiNetPrecision;
                    memcpy(pTempBuf, &ProContent.byPrecision, sizeof(BYTE));
                    pTempBuf += sizeof(BYTE);
                }
            }
        }
    }

    ProContent.wRowCount = wDataIDCount;
    memcpy(pTempBuf, &ProContent.wRowCount, sizeof(WORD));
    pTempBuf += sizeof(WORD);

    pData = pDataMark;
    for(i = 0; i < wDataIDCount; i++)
    {
        memcpy(&dwDataID, pData, sizeof(DWORD));   //接收到的DataID,每循环一次，后移一个DWORD
        pData += sizeof(DWORD);
        ProContent.dwDataID = dwDataID;
        for(j = 0; j < iNetODParaTab.wODNum; j++)
        {
            if(iNetODParaTab.pODList[j].dwiNetDataID = dwDataID)
            {
                if((iNetODParaTab.pODList[j].byiNetPermission == RW) ||
                                    (iNetODParaTab.pODList[j].byiNetPermission == RO))
                {
                    ProContent.wRowDataLen = sizeof(ProContent.wDataType);
                    memcpy(pTempBuf, &ProContent.wRowDataLen, sizeof(WORD));
                    pTempBuf += sizeof(WORD);

                    switch(iNetODParaTab.pODList[j].wiNetDataType)
                    {
                        case DATATYPE_Byte:
                            ProContent.Value.byData = iNetODParaTab.pODList[j].Value.byData;
                            memcpy(pTempBuf, &ProContent.Value.byData, sizeof(BYTE));
                            pTempBuf += sizeof(BYTE);
                            break;
                        case DATATYPE_WORD:
                            ProContent.Value.wData = iNetODParaTab.pODList[j].Value.wData;
                            memcpy(pTempBuf, &ProContent.Value.wData, sizeof(WORD));
                            pTempBuf += sizeof(WORD);
                            break;
                        case DATATYPE_DWORD:
                            ProContent.Value.dwData = iNetODParaTab.pODList[j].Value.dwData;
                            memcpy(pTempBuf, &ProContent.Value.dwData, sizeof(DWORD));
                            pTempBuf += sizeof(DWORD);
                            break;
                        case DATATYPE_Int16:
                            ProContent.Value.sData = iNetODParaTab.pODList[j].Value.sData;
                            memcpy(pTempBuf, &ProContent.Value.sData, sizeof(short int));
                            pTempBuf += sizeof(short int);
                            break;
                        case DATATYPE_Int32:
                            ProContent.Value.lData = iNetODParaTab.pODList[j].Value.lData;
                            memcpy(pTempBuf, &ProContent.Value.lData, sizeof(long int));
                            pTempBuf += sizeof(long int);
                            break;
                        case DATATYPE_Float:
                            ProContent.Value.fData = iNetODParaTab.pODList[j].Value.fData;
                            memcpy(pTempBuf, &ProContent.Value.fData, sizeof(float));
                            pTempBuf += sizeof(float);
                            break;
                        default:
                            ProContent.Value.dwData = iNetODParaTab.pODList[j].Value.dwData;
                            memcpy(pTempBuf, &ProContent.Value.dwData, sizeof(DWORD));
                            pTempBuf += sizeof(DWORD);
                            break;
                    }
                }
            }
        }
    }
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                NoNeed_FeedBack,
                                COMMUNICATION_PROTOCOLS_ID_REPLY_ProReadData);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);
//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
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
static BYTE SendWriteDataToManagementSystem  (BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER  FrameHeader;
    DWORD  CRC_Check = 0;
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;

    pTemp = pTempBuf;
//Fill Protocol Content
    pTempBuf += sizeof(FrameHeader);

    memcpy(pTempBuf, pData+sizeof(FrameHeader), sizeof(WORD));   //资料个数  Count
    pTempBuf += sizeof(WORD);

    memcpy(pTempBuf, &u_byWriteState, sizeof(BYTE));
    pTempBuf += sizeof(BYTE);
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                NoNeed_FeedBack,
                                COMMUNICATION_PROTOCOLS_ID_REPLY_ProWriteData);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);
//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
}
/****************************************************************************************************************************************************/
/************************************************************Additional agreement *******************************************************************/
/****************************************************************************************************************************************************/
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE AddPro_RecvHeartBeatDataFromManagementSystem(BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-4);

    memcpy(&FrameHeader,    pData,  sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(&u_dwTimePeriod, pData,  sizeof(DWORD));

    if(Calc_CRC_Val == Recv_CRC_Val){
        u_dwTimeoutTick = g_dwSystemTick;//心跳包计时再此复位
        if((FrameHeader.byFlag & 0xFF) >> 6){
        		BootloadMark.byHeartBeatMark = 0x55;
            return INET_OK;
        }
        return INET_ERROR_ID_PROFLAG;
    }
    return INET_ERROR_ID_CRC;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE AddPro_SendHeartBeatDataToManagementSystem(BYTE *pData, WORD wlen, BYTE byMode)
{
    FRAMEHEADER  FrameHeader;
    WORD   wProLength   =  0;
    DWORD  CRC_Check    =  0;
    DWORD  dwTimePeriod =  iNet_FlowLink_Timer;
    BYTE   *pTempBuf    =  u_iNetSendBuf;
    BYTE   *pTemp       =  NULL;

    pTemp = pTempBuf;
//Fill Protocol Content
    FrameHeader = InitStaticProtrolHeader(sizeof(FrameHeader)+sizeof(DWORD),
                                Need_FeedBack,
								COMMUNICATION_ADDPROTOCOLS_ID_REPLY_ProHeartBeat);
    memcpy(pTempBuf, (BYTE *)&FrameHeader, sizeof(FrameHeader));
    pTempBuf += sizeof(FrameHeader);

//Fill FrameHeader
    memcpy(pTempBuf, &dwTimePeriod, sizeof(DWORD));   //Time
    pTempBuf += sizeof(DWORD);

    wProLength = pTempBuf - pTemp + sizeof(DWORD);
    memcpy(pTemp, (BYTE *)&wProLength, sizeof(WORD));

//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);

//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, sizeof(FrameHeader)+sizeof(DWORD)*2, byMode);

    return INET_OK;
}

/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE AddPro_RecvDeviceSNDataFromManagementSystem  (BYTE *pData, WORD wlen,BYTE byMode)
{
    FRAMEHEADER FrameHeader;
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-4);

    memcpy(&FrameHeader,    pData,  sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(&u_dwTimePeriod, pData,  sizeof(DWORD));

    if(Calc_CRC_Val == Recv_CRC_Val)
    	BootloadMark.byDeviceSNMark = 0x55;

	return INET_OK;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE AddPro_SendDeviceSNDataToManagementSystem  (BYTE *pData, WORD wlen,BYTE byMode)
{
static BYTE byDeviceInfo[33] = {0x01,                                        //Flag
		                        0x03, 0x00,                                  //Count
		                        0x00, 0x01, 0x00, 0xFF, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00,  //Device SN   +  Attribute  +  DataType  +  UOM  +  byPrecision
						        0x00, 0x02, 0x00, 0xFF, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00,  //SoftwareSN  +  Attribute  +  DataType  +  UOM  +  byPrecision
							    0x00, 0x03, 0x00, 0xFF, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00,  //Device Type +  Attribute  +  DataType  +  UOM  +  byPrecision
						        };
static DWORD dwDeviceAIDInfo[2] = {0, 0x0010000};  //00 00 00 00 00 00 01 00

	FRAMEHEADER FrameHeader;
	BYTE     byInitFlag = 0;
	WORD     i,wRowCount = 0;
	DWORD    CRC_Check, wProLength;
	BYTE_T*  pTemBuf  =  u_iNetSendBuf;
	BYTE_T*  pTemp    =  NULL;
	BYTE_T*  pRowCount=  NULL;

    pTemp = pTemBuf;

//Fill Frame Header
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(FrameHeader));

//Fill Protocol Content
    memcpy((BYTE_T *)pTemBuf, (BYTE *)&byDeviceInfo, sizeof(byDeviceInfo));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(byDeviceInfo));

    for(i = 0; i < iNetODParaTab.wODNum; i++)
    {
    	if(iNetODParaTab.pODList[i].dwDeviceEntityAID != dwDeviceAIDInfo[0])
    	{
    		wRowCount = wRowCount + 1;
    		dwDeviceAIDInfo[0] = iNetODParaTab.pODList[i].dwDeviceEntityAID;

    		if(wRowCount > MAX_DEVICEAID)    return 0;
    			dwDeviceEntityAID[wRowCount-1] = iNetODParaTab.pODList[i].dwDeviceEntityAID;   //Record device entity AID

    	    if(byInitFlag == 0){
    	    	byInitFlag = 0x55;
    	    	pRowCount = pTemBuf;
    	    	pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD));
    	    }
    	    memcpy((BYTE_T *)pRowCount, (BYTE *)&wRowCount, sizeof(WORD));

    	    memcpy((BYTE_T *)pTemBuf, (BYTE *)&dwDeviceAIDInfo, sizeof(dwDeviceAIDInfo));
    	    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(dwDeviceAIDInfo));
    	}
    }
    BootloadMark.wDeviceNum = wRowCount;
//Fill FrameHeader
    wProLength  = pTemBuf - pTemp + sizeof(DWORD_T);
    FrameHeader = InitStaticProtrolHeader(wProLength,    Need_FeedBack,
							     COMMUNICATION_ADDPROTOCOLS_ID_REPLY_SendDeviceSN);
	memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));

    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)u_iNetSendBuf, wProLength-4);
    memcpy((BYTE_T *)pTemBuf,&CRC_Check,sizeof(DWORD_T));

//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, wProLength, byMode);

    return INET_OK;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE AddPro_SendSensorDataToManagementSystem  (BYTE *pData, WORD wlen,BYTE byMode)
{
    WORD       i;
    DWORD      CRC_Check, wProLength;

static  BYTE   byDeviceInfo[10] = {
		  	  	  	  	  	  	  0x00, 0x01, 0x00, 0xFF, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00
								  };
    WORD       wAIDNum = wlen;
    WORD       wDataIDNum  = 0;
    BYTE       byAttribute = 0x01;
    BYTE       byFlag      = 0x01;
    WORD       wRowCount   = 0x1;
    FRAMEHEADER FrameHeader;
    BYTE_T*    pTemBuf = u_iNetSendBuf;
    BYTE_T*    pTemp   = NULL;
    WORD_T*    pIDCount= NULL;

    pTemp = pTemBuf;

//Fill Frame Header
    FrameHeader = InitStaticProtrolHeader(sizeof(FrameHeader)+sizeof(BYTE),
                                          NoNeed_FeedBack,
										  COMMUNICATION_ADDPROTOCOLS_ID_REPLY_SendSensorData);
    memcpy((BYTE_T *)pTemBuf, (BYTE *)&FrameHeader, sizeof(FrameHeader));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(FrameHeader));

//Fill Protocol Content
    memcpy((BYTE_T *)pTemBuf, (BYTE *)&byFlag, sizeof(BYTE));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE));

    pIDCount = (WORD_T *)pTemBuf;
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD));

    memcpy((BYTE_T *)pTemBuf, &byDeviceInfo, sizeof(byDeviceInfo));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(byDeviceInfo));
    for(i = 0; i < iNetODParaTab.wODNum; i++)
    {
    	if(dwDeviceEntityAID[wAIDNum] == iNetODParaTab.pODList[i].dwDeviceEntityAID)
    	{
    		wDataIDNum ++;

		    memcpy((BYTE_T *)pTemBuf, &iNetODParaTab.pODList[i].dwiNetDataID, sizeof(DWORD));
		    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD));

		    memcpy((BYTE_T *)pTemBuf, &byAttribute, sizeof(BYTE));
		    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE));

		    memcpy((BYTE_T *)pTemBuf, &iNetODParaTab.pODList[i].wiNetDataType, sizeof(WORD));
		    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD));

		    memcpy((BYTE_T *)pTemBuf, &iNetODParaTab.pODList[i].wiNetUom, sizeof(WORD));
		    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD));

		    memcpy((BYTE_T *)pTemBuf, &iNetODParaTab.pODList[i].byiNetPrecision, sizeof(BYTE));
		    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE));
    	}
    }

    memcpy((BYTE_T *)pIDCount, &wDataIDNum+1,    sizeof(WORD));
    memcpy((BYTE_T *)pTemBuf , &wRowCount,     sizeof(WORD));
    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD));

    for(i = 0; i < iNetODParaTab.wODNum; i++)
    {
    	if(dwDeviceEntityAID[wAIDNum] == iNetODParaTab.pODList[i].dwDeviceEntityAID)
    	{
            memcpy(pTemBuf, &iNetODParaTab.pODList[i].dwDeviceEntityAID, sizeof(DWORD));
            pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD));

            switch(iNetODParaTab.pODList[i].wiNetDataType)
            {
                case DATATYPE_Byte:
                    memcpy(pTemBuf, &iNetODParaTab.pODList[i].Value.byData, sizeof(BYTE));
                    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(BYTE));
                    break;
                case DATATYPE_WORD:
                    memcpy(pTemBuf, &iNetODParaTab.pODList[i].Value.wData, sizeof(WORD));
                    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(WORD));
                    break;
                case DATATYPE_DWORD:
                    memcpy(pTemBuf, &iNetODParaTab.pODList[i].Value.dwData, sizeof(DWORD));
                    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD));
                    break;
                case DATATYPE_Int16:
                    memcpy(pTemBuf, &iNetODParaTab.pODList[i].Value.sData, sizeof(short int));
                    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(short int));
                    break;
                case DATATYPE_Int32:
                    memcpy(pTemBuf, &iNetODParaTab.pODList[i].Value.lData, sizeof(long int));
                    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(long int));
                    break;
                case DATATYPE_Float:
                    memcpy(pTemBuf, &iNetODParaTab.pODList[i].Value.fData, sizeof(float));
                    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(float));
                    break;
                default:
                    memcpy(pTemBuf, &iNetODParaTab.pODList[i].Value.dwData, sizeof(DWORD));
                    pTemBuf =(BYTE_T *) (((BYTE_T *)pTemBuf) + sizeof(DWORD));
                    break;
            }
    	}
    }
    wProLength = pTemBuf - pTemp + sizeof(DWORD_T);
    memcpy(u_iNetSendBuf, &wProLength,sizeof(WORD_T));

    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)u_iNetSendBuf, (wProLength-4));
    memcpy((BYTE_T *)pTemBuf,&CRC_Check,sizeof(DWORD_T));

//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, wProLength, byMode);

    return INET_OK;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE AddPro_RecvWriteDataCommandFromManagementSystem (BYTE *pData, WORD wlen,BYTE byMode)
{
    int i,j;
    FRAMEHEADER FrameHeader;
    WORD  wCount;
    DWORD Data_ID;
    DWORD Recv_CRC_Val,Calc_CRC_Val;

    memcpy(&Recv_CRC_Val,   pData+wlen-4,  sizeof(DWORD));

    CreateCRC32Table();
    Calc_CRC_Val = GetCRC32((char *)pData, wlen-2);

    memcpy(&FrameHeader,    pData,  sizeof(FrameHeader));
    pData += sizeof(FrameHeader);
    memcpy(&wCount,         pData,  sizeof(WORD));
    pData += sizeof(WORD);

    if(Calc_CRC_Val == Recv_CRC_Val){
        if((wCount > 0) && (wCount < iNetODParaTab.wODNum))
        {
            for(i = 0; i< wCount; i++)
            {
                memcpy(&Data_ID,  pData,  sizeof(WORD));
                for(j = 0; j < iNetODParaTab.wODNum; j++)
                {
                    if(iNetODParaTab.pODList[j].dwiNetDataID = Data_ID)
                    {
                        if((iNetODParaTab.pODList[j].byiNetPermission == RW) ||
                                        (iNetODParaTab.pODList[j].byiNetPermission == WO))
                        {
                            pData += sizeof(WORD);
                            memcpy(&iNetODParaTab.pODList[j].wiNetDataType, pData, sizeof(WORD));
                            pData += sizeof(WORD);
                            memcpy(&iNetODParaTab.pODList[j].wiNetUom, pData, sizeof(WORD));
                            pData += sizeof(WORD);
                            pData += sizeof(WORD);

                            switch(iNetODParaTab.pODList[j].wiNetDataType)
                            {
                                case DATATYPE_Byte:
                                    memcpy(&iNetODParaTab.pODList[j].Value.byData, pData, sizeof(BYTE));
                                    pData += sizeof(BYTE);
                                    break;
                                case DATATYPE_WORD:
                                    memcpy(&iNetODParaTab.pODList[j].Value.wData,  pData, sizeof(WORD));
                                    pData += sizeof(WORD);
                                    break;
                                case DATATYPE_DWORD:
                                    memcpy(&iNetODParaTab.pODList[j].Value.dwData, pData, sizeof(DWORD));
                                    pData += sizeof(DWORD);
                                    break;
                                case DATATYPE_Int16:
                                    memcpy(&iNetODParaTab.pODList[j].Value.sData,  pData, sizeof(short int));
                                    pData += sizeof(long int);
                                    break;
                                case DATATYPE_Int32:
                                    memcpy(&iNetODParaTab.pODList[j].Value.lData,  pData, sizeof(long int));
                                    pData += sizeof(long);
                                    break;
                                case DATATYPE_Float:
                                    memcpy(&iNetODParaTab.pODList[j].Value.fData,  pData, sizeof(float));
                                    pData += sizeof(float);
                                    break;
                                default:
                                    break;
                            }
                        }
                        else  u_byWriteState = 4;//无操作权限   State = 4

                    }
                    else  u_byWriteState = 4;//无操作权限   State = 4
                }
            }
            if((FrameHeader.byFlag & 0xFF) >> 6){
                u_byWriteState = 0;//成功 State = 0
                u_aNetCommMap[PROTOCOLID_ReplyWriteData].DealFunc(pData, wlen, byMode);
                return INET_OK;
            }
        }
        else
        {
            if(wCount == 0) u_byWriteState = 2;  //超过下限 State = 2
            else if(wCount > iNetODParaTab.wODNum) u_byWriteState = 1; //超过下限 State = 1
            u_aNetCommMap[PROTOCOLID_ReplyWriteData].DealFunc(pData, wlen, byMode);
        }
    }
    else   //数据在操作中，需要监控/管理系统再次下达  State = 16
    {
        u_byWriteState = 16;
        u_aNetCommMap[PROTOCOLID_ReplyWriteData].DealFunc(pData, wlen, byMode);
    }

    return 0;
}
/*--------------------------------------------------------------------------+
|  Function :                                                               |
|  Task     :                                                               |
+---------------------------------------------------------------------------+
|  Call     :                                                               |
|                                                                           |
|  Parameter:                           -                                   |
|                                                                           |
|  Return   :                           -                                   |
+--------------------------------------------------------------------------*/
static BYTE AddPro_RecvWriteDataCommandToManagementSystem   (BYTE *pData, WORD wlen,BYTE byMode)
{
    FRAMEHEADER  FrameHeader;
    DWORD  CRC_Check = 0;
    BYTE *pTempBuf = u_iNetSendBuf;
    BYTE *pTemp = NULL;

    pTemp = pTempBuf;
//Fill Protocol Content
    pTempBuf += sizeof(FrameHeader);

    memcpy(pTempBuf, pData+sizeof(FrameHeader), sizeof(WORD));   //资料个数  Count
    pTempBuf += sizeof(WORD);

    memcpy(pTempBuf, &u_byWriteState, sizeof(BYTE));
    pTempBuf += sizeof(BYTE);
//Fill FrameHeader
    FrameHeader = InitStaticProtrolHeader(pTempBuf-pTemp+sizeof(DWORD),
                                NoNeed_FeedBack,
                                COMMUNICATION_PROTOCOLS_ID_REPLY_ProWriteData);
    memcpy(pTemp, (BYTE *)&FrameHeader, sizeof(FrameHeader));
//Fill CRC
    CreateCRC32Table();
    CRC_Check = GetCRC32((char *)pTemp, pTempBuf-pTemp);
    memcpy(pTempBuf, &CRC_Check, sizeof(DWORD));
    pTempBuf += sizeof(DWORD);
//send data from u_iNetSendBuf
    iNet_demo_senddata(u_iNetSendBuf, pTempBuf-pTemp, byMode);

    return INET_OK;
}
/*****************************************************************************************************************************************/
/************************************************************tmIoT Studio ****************************************************************/
/*****************************************************************************************************************************************/
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
BYTE iNetDealRecvParm(BYTE parmtype, BYTE *buf)
{
    WORD t = 0;
    BYTE *pProcont = NULL;
    static WORD wRecvOdNum = 0;

    if(parmtype == IENTSetIPInfoFRAME)
    {
        Flash_Write(&g_byRecv_Buffer[INETCONFIGDATA_OFFSET], INETIPInfo_SIZE, iNetData_Block, iNetIPInfo_ofs);    //保存  IPInfo  (0-25 BYTE)

        g_UpdateFlag = 1;
        InitLwipTcp();
        RuniNetAppFunctionType();
        if((pTotalMode == MODBUSTCP_MASTER) ||(pTotalMode == MODBUSTCP_SLAVE))
        {
        	g_byiNetTaskFlag = 0x55;
        	CreateLwipTcp();
        }
    }
    else if(parmtype == IENTPARAFRAME)
    {
        memcpy(&iNetModeMeg, &g_byRecv_Buffer[INETCONFIGDATA_OFFSET], INETPARA_SIZE);       //资料信息2 BYTE (Mode and Protocol)  (27-28 BYTE)
    }
    else if(parmtype == INETODFRAME)
    {
        pProcont = &g_byRecv_Buffer[INETCONFIGDATA_OFFSET];                 //OD数据首地址

        iNetODParaTab.wODNum        =   htons(*((WORD *)pProcont));
        pProcont += sizeof(WORD);
        iNetODParaTab.wCurrentODNum =   htons(*((WORD *)pProcont));
        pProcont += sizeof(WORD);

        if(iNetODParaTab.wODNum == 0 || iNetODParaTab.wODNum > MAX_INETODLIST)  return 2;

        if(*(&g_byRecv_Buffer[INETPACKETSTAMP_OFFSET_INFLASH]) == 1)        //PACKETSTAMP数据位为拆包后的序号
        {
            g_dwMap = iNetODParaTab.wODNum * sizeof(iNetODPara) + sizeof(WORD)*2;
            if(iNetODParaTab.pODList != NULL)
                free(&iNetODParaTab.pODList);
            iNetODParaTab.pODList = (iNetODPara *)malloc(g_dwMap);
        }

        if(wRecvOdNum < iNetODParaTab.wODNum)
        {
            for(t = 0; t < iNetODParaTab.wCurrentODNum; t++)
            {
                iNetODParaTab.pODList[t+wRecvOdNum].dwTableID         =    htonl(*((DWORD *)pProcont));
                pProcont += sizeof(DWORD);
                iNetODParaTab.pODList[t+wRecvOdNum].dwDeviceEntityAID =    htonl(*((DWORD *)pProcont));
                pProcont += sizeof(DWORD);
                iNetODParaTab.pODList[t+wRecvOdNum].dwiNetDataID      =    htonl(*((DWORD *)pProcont));
                pProcont += sizeof(DWORD);
                iNetODParaTab.pODList[t+wRecvOdNum].wiNetDataType     =    *((WORD *)pProcont);   //htons(*((WORD *)pProcont))
                pProcont += sizeof(WORD);
                iNetODParaTab.pODList[t+wRecvOdNum].wiNetUom          =    *((WORD *)pProcont);
                pProcont += sizeof(WORD);
                iNetODParaTab.pODList[t+wRecvOdNum].byiNetPermission  =    *pProcont;   //权限
                pProcont += sizeof(BYTE);
                iNetODParaTab.pODList[t+wRecvOdNum].byiNetPrecision   =    *pProcont;   //精度
                pProcont += sizeof(BYTE);
                iNetODParaTab.pODList[t+wRecvOdNum].Value.dwData      =    0;

                //Identify the agreement
				if(iNetODParaTab.pODList[t+wRecvOdNum].dwDeviceEntityAID == 0)
					iNetModeMeg.byProVer = STANDARD_PRO;
				else
					iNetModeMeg.byProVer = ADDITIONAL_PRO;
            }
            wRecvOdNum += iNetODParaTab.wCurrentODNum;        //Record the number of received OD
        }
        if(wRecvOdNum == iNetODParaTab.wODNum)
        {
        	;
        }
    }
    else if(parmtype == INETENDFRAME)
    {
        wRecvOdNum = 0;

        RuniNetAppFunctionType();

    	//clear Modbus TCP info
        pTotalMode = MODBUSTCP_NotUsed;
        pTotalCount = 0;
        DeleteDnCounter(&SendTime);
        DeleteDnCounter(&ReConnect);
    	Destroy_Task(g_htask_ModbusTCP);
    }
    else if(parmtype == INETSAVEFRAME)
    {
		Flash_Write((BYTE*)&iNetModeMeg,   sizeof(iNetModeMeg),   iNetData_Block, iNetPara_ofs  );
		Flash_Write((BYTE*)&iNetODParaTab, sizeof(iNetODParaTab), iNetData_Block, iNetODData_ofs);

        //clear Modbus TCP mode flag in flash
    	Flash_Write(0, sizeof(BYTE)+sizeof(WORD), ModbusTcpData_Block, 0);
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
static void RuniNetAppFunctionType()
{
    if(iNetModeMeg.byProtocolType == INETPROTYPETCP)
    {
        g_byiNetTaskFlag = 0x55;
//        InitLwipTcp();
        CreateLwipTcp();
    }
    else if(iNetModeMeg.byProtocolType == INETPROTYPEUDP)
    {
//        InitLwipUdp();
        CreateLwipUdp();
    }
    else if(iNetModeMeg.byProtocolType == INETMODENOTUSED)
    {
//    	iNetModeMeg.byMode = INETMODENOTUSED;
//    	iNetModeMeg.byProtocolType = INETMODENOTUSED;
//    	iNetODParaTab.wODNum = 0;
//    	iNetODParaTab.wCurrentODNum = 0;
//    	if(iNetODParaTab.pODList != NULL)
//    		free(iNetODParaTab.pODList);
//    	Destroy_Task(g_htask_iNet);
    }
}
/******************************************************END***************************************************************/





/*****************************************************************************************************************************************/
/************************************************************CRC_32 ****************************************************************/
/*****************************************************************************************************************************************/
/*===========================================================================+
|Name:         CreateCRC32Table                                              |
|Description:                                                                |
+===========================================================================*/

static void CreateCRC32Table()
{
    unsigned int nTemp;
    unsigned int i, j;
    for (i = 0; i < 256; i++)
    {
        nTemp =i;
        for (j = 0; j < 8; j++)
        {
            if (nTemp & 1)  nTemp = CRC32 ^ (nTemp >> 1);
            else            nTemp >>= 1;
        }
        _CRC32_Talbe[i] = nTemp;
    }
}

//====================================================
// 函数说明：一次性计算32位CRC校验码
// 参数1：pData，表示需要计算CRC的数据
// 参数2：Length，表示此数据的长度
// 返回值：计算所得出的32位的CRC校验码
//====================================================
static unsigned int GetCRC32(char* pData, unsigned  int Length)
{
    return GetCRC32B(0xFFFFFFFF, pData, Length);
}

//====================================================
// 函数说明：多次性计算32位CRC校验码
// 函数1：上一次计算的32位CRC校验码，如果是第一次，此值为0xFFFFFFF
// 参数2：pData，表示需要计算CRC的数据
// 参数3：Length，表示此数据的长度
// 返回值：计算所得出的32位的CRC校验码
//====================================================

static unsigned int GetCRC32B(unsigned int CRCCode, char* pData, unsigned int Length)
{
     unsigned int i;
     unsigned int nResult = CRCCode;
     for (i = 0; i < Length; i++)
       nResult = _CRC32_Talbe[(nResult ^ pData[i]) & 0xFF] ^ (nResult >> 8);

     return nResult;
}
/******************************************************END***************************************************************/




