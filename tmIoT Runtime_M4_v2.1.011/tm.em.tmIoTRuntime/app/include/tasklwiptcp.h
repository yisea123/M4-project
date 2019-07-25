/*==========================================================================+
|  File     : lwiptcp.h                                                     |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : Jia                                                           |
|  Version  : V1.00                                                         |
|  Creation : 2016/08/02                                                    |
|  Revision :                                                               |
+==========================================================================*/
#ifndef D_LWIPTCP_H
#define D_LWIPTCP_H

#include "kernel/include/type.h"
#include "driver/include/_eeprom_at24c16.h"
#include "kernel/include/device.h"
#include "studioprotocol.h"
#include "taskmodbustcp.h"

/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef struct tagINETMSG
{
    BYTE dwDeviceIP[4];
    WORD  wDevicePort;
    BYTE dwGateWay[4];
    BYTE dwMask[4];
    BYTE  byMacAddr[6];
    BYTE dwSeverIP[4];
    WORD  wSeverPort;
}iNetMeg;
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/
#define INETMODENOTUSED                     0x00
#define INETMODECLIENT                      0x11
#define INETMODESERVER                      0x22
#define INETPROTYPETCP                      0x33
#define INETPROTYPEUDP                      0x44

#define     Net_AgainLink_Timer             20000
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
void        CreateLwipTcp();
void        InitLwipTcp();
void        AddDnCounter(long* ptCounter);
//void        SetLwipUdpIp( void );
void 		Init_RemoteIP_Tcp();
void 		Read_MacAddr_FromEeprom();
struct 		tcp_pcb*         u_Tcp_pcb;
extern iNetMeg       iNetMessage;
extern BYTE g_byMacAdderBuf[8];
extern BYTE g_byiNetTaskFlag;
extern BYTE g_UpdateFlag;
extern BYTE 		u_aLocalIP_Add[4];
extern BYTE 		u_aNetMask_Add[4];
extern BYTE 		u_aGateWay_Add[4];
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/

#endif
