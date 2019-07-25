/*==========================================================================+
|  File     : lwiplib_1.4.1.c                                               |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   :                                                               |
|  Version  : V1.00                                                         |
|  Creation : 2016/08/029                                                   |
|  Revision :                                                               |
+==========================================================================*/
#include "lwip-1.4.1/tm/lwiplib_1.4.1.h"
#include "driver/include/_timer.h"
#include "kernel/include/task.h"
#include "config.h"
//*****************************************************************************
//
// Ensure that ICMP checksum offloading is enabled; otherwise the TM4C129
// driver will not operate correctly.
//
//*****************************************************************************
//*****************************************************************************
//
// Include lwIP high-level API code.
//
//*****************************************************************************

#include "lwip-1.4.1/src/api/api_lib.c"
#include "lwip-1.4.1/src/api/api_msg.c"
#include "lwip-1.4.1/src/api/err.c"
#include "lwip-1.4.1/src/api/netbuf.c"
#include "lwip-1.4.1/src/api/netdb.c"
#include "lwip-1.4.1/src/api/netifapi.c"
#include "lwip-1.4.1/src/api/sockets.c"
#include "lwip-1.4.1/src/api/tcpip.c"

//*****************************************************************************
//
// Include the core lwIP TCP/IP stack code.
//
//*****************************************************************************
#include "lwip-1.4.1/src/core/def.c"
#include "lwip-1.4.1/src/core/dhcp.c"
#include "lwip-1.4.1/src/core/dns.c"
#include "lwip-1.4.1/src/core/init.c"
#include "lwip-1.4.1/src/core/mem.c"
#include "lwip-1.4.1/src/core/memp.c"
#include "lwip-1.4.1/src/core/netif.c"
#include "lwip-1.4.1/src/core/pbuf.c"
#include "lwip-1.4.1/src/core/raw.c"
#include "lwip-1.4.1/src/core/stats.c"
#include "lwip-1.4.1/src/core/sys.c"
#include "lwip-1.4.1/src/core/tcp.c"
#include "lwip-1.4.1/src/core/tcp_in.c"
#include "lwip-1.4.1/src/core/tcp_out.c"
#include "lwip-1.4.1/src/core/timers.c"
#include "lwip-1.4.1/src/core/udp.c"
//*****************************************************************************
//
// Include the IPV4 code.
//
//*****************************************************************************
#include "lwip-1.4.1/src/core/ipv4/autoip.c"
#include "lwip-1.4.1/src/core/ipv4/icmp.c"
#include "lwip-1.4.1/src/core/ipv4/igmp.c"
#include "lwip-1.4.1/src/core/ipv4/inet.c"
#include "lwip-1.4.1/src/core/ipv4/inet_chksum.c"
#include "lwip-1.4.1/src/core/ipv4/ip.c"
#include "lwip-1.4.1/src/core/ipv4/ip_addr.c"
#include "lwip-1.4.1/src/core/ipv4/ip_frag.c"

//*****************************************************************************
//
// Include the IPV6 code.
// Note:  Code is experimental and not ready for use.
// References are included for completeness.
//
//*****************************************************************************
#if 0
#include "lwip-1.4.1/src/core/ipv6/icmp6.c"
#include "lwip-1.4.1/src/core/ipv6/inet6.c"
#include "lwip-1.4.1/src/core/ipv6/ip6.c"
#include "lwip-1.4.1/src/core/ipv6/ip6_addr.c"
#endif

//*****************************************************************************
//
// Include the lwIP SNMP code.
//
//*****************************************************************************
//#include "src/core/snmp/asn1_dec.c"
//#include "src/core/snmp/asn1_enc.c"
//#include "src/core/snmp/mib2.c"
//#include "src/core/snmp/mib_structs.c"
//#include "src/core/snmp/msg_in.c"
//#include "src/core/snmp/msg_out.c"
//*****************************************************************************
//
// Include the network interface code.
//
//*****************************************************************************
#include "lwip-1.4.1/src/netif/etharp.c"

