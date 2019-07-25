/*===========================================================================+
|  Function : Canopen definition                                             |
|  Task     : Canopen definition Header File                                 |
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
#ifndef D__CANOPEN_DEF
#define D__CANOPEN_DEF

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_type.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_MAX_LINK                                16

//+++++
// Canopen working mode
//+++++
#define CANOPEN_MASTER                                  1
#define CANOPEN_SLAVE                                   0

//+++++
// Status of the node during the SDO transfer
//+++++
#define CANOPEN_SERVER                                  0x1
#define CANOPEN_CLIENT                                  0x2

//+++++
//  Canopen error
//+++++
#define CANOPEN_ERR_NONE                                0x00000000
#define CANOPEN_ERR_SDO_TOGGLE_NOT_ALTERNED             0x05030000
#define CANOPEN_ERR_SDO_TIMED_OUT                       0x05040000
#define CANOPEN_ERR_SDO_OUT_OF_MEMORY                   0x05040005 // Size data exceed SDO_MAX_LENGTH_TRANSFER
#define CANOPEN_ERR_OD_READ_NOT_ALLOWED                 0x06010001
#define CANOPEN_ERR_OD_WRITE_NOT_ALLOWED                0x06010002
#define CANOPEN_ERR_OD_NO_SUCH_OBJECT                   0x06020000
#define CANOPEN_ERR_OD_NOT_MAPPABLE                     0x06040041
#define CANOPEN_ERR_OD_LENGTH_DATA_INVALID              0x06070010
#define CANOPEN_ERR_OD_NO_SUCH_SUBINDEX                 0x06090011
#define CANOPEN_ERR_OD_VALUE_RANGE_EXCEEDED             0x06090030 // Value range test result
#define CANOPEN_ERR_OD_VALUE_TOO_LOW                    0x06090031 // Value range test result
#define CANOPEN_ERR_OD_VALUE_TOO_HIGH                   0x06090032 // Value range test result
#define CANOPEN_ERR_SDO_GENERAL_ERROR                   0x08000000 // Error size of SDO message
#define CANOPEN_ERR_SDO_LOCAL_CTRL_ERROR                0x08000021

//+++++
//  Canopen Function ID
//+++++
#define CANOPEN_FUNCID_NMT                              0       // NMT
#define CANOPEN_FUNCID_SYNCANDEMERG                     1       // Sync  : NodeID  = 0   Emergency:NodeID !=0
#define CANOPEN_FUNCID_TIMESTAMP                        2       // Stamp : NodeID  = 0
#define CANOPEN_FUNCID_TXPDO1                           3       // TxPDO1: NodeID != 0
#define CANOPEN_FUNCID_RXPDO1                           4       // RxPDO1: NodeID != 0
#define CANOPEN_FUNCID_TXPDO2                           5       // TxPDO2: NodeID != 0
#define CANOPEN_FUNCID_RXPDO2                           6       // RxPDO2: NodeID != 0
#define CANOPEN_FUNCID_TXPDO3                           7       // TxPDO3: NodeID != 0
#define CANOPEN_FUNCID_RXPDO3                           8       // RxPDO3: NodeID != 0
#define CANOPEN_FUNCID_TXPDO4                           9       // TxPDO4: NodeID != 0
#define CANOPEN_FUNCID_RXPDO4                           10      // RxPDO4: NodeID != 0
#define CANOPEN_FUNCID_SDO_S                            11      // SDO Server: NodeID != 0
#define CANOPEN_FUNCID_SDO_C                            12      // SDO Client: NodeID != 0
#define CANOPEN_FUNCID_RESERVED1                        13      // Reserved
#define CANOPEN_FUNCID_NMTE                             14      // NMT Error: NodeID != 0
#define CANOPEN_FUNCID_MAX                              14

//+++++
//  Canopen combo ID
//+++++
#define CANOPEN_COMBID_NMT                              0x0     // NMT
#define CANOPEN_COMBID_SYNCANDEMERG                     0x80    // Sync  : NodeID  = 0   Emergency:NodeID !=0
#define CANOPEN_COMBID_TIMESTAMP                        0x100   // Stamp : NodeID  = 0
#define CANOPEN_COMBID_TXPDO1                           0x180   // TxPDO1: NodeID != 0
#define CANOPEN_COMBID_RXPDO1                           0x200   // RxPDO1: NodeID != 0
#define CANOPEN_COMBID_TXPDO2                           0x280   // TxPDO2: NodeID != 0
#define CANOPEN_COMBID_RXPDO2                           0x300   // RxPDO2: NodeID != 0
#define CANOPEN_COMBID_TXPDO3                           0x380   // TxPDO3: NodeID != 0
#define CANOPEN_COMBID_RXPDO3                           0x400   // RxPDO3: NodeID != 0
#define CANOPEN_COMBID_TXPDO4                           0x480   // TxPDO4: NodeID != 0
#define CANOPEN_COMBID_RXPDO4                           0x500   // RxPDO4: NodeID != 0
#define CANOPEN_COMBID_SDO_S                            0x580   // SDO Server: NodeID != 0
#define CANOPEN_COMBID_SDO_C                            0x600   // SDO Client: NodeID != 0
#define CANOPEN_COMBID_RESERVED1                        0x680   // Reserved
#define CANOPEN_COMBID_NMTE                             0x700   // NMT Error: NodeID != 0

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
struct tagCANOPEN;
struct tagCANOPEN_LINK;

typedef struct tagCANOPEN               CANOPEN_T;
typedef struct tagCANOPEN_LINK          CANOPEN_LINK_T;

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

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
