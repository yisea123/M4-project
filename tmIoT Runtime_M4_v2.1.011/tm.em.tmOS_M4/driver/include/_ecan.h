/*===========================================================================+
|  Function : Can Driver                                                     |
|  Task     : Can Driver Header File                                         |
|----------------------------------------------------------------------------|
|  Compile  : CCS3.1 -                                                       |
|  Link     : CCS3.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Wain.Wei                                                       |
|  Version  : V1.00                                                          |
|  Creation : 28/03/2011                                                     |
|  Revision :                                                                |
+===========================================================================*/
#ifndef     D___ECAN
#define     D___ECAN

#include    "kernel/include/type.h"
#include    "kernel/include/device.h"
#include	"config.h"
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#define     CAN_TX_MAILBOX_STARTNO      19
#define     CAN_TX_MAILBOX_ENDNO        30
#define     CAN_RX_MAILBOX_STARTNO      1
#define     CAN_RX_MAILBOX_ENDNO        12
#define     CAN_ISRTX_MAILBOX_STARTNO   31
#define     CAN_ISRTX_MAILBOX_ENDNO     32
#define     CAN_REMOTE_FRAME_FLAG       0x00001000

#define     CAN_STANDARD_MASK           0x7FL
#define     CAN_EXTEND_MASK             0x1FFFFFFL

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define     MAX_CAN                          8
#define     MAX_CAN_MAILBOX                 32
//+++++
//  Ad control command
//+++++
#define     CAN_CMD_TX                      1
#define     CAN_CMD_RX                      2
//+++++
//  eCan BaudRate
//+++++
//#define     BAUDRATE_1MBPS              0
//#define     BAUDRATE_500KBPS            1
//#define     BAUDRATE_250KBPS            2
//#define     BAUDRATE_125KBPS            3
//#define     BAUDRATE_100KBPS            4
//#define     BAUDRATE_50KBPS             5
//#define     BAUDRATE_25KBPS             6
//#define     BAUDRATE_20KBPS             7
//#define     BAUDRATE_10KBPS             8
//#define     BAUDRATE_5KBPS              9
//#define     BAUDRATE_2KBPS              10

#define     BAUDRATE_1MBPS              1000000
#define     BAUDRATE_500KBPS            500000
#define     BAUDRATE_250KBPS            250000
#define     BAUDRATE_125KBPS            125000
#define     BAUDRATE_100KBPS            100000
#define     BAUDRATE_50KBPS             50000
#define     BAUDRATE_25KBPS             25000
#define     BAUDRATE_20KBPS             20000
#define     BAUDRATE_10KBPS             10000
#define     BAUDRATE_5KBPS              5000
#define     BAUDRATE_2K5BPS              2500//2000
//======
//      eCan Sampling Point
//======
#define     SAMPLINGPOINT_80PERCENT     1
#define     SAMPLINGPOINT_73PERCENT     2
#define     SAMPLINGPOINT_66PERCENT     3
#define     SAMPLINGPOINT_60PERCENT     4
//======
//      eCan Mode
//======
#define     CANMODE_STANDARD            1
#define     CANMODE_ENHANCED            2

//======
//      eCan Result Code
//======
#define     CAN_SUCCESS                 0
#define     CAN_ERR_FAILURE             1
#define     CAN_ERR_RX_EMPTY            2
#define     CAN_ERR_TX_FULL             3
#define     CAN_ERR_RX_BUSY             4
#define     CAN_ERR_TX_BUSY             5
#define     CAN_ERR_RX_SMALL            6
#define     CAN_ERR_TX_SMALL            6
//======
//      eCan state code
//======
#define     CAN_S_CLOSED                0
#define     CAN_S_OPENED                1
#define     CAN_S_IDLE                  2
#define     CAN_S_BUSY                  3
#define     CAN_S_HOLD                  4
#define     CAN_S_ERROR                 5
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct  tagBAUDRATECONF
{
    WORD_T          wBitTime;
    DWORD_T         dwBaudrate;             //change DWORD at 2016/04/11
    WORD_T          wSamplingPoint;
} BAUDRATECONF;

typedef struct  tagCANCONF
{
    BAUDRATECONF        BaudrateConf;
    WORD_T          wCanMode;
    WORD_T          wNodeID;
} CANCONF;




typedef   union   tagCANDATA
{
  BYTE_T          abyData[8];
  WORD_T          awData[4];
  DWORD_T         adwData[2];
} CANDATA;

typedef   union   tagMSGID
{
  DWORD_T             dwAll;
  struct
      {
      WORD_T          wExtID_L:16;
      WORD_T          wExtID_H:2;
      WORD_T          wStdID:11;
      WORD_T          wReserved:2;
      WORD_T          wRTR:1;
      } ID;
} MSGID;

typedef struct tagDATAFRAME
{
  MSGID           MsgID;
  CANDATA         CanData;
  WORD_T            wLength;
} DATAFRAME;

typedef struct tag_CANQUEUE
{
	DATAFRAME*        pQueue;
    WORD_T            wInput;
    WORD_T            wOutput;
    WORD_T            wMaxSize;
    WORD_T            wCount;
} _CANQUEUE;

typedef struct tagECAN
{
    CANCONF         CanConf;
    WORD_T          wRxBufferSize;          // Size of the receive buffer
    BYTE_T*         pRxBuffer;              // Pointer to the receive buffer
    WORD_T          wTxBufferSize;          // Size of the transmit buffer
    BYTE_T*         pTxBuffer;              // Pointer to the transmit buffer
} ECAN;
/*===========================================================================+
|           Class - RS485                                                    |
+===========================================================================*/
/*---------------------------------------------------------------------------+
|           Attributes                                                       |
+---------------------------------------------------------------------------*/
typedef struct tagHCAN
{
    WORD_T          wMode;
    WORD_T          wState;
    WORD_T          wRxState;
    WORD_T          wTxState;
    _CANQUEUE           queueRx;
    _CANQUEUE           queueTx;
} HCAN;
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
WORD_T      _eCan0_Read(WORD_T *, WORD_T, WORD_T* );
WORD_T      _eCan0_Write(WORD_T *, WORD_T);
WORD_T      _eCan1_Read(WORD_T *, WORD_T, WORD_T* );
WORD_T      _eCan1_Write(WORD_T *, WORD_T);
/*===========================================================================+
|           Externals                                                        |
+===========================================================================*/
extern      DEV_ENGINE_T            g_CAN0_Engine;
extern      DEV_ENGINE_T            g_CAN1_Engine;
#endif

