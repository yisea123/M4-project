/*
 * studiolwipudp.h
 *
 *  Created on: 2016Äê12ÔÂ8ÈÕ
 *      Author: pro
 */

#ifndef APP_STUDIOLWIPUDP_H_
#define APP_STUDIOLWIPUDP_H_

#include   "tasklwipudp.h"
#include   "tasklwiptcp.h"
#include   "InitDevice.h"
#include   "task_Runtime.h"
#include   "taskcanopenmaster.h"
#include   "taskcanopenslave.h"
#include 	"taskmodbustcp.h"
//#include   "database.h"
#include   "comlib.h"
//#include   "taskcanopen.h"

//#include    "kernel/include/type.h"
//#include    "eeprom.h"
//#include    "driver/include/_timer.h"
//#include    "iNet.h"
//#include    "_flash.h"
#include	"taskusb.h"

//#define  CAN_MASTER
//#define  CAN_SLAVE
//#define    EEPROM


typedef struct tagpProtocolMap
{
    DWORD_T    dwProId;
    BOOL_T     (*pFun)(BYTE_T  FlagRecvOrSend);

}PROTOMAP_T;

typedef struct taskdata
{
    BYTE_T     Number;
    BYTE_T     Class;
    WORD_T	   Cycle;
    WORD_T	   StartComponentNo;
    WORD_T	   EndComponentNo;
}TASKDATA_T;

enum PROTOCAL_INDEX1
{
    APPLY_HANDSHAKE,
    APPLY_STARTFRAME,
    APPLY_DATAFRAME,
    APPLY_ENDFRAME,
    APPLY_SAVECONFIGDATA,
    APPLY_REALTIMEDATAATTR,
    APPLY_CONFIGSERVERIP,
    APPLY_CONFIGTMIOTIP,
    APPLY_DEBUGBKPT,
	APPLY_COMMDATAFRAME,
	APPLY_GLOBALVARIABLEFRAME,
    APPLY_DEBUGRUN,
    APPLY_DEBUGPAUSE,
    APPLY_DEBUGSTEP,
    APPLY_DEBUGMODE,
	APPLY_TASKDATA,
	APPLY_REALTIMEDATA,
	APPLY_COMPONENTSCANCNT,
	APPLY_MODBUSRTUPARA,
	APPLY_MODBUSRTUOD,
	APPLY_MODBUSRTUENDFRAM,
	APPLY_MODBUSRTUSAVECONFIGDATA,
	APPLY_CANSLAVEPARA,
	APPLY_DOWNLOADOD,
	APPLY_CANENDFRAME,
	APPLY_CANSAVECONFIGDATA,
	APPLY_INETSETIPINFO,
	APPLY_INETGETIPINFO,
	APPLY_INETPARA,
	APPLY_INETODData,
	APPLY_INETENDFRAME,
	APPLY_INETCONFIGDATA,
	APPLY_MODBUSTCPPARA,
	APPLY_MODBUSTCPOD,
	APPLY_MODBUSTCPENDFRAM,
	APPLY_MODBUSTCPSAVECONFIGDATA,

    REPLY_HANDSHAKE ,
    REPLY_STARTFRAME,
    REPLY_DATAFRAME,
    REPLY_ENDFRAME,
    REPLY_SAVECONFIGDATA,
    REPLY_REALTIMEDATAATTR,
    REPLY_CONFIGSERVERIP,
    REPLY_CONFIGTMIOTIP,
    REPLY_DEBUGBKPT,
	REPLY_COMMDATAFRAME,
	REPLY_GLOBALVARIABLEFRAME,
    REPLY_DEBUGRUN,
    REPLY_DEBUGPAUSE,
    REPLY_DEBUGSTEP,
    REPLY_DEBUGMODE,
	REPLY_TASKDATA,
	REPLY_REALTIMEDATA,
	REPLY_COMPONENTSCANCNT,
	REPLY_MODBUSRTUPARA,
	REPLY_MODBUSRTUOD,
	REPLY_MODBUSRTUENDFRAM,
	REPLY_MODBUSRTUSAVECONFIGDATA,
	REPLY_CANSLAVEPARA,
	REPLY_DOWNLOADOD,
	REPLY_CANENDFRAME,
	REPLY_CANSAVECONFIGDATA,
	REPLY_INETSETIPINFO,
	REPLY_INETGETIPINFO,
	REPLY_INETPARA,
	REPLY_INETODData,
	REPLY_INETENDFRAME,
	REPLY_INETCONFIGDATA,
	REPLY_MODBUSTCPSETMODEINFO,
	REPLY_MODBUSTCPODDATA,
	REPLY_MODBUSTCPENDFRAM,
	REPLY_MODBUSTCPSAVEFRAM,

