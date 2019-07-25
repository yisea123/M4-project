/*===========================================================================+
|  Function : rs485 Driver                                                   |
|  Task     : rs485 Driver Header File                                       |
|----------------------------------------------------------------------------|
|  Compile  : CCS6.1 -                                                       |
|  Link     : CCS6.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Jia                                                            |
|  Version  : V1.00                                                          |
|  Creation : 2016/08/15                                                     |
|  Revision :                                                                |
+===========================================================================*/
#ifndef     D_RS485_H
#define     D_RS485_H

// #include    "type.h"
// #include    "device.h"
#include    "kernel/include/type.h"
#include    "kernel/include/device.h"
#include	"config.h"
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#define MAX_RS485_SIZE      128

#define MAX_RS485           2

#define RS485_READ          0
#define RS485_WRITE         1

#define COM_SUCCESS         0x0000
#define COM_E_RX_EMPTY      0x00A0
#define COM_E_TX_FULL       0x00B0

//+++++
//  RS485 control command
//+++++
#define     RS485_CMD_TX                      1
#define     RS485_CMD_RX                      2

#define 	RS485_1 						  0
#define 	RS485_2 						  1
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct  tagRS485CONF {
        WORD            wChannel;
        DWORD           dwBaud;
        DWORD           dwMode;
} RS485CONF;

typedef struct tagRS485QUEUE {
        BYTE*           pQueue;
        WORD            wInput;
        WORD            wOutput;
        WORD            wEnd;
}RS485QUEUE;


typedef struct  tagRS485 {
        WORD            wRxBufferSize;          // Size of the receive buffer
        BYTE*           pbyRxBuffer;            // Pointer to the receive buffer
        WORD            wTxBufferSize;          // Size of the transmit buffer
        BYTE*           pbyTxBuffer;            // Pointer to the transmit buffer
}RS485;

typedef struct tagHRS485 {
        WORD            wState;
        WORD            wRxState;
        WORD            wTxState;
        RS485QUEUE      queueRx;
        RS485QUEUE      queueTx;
        DWORD			dwRxTick;
}HRS485;

/*===========================================================================+
|           Class - RS485                                                    |
+===========================================================================*/
/*---------------------------------------------------------------------------+
|           Attributes                                                       |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
ERROR_T _RS485_Open(void  *pArg);

void _Rs485_Close1(void);
WORD_T _RS485_Write1(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);
WORD_T _RS485_Read1(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);

void _Rs485_Close2(void);
WORD_T _RS485_Write2(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);
WORD_T _RS485_Read2(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);

ERROR_T _RS485_Control(BYTE_T byCmd, void *pArg);
void _RS485_Rx(WORD Port);
void _RS485_Tx(WORD Port);

void _Set_RS485_Mode(WORD wChannel, WORD wMode);

/*===========================================================================+
|           Externals                                                        |
+===========================================================================*/
extern DEV_ENGINE_T            g_RS485_Engine[MAX_RS485];
#endif