//*****************************************************************************
//
// Include the network interface PPP code.
//
//*****************************************************************************
//#include "src/netif/ppp/auth.c"
//#include "src/netif/ppp/chap.c"
//#include "src/netif/ppp/chpms.c"
//#include "src/netif/ppp/fsm.c"
//#include "src/netif/ppp/ipcp.c"
//#include "src/netif/ppp/lcp.c"
//#include "src/netif/ppp/magic.c"
//#include "src/netif/ppp/md5.c"
//#include "src/netif/ppp/pap.c"
//#include "src/netif/ppp/ppp.c"
//#include "src/netif/ppp/ppp_oe.c"
//#include "src/netif/ppp/randm.c"
//#include "src/netif/ppp/vj.c"
//*****************************************************************************
//
// Include Tiva-specific lwIP interface/porting layer code.
//
//*****************************************************************************
#include "lwip-1.4.1/ports/tiva-tm4c129/perf.c"
#include "lwip-1.4.1/ports/tiva-tm4c129/sys_arch.c"
#include "lwip-1.4.1/ports/tiva-tm4c129/netif/tiva-tm4c129.c"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
struct netif    u_sNetif;
struct netif    u_sNetif2;
struct tcp_pcb* u_sTcp_pcb[LWIP_MAX_CONNECT] = {NULL};
struct udp_pcb* u_sUdp_pcb[LWIP_MAX_CONNECT] = {NULL};
struct udp_pcb* u_sUdp_pcb_add[LWIP_MAX_CONNECT] = {NULL};
BYTE   u_byLocalIP[4];
WORD   u_wLocalPort = 0;
uint32_t g_ui32LocalTimer = 0;
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
static void lwIPPrivateInit(NETCONFIGMESG* pNetConfig);
static void LwipPrivateTaskCreate(void);
static void LwipPrivateRun(void *pParam);
static void ExlwIPPrivateInit(NETCONFIGMESG* pNetConfig);
static void echo_client_conn_err(void *arg, err_t err);
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
void    InitLwip(NETCONFIGMESG* pNetConfig)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    uint32_t ui32User0 = 0;
    uint32_t ui32User1 = 0;

    ui32User0 |= (pNetConfig->aMac[0]);
    ui32User0 |= (pNetConfig->aMac[1] << 8);
    ui32User0 |= (pNetConfig->aMac[2] << 16);
    HWREG(FLASH_USERREG0) = ui32User0;

    ui32User1 |= pNetConfig->aMac[3];
    ui32User1 |= (pNetConfig->aMac[4] << 8);
    ui32User1 |= (pNetConfig->aMac[5] << 16);
    HWREG(FLASH_USERREG1) = ui32User1;

    //
    // Enable the Ethernet peripheral.
    //
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EMAC0);
    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EPHY0);

    // Set Ethernet LED polarity to be active high
#ifdef IO2C1122
    HWREG(EMAC0_BASE + EMAC_O_CC) |= EMAC_CC_PTPCEN;
#else
    HWREG(EMAC0_BASE + EMAC_O_CC) |= 0;
#endif
    //
    // PF0/PF4 are used for Ethernet LEDs.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    ROM_GPIOPinConfigure(GPIO_PF0_EN0LED0);
    ROM_GPIOPinConfigure(GPIO_PF4_EN0LED1);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_4);

    // Set Ethernet LED polarity to be active low
//    HWREG(EMAC0_BASE + EMAC_O_CC) |= EMAC_CC_POL;

    //
    // PF1/PK4/PK6 are used for Ethernet LEDs.
    //