    PRO_NUM
};

//enum protocol_head
//{
//    protocol_H_len,
//    protocol_L_len,
//    protocol_H_version,
//    protocol_L_version,
//    protocol_packetstamp,
//    protocol_flag,
//    protocol_proid1,
//    protocol_proid2,
//    protocol_proid3,
//    protocol_proid4,
//    protocol_controlid1,
//    protocol_controlid2,
//    protocol_controlid3,
//    protocol_controlid4,
//    protocol_controlid5,
//    protocol_controlid6,
//    protocol_content,
//};

#define     FLAG_RECEIVE                             0
#define     FLAG_SEND                                1

#define    	MaxCommData			 					 500
#define    	MaxGlobalVariable			 			 100

#define    	MaxTask			 						 8

#define     SHIFT24                                  0x18
#define     SHIFT16                                  0x10
#define     SHIFT08                                  0x08
#define     SHIFT00                                  0x00
#define     TYPEF8                                   0xFFFFFFFF
#define     TYPEF2                                   0xFF
#define     TYPEF4                                   0xFFFF

#define     IPV4IP_POSITION1                         0x10
#define     IPV4IP_POSITION2                         0x11
#define     IPV4IP_POSITION3                         0x12
#define     IPV4IP_POSITION4                         0x13

#define     IPV4PORT_POSITION1                       0x14
#define     IPV4PORT_POSITION2                       0x15

#define     IPV4MASK_POSITION1                       0x14
#define     IPV4MASK_POSITION2                       0x15
#define     IPV4MASK_POSITION3                       0x16
#define     IPV4MASK_POSITION4                       0x17

#define     IPV4GATEWAY_POSITION1                    0x18
#define     IPV4GATEWAY_POSITION2                    0x19
#define     IPV4GATEWAY_POSITION3                    0x1A
#define     IPV4GATEWAY_POSITION4                    0x1B

#define     PACKETSTAMP_POSITION                     0x04
#define     SERIALNO1_POSITION                       0x10
#define     SERIALNO2_POSITION                       0x11

#define     COMMLEN_POSITION                       	 0x10
#define     COMMDATA_POSITION                        0x11

#define     TASKCOUNT_POSITION                       0x10
#define     TASKNUMBER_POSITION                      0x11
#define     TASKCLASS_POSITION                       0x12
#define     TASKCYCLE1_POSITION                      0x13
#define     TASKCYCLE2_POSITION                      0x14
#define     TASKSTART1_POSITION                      0x15
#define     TASKSTART2_POSITION                      0x16
#define     TASKEND1_POSITION                      	 0x17
#define     TASKEND2_POSITION                      	 0x18

#define     COMPONENTCOUNT1_POSITION                 0x12
#define     COMPONENTCOUNT2_POSITION                 0x13
#define     DATA_POSITION_POINTSKIP_14               0x14

#define     RESULTFLAG_POSITION                      0x10
#define     ERRORID1_POSITION                        0x11
#define     ERRORID2_POSITION                        0x12

#define     PARADATAID_POSITION                      0x11
#define     PARACOUNT_POSITION                       0x10
#define     SLAVECOUNT_POSITION                      0x11
//#define     PARANODEID_POSITION                      0x10


