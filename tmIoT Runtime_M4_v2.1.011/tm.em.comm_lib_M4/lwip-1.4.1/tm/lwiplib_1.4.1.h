/*==========================================================================+
|  File     : lwiplib_1.4.1.h                                               |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : Jia                                                           |
|  Version  : V1.00                                                         |
|  Creation : 2016/08/09                                                    |
|  Revision :                                                               |
+==========================================================================*/
#ifndef D_LWIPLIB141_H
#define D_LWIPLIB141_H

#include "kernel/include/type.h"
#include "M4_lib/utils/lwiplib.h"
#include "lwip-1.4.1/src/include/lwip/init.h"
#include "lwip-1.4.1/src/include/netif/etharp.h"
#include "lwip-1.4.1/ports/tiva-tm4c129/include/netif/tivaif.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
#define     LWIP_MAX_CONNECT                 4
#define     LWIP_CLIENT                      0
#define     LWIP_SERVER                      1
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/ 
typedef struct tag_NETCONFIGMESG
{
    BYTE    aSrcIP[4];
    BYTE    aNetMask[4]; 
    BYTE    aGateWay[4];
    BYTE    aMac[6];
}NETCONFIGMESG;

typedef struct tag_NETCONNECTMESG
{
    BYTE               aDstIP[4];
    WORD			   wSrcPort;
    WORD               wDstPort;
    tcp_connected_fn   pTcpConnected;
    tcp_recv_fn        pTcpRecv;
    udp_recv_fn        pUdpRecv;
}NETCONNECTMESG;
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/
extern struct netif    u_sNetif;
extern struct netif    u_sNetif2;
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
void    InitLwip(NETCONFIGMESG* pNetConfig);
void    InitExLwip(NETCONFIGMESG* pNetConfig);
struct tcp_pcb* Create_LwipTcp(NETCONNECTMESG* pConnectMesg,  BYTE byCliServ);
struct udp_pcb* Create_LwipUdp(NETCONNECTMESG* pConnectMesg, BYTE byCliServ);
struct udp_pcb* Create_LwipUdp_Add(NETCONNECTMESG* pConnectMesg, BYTE byCliServ);
void Lwip_TcpWrite(struct tcp_pcb* pcb, BYTE* pData, WORD wLen);
void Lwip_UdpWrite(struct udp_pcb* pcb, BYTE* pData, WORD wLen);
void LwipIp2Update(NETCONFIGMESG* sLocalMesg);
err_t Lwip_TcpClose(struct tcp_pcb* pcb);
void Lwip_UdpClose(struct udp_pcb* pcb);
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
#endif
