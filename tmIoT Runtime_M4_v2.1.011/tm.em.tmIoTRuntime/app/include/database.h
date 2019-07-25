/*==========================================================================+
|  File     : database.h                                                    |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : zhao                                                          |
|  Version  : V1.00                                                         |
|  Creation : 2015/12/14                                                    |
|  Revision :                                                               |
+==========================================================================*/

#ifndef     D__DATABASE
#define     D__DATABASE

#include	"kernel/include/type.h"
#include    "config.h"

/*---------------------------------------------------------------------------+
|          Constant                                                          |
+---------------------------------------------------------------------------*/
#define     CARD_MAX            6
#define     LINK_MAX            5
#define     _UNKNOWN            0
#define     _BOOL               1
#define     _BYTE               2
#define     _WORD               3
#define     _DWORD              4
#define     _SINT               11
#define     _USINT              12
#define     _INT                13
#define     _UINT               14
#define     _DINT               15
#define     _UDINT              16
#define     _REAL               21

#define     DB_RO               0
#define     DB_RW               1

//+++++
//  M3 data ID
//+++++



#define     DBID_CARD_0_INTERFACE_IO_INPUTG1                                             0x600000
#define     DBID_CARD_0_INTERFACE_IO_INPUTG2                                             0x600001
#define     DBID_CARD_0_INTERFACE_IO_INPUTG3                                             0x600002
#define     DBID_CARD_0_INTERFACE_IO_INPUTG4                                             0x600003
#define     DBID_CARD_0_INTERFACE_IO_INPUTG5                                             0x600004
#define     DBID_CARD_0_INTERFACE_IO_INPUTG6                                             0x600005
#define     DBID_CARD_0_INTERFACE_IO_INPUTG7                                             0x600006
#define     DBID_CARD_0_INTERFACE_IO_INPUTG8                                             0x600007

#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG1                                            0x620000
#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG2                                            0x620001
#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG3                                            0x620002
#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG4                                            0x620003
#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG5                                            0x620004
#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG6                                            0x620005
#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG7                                            0x620006
#define     DBID_CARD_0_INTERFACE_IO_OUTPUTG8                                            0x620007

#define     DBID_CARD_0_INTERFACE_IO_AD1                                                 0x640000
#define     DBID_CARD_0_INTERFACE_IO_AD2                                                 0x640001
#define     DBID_CARD_0_INTERFACE_IO_AD3                                                 0x640002
#define     DBID_CARD_0_INTERFACE_IO_AD4                                                 0x640003

#define     DBID_CARD_0_INTERFACE_IO_DA1                                                 0x641000
#define     DBID_CARD_0_INTERFACE_IO_DA2                                                 0x641001
#define     DBID_CARD_0_INTERFACE_IO_DA3                                                 0x641002
#define     DBID_CARD_0_INTERFACE_IO_DA4                                                 0x641003

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/


enum  database{

	    ENUM_CARD_0_INTERFACE_IO_INPUTG1            ,//540
		ENUM_CARD_0_INTERFACE_IO_INPUTG2            ,//541
		ENUM_CARD_0_INTERFACE_IO_INPUTG3            ,//542
		ENUM_CARD_0_INTERFACE_IO_INPUTG4            ,//543
		ENUM_CARD_0_INTERFACE_IO_INPUTG5            ,//544
		ENUM_CARD_0_INTERFACE_IO_INPUTG6            ,//545
		ENUM_CARD_0_INTERFACE_IO_INPUTG7            ,//546
		ENUM_CARD_0_INTERFACE_IO_INPUTG8            ,//547
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG1           ,//548
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG2           ,//549
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG3           ,//550
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG4           ,//551
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG5           ,//552
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG6           ,//553
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG7           ,//554
		ENUM_CARD_0_INTERFACE_IO_OUTPUTG8           ,//555
		ENUM_CARD_0_INTERFACE_IO_AD1                ,//556
		ENUM_CARD_0_INTERFACE_IO_AD2                ,//557
		ENUM_CARD_0_INTERFACE_IO_AD3                ,//558
		ENUM_CARD_0_INTERFACE_IO_AD4                ,//559
		ENUM_CARD_0_INTERFACE_IO_DA1                ,//562
		ENUM_CARD_0_INTERFACE_IO_DA2                ,//563
		ENUM_CARD_0_INTERFACE_IO_DA3                ,//560
		ENUM_CARD_0_INTERFACE_IO_DA4                ,//561

	    DB_M3_TEMPER_NUM,

};
/*---------------------------------------------------------------------------+
|           Type Definition                                                  |
+---------------------------------------------------------------------------*/
typedef BOOL_T (*DBFUNC)(WORD_T   wOffset);
/*---------------------------------------------------------------------------+*/
typedef struct tagDB_DATAATTRIBUTE
{
    DWORD_T					  dwID;
	BYTE_T                    byDataType;
	BYTE_T                    byAccess;
	DBFUNC                    DealFunc;
} DB_DATAATTRIBUTE;

/*---------------------------------------------------------------------------+*/
typedef union	tagDBVALUE
{
	char 			cData;		//1
	DWORD_T         dwData;		//2
	BYTE_T          byData;		//3
	WORD_T          wData;		//4
	short			sData;		//5
	long 			lData;		//6
	float           fData;		//7
} DBVALUE;
/*---------------------------------------------------------------------------+*/
typedef	struct	tagDB_DATA
{
	DBVALUE	dbValue;
} DB_DATA;
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
extern   void           DBRun                       (void);
extern   void	        DB_Init                     (void);
extern   void 			DBSetValueByDataID          (DWORD_T dwDataID , DBVALUE dbValue);
extern   void           DBSetValueByOffset          (long    lOffset  , DBVALUE dbValue);

extern   DBVALUE		DBGetValueByDataID          (DWORD_T dwDataID);
extern   DBVALUE        DBGetValueByOffset          (long    lOffset);

extern   long			DataIDToOffset              (DWORD_T dwDataID);

extern   BOOL_T    		DBSendvalue                 (WORD_T  wData_Num, DBVALUE dbValue);
extern   BOOL_T         DBGetValueFromODByOffset    (DWORD_T dwOffset);
extern   void	        DBSetFlag                   (DWORD_T dwOffset);
extern   void	        DBCleanFlag                 (DWORD_T dwOffset);

/*==============================================================================+
|           Externs Veriable                                                    |
+==============================================================================*/

extern          WORD_T		                 g_CardID[CARD_MAX];
extern          WORD_T                       g_CardNum;
extern          WORD_T		                 g_SegNum;
extern          const                        DB_DATAATTRIBUTE		g_Master_DataAttr[];
extern          DB_DATA		                 g_DBData[DB_M3_TEMPER_NUM];
extern          DWORD_T                      u_dwDBFlag  [(DB_M3_TEMPER_NUM+31)/32];


#endif