#define     PROTOCOLID1_POSITION                     0X06
#define     PROTOCOLID2_POSITION                     0X07
#define     PROTOCOLID3_POSITION                     0X08
#define     PROTOCOLID4_POSITION                     0X09


#define     PAKAGESIZE1_POSITION                     0x10
#define     PAKAGESIZE2_POSITION                     0x11

#define     DATAIDCOUNT_POSITION                     0x10


#define     PROVESION                                0x0
/*Protocal ID defination*/
#define     PROTOCOL_ID_APPLY_HANDSHAKE             0x01000006
#define     PROTOCOL_ID_APPLY_STARTFRAME            0x01000007
#define     PROTOCOL_ID_APPLY_DATAFRAME             0x01000008
#define     PROTOCOL_ID_APPLY_ENDFRAME              0x01000009
#define     PROTOCOL_ID_APPLY_SAVECONFIGDATA        0x0100000A
#define     PROTOCOL_ID_APPLY_REALTIMEDATAATTR      0x0100000B
#define     PROTOCOL_ID_APPLY_CONFIGSERVERIP        0x0100000C
#define     PROTOCOL_ID_APPLY_CONFIGTMIOTIP         0x0100000D
#define     PROTOCOL_ID_APPLY_DEBUGBKPT             0x0100000E
#define     PROTOCOL_ID_APPLY_COMMDATAFRAME         0x01000010
#define     PROTOCOL_ID_APPLY_GLOBALVARIABLEFRAME   0x01000011
#define     PROTOCOL_ID_APPLY_DEBUGRUN              0x01000020
#define     PROTOCOL_ID_APPLY_DEBUGPAUSE            0x01000021
#define     PROTOCOL_ID_APPLY_DEBUGSTEP             0x01000022
#define     PROTOCOL_ID_APPLY_DEBUGMODE             0x01000023
#define     PROTOCOL_ID_APPLY_TASKDATA              0x01000028
#define     PROTOCOL_ID_APPLY_REALTIMEDATA      	0x0100002A
#define     PROTOCOL_ID_APPLY_COMPONENTSCANCNT      0x0100002B
#define 	PROTOCOL_ID_APPLY_MODBUSRTUPARA			0x01000031
#define 	PROTOCOL_ID_APPLY_MODBUSRTUOD			0x01000032
#define 	PROTOCOL_ID_APPLY_MODBUSRTUENDFRAM		0x01000033
#define 	PROTOCOL_ID_APPLY_MODBUSRTUSAVECONFIGDATA		0x01000034
#define     PROTOCOL_ID_APPLY_CANSLAVEPARA          0x01000041
#define     PROTOCOL_ID_APPLY_DOWNLOADOD            0x01000042
#define     PROTOCOL_ID_APPLY_CANENDFRAME			0x01000043
#define     PROTOCOL_ID_APPLY_CANSAVECONFIGDATA		0x01000044
#define     PROTOCOL_ID_APPLY_INETSETIPINFO         0x01000051
#define     PROTOCOL_ID_APPLY_INETGETIPINFO         0x01000052
#define     PROTOCOL_ID_APPLY_INETPARA              0x01000053
#define     PROTOCOL_ID_APPLY_INETODData            0x01000054
#define     PROTOCOL_ID_APPLY_INETENDFRAME          0x01000055
#define     PROTOCOL_ID_APPLY_INETCONFIGDATA        0x01000056
#define 	PROTOCOL_ID_APPLY_MODBUSTCPPARA			0x01000057
#define		PROTOCOL_ID_APPLY_MODBUSTCPOD			0x01000058
#define		PROTOCOL_ID_APPLY_MODBUSTCPENDFRAME		0x01000059
#define 	PROTOCOL_ID_APPLY_MODBUSTCPSAVECONFIGDATA		0x01000060