//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
//    ROM_GPIOPinConfigure(GPIO_PF1_EN0LED2);
//    ROM_GPIOPinConfigure(GPIO_PK4_EN0LED0);
//    ROM_GPIOPinConfigure(GPIO_PK6_EN0LED1);
//    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_1);
//    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_4);
//    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_6);
    //
    // Wait for the MAC to be ready.
    //

    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EMAC0))
    {
    }
    //
    // Configure for use with the internal PHY.
    //

    MAP_EMACPHYConfigSet(EMAC0_BASE, (EMAC_PHY_TYPE_INTERNAL | EMAC_PHY_INT_MDIX_EN |
                                       EMAC_PHY_AN_100B_T_FULL_DUPLEX));

    //
    // Reset the MAC. to latch the PHY configuration
    //
    MAP_EMACReset(EMAC0_BASE);
    //
    // Initialize the MAC and set the DMA mode.
    //

    MAP_EMACInit(EMAC0_BASE, g_dwSystemClock,
                     EMAC_BCONFIG_MIXED_BURST | EMAC_BCONFIG_PRIORITY_FIXED, 4, 4,
                     0);

    //
    // Set MAC configuration options.
    //
    MAP_EMACConfigSet(EMAC0_BASE,
                      (EMAC_CONFIG_FULL_DUPLEX | EMAC_CONFIG_CHECKSUM_OFFLOAD |
                       EMAC_CONFIG_7BYTE_PREAMBLE | EMAC_CONFIG_IF_GAP_96BITS |
                       EMAC_CONFIG_USE_MACADDR0 |
                       EMAC_CONFIG_SA_FROM_DESCRIPTOR |
                       EMAC_CONFIG_BO_LIMIT_1024),
                      (EMAC_MODE_RX_STORE_FORWARD |
                       EMAC_MODE_TX_STORE_FORWARD |
                       EMAC_MODE_TX_THRESHOLD_64_BYTES |
                       EMAC_MODE_RX_THRESHOLD_64_BYTES), 0);

    //+++++
    // Program the hardware with its MAC address (for filtering).
    //++++
    MAP_EMACAddrSet(EMAC0_BASE, 0, (uint8_t *)pNetConfig->aMac);

    lwIPPrivateInit(pNetConfig);
    LwipPrivateTaskCreate();
    g_ui32LocalTimer = g_dwSystemTick;
#endif
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
static void lwIPPrivateInit(NETCONFIGMESG* pNetConfig)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    struct ip_addr IPAddr;
    struct ip_addr NetMask;
    struct ip_addr GWAddr;

    /* initialize local vars */
    memset(&u_sNetif, 0, sizeof(u_sNetif));

    IP4_ADDR(&IPAddr, pNetConfig->aSrcIP[0], pNetConfig->aSrcIP[1],
    		pNetConfig->aSrcIP[2], pNetConfig->aSrcIP[3]);
    IP4_ADDR(&NetMask, pNetConfig->aNetMask[0], pNetConfig->aNetMask[1],
    		pNetConfig->aNetMask[2], pNetConfig->aNetMask[3]);
    IP4_ADDR(&GWAddr, pNetConfig->aGateWay[0], pNetConfig->aGateWay[1],
    		pNetConfig->aGateWay[2], pNetConfig->aGateWay[3]);

    memcpy(u_sNetif.hwaddr, pNetConfig->aMac, sizeof(pNetConfig->aMac));
	memcpy(u_byLocalIP, pNetConfig->aSrcIP, sizeof(u_byLocalIP));

    lwip_init();

    netif_add(&u_sNetif, &IPAddr, &NetMask, &GWAddr, NULL, tivaif_init, ethernet_input);// ip_input);
//    netif_set_default(&u_sNetif);

    // Bring the interface up.
    //
    netif_set_up(&u_sNetif);
