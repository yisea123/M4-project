/*===========================================================================+
|  Function : Can Slave Application                                          |
|  Task     : Can Slave Application Header File                              |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : jiaoyang                                                       |
|  Version  : V1.00                                                          |
|  Creation : 2016.3.5                                                       |
|  Revision :                                                                |
+===========================================================================*/
#ifndef D__CANSLAVEAPP
#define D__CANSLAVEAPP


#ifdef __cplusplus
extern "C" {
#endif

#include "kernel/include/task.h"
#include "canopen/include/canopen.h"


/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
//+++++
//  CANOPEN ID
//+++++
#define CANOPEN_ID_DEVICETYPE                   0x100000
#define CANOPEN_ID_HEARTBEAT                    0x101700


//#define CANOPEN_ID_Modbus0_ID                   0x600000
//#define CANOPEN_ID_Modbus0_CMD                  0x600001
//#define CANOPEN_ID_Modbus0_ADDR                 0x600002
//#define CANOPEN_ID_Modbus0_LEN                  0x600003
//#define CANOPEN_ID_Modbus0_RDATA                0x600004
//#define CANOPEN_ID_Modbus0_WDATA                0x600005

//#define CANOPEN_ID_Modbus1_ID                   0x600100
//#define CANOPEN_ID_Modbus1_CMD                  0x600101
//#define CANOPEN_ID_Modbus1_ADDR                 0x600102
//#define CANOPEN_ID_Modbus1_LEN                  0x600103
//#define CANOPEN_ID_Modbus1_RDATA                0x600104
//#define CANOPEN_ID_Modbus1_WDATA                0x600105

//#define CANOPEN_ID_Modbus2_ID                   0x600200
//#define CANOPEN_ID_Modbus2_CMD                  0x600201
//#define CANOPEN_ID_Modbus2_ADDR                 0x600202
//#define CANOPEN_ID_Modbus2_LEN                  0x600203
//#define CANOPEN_ID_Modbus2_RDATA                0x600204
//#define CANOPEN_ID_Modbus2_WDATA                0x600205

//#define CANOPEN_ID_Modbus3_ID                   0x600300
//#define CANOPEN_ID_Modbus3_CMD                  0x600301
//#define CANOPEN_ID_Modbus3_ADDR                 0x600302
//#define CANOPEN_ID_Modbus3_LEN                  0x600303
//#define CANOPEN_ID_Modbus3_RDATA                0x600304
//#define CANOPEN_ID_Modbus3_WDATA                0x600305

//#define IO2CCARD_INPUT1                         0xC00000
//#define IO2CCARD_INPUT2                         0xC00001
//#define IO2CCARD_INPUT3                         0xC00002
//#define IO2CCARD_INPUT4                         0xC00003
//#define IO2CCARD_INPUT5                         0xC00004
//#define IO2CCARD_INPUT6                         0xC00005
//#define IO2CCARD_INPUT7                         0xC00006
//#define IO2CCARD_INPUT8                         0xC00007

//#define IO2CCARD_INPUT1                         0x600000
//#define IO2CCARD_INPUT2                         0x600001
//#define IO2CCARD_INPUT3                         0x600002
//#define IO2CCARD_INPUT4                         0x600003
//#define IO2CCARD_INPUT5                         0x600004
//#define IO2CCARD_INPUT6                         0x600005
//#define IO2CCARD_INPUT7                         0x600006
//#define IO2CCARD_INPUT8                         0x600007


//#define IO2CCARD_OUTPUT1                        0xC10000
//#define IO2CCARD_OUTPUT2                        0xC10001
//#define IO2CCARD_OUTPUT3                        0xC10002
//#define IO2CCARD_OUTPUT4                        0xC10003
//#define IO2CCARD_OUTPUT5                        0xC10004
//#define IO2CCARD_OUTPUT6                        0xC10005
//#define IO2CCARD_OUTPUT7                        0xC10006
//#define IO2CCARD_OUTPUT8                        0xC10007

//#if defined(CANOPEN)
//#define IO2CCARD_OUTPUT1                        0x600100
//#define IO2CCARD_OUTPUT2                        0x600101
//#define IO2CCARD_OUTPUT3                        0x600102
//#define IO2CCARD_OUTPUT4                        0x600103
//#define IO2CCARD_OUTPUT5                        0x600104
//#define IO2CCARD_OUTPUT6                        0x600105
//#define IO2CCARD_OUTPUT7                        0x600106
//#define IO2CCARD_OUTPUT8                        0x600107
//#endif





enum CANOPEN_INDEX {
        CANOPEN_DEVICETYPE,                     // 0
        CANOPEN_HEARTBEAT,                      // 1
};

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef  struct tagCan_Config
{
    DWORD_T  Can_BaudRate;
    BYTE_T  Can_Node_ID;

}CAN_PARAM_CONFIG;

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/

/*===========================================================================+
|           Class declaration                                                |
+===========================================================================*/

/*===========================================================================+
|           Attributes                                                       |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
extern CANOPEN_OD_INDEX_T  watch_p1[20];
extern CANOPEN_OD_INDEX_T  watch_p2[20];
/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
void Write_Canod(BYTE *byEeprom_buff, SIZE_T Index, OFFSET_T Offset);
void Read_Canod(BYTE *wrdata, SIZE_T DataSize, OFFSET_T Offset);
void InitCanopenSlave(void);
void InitOutput(void);
void CreateTaskCanopenSlave(void);
extern void    Create_CanSlave(void);
void Create_CanopenSlave(BYTE	DeviceNUM);
/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/
//extern CANOPEN_T                g_CanSlave;
//extern CANOPEN_LINK_T           u_CanSlaveLink;
extern HTASK                    g_htask_CanopenSlave;

extern CANOPEN_OD_DATA_T        u_aCanSlave_OdValue_Tbl[];
extern CANOPEN_OD_INDEX_T       u_aCanSlave_OdPara_Tbl[];
extern BYTE_T                   g_CAN_SlavePara_Buff[];

extern CANOPEN_OD_DATA_T	*u_aCanSlave_OdValue_TblP1;
extern CANOPEN_OD_DATA_T	*u_aCanSlave_OdValue_TblP2;
extern CANOPEN_OD_INDEX_T	*u_aCanSlave_OdPara_TblP1;
extern CANOPEN_OD_INDEX_T	*u_aCanSlave_OdPara_TblP2;
//void sendSDO(
//        CANOPEN_BYTE_T          byNodeID,
//        CANOPEN_DWORD_T         dwIndex,
//        CANOPEN_BYTE_T          pbySize,
//        void                    *pBuf)

typedef struct tag_ModbusData
{
    BYTE    Id;
    BYTE    Cmd;
    WORD    Addr;
    WORD    Len;
} MODBUSDATA;

#ifdef __cplusplus
}
#endif


#endif