#define     PROTOCOL_ID_REPLY_HANDSHAKE             0x02000006
#define     PROTOCOL_ID_REPLY_STARTFRAME            0x02000007
#define     PROTOCOL_ID_REPLY_DATAFRAME             0x02000008
#define     PROTOCOL_ID_REPLY_ENDFRAME              0x02000009
#define     PROTOCOL_ID_REPLY_SAVECONFIGDATA        0x0200000A
#define     PROTOCOL_ID_REPLY_REALTIMEDATAATTR      0x0200000B
#define     PROTOCOL_ID_REPLY_CONFIGSERVERIP        0x0200000C
#define     PROTOCOL_ID_REPLY_DEBUGBKPT             0x0200000E
#define     PROTOCOL_ID_REPLY_SENDSCANSERIALNO      0x0200000F
#define     PROTOCOL_ID_REPLY_CONFIGTMIOTIP         0x0200000D
#define     PROTOCOL_ID_REPLY_COMMDATAFRAME         0x02000010
#define     PROTOCOL_ID_REPLY_GLOBALVARIABLEFRAME   0x02000011
#define     PROTOCOL_ID_REPLY_DEBUGRUN              0x02000020
#define     PROTOCOL_ID_REPLY_DEBUGPAUSE            0x02000021
#define     PROTOCOL_ID_REPLY_DEBUGSTEP             0x02000022
#define     PROTOCOL_ID_REPLY_DEBUGMODE             0x02000023
#define     PROTOCOL_ID_REPLY_TASKDATA              0x02000028
#define     PROTOCOL_ID_REPLY_REALTIMEDATA      	0x0200002A
#define     PROTOCOL_ID_REPLY_COMPONENTSCANCNT      0x0200002B
#define     PROTOCOL_ID_REPLY_MODBUSRTUPARA         0x02000031
#define     PROTOCOL_ID_REPLY_MODBUSRTUOD           0x02000032
#define 	PROTOCOL_ID_REPLY_MODBUSRTUENDFRAM		0x02000033
#define 	PROTOCOL_ID_REPLY_MODBUSRTUSAVECONFIGDATA		0x02000034
#define     PROTOCOL_ID_REPLY_CANSLAVEPARA          0x02000041
#define     PROTOCOL_ID_REPLY_DOWNLOADOD            0x02000042
#define     PROTOCOL_ID_REPLY_CANENDFRAME			0x02000043
#define     PROTOCOL_ID_REPLY_CANSAVECONFIGDATA		0x02000044
#define     PROTOCOL_ID_REPLY_INETSETIPINFO         0x02000051
#define     PROTOCOL_ID_REPLY_INETGETIPINFO         0x02000052
#define     PROTOCOL_ID_REPLY_INETPARA              0x02000053
#define     PROTOCOL_ID_REPLY_INETODData            0x02000054
#define     PROTOCOL_ID_REPLY_INETENDFRAME          0x02000055
#define     PROTOCOL_ID_REPLY_INETCONFIGDATA        0x02000056
#define     PROTOCOL_ID_REPLY_ModbusTCPSETMODEINFO  0x02000057
#define     PROTOCOL_ID_REPLY_MODBUSTCPOD           0x02000058
#define 	PROTOCOL_ID_REPLY_MODBUSTCPENDFRAM		0x02000059
#define 	PROTOCOL_ID_REPLY_MODBUSTCPSAVEFRAM		0x02000060

#define     READFLASHFLAG                           0x33
#define     MAX_RECVNUM                             300


#define     RECEIVE_ERROR_FLAG                      0
#define     RECEIVE_RIGHT_FLAG                      1
#define     PACKLENGTH                              600//the default is 1400

/*The blow micro defination is for download the OD Para*/
#define     CANOD_SIZETEN                           10
#define     CANOD_SIZETEN_CNTS                      20

#define     CANOD_SIZETWENTY                        24
#define     CANOD_SIZETWENTY_CNTS                   10

#define    CANOD_SIZETHIRTY                         24
#define    CANOD_SIZETHIRTY_CNTS                    10
#define    MAX_SLAVE_NUM                            5

//#define   OBJDATASESSION                            200
//#define   ODPARASESSION                             203

#define   para_Block								20
#define   Runtime_Block								21
#define   Can1OD_Block                              22
#define   Can2OD_Block                              23
#define   ModbusOD_Block							24
#define   iNetData_Block                            25
#define   ModbusTcpData_Block                       26

