/*==========================================================================+
|  File     : tminet.c                                                      |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : Jia                                                           |
|  Version  : V1.00                                                         |
|  Creation : 2016/04/15                                                    |
|  Revision :                                                               |
+==========================================================================*/
#include "ethernet/include/tminet.h"
#include "ethernet/include/ethernet.h"
/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
void    SendProtocolVersion(int nSession, BYTE *pData, WORD wLength);
void    RespProtocolVersion(int nSession, BYTE *pData, WORD wLength);
void    RespDynamicProCommand(int nSession, BYTE *pData, WORD wLength);
void    RespDynamicProList(int nSession, BYTE *pData, WORD wLength);
void    RespAddDynamicProtocol(int nSession, BYTE *pData, WORD wLength);
void    EndSynchDynamicProtocol(int nSession, BYTE *pData, WORD wLength);
void    RespHeartBeat(int nSession, BYTE *pData, WORD wLength);
void    RespSynchIPResult(int nSession, BYTE *pData, WORD wLength);
void    ApplySynchIP(int nSession);
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
const    PROTOCOLMAP    ProtocolMap[] = 
{                                                                                    
    {PROTOCOL_ID_APPLY_PROTOCOL_VERSION           ,  RespProtocolVersion         },  //120
    {PROTOCOL_ID_SEND_DYNAMICS_PROTOCOL_COMMAND   ,  RespDynamicProCommand       },  //123
    {PROTOCOL_ID_APPLY_DYNAMICS_PROTOCOL_LIST     ,  RespDynamicProList          },  //122
    {PROTOCOL_ID_ADD_DYNAMIC_PROTOCOL             ,  RespAddDynamicProtocol      },  //124
    {PROTOCOL_ID_END_SYNCH_DYNAMICS_PROTOCOL      ,  EndSynchDynamicProtocol     },  //127
    {PROTOCOL_ID_INET_SEND_HEART_BEAT             ,  RespHeartBeat               },  //103
    {PROTOCOL_ID_SEND_SYNCH_TIME                  ,  RespSynchIPResult           },  //101
};
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
#define    SYNCHPRONUM       10
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/
SYNCHPROTO     u_aProtoIdList[SYNCHPRONUM];
DWORD          u_dwProtoIdCount = 0;

