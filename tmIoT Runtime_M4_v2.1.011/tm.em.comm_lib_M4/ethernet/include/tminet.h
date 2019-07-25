/*==========================================================================+
|  File     : tminet.h                                                      |
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
#ifndef D_TMINET_H
#define D_TMINET_H

#include    "kernel/include/type.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
#define     MAX_BUFFER_SIZE     500

#define     PROTOCOL_VERSION    5

//#define     PROTOCOL_ID_SYNCH_FORMULA                           0x09000000  //900

#define     PROTOCOL_ID_APPLY_SYNCH_IP                          0x01000000
#define     PROTOCOL_ID_APPLY_PROTOCOL_VERSION                  0x01020000
#define     PROTOCOL_ID_SEND_PROTOCOL_VERSION                   0x01020001
#define     PROTOCOL_ID_SEND_DYNAMICS_PROTOCOL_COMMAND          0x01020003
#define     PROTOCOL_ID_APPLY_DYNAMICS_PROTOCOL_LIST            0x01020002
#define     PROTOCOL_ID_ADD_DYNAMIC_PROTOCOL                    0x01020004
#define     PROTOCOL_ID_END_SYNCH_DYNAMICS_PROTOCOL             0x01020007
#define     PROTOCOL_ID_INET_SEND_HEART_BEAT                    0x01000003
#define     PROTOCOL_ID_MASTER_SEND_HEART_BEAT                  0x01000002
#define     PROTOCOL_ID_SEND_SYNCH_TIME                         0x01000001
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef     void (* pFUNC)(int, BYTE *, WORD);

typedef struct tagPROTOCOLMAP
{
    DWORD   dwProID;
    pFUNC   pFunc;  
}PROTOCOLMAP;

typedef struct tagPROYOCOLIDTYPE
{
    BYTE    byType;
    BYTE    bySubType;
    WORD    wAttribute;       
}PROTOCOLID;
typedef struct tagHEADER
{
    WORD    wVersion;
    DWORD   dwCRC;
    DWORD   dwReserve;
    WORD    wReserve;
    WORD    wEcho;
    WORD    wTimestamp;
    DWORD   dwProtocolID;
}HEADER;

#pragma pack(push)  
#pragma pack(1)
typedef struct       tagProtocolConnect_ID
{
    DWORD             dwID;       
    WORD              wID_Flag;         
} PROTOCONNECTID;
#pragma pack(pop)  

#pragma pack(push)  
#pragma pack(1)
typedef    struct        tagTrigger  
{
    DWORD             dwKind ;    
    DWORD             dwParam[4];             
} TRIGGER;
#pragma pack(pop) 

#pragma pack(push)  
#pragma pack(1)
typedef struct tagSynchProtocol  
{
    DWORD                  dwProtocolID;
    WORD                   wVersion;
    DWORD                  dwProtoConCount; 
    PROTOCONNECTID         pProtoContent[150];  
    DWORD                  dwTriggerCount; 
    TRIGGER                pTrigger[5];    
}SYNCHPROTO;
#pragma pack(pop) 

typedef struct tagIpv4_Type     
{                                   
    WORD  wType;
    BYTE  abyIP[4];       
}IPV4TYPE;

typedef struct tagDataTime_Type  
{                     
    WORD  wType; 
    WORD  wYear;
    WORD  wMonth;
    WORD  wDay;
    WORD  wHour;
    WORD  wMinute;
    WORD  wSecond;
    WORD  wMilliSecond;               
}DATATIMETYPE;

typedef   struct tagMac_Type
{                     
    WORD  wType; 
    BYTE  abyMacAdd[6]; 
}MACTYPE;
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Class declaration -                                             |
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/

/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
#endif

