/*==========================================================================+
|  Class    : Ethernet                                                      |
|             Ethernet Protocol Function                                    |
|  Task     : Ethernet service routine                                      |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   :                                                               |
|  Version  : V1.00                                                         |
|             V2.00 090722 zholy                                            |
|  Creation :                                                               |
|  Revision :                                                               |
+==========================================================================*/
#include "driver/include/_timer.h"
#include "ethernet/include/ethernet.h"
#include <string.h>
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
const   BYTEX   g_logo[5] =
{
    {'T', 'E'},         //'T','E'
    {'C', 'H'},         //'C','H'
    {'M', 'A'},         //'M','A'
    {'T', 'I'},         //'T','I'
    {'O', 'N'}          //'O','N'
};
/*===========================================================================+
|           Function prototype                                               |
+===========================================================================*/
int  FindAllParamEthSession(int nDeviceHandle, BYTEX *pdstMAC, BYTEX *psrcIP, BYTEX *pdstIP, \
                            WORD_T wsrcPort, WORD_T wdstPort);
int  FindIPEthSession(int nDeviceHandle, BYTEX *psrcIP);
int  FindMACEthSession(int nDeviceHandle, BYTEX *pdstMAC);
void ActivateEthSession(int nDeviceHandle, BYTEX *pdstMAC, BYTEX *pdstIP);

void DealARP(int nDeviceHandle, MACARP *pdata);
void DealIP(int nDeviceHandle, WORD_T *pdata, WORD_T wlength);
void DealICMP(int nDeviceHandle, WORD_T *pdata, WORD_T wlength);
void DealUDP(int nDeviceHandle, WORD_T *pdata, WORD_T wlength);
void DealTECH(int nDeviceHandle, WORD_T *pdata, WORD_T wlength);

void SendARPData(int nSessionHandle);
WORD_T SendUDPData(int nDeviceHandle, int nHandle, WORD_T *pw, WORD_T wLength);
WORD_T SendTECHData(int nDeviceHandle, int nHandle, WORD_T *pw, WORD_T wLength);

void RoutineCheckSessionState(void);

WORD_T chksum(WORD_T *pdata, WORD_T datalen);
WORD_T comparebuff(BYTEX *p1, BYTEX *p2, WORD_T length);


/*===========================================================================+
|           Global variable                                                  |
+===========================================================================*/

static  WORD_T    u_awEthTxBufTemp[MAX_NETDEVICE][CONST_SIZE_SENDPACKET / 2];

static  WORD_T    u_awEthRxBufTemp[MAX_NETDEVICE][CONST_SIZE_SENDPACKET / 2];

/*===========================================================================+
|           Local variable                                                   |
+===========================================================================*/

PARAMETH   u_parameth;

ROUTINE g_routinechecksessionstate = { 1000, &RoutineCheckSessionState };

/*===========================================================================+
|           Class implementation                                             |
+===========================================================================*/

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
WORD_T     OpenEthernet(CONFIGNET *pconfignet, BYTE  Ethernet_CMD, BYTE *mac)
{
    WORD_T  i, wResult;

    i = pconfignet->nDeviceHandle;
    if (i >= MAX_NETDEVICE) return 0;

    u_parameth.apDefaultIPFunc[i]   = pconfignet->pDefaultIPFunc;
    u_parameth.apDefaultTECHFunc[i] = pconfignet->pDefaultTECHFunc;
    memcpy(u_parameth.asrcIP[i], pconfignet->asrcIP, CONST_LENGTH_IP);

    wResult =  Device_Control(g_hEthernet, Ethernet_CMD, mac);
    if (wResult == NIC_SUCCESS)    return 1;
    else                           return 0;
}

void    CloseEthernet()
{

    Device_Close(g_hEthernet);
}