IPV4TYPE       u_Ipv4;
DATATIMETYPE   u_DataTime;
WORD           u_RunProtocolStatus = 0;
MACTYPE        u_srcMac;
/*==============================================================================+
|           Class declaration                                                   |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
BYTE  u_abySendBuffer[1000];
/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/

/*==============================================================================+
|           Class declaration                                                   |
+==============================================================================*/
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
void    RunProtocol(int nSession)
{
    if(u_RunProtocolStatus == 0)
    {
        ApplySynchIP(nSession);
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
void    SetMacAddr(WORD wType, BYTE* byMac)
{
    u_srcMac.wType = wType;
    memcpy(u_srcMac.abyMacAdd, byMac, sizeof(u_srcMac.abyMacAdd));   
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
void    ParseInetData(int nSession, BYTE *pData, WORD wLength)
{
    int i;
    HEADER  *Header = (HEADER*)pData;
   
    for(i = 0; i < sizeof(ProtocolMap)/sizeof(PROTOCOLMAP); i++)
    {
        if(Header->dwProtocolID = ProtocolMap[i].dwProID)
        {
            if(ProtocolMap[i].pFunc != NULL)
            {
                ProtocolMap[i].pFunc(nSession, pData, wLength);
                break;
            }
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
// 100协议
void    ApplySynchIP(int nSession)
{
    HEADER  Header;
    DWORD   dwAssigeIP = 1;
    DWORD   dwControllerType = 0x0d;    
    WORD    wLength;    

    Header.wVersion = PROTOCOL_VERSION;
    Header.dwCRC = 0;
    Header.dwReserve = 0;
    Header.wReserve = 0;
    Header.wEcho = 0;
    Header.wTimestamp = 0;
    Header.dwProtocolID = PROTOCOL_ID_APPLY_SYNCH_IP;

    memset(u_abySendBuffer, 0, sizeof(u_abySendBuffer));
    memcpy(u_abySendBuffer, (BYTE*)&Header, sizeof(HEADER));
    wLength = sizeof(HEADER);
    memcpy(u_abySendBuffer + wLength, (BYTE*)&u_srcMac, sizeof(u_srcMac));
    wLength +=  sizeof(u_srcMac);
    memcpy(u_abySendBuffer + wLength, &dwAssigeIP, sizeof(dwAssigeIP));
    wLength += sizeof(dwAssigeIP);
    memcpy(u_abySendBuffer + wLength, &dwControllerType, sizeof(dwControllerType));

    SendNetData(nSession, (WORD_T *)&u_abySendBuffer, wLength);
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
//101协议
void    RespSynchIPResult(int nSession, BYTE *pData, WORD wLength)
{
    BYTE*   pTempbuf = pData;
    pTempbuf = pTempbuf+28;

    memcpy(&u_Ipv4, pTempbuf, sizeof(u_Ipv4));
    ChangeSourceIP(0, (BYTEX *)&u_Ipv4.abyIP);
    
    pTempbuf += sizeof(u_Ipv4);
    memcpy(&u_DataTime, pTempbuf, sizeof(u_DataTime));

    SendProtocolVersion(nSession, pData, wLength);
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
//120协议
void    RespProtocolVersion(int nSession, BYTE *pData, WORD wLength)
{
    u_RunProtocolStatus = 1;
    SendProtocolVersion(nSession, pData, wLength);  
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
//121协议
void    SendProtocolVersion(int nSession, BYTE *pData, WORD wLength)
{
    HEADER  Header;
    DWORD   dwVersion = 5; 
    WORD    wLen = 0;    

    Header.wVersion = PROTOCOL_VERSION;
    Header.dwCRC = 0;
    Header.dwReserve = 0;
    Header.wReserve = 0;
    Header.wEcho = 0;
    Header.wTimestamp = 0;
    Header.dwProtocolID = PROTOCOL_ID_SEND_PROTOCOL_VERSION;

    memset(u_abySendBuffer, 0, sizeof(u_abySendBuffer));
    memcpy(u_abySendBuffer, (BYTE*)&Header, sizeof(HEADER));
    wLen = sizeof(HEADER);
    memcpy(u_abySendBuffer + wLen, (BYTE*)&u_srcMac, sizeof(u_srcMac));
    wLen += sizeof(u_srcMac);
    memcpy(u_abySendBuffer + wLen, &dwVersion, sizeof(dwVersion));
    wLen += sizeof(dwVersion);

    SendNetData(nSession, (WORD_T *)&u_abySendBuffer, wLen);
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
//102,103协议
void    RespHeartBeat(int nSession, BYTE *pData, WORD wLength)
{
    HEADER  Header;
    WORD    wLen = 0;

    Header.wVersion = PROTOCOL_VERSION;
    Header.dwCRC = 0;
    Header.dwReserve = 0;
    Header.wReserve = 0;
    Header.wEcho = 0;
    Header.wTimestamp = 0;
    Header.dwProtocolID = PROTOCOL_ID_MASTER_SEND_HEART_BEAT;

    memset(u_abySendBuffer, 0, sizeof(u_abySendBuffer));
    memcpy(u_abySendBuffer, (BYTE*)&Header, sizeof(HEADER));
    wLen = sizeof(HEADER);
    memcpy(u_abySendBuffer + wLen, (BYTE*)&u_srcMac, sizeof(u_srcMac));
    wLen += sizeof(u_srcMac);

    SendNetData(nSession, (WORD_T *)&u_abySendBuffer, wLen);
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
//123协议
void    RespDynamicProCommand(int nSession, BYTE *pData, WORD wLength)
{
    int i;
    HEADER  *Header = (HEADER*)pData;
    BYTE*   pTempbuf = pData;

    pTempbuf = pTempbuf+30;
    u_dwProtoIdCount = *(DWORD*)pTempbuf;

    pTempbuf = pTempbuf + sizeof(u_dwProtoIdCount);
    for(i = 0; i < u_dwProtoIdCount; i++)
    {
        u_aProtoIdList[i].dwProtocolID = *(DWORD*)pTempbuf;
        pTempbuf = pTempbuf + sizeof(DWORD);
    }

    Header->wEcho = ~(Header->wEcho);
    memset(u_abySendBuffer, 0, sizeof(u_abySendBuffer));  
    memcpy(u_abySendBuffer, pData, wLength);
    memcpy(u_abySendBuffer, Header, sizeof(HEADER));

    SendNetData(nSession, (WORD_T *)&u_abySendBuffer, wLength);
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
//122协议
void    RespDynamicProList(int nSession, BYTE *pData, WORD wLength)
{
    HEADER  *Header = (HEADER*)pData;  
    
    Header->wEcho = ~(Header->wEcho);
    memset(u_abySendBuffer, 0, sizeof(u_abySendBuffer));  
    memcpy(u_abySendBuffer, pData, wLength);
    memcpy(u_abySendBuffer, Header, sizeof(HEADER)); 

    SendNetData(nSession, (WORD_T *)&u_abySendBuffer, wLength);
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
//124协议
void    RespAddDynamicProtocol(int nSession, BYTE *pData, WORD wLength)
{
    int i;
    DWORD   dwProtocolID;
    DWORD   dwProtoIdCount;
    HEADER  *Header = (HEADER*)pData; 
    BYTE*   pTempbuf = pData;

    pTempbuf = pTempbuf + 28;
    dwProtocolID = *(DWORD*)pTempbuf;

    pTempbuf = pTempbuf + 6;
    dwProtoIdCount = *(DWORD*)pTempbuf;
    pTempbuf = pTempbuf + 4;

    for(i = 0; i < u_dwProtoIdCount; i++)
    {
        if(dwProtocolID == u_aProtoIdList[i].dwProtocolID)
        {
            u_aProtoIdList[i].dwProtoConCount = dwProtoIdCount;
            memcpy(u_aProtoIdList[i].pProtoContent, pTempbuf, sizeof(PROTOCONNECTID)*dwProtoIdCount);
        }  
    }

    Header->wEcho = ~(Header->wEcho);
    memset(u_abySendBuffer, 0, sizeof(u_abySendBuffer));  
    memcpy(u_abySendBuffer, pData, wLength);
    memcpy(u_abySendBuffer, Header, sizeof(HEADER));

    SendNetData(nSession, (WORD_T *)&u_abySendBuffer, wLength);
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
//127协议
void    EndSynchDynamicProtocol(int nSession, BYTE *pData, WORD wLength)
{
    //动态协议结束后，发送心跳
    RespHeartBeat(nSession, pData, wLength);
}
