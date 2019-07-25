/*===========================================================================+
|  Function : Canopen object dictionary                                      |
|  Task     : Canopen object dictionary Header File                          |
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
#ifndef D__CANOPEN_OB
#define D__CANOPEN_OB

#ifdef __cplusplus
extern "C" {
#endif

#include "canopen_def.h"

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
//+++++
//  Object dictionary unit type
//+++++
#define OD_TYPE_NULL                    0
#define OD_TYPE_DOMAIN                  1
#define OD_TYPE_DEFTYPE                 2
#define OD_TYPE_DEFSTRUCTD              3
#define OD_TYPE_VAR                     4
#define OD_TYPE_ARRAY                   5
#define OD_TYPE_RECORD                  6

//+++++
//  Object dictionary unit data type
//+++++
#define OD_DATATYPE_BOOL                0x01
#define OD_DATATYPE_CHAR                0x02
#define OD_DATATYPE_SHORT               0x03
#define OD_DATATYPE_LONG                0x04
#define OD_DATATYPE_BYTE                0x05
#define OD_DATATYPE_WORD                0x06
#define OD_DATATYPE_DWORD               0x07
#define OD_DATATYPE_REAL                0x08

//+++++
//  Od access
//+++++
#define OD_ACCESS_RO                    0x1
#define OD_ACCESS_RW                    0x2
#define OD_ACCESS_WO                    0x3

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef union  tagCANOPEN_OD_VALUE      CANOPEN_OD_VALUE_T;
typedef struct tagCANOPEN_OD_INDEX      CANOPEN_OD_INDEX_T;
typedef struct tagCANOPEN_OD_DATA       CANOPEN_OD_DATA_T;
typedef struct tagCANOPEN_OD            CANOPEN_OD_T;

typedef void (* CANOPEN_ODFUNC)(CANOPEN_BYTE_T byNodeID, CANOPEN_LONG_T lOffset);

union tagCANOPEN_OD_VALUE {
        CANOPEN_BOOL_T                  bValue;
        CANOPEN_CHAR_T                  cValue;
        CANOPEN_SHORT_T                 sValue;
        CANOPEN_LONG_T                  lValue;
        CANOPEN_BYTE_T                  byValue;
        CANOPEN_WORD_T                  wValue;
        CANOPEN_DWORD_T                 dwValue;
        CANOPEN_FLOAT_T                 fValue;
};

struct tagCANOPEN_OD_INDEX {
        CANOPEN_DWORD_T                 dwIndex;
        CANOPEN_BYTE_T                  byDataType;
        CANOPEN_BYTE_T                  byAccess;
        CANOPEN_ODFUNC                  WriteFunc;
};

struct tagCANOPEN_OD_DATA {
        CANOPEN_BOOL_T                  bUpdate;
        CANOPEN_BYTE_T                  byUpDown;
        CANOPEN_BOOL_T                  bDirty;
        CANOPEN_OD_VALUE_T              DbValue;
        CANOPEN_BYTE_T					UpDownChangFlag;
};


struct tagCANOPEN_OD {
        CANOPEN_WORD_T                  wNumber;
        const CANOPEN_OD_INDEX_T        *pIndex;
        CANOPEN_OD_DATA_T               *pValue;
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

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/
CANOPEN_DWORD_T Canopen_OD_Read(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf);

CANOPEN_DWORD_T Canopen_OD_Write(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf);

CANOPEN_DWORD_T Canopen_OD_ReadByIndex(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_LONG_T          lOffset,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf);

CANOPEN_DWORD_T Canopen_OD_WriteByIndex(
        CANOPEN_T               *pCanopen,
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_BYTE_T          byNodeID,
        CANOPEN_LONG_T          lOffset,
        CANOPEN_BYTE_T          *pbySize,
        void                    *pBuf);

CANOPEN_LONG_T Canopen_OD_IndexToOffset(
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_WORD_T          wIndex,
        CANOPEN_BYTE_T          bySubIndex);

void Canopen_Od_SetAddrbyIndex(
        CANOPEN_OD_T            *pObjDict,
        CANOPEN_LONG_T          lOffset,
        CANOPEN_BOOL_T          bUpdate,
        CANOPEN_BYTE_T          byUpDown);

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
