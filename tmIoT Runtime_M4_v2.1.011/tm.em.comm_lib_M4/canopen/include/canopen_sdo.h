/*===========================================================================+
|  Function : Canopen sdo                                                    |
|  Task     : Canopen sdo Header File                                        |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   :                                                                |
|  Version  : V1.00                                                          |
|  Creation : 2016.3.5                                                       |
|  Revision :                                                                |
+===========================================================================*/
#ifndef D__CANOPEN_SDO
#define D__CANOPEN_SDO

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_def.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_SDO_MAX_TRANSFERDATA                    (4/CANOPEN_BYTE_SIZE)
#define CANOPEN_SDO_FRAME_SIZE                          (8/CANOPEN_BYTE_SIZE)
#define CANOPEN_SDO_RESEND_TIMER                        200

//+++++
//  SDO State
//+++++
#define CANOPEN_SDO_STATE_RESET                         0x0     // Transmission not started. Init state.
#define CANOPEN_SDO_STATE_FINISHED                      0x1     // data are available
#define CANOPEN_SDO_STATE_ABORTED_RCV                   0x80    // Received an abort message. Data not available
#define CANOPEN_SDO_STATE_ABORTED_INTERNAL              0x85    // Aborted but not because of an abort message.
#define CANOPEN_SDO_STATE_DOWNLOAD_IN_PROGRESS          0x2
#define CANOPEN_SDO_STATE_UPLOAD_IN_PROGRESS            0x3
#define CANOPEN_SDO_STATE_BLOCK_DOWNLOAD_IN_PROGRESS    0x4
#define CANOPEN_SDO_STATE_BLOCK_UPLOAD_IN_PROGRESS      0x5

//+++++
// SDO Command
//+++++
// Client command
#define CANOPEN_SDO_DOWNLOAD_SEGMENT_REQUEST            0
#define CANOPEN_SDO_INITIATE_DOWNLOAD_REQUEST           1
#define CANOPEN_SDO_INITIATE_UPLOAD_REQUEST             2
#define CANOPEN_SDO_UPLOAD_SEGMENT_REQUEST              3
#define CANOPEN_SDO_ABORT_TRANSFER_REQUEST              4
#define CANOPEN_SDO_BLOCK_UPLOAD_REQUEST                5
#define CANOPEN_SDO_BLOCK_DOWNLOAD_REQUEST              6

// Server command
#define CANOPEN_SDO_UPLOAD_SEGMENT_RESPONSE             0
#define CANOPEN_SDO_DOWNLOAD_SEGMENT_RESPONSE           1
#define CANOPEN_SDO_INITIATE_UPLOAD_RESPONSE            2
#define CANOPEN_SDO_INITIATE_DOWNLOAD_RESPONSE          3
#define CANOPEN_SDO_ABORT_TRANSFER_RESPONSE             4
#define CANOPEN_SDO_BLOCK_DOWNLOAD_RESPONSE             5
#define CANOPEN_SDO_BLOCK_UPLOAD_RESPONSE               6


#define CANOPEN_SDO_SEG_ABORT                           0x80

//+++++
//  Sdo block sub command
//+++++
// upload client subcommand
#define CANOPEN_SDO_BCS_INITIATE_UPLOAD_REQUEST         0
#define CANOPEN_SDO_BCS_END_UPLOAD_REQUEST              1
#define CANOPEN_SDO_BCS_UPLOAD_RESPONSE                 2
#define CANOPEN_SDO_BCS_START_UPLOAD                    3

// upload server subcommand
#define CANOPEN_SDO_BSS_INITIATE_UPLOAD_RESPONSE        0
#define CANOPEN_SDO_BSS_END_UPLOAD_RESPONSE             1

// download client subcommand
#define CANOPEN_SDO_BCS_INITIATE_DOWNLOAD_REQUEST       0
#define CANOPEN_SDO_BCS_END_DOWNLOAD_REQUEST            1

// download server subcommand
#define CANOPEN_SDO_BSS_INITIATE_DOWNLOAD_RESPONSE      0
#define CANOPEN_SDO_BSS_END_DOWNLOAD_RESPONSE           1
#define CANOPEN_SDO_BSS_DOWNLOAD_RESPONSE               2


//  Sdo table parameter
#define CANOPEN_SDO_MODE_UNKNOWN                        0
#define CANOPEN_SDO_MODE_DOWNLOAD                       1
#define CANOPEN_SDO_MODE_UPLOAD                         2

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tagCANOPEN_SDO_COMM_TBL                  CANOPEN_SDO_COMM_TBL_T;
typedef struct tagCANOPEN_SDO_COMM                      CANOPEN_SDO_COMM_T;
typedef struct tagCANOPEN_SDO                           CANOPEN_SDO_T;

struct tagCANOPEN_SDO_COMM_TBL {
        CANOPEN_WORD_T                  wIndex;
        CANOPEN_BYTE_T                  bySubIndex;
        CANOPEN_BYTE_T                  byUpDown;
        CANOPEN_DWORD_T                 dwValue;
        CANOPEN_INT_T                   nPeriod;
        CANOPEN_DWORD_T                 dwTimeoutTick;
};

struct tagCANOPEN_SDO_COMM {
        CANOPEN_WORD_T                  wMaxIndex;
        CANOPEN_WORD_T                  wCurIndex;
        CANOPEN_SDO_COMM_TBL_T          *pTable;
};

struct tagCANOPEN_SDO {
        CANOPEN_TIMER_T                 Timer;
        CANOPEN_SDO_COMM_T              ConfComm;
        CANOPEN_SDO_COMM_T              NormalComm;
        CANOPEN_DWORD_T                 dwAbortCode;
        CANOPEN_BYTE_T                  byState;
        CANOPEN_WORD_T                  wIndex;
        CANOPEN_BYTE_T                  bySubIndex;
        CANOPEN_DWORD_T                 dwHeader;
        CANOPEN_BYTE_T                  abyData[CANOPEN_SDO_MAX_TRANSFERDATA];
};

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
void Canopen_Sdo_Init(CANOPEN_T *pCanopen, CANOPEN_LINK_T *pLink);

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_ERR_T Canopen_Sdo_DownLoad(
        CANOPEN_T       *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        void                    *pBuf,
        CANOPEN_BOOL_T          bUseBlock);

void Canopen_Sdo_UpLoad(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BOOL_T          bUseBlock);

void Canopen_Sdo_Recv(CANOPEN_T *pCanopen, CANOPEN_DEV_FRAME_T *pFrame);

void Canopen_Sdo_AddConfComm(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_SDO_COMM_TBL_T  *pTable,
        CANOPEN_WORD_T          wNumber);

void Canopen_Sdo_AddNormalComm(
        CANOPEN_T               *pCanopen,
        CANOPEN_LINK_T          *pLink,
        CANOPEN_SDO_COMM_TBL_T  *pTable,
        CANOPEN_WORD_T          wNumber);

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