#endif
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
void    InitExLwip(NETCONFIGMESG* pNetConfig)
{
	ExlwIPPrivateInit(pNetConfig);
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
static void ExlwIPPrivateInit(NETCONFIGMESG* pNetConfig)
{
	struct ip_addr IPAddr;
	struct ip_addr NetMask;
	struct ip_addr GWAddr;

	 memset(&u_sNetif2, 0, sizeof(u_sNetif2));
	IP4_ADDR(&IPAddr, pNetConfig->aSrcIP[0], pNetConfig->aSrcIP[1],
			pNetConfig->aSrcIP[2], pNetConfig->aSrcIP[3]);
	IP4_ADDR(&NetMask, pNetConfig->aNetMask[0], pNetConfig->aNetMask[1],
			pNetConfig->aNetMask[2], pNetConfig->aNetMask[3]);
	IP4_ADDR(&GWAddr, pNetConfig->aGateWay[0], pNetConfig->aGateWay[1],
			pNetConfig->aGateWay[2], pNetConfig->aGateWay[3]);

	netif_add(&u_sNetif2, &IPAddr, &NetMask, &GWAddr, NULL, tivaif_init, ethernet_input);
	netif_set_up(&u_sNetif2);
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
static void LwipPrivateTaskCreate()
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    TASK    Task;
    Task.wType = TASK_WHILELOOP;
    Task.period.tCounter = 0;
    Task.period.tTimer = 250;
    Task.pRun = LwipPrivateRun;
    Task.pParam = 0;
    Create_Task(&Task, 0);
#endif
}

static void LwipPrivateRun(void *pParam)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    int i = 0;

    //
    // 鍒ゆ柇鏄惁鏈夊垱寤篢CP,濡傛灉宸茬粡鏈塗CP鍒涘缓锛屾墽琛宼cp_tmr
    //
    for( i = 0; i < LWIP_MAX_CONNECT; i++)
    {
        if(u_sTcp_pcb[i] != NULL)
        {
            tcp_tmr();
            break;
        }
    }
#endif
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
struct tcp_pcb* Create_LwipTcp(NETCONNECTMESG* pConnectMesg, BYTE byCliServ)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    int i = 0;
    struct ip_addr  RemoteIP;

    u_wLocalPort = pConnectMesg->wSrcPort;

    IP4_ADDR(&RemoteIP, pConnectMesg->aDstIP[0], pConnectMesg->aDstIP[1],
    		pConnectMesg->aDstIP[2], pConnectMesg->aDstIP[3]);
//    IP4_ADDR(&LocalIP, u_byLocalIP[0], u_byLocalIP[1],
//    		u_byLocalIP[2], u_byLocalIP[3]);

    for(i = 0; i < LWIP_MAX_CONNECT; i++)
    {
        if(u_sTcp_pcb[i] == NULL)
        {
            //
            //鍒涘缓涓�釜TCP鍗忚鎺у埗鍧椾絾骞朵笉鎶婂畠鏀惧埌浠讳綍TCP PCB鍒楄〃,鐩村埌浣跨敤tcp_bind()鍑芥暟缁戝畾.
            //
            u_sTcp_pcb[i] = tcp_new();
            //
            //缁戝畾杩炴帴鍒颁竴涓湰鍦扮鍙ｅ彿鍜孖P鍦板潃. 鎴愬姛杩斿洖ERR_OK.濡傛灉缁欏嚭鐨勫弬鏁颁笉鍚堥�,鍒欒繑鍥濫RR_USE.
            //
            tcp_bind(u_sTcp_pcb[i], IP_ADDR_ANY, pConnectMesg->wSrcPort);
//            tcp_bind(u_sTcp_pcb[i], &LocalIP, u_wLocalPort);

            //
            //褰撳垱寤篢CP鏃�璋冪敤tcp_connect寤虹珛杩炴帴
            //
            if(byCliServ == LWIP_CLIENT)
            {
            	u_sTcp_pcb[i]->so_options |= SOF_KEEPALIVE;
            	u_sTcp_pcb[i]->keep_intvl = 2000;
            	tcp_connect(u_sTcp_pcb[i], &RemoteIP, pConnectMesg->wDstPort, pConnectMesg->pTcpConnected);
                tcp_arg(u_sTcp_pcb[i], NULL);
                tcp_err(u_sTcp_pcb[i], echo_client_conn_err);
                tcp_recv(u_sTcp_pcb[i], pConnectMesg->pTcpRecv);  // 鎸囧畾TCP鎺ユ敹鍥炶皟鍑芥暟
                return u_sTcp_pcb[i];
            }


            if(byCliServ == LWIP_SERVER)
            {
            	u_sTcp_pcb[i] = tcp_listen(u_sTcp_pcb[i]);  //鎸囧畾灏嗚杩涘叆鐩戝惉鐘舵�鐨勮繛鎺�
            	tcp_accept(u_sTcp_pcb[i], pConnectMesg->pTcpConnected);
               // tcp_arg(u_sTcp_pcb[i], NULL);
               // tcp_recv(u_sTcp_pcb[i], sRemoteMesg->pTcpRecv);  // 鎸囧畾TCP鎺ユ敹鍥炶皟鍑芥暟
                return u_sTcp_pcb[i];
            }
        }
    }
    return NULL;
#endif
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
void LwipEthernet_ISR(void)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    unsigned long ulStatus;
    ulStatus = MAP_EMACIntStatus(EMAC0_BASE, true);

    if (ulStatus) {
        MAP_EMACIntClear(EMAC0_BASE, ulStatus);
        tivaif_interrupt(&u_sNetif, ulStatus);
        tivaif_interrupt(&u_sNetif2, ulStatus);
    }
#endif
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
void Lwip_TcpWrite(struct tcp_pcb* pcb, BYTE* pData, WORD wLen)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    if (pcb->state == ESTABLISHED) {
        tcp_write(pcb, pData, wLen, TCP_WRITE_FLAG_COPY);
    }
#endif
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
err_t Lwip_TcpClose(struct tcp_pcb* pcb)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    int i = 0;
    err_t err;

    for(i = 0; i < LWIP_MAX_CONNECT; i++)
    {
        if(pcb == u_sTcp_pcb[i])
        {
            err = tcp_close(u_sTcp_pcb[i]);
            if(err == ERR_OK)
            {
            	u_sTcp_pcb[i] = NULL;
            	pcb = NULL;
            }
            return err;
        }
    }
    return 1;
#endif
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
struct udp_pcb* Create_LwipUdp(NETCONNECTMESG* pConnectMesg, BYTE byCliServ)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    int i = 0;

    struct ip_addr  RemoteIP;
    struct ip_addr  LocalIP;

    u_wLocalPort = pConnectMesg->wSrcPort;

    IP4_ADDR(&RemoteIP, pConnectMesg->aDstIP[0], pConnectMesg->aDstIP[1],
    		pConnectMesg->aDstIP[2], pConnectMesg->aDstIP[3]);
    IP4_ADDR(&LocalIP, u_byLocalIP[0], u_byLocalIP[1],
      		u_byLocalIP[2], u_byLocalIP[3]);

    for(i = 0; i < LWIP_MAX_CONNECT; i++)
    {
        if(u_sUdp_pcb[i] == NULL)
        {
            u_sUdp_pcb[i] = udp_new();
            udp_bind(u_sUdp_pcb[i], IP_ADDR_ANY, pConnectMesg->wSrcPort);
//            udp_bind(u_sUdp_pcb[i], &LocalIP, u_wLocalPort);
            if(byCliServ == LWIP_CLIENT)
            {
                udp_connect(u_sUdp_pcb[i], &RemoteIP, pConnectMesg->wDstPort);
            }
            udp_recv(u_sUdp_pcb[i], pConnectMesg->pUdpRecv, NULL);
            return u_sUdp_pcb[i];
        }
    }
    return NULL;
#endif
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
struct udp_pcb* Create_LwipUdp_Add(NETCONNECTMESG* pConnectMesg, BYTE byCliServ)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    int i = 0;

    struct ip_addr  RemoteIP;

    u_wLocalPort = pConnectMesg->wSrcPort;

    IP4_ADDR(&RemoteIP, pConnectMesg->aDstIP[0], pConnectMesg->aDstIP[1],
            pConnectMesg->aDstIP[2], pConnectMesg->aDstIP[3]);

    for(i = 0; i < LWIP_MAX_CONNECT; i++)
    {
        if(u_sUdp_pcb_add[i] == NULL)
        {
            u_sUdp_pcb_add[i] = udp_new();

            udp_bind(u_sUdp_pcb_add[i], IP_ADDR_ANY, pConnectMesg->wSrcPort);
            if(byCliServ == LWIP_CLIENT)
            {
                udp_connect(u_sUdp_pcb_add[i], &RemoteIP, pConnectMesg->wDstPort);
            }
            udp_recv(u_sUdp_pcb_add[i], pConnectMesg->pUdpRecv, NULL);
            return u_sUdp_pcb_add[i];
        }
    }
    return NULL;
#endif
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
void Lwip_UdpWrite(struct udp_pcb* pcb, BYTE* pData, WORD wLen)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    struct pbuf* pSendBuf = pbuf_alloc(PBUF_TRANSPORT, wLen, PBUF_RAM);
    //pSendBuf->payload = pData;
    memcpy(pSendBuf->payload, pData, wLen);

    if (pcb != NULL) {
        udp_send(pcb, pSendBuf);
    }
    pbuf_free(pSendBuf);
#endif
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
void Lwip_UdpClose(struct udp_pcb* pcb)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    int i = 0;

    for(i = 0; i < LWIP_MAX_CONNECT; i++)
    {
        if(pcb == u_sUdp_pcb[i])
        {
        	udp_remove(u_sUdp_pcb[i]);
            u_sUdp_pcb[i] = NULL;
            pcb = NULL;
            break;
        }
    }
#endif
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
void LwipIp2Update(NETCONFIGMESG* sLocalMesg)
{
#if (!defined IO2C1111) && (!defined IO2C1112)
    struct ip_addr ip_update;
    struct ip_addr Mask_update;
    struct ip_addr GW_updata;

    IP4_ADDR(&ip_update,sLocalMesg->aSrcIP[0],sLocalMesg->aSrcIP[1],
               sLocalMesg->aSrcIP[2],sLocalMesg->aSrcIP[3]);
    IP4_ADDR(&Mask_update, sLocalMesg->aNetMask[0], sLocalMesg->aNetMask[1],
               sLocalMesg->aNetMask[2], sLocalMesg->aNetMask[3]);
    IP4_ADDR(&GW_updata, sLocalMesg->aGateWay[0], sLocalMesg->aGateWay[1],
               sLocalMesg->aGateWay[2], sLocalMesg->aGateWay[3]);

    ip_addr_set(&(u_sNetif2.ip_addr), &ip_update);
    ip_addr_set(&(u_sNetif2.gw), &GW_updata);
    ip_addr_set(&(u_sNetif2.netmask), &Mask_update);

#endif
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
//缃戠粶杩炴帴閿欒鏃跺�欑殑鍥炶皟鍑芥暟                       2018/10/15      ZZYuan
static void echo_client_conn_err(void *arg, err_t err)    //close_wait鐘舵��  32s閲嶅惎
{
    int i = 0;
    err_t ERR;
    struct ip_addr  SeverIP;
    NETCONNECTMESG * pReconnectMesg = (NETCONNECTMESG *)arg;

    if(err != ERR_ABRT)  return;
    IP4_ADDR(&SeverIP, pReconnectMesg->aDstIP[0], pReconnectMesg->aDstIP[1],
            pReconnectMesg->aDstIP[2], pReconnectMesg->aDstIP[3]);

    for(i = 0; i < LWIP_MAX_CONNECT; i++)
    {
        if(u_sTcp_pcb[i] != NULL)
        {
            ERR = tcp_close(u_sTcp_pcb[i]);
            if(ERR == ERR_OK)
                u_sTcp_pcb[i] = NULL;
        }
        if(u_sTcp_pcb[i] == NULL)
        {
            u_sTcp_pcb[i] = tcp_new();
            if(u_sTcp_pcb[i] != NULL)
                ip_set_option(u_sTcp_pcb[i], SOF_KEEPALIVE);
            tcp_bind(u_sTcp_pcb[i], IP_ADDR_ANY, pReconnectMesg->wSrcPort);

            tcp_connect(u_sTcp_pcb[i], &SeverIP, pReconnectMesg->wDstPort,
                                                   pReconnectMesg->pTcpConnected);
            tcp_arg(u_sTcp_pcb[i], pReconnectMesg);
            tcp_err(u_sTcp_pcb[i], echo_client_conn_err);
            tcp_recv(u_sTcp_pcb[i], pReconnectMesg->pTcpRecv);
            return;
        }
    }
}

