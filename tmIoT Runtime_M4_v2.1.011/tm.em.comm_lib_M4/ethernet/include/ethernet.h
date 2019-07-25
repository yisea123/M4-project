/*==========================================================================+
|  Class    : Ethernet                                                      |
|             Ethernet Protocol Function                                    |
|  Task     : Ethernet service routine  file                                |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   :                                                               |
|  Version  : V1.00                                                         |
|  Creation :                                                               |
|  Revision :                                                               |
+==========================================================================*/

#ifndef     D__ETHERNET
#define     D__ETHERNET

#include    "kernel/include/type.h"
#include    "driver/include/_ethernet.h"
#include    "driver/include/_timer.h"

#ifdef      __cplusplus
extern      "C" {
#endif

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define     PROTOCOL_IP                 0x0008          // IP protocol flag
#define     PROTOCOL_ARP                0x0608          // ARP protocol flag
#define     PROTOCOL_TECH               0xA588          // Techmation protocol flag 0x88A5
#define     PROTOCOL_ETHERCAT           0xA488          // EtherCAT protocol flag 0x88A4

#define     PROTOCOL_IP_ICMP            1               // ICMP protocol flag
#define     PROTOCOL_IP_UDP             17              // UDP protocol flag
#define     PROTOCOL_ARP_REQUEST        0x0100          // ARP request flag
#define     PROTOCOL_ARP_REPLY          0x0200          // ARP reply flag
#define     PROTOCOL_ICMP_REQUEST       0x0008          // ICMP request flag
#define     PROTOCOL_ICMP_REPLY         0x0000          // ICMP reply flag
#define     PROTOCOL_IP_VERLEN          0x45            // ip protocol version
#define     PROTOCOL_UDP_SPORT          0x3930          // Source machine port number
#define     PROTOCOL_UDP_DPORT          0x3930          // Destination machine port number

#define     CONST_LENGTH_IP             4/2             // IP length  
#define     CONST_LENGTH_MAC            6/2
#define     CONST_LENGTH_ETHERHEAD      14/2            // ethernet header length           
#define     CONST_LENGTH_IPHEAD         20/2            // ip header length                 
#define     CONST_LENGTH_ARPHEAD        8/2             // arp header length                
#define     CONST_LENGTH_UDPHEAD        8/2             // udp header length                
#define     CONST_LENGTH_ICMPHEAD       8/2             // icmp header length               
#define     CONST_LENGTH_PROHEAD        (14+20+8)/2     // protocol header length           

#define     CONST_TIME_LIVE             128             // packet live time                 

#define     CONST_SIZE_BUFFER           1400            // buffer size 1518*2-->1518
#define     CONST_SIZE_DATA             1472            // data size of one packet
#define     CONST_SIZE_SENDPACKET       600            // size of one packet 'calculated by BYTE',Default was 1400

#define     NETDEVICE_NONE              0
#define     NETDEVICE_CS8900A           1
#define     NETDEVICE_AX88796B          2

#define     MAX_NETDEVICE               1
#define     MAX_SESSIONCH               6

#define     STATESESSIONCH_NULL         0
#define     STATESESSIONCH_OPEN         1
#define     STATESESSIONCH_ACTIVE       2


#define     NIC_SUCCESS              0x0000
#define     NIC_E_TOKEN_FAILURE      0x0000
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef void (* DFTFUNC)(WORD_T *, WORD_T);
typedef void (* NETFUNC)(int, WORD_T *, WORD_T);


typedef struct tagBYTEX
{
    WORD_T  LBYTE: 8;
    WORD_T  HBYTE: 8;
} BYTEX;

typedef struct tagDLCHR
{
    BYTEX       acDstMAC[CONST_LENGTH_MAC];
    BYTEX       acSrcMAC[CONST_LENGTH_MAC];
    WORD_T      wProType;
} DLCHR;

typedef struct  tagIPHR
{
    BYTEX       VerType;
    WORD_T      ip_len;
    WORD_T      ip_id;
    WORD_T      ip_fragoff;
    BYTEX       TTLProtocol;
    WORD_T      ip_cksum;
    BYTEX       ip_src[CONST_LENGTH_IP];
    BYTEX       ip_dst[CONST_LENGTH_IP];
} IPHR;

typedef struct tagUDPHR
{
    WORD_T      udp_src;
    WORD_T      udp_des;
    WORD_T      udp_len;
    WORD_T      udp_chk;
} UDPHR;

typedef struct tagARP
{
    BYTEX       ArmMac[CONST_LENGTH_MAC], RootMac[CONST_LENGTH_MAC];
    WORD_T      DataType;
    WORD_T      MacType, ProType;
    BYTEX       HLenPLen;
    WORD_T      Opcode;
    BYTEX       Root_Mac[CONST_LENGTH_MAC], Root_IP[CONST_LENGTH_IP], Arm_Mac[CONST_LENGTH_MAC], Arm_IP[CONST_LENGTH_IP];
} ARP;

typedef struct  tagMACPRO
{
    BYTEX       ma_tha[CONST_LENGTH_MAC];
    BYTEX       ma_sha[CONST_LENGTH_MAC];
    short       ma_pro;
} MACPRO;

typedef struct tagARPPRO
{
    short       ar_hwtype;
    short       ar_prtype;
    BYTEX       HwlenPrlen;
    short       ar_op;
    BYTEX       ar_sha[CONST_LENGTH_MAC];
    BYTEX       ar_spa[CONST_LENGTH_IP];
    BYTEX       ar_tha[CONST_LENGTH_MAC];
    BYTEX       ar_tpa[CONST_LENGTH_IP];
} ARPPRO;

typedef struct tagMACARP
{
    MACPRO      mac;
    ARPPRO      arp;
} MACARP;

typedef struct tagICMP
{
    BYTEX       TypeCode;
    WORD_T      ic_chksum;
    WORD_T      ic_iden;
    WORD_T      ic_sequ;
} ICMP;

// zholy090722 add
typedef struct tagIOETH
{
    WORD_T        wType;                                      // PROTOCOL_IP, PROTOCOL_ARP, PROTOCOL_TECH, PROTOCOL_ETHERCAT
    BYTEX       adstMAC[CONST_LENGTH_MAC];                  // destination MAC (when using IP protocol need get this address by ARP or other method)
    BYTEX       asrcIP[CONST_LENGTH_IP];
    BYTEX       adstIP[CONST_LENGTH_IP];
    WORD_T      wsrcPort;
    WORD_T      wdstPort;
    NETFUNC     pNetFunc;                                   // callback function
} IOETH;

typedef struct tagSESSIONCH
{
    WORD_T        wState;                                     // 0: not open; 1: open but not active; 2: open and active.
    IOETH       ioeth;
} SESSIONCH;


typedef struct tagCONFIGNET
{
    int         nDeviceHandle;
    DFTFUNC     pDefaultIPFunc;
    DFTFUNC     pDefaultTECHFunc;
    BYTEX       asrcIP[CONST_LENGTH_IP];
//  BUFFER      buffer;
} CONFIGNET;

typedef struct tagPARAMETH
{
    int         nSessionHandle;                              // current processing session handle
    WORD_T        wSendPacketIndex;
    WORD_T        awAllActive[MAX_NETDEVICE];                  // indicate whether connect destination IP with destination MAC when using IP protocol

    DFTFUNC     apDefaultIPFunc[MAX_NETDEVICE];              // used in IP protocol for not matching any session
    DFTFUNC     apDefaultTECHFunc[MAX_NETDEVICE];            // used in techmation protocol for not matching any session

    BYTEX       asrcIP[MAX_NETDEVICE][CONST_LENGTH_IP];      // default source IP address
    BYTEX       asrcMAC[MAX_NETDEVICE][CONST_LENGTH_MAC];    // defult source MAC

    SESSIONCH   sessionch[MAX_NETDEVICE][MAX_SESSIONCH];     // session table (use source IP address or other for special session)

    ARP         arp;
} PARAMETH;

/*==========================================================================+
|           Function and Class prototype                                    |
+==========================================================================*/
extern  HANDLE_T    g_hEthernet ;

WORD_T  OpenEthernet(CONFIGNET *pconfignet, BYTE , BYTE *mac);
void    CloseEthernet(void);

void    ResetParamEth(BYTE *mac);
int     OpenEthSession(int nDeviceHandle, IOETH *pIOETH);
void    CloseEthSession(int nSessionHandle);
void    ChangeSourceIP(int nDeviceHandle, BYTEX *psrcIP);
void    ChangeSessionchIP(int nSessionHandle, BYTEX *psrcIP);
int     GetSessionchState(int nSessionHandle);

void    DealData(int nDeviceHandle);
WORD_T    SendNetData(int nSessionHandle, WORD_T *pw, WORD_T wLength);
void    SendARPData(int nSessionHandle);

/*==========================================================================+
|           External                                                        |
+==========================================================================*/
extern  ROUTINE g_routinechecksessionstate;

/*---------------------------------------------------------------------------+
|           Helpers                                                          |
+---------------------------------------------------------------------------*/

#ifdef      __cplusplus
}
#endif

#endif

