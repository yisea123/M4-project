#ifndef TSAK_LWIP_UDP_H
#define TSAK_LWIP_UDP_H

#include "kernel/include/type.h"
#include "kernel/include/task.h"
#include "studioprotocol.h"
#include "lwip-1.4.1/tm/lwiplib_1.4.1.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/

/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/

extern      void        Lwip_UdpWrite_Studio(BYTE *bypData, WORD_T  wLength);
extern      void        AddDnCounter(long* ptCounter);
extern      void        CreateLwipUdp();
extern      void        InitLwipUdp();
extern      void        del_list(BYTE index);
extern      void 		Init_RemoteIP();
extern      struct 		udp_pcb*       u_Udp_pcb;
extern		struct 		udp_pcb*       u_Udp_pcb_Add;
extern      NETCONNECTMESG 		 g_sRemoteMesg;
extern      NETCONNECTMESG 		 g_sAddRemoteMesg;
/*==============================================================================+
|           External Variable                                                   |
+==============================================================================*/
extern      BYTE       u_aRemoteIP[4];
extern      TASK       TaskLwipUdp;

#endif