#define	  CommData_ofs								1024*10
#define	  ALLObjectListData_ofs						1024
#define	  TaskData_ofs								0
#define   CanOD_ofs									1024
#define	  CanODPara_ofs								0

#define   iNetIPInfo_ofs                            0
#define   iNetPara_ofs                              26
#define   iNetODData_ofs                            29

extern     BYTE_T              g_byODCnts[];
extern     WORD_T              g_byTotal_ODCnts;
extern     BYTE_T              g_byRecv_OdCount;
extern     BYTE_T              g_byDownLoadOd_Flag;
extern     BYTE_T              g_byCanSlave_Count;
extern     BYTE_T			   g_cTaskTotalCount;

extern      BOOL_T      DealRcvProtocol (WORD_T   wlen);
extern      void        Read_ObjectListData_FromFlash( void );
extern      BOOL_T      SendScanSerialNo (WORD_T Number);
extern      BOOL_T      WriteOdToFlash( BYTE_T  g_byCanSlave_Count );
extern      BOOL_T      ReadOdFromFlash(   void );
extern      void		ComponenttoTaskData( void );

extern      BYTE_T              g_OBJDATACHANGE_FLAG_OFFSET;
extern      BYTE_T              g_OBJLISTNUM_OFFSET;
extern      BYTE_T              g_SERVERCONFIGFLAG_OFFSET ;
extern      BYTE_T              g_SERVERIP_OFFSET ;
extern      BYTE_T              g_SERVERPORT_OFFSET;
//extern      BYTE_T              g_SERVERMAC_OFFSET;
extern      BYTE_T              g_CANFLAG_OFFSET;
extern      BYTE_T				g_TASKTOTALNUM_OFFSET;
extern      BYTE_T              g_TMIOTCONFIGFLAG_OFFSET;
extern      BYTE_T              g_CANSLAVEPARA_FLAG_OFFSET;
//extern     BYTE_T              u_WriteSlaveParaToEE_Flag;
//extern      DWORD_T              u_WriteSlaveParaToEE_Flag;
extern      BYTE_T              u_ServerConfig_Flag;
extern      BYTE_T              u_TmIoTConfig_Flag;
extern      BYTE_T              g_TMIOTIP_OFFSET;
extern      BYTE_T              g_TMIOTMAC_OFFSET;
extern      BYTE_T              g_TMIOTMASK_OFFSET;
extern      BYTE_T              g_TMIOTGATEWAY_OFFSET;
extern      BYTE_T              g_byRecv_Buffer[];
extern      BYTE_T				g_cSendStartDebugFBNum;

extern   const     BYTE_T            g_CANSLAVEPARA_OFFSET;
extern   TASKDATA_T			TaskData[];
extern   BYTE_T				g_RcvPortflag;
extern   BYTE_T             g_byObJlistFlag;
extern   BYTE_T             u_ObjDataChange_Flag;


/*********************************Modbus********************************************/\
#define 	PROTOCOL_BASIC_SIZE							18
#define 	PROTOCOL_DATACONFIG_POSITION 				16
#define 	PROTOCOL_DATAOD_POSITION 					19
#define 	PROTOCOL_MODBUSMASTER_LEN 					8
#define 	PROTOCOL_MODBUSSLAVE_LEN					6

#define 	APPLY_MODBUSRTUPARA_FRAME_SIZE				32
#define 	APPLY_MODBUSRTUOD_FRAME_SIZE				500

#define 	REPLY_MODBUSRTUPARA_FRAME_SIZE				PROTOCOL_BASIC_SIZE
#define 	REPLY_MODBUSRTUOD_FRAME_SIZE 				PROTOCOL_BASIC_SIZE

void 	Read_ModbusData_FromFlash(void);
void 	Read_ModbusData_FromStudio(void);
void 	Reorder(WORD_T *dest);
void    Param_Init();
#endif /* APP_STUDIOLWIPUDP_H_ */
