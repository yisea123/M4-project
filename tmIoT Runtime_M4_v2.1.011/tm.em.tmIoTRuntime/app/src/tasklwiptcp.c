/*==========================================================================+
|  File     : tasklwiptcp.h                                                 |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : YW                                                            |
|  Version  : V2.00                                                         |
|  Creation : 2018/01/19                                                    |
|  Revision :                                                               |
+==========================================================================*/
#include "tasklwiptcp.h"
#include "kernel/include/task.h"
#include "lwip-1.4.1/tm/lwiplib_1.4.1.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|           Helpers  Implement                                               |
+===========================================================================*/
static      err_t TcpReceiveData(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
static      err_t do_connected(void *arg, struct tcp_pcb *pcb, err_t err);
static      void  RunLwipTcpMeg(void *pParam);
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Type definition                                                  |
+============================================================================*/
/*===========================================================================+
|           Constructor and destructor                                       |
+============================================================================*/
/*===========================================================================+
|           Operations                                                       |
+============================================================================*/
/*===========================================================================+
|           Inner Global Variable                                            |
+===========================================================================*/
BYTE        u_aLocalIP[4] = {192, 168, 11, 112};
const       BYTE u_aNetMask[4] = {255, 255, 255, 0};
const       BYTE u_aGateWay[4] = {192, 168, 11, 1};
//BYTE        u_aMacAddr[6];

BYTE        u_aRemoteIP[4] = {0, 0, 0, 0};
WORD		u_aRemotePort;

BYTE 		u_aLocalIP_Add[4] = {192, 168, 10, 177};
BYTE 		u_aNetMask_Add[4] = {255, 255, 255, 0};
BYTE 		u_aGateWay_Add[4] = {192, 168, 10, 1};
BYTE 		u_aRemoteIP_Add[4];

BYTE_T     u_ServerFlag_ReadbyEeprom;
BYTE_T     u_ServerIP_ReadbyEeprom[4];
//BYTE       u_ServerMAC_ReadbyEeprom[6];

BYTE_T     u_TmIoTFlag_ReadbyEeprom;
BYTE_T     u_TmIoTIP_ReadbyEeprom[4];
BYTE_T     u_TmIoTMASK_ReadbyEeprom[4];
BYTE_T     u_TmIoTGATEWAY_ReadbyEeprom[4];
BYTE       u_TmIoTMAC_ReadbyEeprom[6];

struct 	   tcp_pcb*         u_Tcp_pcb;

NETCONFIGMESG u_sLocalMesg;
NETCONFIGMESG sNetConfig_Add;
BYTE  RecvTcpBuf[1024] = {0};

iNetMeg       iNetMessage;
extern iNetMODEMES   iNetModeMeg;
static DWORD u_dwLineTick = 0;
BYTE g_byiNetTaskFlag = 0x55;
static WORD u_wDtask = 0;

BYTE g_byMacAdderBuf[8];
BYTE g_UpdateFlag = 0;

void Read_MacAddr_FromEeprom()
{
    Device_Read(g_hEeprom, g_byMacAdderBuf, sizeof(g_byMacAdderBuf), 0);
}
/*===========================================================================+
| Name:        SetLwipPara                                                   |
| Description: Set the Communication Parameter, Read the Server IP/Mac and   |
|              the device IP/Mac from Eeprom, Then Set the parameter through |
|              the Flag.This Function main purpose is change the IP/Mac by   |
|              tmIoT Studio                                                  |
+===========================================================================*/
void  SetLwipPara()
{
    BYTE_T buff[26];

    Flash_Read(buff, sizeof(iNetMessage), iNetData_Block, iNetIPInfo_ofs);
    memcpy(&iNetMessage, buff, sizeof(iNetMessage));

    iNetMessage.wDevicePort = htons(iNetMessage.wDevicePort);
    iNetMessage.wSeverPort = htons(iNetMessage.wSeverPort);
}
/*===========================================================================+
| Name:        InitLwipTcp                                                   |
| Description: Initial the lwip Parameter, such as IP/Netmask/Gateway        |
+===========================================================================*/
void InitLwipTcp()
{

	static BYTE InitFlag=0;

	SetLwipPara();

	if(InitFlag==0)
	{
		memcpy(u_sLocalMesg.aSrcIP, u_aLocalIP, sizeof(u_aLocalIP));
		memcpy(u_sLocalMesg.aNetMask, u_aNetMask, sizeof(u_aNetMask));
		memcpy(u_sLocalMesg.aGateWay, u_aGateWay, sizeof(u_aGateWay));
		memcpy(u_sLocalMesg.aMac, g_byMacAdderBuf, sizeof(g_byMacAdderBuf)-2);

		InitLwip(&u_sLocalMesg);

		InitFlag=1;
	}
	if (( iNetModeMeg.byMode != INETMODENOTUSED ) || (pTotalMode != MODBUSTCP_NotUsed))
	{
		if(u_Udp_pcb_Add != NULL)
			Lwip_UdpClose(u_Udp_pcb_Add);
		if(u_Tcp_pcb != NULL)
			Lwip_TcpClose(u_Tcp_pcb);
	}
	memcpy(sNetConfig_Add.aSrcIP,   &iNetMessage.dwDeviceIP, sizeof(u_aLocalIP_Add));
	memcpy(sNetConfig_Add.aNetMask, &iNetMessage.dwMask,     sizeof(u_aNetMask_Add));
	memcpy(sNetConfig_Add.aGateWay, &iNetMessage.dwGateWay,  sizeof(u_aGateWay_Add));
	memcpy(sNetConfig_Add.aMac,     g_byMacAdderBuf,         sizeof(g_byMacAdderBuf)-2);

	if(g_UpdateFlag)
		LwipIp2Update(&sNetConfig_Add);
	else
		InitExLwip(&sNetConfig_Add);
	g_UpdateFlag = 0;
}
/*===========================================================================+
| Name:        Init_RemoteIP_Tcp                                             |
| Description: Initial the lwip Parameter, such as IP/Netmask/Gateway        |
+===========================================================================*/
void Init_RemoteIP_Tcp()
{
	memcpy(g_sAddRemoteMesg.aDstIP, &iNetMessage.dwSeverIP, sizeof(iNetMessage.dwSeverIP));
	g_sAddRemoteMesg.wDstPort = iNetMessage.wSeverPort;
	g_sAddRemoteMesg.wSrcPort = iNetMessage.wDevicePort;
	g_sAddRemoteMesg.pTcpConnected = do_connected;
	g_sAddRemoteMesg.pTcpRecv = TcpReceiveData;
}
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void CreateLwipTcp()
{
	Init_RemoteIP_Tcp();

	if((pTotalMode == MODBUSTCP_MASTER) || (iNetModeMeg.byMode == INETMODECLIENT)) //  ModbusTCP Master\iNet tcp client
	{
		u_Tcp_pcb = Create_LwipTcp(&g_sAddRemoteMesg, LWIP_CLIENT);
	}
	else if((pTotalMode == MODBUSTCP_SLAVE) || (iNetModeMeg.byMode == INETMODESERVER))  //  ModbusTCP Slave\iNet tcp sever
	{
		u_Tcp_pcb = Create_LwipTcp(&g_sAddRemoteMesg, LWIP_SERVER);
	}
	//reconnect
    if((g_byiNetTaskFlag == 0x55) && ((iNetModeMeg.byMode != INETMODENOTUSED) || (pTotalMode != MODBUSTCP_NotUsed)))
    {
        g_byiNetTaskFlag = 0;
        TASK    Task;
        Task.wType = TASK_WHILELOOP;
        Task.period.tCounter = 0;
        Task.period.tTimer = 1;
        Task.pRun = RunLwipTcpMeg;
        Task.pParam = 0;
        u_wDtask = Create_Task(&Task, 0);
    }
    if ((pTotalMode == MODBUSTCP_MASTER) || (pTotalMode == MODBUSTCP_SLAVE))
    	CreateTaskModbusTCP();
    if ((iNetModeMeg.byMode == INETMODECLIENT) || (iNetModeMeg.byMode == INETMODESERVER))
    	CreateTaskiNet();
}

/*===========================================================================+
| Name:        RunLwip                                                       |
| Description:                                                               |
+===========================================================================*/
//void RunLwip(void *pParam)
//{
//
//}
static void RunLwipTcpMeg(void *pParam)
{
    if(u_Tcp_pcb->state != ESTABLISHED)
    {
        if(g_dwSystemTick - u_dwLineTick > Net_AgainLink_Timer)
        {
            if(u_Tcp_pcb != NULL)
                   Lwip_TcpClose(u_Tcp_pcb);
            CreateLwipTcp();
            u_dwLineTick = g_dwSystemTick;
        }
    }
    else Destroy_Task(u_wDtask);
}
/*===========================================================================+
| Name:        do_connected                                                  |
| Description:                                                               |
+===========================================================================*/
/* callback function to can when connected (or on error) */
static err_t do_connected (void *arg, struct tcp_pcb *pcb, err_t err)
{
    tcp_recv(pcb,TcpReceiveData);
    return 0;
}


/*===========================================================================+
| Name:        TcpReceiveData                                                |
| Description: Deal the Receive Data from Tcp                                |
+===========================================================================*/
static err_t TcpReceiveData(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
	if ((err == ERR_OK) && (p != NULL))
	{
		/* Inform TCP that we have taken the data. */
		tcp_recved(pcb, p->tot_len);
		if(pTotalMode == MODBUSTCP_MASTER)
		{
			memcpy(RecvTcpBuf, p->payload, p->len);
    		ReConnect = 5000;
    		HandleModbusTCPData(RecvTcpBuf);
    	}
    	else if(pTotalMode == MODBUSTCP_SLAVE)
    	{
    		ModbusTCPSlave_Recv(pcb, p);
    		if(ResponseFlag == 1)
    		{
    			Lwip_TcpWrite(pcb, ModbusTCPResBuf, ModbusTCPRes_Length);
    			ResponseFlag = 0;
    		}
    	}
    	else if((iNetModeMeg.byMode == INETMODECLIENT) || (iNetModeMeg.byMode == INETMODESERVER))
    	{
    		iNetParseRecvData(p->payload, p->len, INETPROTYPETCP);
    	}
    }
    pbuf_free(p);

    return ERR_OK;
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