void    ResetParamEth(BYTE *mac)
{

    memset(&u_parameth, 0, sizeof(u_parameth));

    u_parameth.nSessionHandle   = -1;
    u_parameth.wSendPacketIndex =  1;
    memcpy(u_parameth.asrcMAC[0], mac, CONST_LENGTH_MAC * 2);

    AddRoutine(&g_routinechecksessionstate);
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
// return nSessionHandle: -1 --- NULL; 0 ~ MAX_NETDEVICE * MAX_SESSIONCH-1 --- OK.
int     OpenEthSession(int nDeviceHandle, IOETH *pIOETH)
{
    int     i, nFirstNull = -1;
    WORD_T    wOldAllActive = u_parameth.awAllActive[nDeviceHandle];
    BYTEX   aMAC[CONST_LENGTH_MAC] = {{0xFF, 0xFF}, {0xFF, 0xFF}, {0xFF, 0xFF}};

    if (nDeviceHandle >= MAX_NETDEVICE) return -1;

    // find first null and check whether already exist.
    for (i = 0; i < MAX_SESSIONCH; i++)
    {
        if (u_parameth.sessionch[nDeviceHandle][i].wState == STATESESSIONCH_NULL)
        {
            if (nFirstNull == -1)
                nFirstNull = i;
        }
        else
        {
            if (pIOETH->wType == PROTOCOL_TECH && u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_TECH)
            {
                // only check dstMAC
                if (comparebuff(pIOETH->adstMAC, u_parameth.sessionch[nDeviceHandle][i].ioeth.adstMAC, CONST_LENGTH_MAC))
                    return (i + nDeviceHandle * MAX_SESSIONCH);
            }
            else if (pIOETH->wType == PROTOCOL_IP && u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_IP)
            {
                // check IP and Port
                if (comparebuff(pIOETH->asrcIP, u_parameth.sessionch[nDeviceHandle][i].ioeth.asrcIP, CONST_LENGTH_IP) && \
                        comparebuff(pIOETH->adstIP, u_parameth.sessionch[nDeviceHandle][i].ioeth.adstIP, CONST_LENGTH_IP) && \
                        pIOETH->wsrcPort == CHANGEW(u_parameth.sessionch[nDeviceHandle][i].ioeth.wsrcPort) && \
                        pIOETH->wdstPort == CHANGEW(u_parameth.sessionch[nDeviceHandle][i].ioeth.wdstPort))
                    return (i + nDeviceHandle * MAX_SESSIONCH);
            }
        }
    }

    // open new session
    if (nFirstNull == -1) return -1;
    else
    {
        memcpy(&u_parameth.sessionch[nDeviceHandle][nFirstNull].ioeth, pIOETH, sizeof(IOETH));
        u_parameth.sessionch[nDeviceHandle][nFirstNull].wState       = STATESESSIONCH_OPEN;
        u_parameth.awAllActive[nDeviceHandle]                        = 0;
        if (pIOETH->wType == PROTOCOL_TECH)
        {
            u_parameth.sessionch[nDeviceHandle][nFirstNull].wState   = STATESESSIONCH_ACTIVE;
            u_parameth.awAllActive[nDeviceHandle]                    = wOldAllActive;
        }
        else if (pIOETH->wType == PROTOCOL_IP)
        {
            u_parameth.sessionch[nDeviceHandle][nFirstNull].ioeth.wsrcPort = \
                    CHANGEW(u_parameth.sessionch[nDeviceHandle][nFirstNull].ioeth.wsrcPort);
            u_parameth.sessionch[nDeviceHandle][nFirstNull].ioeth.wdstPort = \
                    CHANGEW(u_parameth.sessionch[nDeviceHandle][nFirstNull].ioeth.wdstPort);
            if (comparebuff(pIOETH->adstMAC, aMAC, CONST_LENGTH_MAC))
                u_parameth.sessionch[nDeviceHandle][nFirstNull].wState = STATESESSIONCH_ACTIVE;
        }

        return (nFirstNull + nDeviceHandle * MAX_SESSIONCH);
    }
}

void    CloseEthSession(int nSessionHandle)
{
    int     i, j;

    if (nSessionHandle >= MAX_NETDEVICE * MAX_SESSIONCH) return;
    i = nSessionHandle / MAX_SESSIONCH;
    j = nSessionHandle % MAX_SESSIONCH;
    u_parameth.sessionch[i][j].wState = STATESESSIONCH_NULL;
    memset(&u_parameth.sessionch[i][j].ioeth, 0, sizeof(IOETH));
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
void    ActivateEthSession(int nDeviceHandle, BYTEX *pdstMAC, BYTEX *pdstIP)
{
    int     i;

    if (nDeviceHandle >= MAX_NETDEVICE || u_parameth.awAllActive[nDeviceHandle]) return;

    for (i = 0; i < MAX_SESSIONCH; i++)
    {
        if (u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_IP && \
                u_parameth.sessionch[nDeviceHandle][i].wState == STATESESSIONCH_OPEN)
        {
            if (comparebuff(pdstIP, u_parameth.sessionch[nDeviceHandle][i].ioeth.adstIP, CONST_LENGTH_IP))
            {
                memcpy(u_parameth.sessionch[nDeviceHandle][i].ioeth.adstMAC, pdstMAC, CONST_LENGTH_MAC * 2);
                u_parameth.sessionch[nDeviceHandle][i].wState = STATESESSIONCH_ACTIVE;
            }
        }
    }
}


WORD_T    GetSessionState(int nSessionHandle)
{
    return 0;
}

int    FindAllParamEthSession(int nDeviceHandle, BYTEX *pdstMAC, BYTEX *psrcIP, BYTEX *pdstIP, WORD_T wsrcPort, WORD_T wdstPort)
{
    int     i;
    BYTEX   aBroadcastIP[CONST_LENGTH_IP] = {{0xFF, 0xFF}, {0xFF, 0xFF}};

    if (nDeviceHandle >= MAX_NETDEVICE) return -1;

    for (i = 0; i < MAX_SESSIONCH; i++)
    {
        if (u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_IP && \
                u_parameth.sessionch[nDeviceHandle][i].wState == STATESESSIONCH_ACTIVE)
        {
            if (comparebuff(pdstMAC, u_parameth.sessionch[nDeviceHandle][i].ioeth.adstMAC, CONST_LENGTH_MAC))
            {
                if (comparebuff(psrcIP, aBroadcastIP, CONST_LENGTH_IP))
                {
                    if (comparebuff(pdstIP, u_parameth.sessionch[nDeviceHandle][i].ioeth.adstIP, CONST_LENGTH_IP))
                    {
                        if (wsrcPort == u_parameth.sessionch[nDeviceHandle][i].ioeth.wsrcPort && \
                                wdstPort == u_parameth.sessionch[nDeviceHandle][i].ioeth.wdstPort)
                            return (i + nDeviceHandle * MAX_SESSIONCH);
                    }
                }
                else
                {
                    if (comparebuff(psrcIP, u_parameth.sessionch[nDeviceHandle][i].ioeth.asrcIP, CONST_LENGTH_IP) && \
                            comparebuff(pdstIP, u_parameth.sessionch[nDeviceHandle][i].ioeth.adstIP, CONST_LENGTH_IP))
                    {
                        if (wsrcPort == u_parameth.sessionch[nDeviceHandle][i].ioeth.wsrcPort && \
                                wdstPort == u_parameth.sessionch[nDeviceHandle][i].ioeth.wdstPort)
                            return (i + nDeviceHandle * MAX_SESSIONCH);
                    }
                }
            }
        }
    }
    return -1;
}

int     FindIPEthSession(int nDeviceHandle, BYTEX *psrcIP)
{
    int     i;

    if (nDeviceHandle >= MAX_NETDEVICE) return -1;

    for (i = 0; i < MAX_SESSIONCH; i++)
    {
        if (u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_IP)
            if (comparebuff(u_parameth.sessionch[nDeviceHandle][i].ioeth.asrcIP, psrcIP, CONST_LENGTH_IP))
                return (i + nDeviceHandle * MAX_SESSIONCH);
    }
    return -1;
}

int     FindMACEthSession(int nDeviceHandle, BYTEX *pdstMAC)
{
    int     i;

    if (nDeviceHandle >= MAX_NETDEVICE) return -1;

    for (i = 0; i < MAX_SESSIONCH; i++)
    {
        if (u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_TECH)
            if (comparebuff(u_parameth.sessionch[nDeviceHandle][i].ioeth.adstMAC, pdstMAC, CONST_LENGTH_MAC))
                return (i + nDeviceHandle * MAX_SESSIONCH);
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
WORD_T  comparebuff(BYTEX *p1, BYTEX *p2, WORD_T length)
{
    WORD_T i;
    for (i = 0; i < length; i++)
    {
        if (((p1 + i)->LBYTE != (p2 + i)->LBYTE) || ((p1 + i)->HBYTE != (p2 + i)->HBYTE)) return 0;
    }
    return 1;
}

WORD_T  chksum(WORD_T *pdata, WORD_T datalen)
{
    long sum = 0;

    while (datalen > 1)
    {
        sum += *(pdata++);
        if (sum & 0x80000000) sum = (sum & 0xFFFF) + (sum >> 16);
        datalen -= 2;
    }

    if (datalen) sum += (WORD_T)(*(BYTE *)pdata);

    while (sum >> 16) sum = (sum & 0xFFFF) + (sum >> 16);
    return (WORD_T)~sum;
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
//void  DealData(int nDeviceHandle)
//{
//    MACPRO    *pmac;
//    WORD_T        wReadState, wReadLength;
//  BYTE        u_awEthRxBuf[CONST_SIZE_SENDPACKET];
//  if (nDeviceHandle >= MAX_NETDEVICE) return;
//
//    wReadState = Device_Read(g_hEthernet, (unsigned char *)u_awEthRxBuf, CONST_SIZE_SENDPACKET, 0);
//  wReadLength = wReadState;
//  memcpy(u_awEthRxBufTemp[nDeviceHandle], (WORD_T *)u_awEthRxBuf, wReadLength);
//
//  while (wReadState > 0)
//    {
//        pmac = (MACPRO *)u_awEthRxBufTemp[nDeviceHandle];
//        switch(pmac->ma_pro)
//        {
//            case PROTOCOL_ARP:
//                DealARP(nDeviceHandle, (MACARP *)pmac);
//                break;
//            case PROTOCOL_IP:
//                DealIP(nDeviceHandle, u_awEthRxBufTemp[nDeviceHandle], wReadLength);
//                break;
//            case PROTOCOL_TECH:
//                DealTECH(nDeviceHandle, u_awEthRxBufTemp[nDeviceHandle], wReadLength);
//                break;
//            default:
//              break;
//        }
//            wReadState = Device_Read(g_hEthernet, (unsigned char *)u_awEthRxBufTemp[nDeviceHandle], CONST_SIZE_SENDPACKET, 0);
//    }
//}
void    DealData(int nDeviceHandle)
{
    MACPRO      *pmac;
    WORD_T      wReadLength;
    if (nDeviceHandle >= MAX_NETDEVICE) return;

    memset(u_awEthRxBufTemp[nDeviceHandle], 0, sizeof(u_awEthRxBufTemp[nDeviceHandle]));
    wReadLength = Device_Read(g_hEthernet, (unsigned char *)u_awEthRxBufTemp[nDeviceHandle], CONST_SIZE_SENDPACKET, 0);

    while (wReadLength > 0)
    {
        pmac = (MACPRO *)u_awEthRxBufTemp[nDeviceHandle];
        switch (pmac->ma_pro)
        {
            case PROTOCOL_ARP:
                DealARP(nDeviceHandle, (MACARP *)pmac);
                break;
            case PROTOCOL_IP:
                DealIP(nDeviceHandle, u_awEthRxBufTemp[nDeviceHandle], wReadLength);
                break;
            case PROTOCOL_TECH:
                DealTECH(nDeviceHandle, u_awEthRxBufTemp[nDeviceHandle], wReadLength);
                break;
            default:
                break;
        }
        wReadLength = Device_Read(g_hEthernet, (unsigned char *)u_awEthRxBufTemp[nDeviceHandle], CONST_SIZE_SENDPACKET, 0);
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
void    DealARP(int nDeviceHandle, MACARP *pdata)
{
    int     i;
    MACARP  data;

    // Check whether match with source IP or session IP
    if (comparebuff(pdata->arp.ar_tpa, u_parameth.asrcIP[nDeviceHandle], CONST_LENGTH_IP) || \
            FindIPEthSession(nDeviceHandle, pdata->arp.ar_tpa) != -1)
    {
        if ((pdata->arp.ar_op) == PROTOCOL_ARP_REQUEST)
        {

            // Activate session connection
            ActivateEthSession(nDeviceHandle, pdata->arp.ar_sha, pdata->arp.ar_spa);

            // Deal ARP request
            for ( i = 0; i < CONST_LENGTH_MAC; i++)
            {
                data.mac.ma_tha[i] = pdata->arp.ar_sha[i];
                data.mac.ma_sha[i] = u_parameth.asrcMAC[nDeviceHandle][i];
                data.arp.ar_tha[i] = pdata->arp.ar_sha[i];
                data.arp.ar_sha[i] = u_parameth.asrcMAC[nDeviceHandle][i];
            }
            for ( i = 0; i < CONST_LENGTH_IP; i++)
            {
                data.arp.ar_tpa[i] = pdata->arp.ar_spa[i];
                data.arp.ar_spa[i] = pdata->arp.ar_tpa[i];
            }
            data.mac.ma_pro             = PROTOCOL_ARP;
            data.arp.ar_hwtype          = 0x100;
            data.arp.ar_prtype          = PROTOCOL_IP;
            data.arp.HwlenPrlen.LBYTE   = CONST_LENGTH_MAC * 2;
            data.arp.HwlenPrlen.HBYTE   = CONST_LENGTH_IP * 2;
            data.arp.ar_op              = PROTOCOL_ARP_REPLY;

            Device_Write(g_hEthernet, (BYTE *)&data, sizeof(MACARP), 0 );
        }
        else if ((pdata->arp.ar_op) == PROTOCOL_ARP_REPLY)
        {
            // Activate session connection
            ActivateEthSession(nDeviceHandle, pdata->arp.ar_sha, pdata->arp.ar_spa);
        }
    }
}

void    DealIP(int nDeviceHandle, WORD_T *pdata, WORD_T wlength)
{
    IPHR    *piphr;


    piphr = (IPHR *)(pdata + CONST_LENGTH_ETHERHEAD);

    if (piphr->VerType.LBYTE == PROTOCOL_IP_VERLEN)
    {
        switch (piphr->TTLProtocol.HBYTE)
        {
            case PROTOCOL_IP_ICMP:
                DealICMP(nDeviceHandle, pdata, wlength);
                break;
            case PROTOCOL_IP_UDP:
                DealUDP(nDeviceHandle, pdata, wlength);
                break;
            default:
                break;
        }
    }
}


void    BindAddrToSession(int nDeviceHandle, BYTEX *pdstMAC, BYTEX *pdstIP, WORD_T wsrcPort, WORD_T wdstPort)
{
    int     i;
    BYTEX   aIP[CONST_LENGTH_IP] = {{1, 2}, {3, 4}};

    if (nDeviceHandle >= MAX_NETDEVICE || u_parameth.awAllActive[nDeviceHandle]) return;

    for (i = 0; i < MAX_SESSIONCH; i++)
    {
        if (u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_IP && \
                u_parameth.sessionch[nDeviceHandle][i].wState == STATESESSIONCH_OPEN)
        {
            if (comparebuff(aIP, u_parameth.sessionch[nDeviceHandle][i].ioeth.adstIP, CONST_LENGTH_IP))
            {
                //if ( pdstIP->LBYTE == 169 && pdstIP->HBYTE == 254 && \
                //        (pdstIP + 1)->LBYTE == 11 && (pdstIP + 1)->HBYTE != 11 && (pdstIP + 1)->HBYTE != 12)
                if(wsrcPort == u_parameth.sessionch[nDeviceHandle][i].ioeth.wsrcPort)   
                {    
                    memcpy(u_parameth.sessionch[nDeviceHandle][i].ioeth.adstIP, pdstIP, CONST_LENGTH_IP * 2);
                    u_parameth.sessionch[nDeviceHandle][i].ioeth.wdstPort = wdstPort;
                }
            }
        }
    }
}


void    DealICMP(int nDeviceHandle, WORD_T *pdata, WORD_T wlength)
{
    WORD_T    *pSenddata;
    WORD_T  awTempData[96];
    MACPRO  *prmac, *psmac;
    IPHR    *prip, *psip;
    ICMP    *pricmp, *psicmp;
    int     i;
    //BYTEX*  pMatchData;

    if (wlength > 96) return;
    pricmp  = (ICMP *)(pdata + CONST_LENGTH_ETHERHEAD + CONST_LENGTH_IPHEAD);
    if (pricmp->TypeCode.LBYTE != PROTOCOL_IP) return;

    // check IP address whether match with self and in session table
    prip    = (IPHR *)(pdata + CONST_LENGTH_ETHERHEAD);
    if (comparebuff(prip->ip_dst, u_parameth.asrcIP[nDeviceHandle], CONST_LENGTH_IP) || \
            FindIPEthSession(nDeviceHandle, prip->ip_dst) != -1)
    {
        pSenddata   = awTempData;
//        memcpy(pSenddata, pdata, (wlength+1)/2);
        memcpy(pSenddata, pdata, wlength);
        prmac       = (MACPRO *)pdata;
        pricmp      = (ICMP   *)(pdata + CONST_LENGTH_ETHERHEAD + CONST_LENGTH_IPHEAD);
        psmac       = (MACPRO *)pSenddata;
        psip        = (IPHR   *)(pSenddata + CONST_LENGTH_ETHERHEAD);
        psicmp      = (ICMP   *)(pSenddata + CONST_LENGTH_ETHERHEAD + CONST_LENGTH_IPHEAD);
        //pMatchData  = (BYTEX  *)(pSenddata + CONST_LENGTH_ETHERHEAD + CONST_LENGTH_IPHEAD + CONST_LENGTH_ICMPHEAD);
        //if (comparebuff(pMatchData, (BYTEX*)g_logo, 5))
        //    BindAddrToSession(nDeviceHandle, prmac->ma_sha, prip->ip_src);
        // Activate session connection
        //ActivateEthSession(nDeviceHandle, prmac->ma_sha, prip->ip_src);

        // Deal ICMP
        for ( i = 0; i < CONST_LENGTH_MAC; i++)
        {
            psmac->ma_tha[i] = prmac->ma_sha[i];
            psmac->ma_sha[i] = prmac->ma_tha[i];
        }
        for ( i = 0; i < CONST_LENGTH_IP; i++)
        {
            psip->ip_src[i] = prip->ip_dst[i];
            psip->ip_dst[i] = prip->ip_src[i];
        }
        psicmp->TypeCode.LBYTE  = 0;
        psip->ip_cksum          = 0;
        psip->ip_cksum          = chksum((WORD_T *)psip, CONST_LENGTH_IPHEAD * 2);
        psicmp->ic_chksum       = 0;
        psicmp->ic_chksum       = chksum((WORD_T *)psicmp, CHANGEW(psip->ip_len) - CONST_LENGTH_IPHEAD * 2);

        Device_Write(g_hEthernet, (BYTE *)pSenddata, wlength, 0);
    }
}

void    DealUDP(int nDeviceHandle, WORD_T *pdata, WORD_T wlength)
{
    int     i;
    MACPRO  *pmac;
    IPHR    *piphr;
    UDPHR   *pudphr;

    pmac    = (MACPRO *)pdata;
    piphr   = (IPHR   *)(pdata + CONST_LENGTH_ETHERHEAD);
    pudphr  = (UDPHR  *)(pdata + CONST_LENGTH_ETHERHEAD + CONST_LENGTH_IPHEAD);

    //作为Server时，用于绑定Client的IP和Port
    BindAddrToSession(nDeviceHandle, pmac->ma_sha, piphr->ip_src, pudphr->udp_des, pudphr->udp_src);
     
    // Activate session connection
    ActivateEthSession(nDeviceHandle, pmac->ma_sha, piphr->ip_src);

    // look for matching session in session table
    u_parameth.nSessionHandle = \
                                FindAllParamEthSession(nDeviceHandle, pmac->ma_sha, piphr->ip_dst, piphr->ip_src, pudphr->udp_des, pudphr->udp_src);
    if (u_parameth.nSessionHandle != -1)
    {
        // call pre-defined callback function matching with one session to deal data.
        i = u_parameth.nSessionHandle % 10;
        if (u_parameth.sessionch[nDeviceHandle][i].ioeth.pNetFunc != NULL)
            (u_parameth.sessionch[nDeviceHandle][i].ioeth.pNetFunc)(u_parameth.nSessionHandle, pdata + CONST_LENGTH_PROHEAD, \
                    CHANGEW(piphr->ip_len) - CONST_LENGTH_IPHEAD * 2 - CONST_LENGTH_UDPHEAD * 2);
    }
    else
    {
        // call pre-defined default callback function not matching with any session to deal data.
        if (u_parameth.apDefaultIPFunc[nDeviceHandle] != NULL)
            (u_parameth.apDefaultIPFunc[nDeviceHandle])(pdata, wlength);
    }
}

void    DealTECH(int nDeviceHandle, WORD_T *pdata, WORD_T wlength)
{
    int     i;
    MACPRO  *pmac;

    pmac    = (MACPRO *)pdata;
    u_parameth.nSessionHandle = FindMACEthSession(nDeviceHandle, pmac->ma_sha);
    if (u_parameth.nSessionHandle != -1)
    {
        i = u_parameth.nSessionHandle % MAX_SESSIONCH;
        if (u_parameth.sessionch[nDeviceHandle][i].ioeth.pNetFunc != NULL)
            (u_parameth.sessionch[nDeviceHandle][i].ioeth.pNetFunc)(u_parameth.nSessionHandle, pdata + CONST_LENGTH_ETHERHEAD, \
                    wlength - CONST_LENGTH_ETHERHEAD * 2);
    }
    else
    {
        if (u_parameth.apDefaultTECHFunc[nDeviceHandle] != NULL)
            (u_parameth.apDefaultTECHFunc[nDeviceHandle])(pdata, wlength);
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
void    SendARPData(int nSessionHandle)
{
    int     i, j, k;

    if (nSessionHandle >= MAX_NETDEVICE * MAX_SESSIONCH) return;
    j = nSessionHandle / MAX_SESSIONCH;
    k = nSessionHandle % MAX_SESSIONCH;

    if (u_parameth.sessionch[j][k].wState == STATESESSIONCH_NULL || \
            u_parameth.sessionch[j][k].ioeth.wType != PROTOCOL_IP) return;

    memset(&u_parameth.arp, 0, sizeof(ARP));
    for (i = 0; i < CONST_LENGTH_MAC; i++)
    {
        u_parameth.arp.ArmMac[i].LBYTE  = 0xFF;
        u_parameth.arp.ArmMac[i].HBYTE  = 0xFF;
        u_parameth.arp.RootMac[i]       = u_parameth.asrcMAC[j][i];
        u_parameth.arp.Root_Mac[i]      = u_parameth.asrcMAC[j][i];
        u_parameth.arp.Arm_Mac[i].LBYTE = 0;
        u_parameth.arp.Arm_Mac[i].HBYTE = 0;
    }

    for (i = 0; i < CONST_LENGTH_IP; i++)
    {
        u_parameth.arp.Arm_IP[i]    = u_parameth.sessionch[j][k].ioeth.adstIP[i];
        u_parameth.arp.Root_IP[i]   = u_parameth.sessionch[j][k].ioeth.asrcIP[i];
    }

    u_parameth.arp.DataType         = PROTOCOL_ARP;
    u_parameth.arp.MacType          = 0x100;
    u_parameth.arp.ProType          = PROTOCOL_IP;
    u_parameth.arp.HLenPLen.LBYTE   = CONST_LENGTH_MAC * 2;
    u_parameth.arp.HLenPLen.HBYTE   = CONST_LENGTH_IP * 2;
    u_parameth.arp.Opcode           = PROTOCOL_ARP_REQUEST;

    Device_Write(g_hEthernet, (BYTE *)&u_parameth.arp, sizeof(ARP), 0);
}

void    RoutineCheckSessionState()
{
    int     i, j;
    WORD_T    wAllActive;

    for (i = 0; i < MAX_NETDEVICE; i++)
    {
        if (u_parameth.awAllActive[i]) continue;
        else wAllActive = 1;
        for (j = 0; j < MAX_SESSIONCH; j++)
        {
            if (u_parameth.sessionch[i][j].wState == STATESESSIONCH_OPEN && \
                    u_parameth.sessionch[i][j].ioeth.wType == PROTOCOL_IP)
            {
                wAllActive = 0;
                SendARPData(j + i * MAX_SESSIONCH);
            }
        }
        u_parameth.awAllActive[i] = wAllActive;
    }
}

void    ChangeSourceIP(int nDeviceHandle, BYTEX *psrcIP)
{
    int     i;

    if (nDeviceHandle >= MAX_NETDEVICE) return;
    memcpy(u_parameth.asrcIP[nDeviceHandle], psrcIP, CONST_LENGTH_IP);
    for (i = 0; i < MAX_SESSIONCH; i++)
    {
        if (u_parameth.sessionch[nDeviceHandle][i].wState != STATESESSIONCH_NULL && \
                u_parameth.sessionch[nDeviceHandle][i].ioeth.wType == PROTOCOL_IP)
            memcpy(u_parameth.sessionch[nDeviceHandle][i].ioeth.asrcIP, psrcIP, CONST_LENGTH_IP);
    }
}

void    ChangeSessionchIP(int nSessionHandle, BYTEX *psrcIP)
{
    int     i, j;

    if (nSessionHandle >= MAX_NETDEVICE * MAX_SESSIONCH) return;
    i = nSessionHandle / MAX_SESSIONCH;
    j = nSessionHandle % MAX_SESSIONCH;

    if (u_parameth.sessionch[i][j].wState != STATESESSIONCH_NULL && \
            u_parameth.sessionch[i][j].ioeth.wType == PROTOCOL_IP)
        memcpy(u_parameth.sessionch[i][j].ioeth.asrcIP, psrcIP, CONST_LENGTH_IP);
}

int    GetSessionchState(int nSessionHandle)
{
    int     i, j;

    if (nSessionHandle >= MAX_NETDEVICE * MAX_SESSIONCH) return -1;
    i = nSessionHandle / MAX_SESSIONCH;
    j = nSessionHandle % MAX_SESSIONCH;
    return u_parameth.sessionch[i][j].wState;
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
// wLength is calculated by 'BYTE'
WORD_T    SendNetData(int nSessionHandle, WORD_T *pw, WORD_T wLength)
{
    int     i, j;

    if (nSessionHandle >= MAX_NETDEVICE * MAX_SESSIONCH) return 0;
    i = nSessionHandle / MAX_SESSIONCH;
    j = nSessionHandle % MAX_SESSIONCH;

    if (u_parameth.sessionch[i][j].wState != STATESESSIONCH_ACTIVE) return 0;

    if (u_parameth.sessionch[i][j].ioeth.wType == PROTOCOL_IP)          return SendUDPData(i, j, pw, wLength);
    else if (u_parameth.sessionch[i][j].ioeth.wType == PROTOCOL_TECH)   return SendTECHData(i, j, pw, wLength);

    return 0;
}

// wLength is calculated by 'BYTE'
WORD_T  SendUDPData(int nDeviceHandle, int nHandle, WORD_T *pw, WORD_T wLength)
{
    DLCHR       dlchr;
    IPHR        iphr;
    UDPHR       udphr;
    WORD_T        wIPLength, wUDPLength;


    if (wLength > CONST_SIZE_SENDPACKET) return 0;

    // Fill DLC header
    memcpy(dlchr.acDstMAC, u_parameth.sessionch[nDeviceHandle][nHandle].ioeth.adstMAC, CONST_LENGTH_MAC * 2);
    memcpy(dlchr.acSrcMAC, u_parameth.asrcMAC[nDeviceHandle], CONST_LENGTH_MAC * 2);
    dlchr.wProType          = PROTOCOL_IP;


    // Fill IP header
    iphr.VerType.LBYTE      = PROTOCOL_IP_VERLEN;
    iphr.VerType.HBYTE      = 0;
    iphr.ip_fragoff         = 0;
    iphr.ip_cksum           = 0;
    iphr.TTLProtocol.LBYTE  = CONST_TIME_LIVE;
    iphr.TTLProtocol.HBYTE  = PROTOCOL_IP_UDP;
    iphr.ip_id              = CHANGEW(u_parameth.wSendPacketIndex);
    memcpy(iphr.ip_dst, u_parameth.sessionch[nDeviceHandle][nHandle].ioeth.adstIP, CONST_LENGTH_IP * 2);
    memcpy(iphr.ip_src, u_parameth.sessionch[nDeviceHandle][nHandle].ioeth.asrcIP, CONST_LENGTH_IP * 2);

    // Fill UDP header
    udphr.udp_src   = u_parameth.sessionch[nDeviceHandle][nHandle].ioeth.wsrcPort;
    udphr.udp_des   = u_parameth.sessionch[nDeviceHandle][nHandle].ioeth.wdstPort;
    udphr.udp_chk   = 0;

    // Fill UDP length and IP length
    wUDPLength      = CONST_LENGTH_UDPHEAD * 2 + wLength;
    udphr.udp_len   = CHANGEW(wUDPLength);

    wIPLength       = CONST_LENGTH_IPHEAD * 2 + wUDPLength;
    iphr.ip_len     = CHANGEW(wIPLength);
    iphr.ip_cksum   = chksum((WORD_T *)&iphr, CONST_LENGTH_IPHEAD * 2);

    // Copy each header and data
    memcpy(u_awEthTxBufTemp[nDeviceHandle], &dlchr, CONST_LENGTH_ETHERHEAD * 2);
    memcpy(u_awEthTxBufTemp[nDeviceHandle] + CONST_LENGTH_ETHERHEAD, &iphr, CONST_LENGTH_IPHEAD * 2);
    memcpy(u_awEthTxBufTemp[nDeviceHandle] + CONST_LENGTH_ETHERHEAD + CONST_LENGTH_IPHEAD, &udphr, CONST_LENGTH_UDPHEAD * 2);

    memcpy(u_awEthTxBufTemp[nDeviceHandle] + CONST_LENGTH_PROHEAD, pw, (wLength + 1));


    if (u_parameth.wSendPacketIndex == 0xFFFF)  u_parameth.wSendPacketIndex = 1;
    else                                        u_parameth.wSendPacketIndex ++;

    // Send data
    if (Device_Write(g_hEthernet, (BYTE *)u_awEthTxBufTemp[nDeviceHandle], wLength + CONST_LENGTH_PROHEAD * 2, 0) == NIC_E_TOKEN_FAILURE)  return 0;
    return 1;
}

// wLength is calculated by 'BYTE'
WORD_T    SendTECHData(int nDeviceHandle, int nHandle, WORD_T *pw, WORD_T wLength)
{
    DLCHR   dlchr;

    if (wLength > CONST_SIZE_SENDPACKET) return 0;

    // Fill DLC header
    memcpy(dlchr.acDstMAC, u_parameth.sessionch[nDeviceHandle][nHandle].ioeth.adstMAC, CONST_LENGTH_MAC);
    memcpy(dlchr.acSrcMAC, u_parameth.asrcMAC[nDeviceHandle], CONST_LENGTH_MAC);
    dlchr.wProType = PROTOCOL_TECH;

    // Copy header and data
    memcpy(u_awEthTxBufTemp[nDeviceHandle], &dlchr, CONST_LENGTH_ETHERHEAD);
    memcpy(u_awEthTxBufTemp[nDeviceHandle] + CONST_LENGTH_ETHERHEAD, pw, (wLength + 1) >> 1);

    // Send data
    if (Device_Write(g_hEthernet, (BYTE *)u_awEthTxBufTemp[nDeviceHandle], wLength + CONST_LENGTH_ETHERHEAD * 2, 0) == NIC_E_TOKEN_FAILURE) return 0;
    return 1;
}

