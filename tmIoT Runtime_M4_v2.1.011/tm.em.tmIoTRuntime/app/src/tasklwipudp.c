/*==========================================================================+
|  File     : tasklwipudp.c                                                 |
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
#include "tasklwipudp.h"
#include "kernel/include/task.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/

/*===========================================================================+
|           Helpers  Implement                                               |
+===========================================================================*/
static void UdpReceiveData (void *arg, struct udp_pcb *upcb, struct pbuf *p,
        struct ip_addr *addr, u16_t port);
//static void RunLwipUdp(void *pParam);
static void iNetUdpRecvData (void *arg, struct udp_pcb *upcb, struct pbuf *p,
        struct ip_addr *addr, u16_t port);
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
TASK    TaskLwipUdp;
/*===========================================================================+
|           Inner Global Variable                                            |
+===========================================================================*/
struct udp_pcb       *u_pcb = NULL;
struct udp_pcb       *u_pcb_add  = NULL;
struct udp_pcb*       u_Udp_pcb;
struct udp_pcb*       u_Udp_pcb_Add;
NETCONNECTMESG 		 g_sRemoteMesg;
NETCONNECTMESG 		 g_sAddRemoteMesg;
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/

 /*===========================================================================+
 |Name:        del_list                                                       |
 |Description:                                                                |
 +===========================================================================*/
void del_list(BYTE index)
{
    if(u_Udp_pcb)
        udp_remove(u_Udp_pcb);
}
/*===========================================================================+
|Name:        del_list                                                       |
|Description:                                                                |
+===========================================================================*/
void Init_RemoteIP()
{
	memcpy(g_sRemoteMesg.aDstIP, u_aRemoteIP, sizeof(u_aRemoteIP));
	g_sRemoteMesg.wDstPort = 2330;
	g_sRemoteMesg.wSrcPort = 32002;
	g_sRemoteMesg.pUdpRecv = UdpReceiveData;

	if(iNetModeMeg.byProtocolType == INETPROTYPEUDP)
	{
		memcpy(g_sAddRemoteMesg.aDstIP, &iNetMessage.dwSeverIP, sizeof(iNetMessage.dwSeverIP));
		g_sAddRemoteMesg.wDstPort = iNetMessage.wSeverPort;
		g_sAddRemoteMesg.wSrcPort = iNetMessage.wDevicePort;
		g_sAddRemoteMesg.pUdpRecv = iNetUdpRecvData;
	}
}
/*===========================================================================+
| Name:        InitLwipUdp                                                   |
| Description: Initial the lwip Parameter, such as IP/Netmask/Gateway        |
+===========================================================================*/
void InitLwipUdp()
{
	InitLwipTcp();//IP初始化与Tcp一样
}
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void CreateLwipUdp()
{
	static BYTE InitFlag=0;

	Init_RemoteIP();

	if(InitFlag==0)
	{
	    InitFlag=1;
		u_Udp_pcb = Create_LwipUdp(&g_sRemoteMesg, LWIP_SERVER);
	}
    if(iNetModeMeg.byProtocolType == INETPROTYPEUDP)
    {
        if(iNetModeMeg.byMode == INETMODECLIENT)
            u_Udp_pcb_Add = Create_LwipUdp(&g_sAddRemoteMesg, LWIP_CLIENT);

        else if(iNetModeMeg.byMode == INETMODESERVER)
            u_Udp_pcb_Add = Create_LwipUdp(&g_sAddRemoteMesg, LWIP_SERVER);
    }
    if ((iNetModeMeg.byMode == INETMODECLIENT) || (iNetModeMeg.byMode == INETMODESERVER))
    	CreateTaskiNet();
}

/*===========================================================================+
|Name:        RunLwipUdp                                                     |
|Description:                                                                |
+===========================================================================*/
//void RunLwipUdp(void *pParam)
//{
//
//}


/* callback function to can when connected (or on error) */


/*===========================================================================+
|Name:        UdpReceiveData                                                 |
|Description:  Deal Receive Data from Lwip Udp                               |
+===========================================================================*/
void UdpReceiveData (void *arg, struct udp_pcb *upcb, struct pbuf *p,
                   struct ip_addr *addr, u16_t port)
{
    WORD_T   wlen = 0;

    u_pcb = upcb;
    wlen = p->len;
    memcpy(g_byRecv_Buffer,p->payload,p->len);
    upcb->remote_port = port;
    upcb->remote_ip =  *addr;
    g_RcvPortflag = 1;
    DealRcvProtocol(wlen);
    pbuf_free(p);
}
/*===========================================================================+
|Name:        iNetUdpRecvData                                                |
|Description:  Deal Receive Data from iNet Udp                               |
+===========================================================================*/
void iNetUdpRecvData (void *arg, struct udp_pcb *pcb, struct pbuf *p,
                   struct ip_addr *addr, u16_t port)
{
    if (p != NULL)
    {
    	u_pcb_add = pcb;
    	u_pcb_add->remote_port = port;
    	u_pcb_add->remote_ip = *addr;

        iNetParseRecvData(p->payload, p->len, INETPROTYPEUDP);

        pbuf_free(p);
    }
}
/*===========================================================================+
|Name:         Lwip_UdpWrite_Studio                                          |
|Description:  Send Data to tmIoT Studio by Lwip Udp                         |
+===========================================================================*/
void Lwip_UdpWrite_Studio(BYTE *bypData, WORD_T  wLength)
{
    Lwip_UdpWrite(u_pcb,bypData,wLength);
}
