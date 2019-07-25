/*==========================================================================+
|  File     : database.c                                                    |
|  Function : Source File                                                   |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : YW                                                            |
|  Version  : V2.00                                                         |
|  Creation : 2018/01/19                                                    |
|  Revision :                                                               |
+==========================================================================*/
#include	"database.h"
#include	"canopen/include/canopen_type.h"
#include	"string.h"
/*===========================================================================+
|           Function Prototype                                               |
+===========================================================================*/
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Inner Global Variable                                               |
+==============================================================================*/
DWORD_T        u_dwDBFlag  [(DB_M3_TEMPER_NUM+31)/32];
/*==============================================================================+
|            Global Variable                                                    |
+==============================================================================*/
WORD_T         g_CardNum;
WORD_T         g_SegNum;
//WORD_T         g_CardID[CARD_MAX];
DB_DATA        g_DBData[DB_M3_TEMPER_NUM];


const   DB_DATAATTRIBUTE		g_DataAttr[] =
{
	{DBID_CARD_0_INTERFACE_IO_INPUTG1                       ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_INPUTG2                       ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_INPUTG3                       ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_INPUTG4                       ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_INPUTG5                       ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_INPUTG6                       ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_INPUTG7                       ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_INPUTG8                       ,   _WORD , DB_RW,  NULL},

	{DBID_CARD_0_INTERFACE_IO_OUTPUTG1                      ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_OUTPUTG2                      ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_OUTPUTG3                      ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_OUTPUTG4                      ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_OUTPUTG5                      ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_OUTPUTG6                      ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_OUTPUTG7                      ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_OUTPUTG8                      ,   _WORD , DB_RW,  NULL},

	{DBID_CARD_0_INTERFACE_IO_AD1                           ,   _REAL , DB_RW,  NULL},
	{DBID_CARD_0_INTERFACE_IO_AD2                           ,   _REAL , DB_RW,  NULL},
	{DBID_CARD_0_INTERFACE_IO_AD3                           ,   _REAL , DB_RW,  NULL},
	{DBID_CARD_0_INTERFACE_IO_AD4                           ,   _REAL , DB_RW,  NULL},

    {DBID_CARD_0_INTERFACE_IO_DA1                           ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_DA2                           ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_DA3                           ,   _WORD , DB_RW,  NULL},
    {DBID_CARD_0_INTERFACE_IO_DA4                           ,   _WORD , DB_RW,  NULL},
};


/*===========================================================================+
|Name:        DB_Init                                                        |
|Description: Initial Data and DataFlag of database                          |
+===========================================================================*/
void	DB_Init()
{
//
//	memset ( &g_DBData,   0, sizeof( g_DBData   ) );
//	memset ( &u_dwDBFlag, 0, sizeof( u_dwDBFlag ) );
}
/*===========================================================================+
|Name:        DBSetValueByDataID                                             |
|Description: Set database value by Data Id                                  |
+===========================================================================*/
void	DBSetValueByDataID( DWORD_T dwDataID, DBVALUE dbValue )
{
    long      lOffset;

	lOffset = DataIDToOffset( dwDataID );

	if(( lOffset >= ENUM_CARD_0_INTERFACE_IO_INPUTG1 )&&( lOffset < DB_M3_TEMPER_NUM ))
	{
	 	DBSetValueByOffset( lOffset,  dbValue );
	}
}
/*===========================================================================+
|Name:        DataIDToOffset                                                 |
|Description: Search Data ID  by the Offset                                  |
+===========================================================================*/
long	DataIDToOffset( DWORD_T	dwDataID )
{
    long     lOffset;
	WORD_T	 wMinOffset, wMaxOffset;

	wMinOffset 	= 0;
	wMaxOffset	= DB_M3_TEMPER_NUM - 1;

	while ( wMinOffset <= wMaxOffset )
	{
		lOffset = ( wMinOffset + wMaxOffset ) >> 1;

		if( g_DataAttr[lOffset].dwID == dwDataID )
		{
			return lOffset ;
		}

		if( dwDataID > g_DataAttr[lOffset].dwID )
		{
			wMinOffset = lOffset + 1;
		}

		else
		{
			wMaxOffset = lOffset - 1;
		}
	}
	return   -1;
}
/*===========================================================================+
|Name:        DBSetValueByOffset                                             |
|Description: Set database value by the Offset                               |
+===========================================================================*/
void	DBSetValueByOffset( long lOffset, DBVALUE dbValue )
{

    if(( lOffset >= ENUM_CARD_0_INTERFACE_IO_INPUTG1 ) && ( lOffset <= DB_M3_TEMPER_NUM ))
	{
	    DBSetFlag( lOffset );
	    memcpy( &g_DBData[lOffset].dbValue, &dbValue, sizeof( DBVALUE ) );
	}
}
/*===========================================================================+
|Name:        DBRun                                                          |
|Description: Database Processor                                             |
+===========================================================================*/
void	DBRun()
{
//   WORD_T 		i, j;
//   DWORD_T 		dwOffset;
//
//   for ( i = 0; i < ( DB_M3_TEMPER_NUM+31 )/32; i++ )
//   {
//	   	if ( u_dwDBFlag[i] != 0 )
//        {
//            for ( j = 0; j < 32; j++ )
//            {
//                if ( u_dwDBFlag[i] & ( 1<<j ) )
//                {
//                    dwOffset  =  ( i<<5 ) + j;
//					if( g_DataAttr[dwOffset].DealFunc != NULL )
//					{
//	                    if ( g_DataAttr[dwOffset].DealFunc( dwOffset ) )
//	                    {
//	                        DBCleanFlag( dwOffset );
//	                    }
//					}
//                }
//            }
//        }
//   }
}
/*===========================================================================+
|Name:        DBGetValueByDataID                                             |
|Description: Get a Value from database by Data ID                           |
+===========================================================================*/
DBVALUE		DBGetValueByDataID( DWORD_T dwDataID )
{
	DBVALUE		dbValue;
	long        lOffSet;

	dbValue.dwData = 0;

	lOffSet = DataIDToOffset( dwDataID );

	if(( lOffSet >= ENUM_CARD_0_INTERFACE_IO_INPUTG1 ) && ( lOffSet < DB_M3_TEMPER_NUM ))
	{
    	memcpy( &dbValue,&g_DBData[lOffSet].dbValue,sizeof( DBVALUE ) );
	}

	return dbValue;
}

/*===========================================================================+
|Name:        DBGetValueByOffset                                             |
|Description: Get a Value from database by the Offset                        |
+===========================================================================*/
DBVALUE		DBGetValueByOffset( long lOffset )
{
	DBVALUE		dbValue ;
	dbValue.dwData = 0;

	if(( lOffset >= ENUM_CARD_0_INTERFACE_IO_INPUTG1 ) && ( lOffset <= DB_M3_TEMPER_NUM ))
	{
	    memcpy( &dbValue, &g_DBData[lOffset].dbValue, sizeof( DBVALUE ) );
	}
	return	  dbValue;
}
/*===========================================================================+
|Name:        DBSetFlag                                                      |
|Description: Set database Flag by the Offset                                |
+===========================================================================*/
void	DBSetFlag( DWORD_T	dwOffset )
{
	if ( dwOffset >= DB_M3_TEMPER_NUM )
	{
		return;
	}
	u_dwDBFlag[dwOffset>>5] |= 1<<( dwOffset%32 );
}
/*===========================================================================+
|Name:        DBCleanFlag                                                    |
|Description: ReSet database Flag by the Offset                              |
+===========================================================================*/
void	DBCleanFlag( DWORD_T	dwOffset )
{
//	if ( dwOffset >= DB_M3_TEMPER_NUM )
//	{
//		return;
//	}
//	u_dwDBFlag[dwOffset>>5] &= ~(1<<( dwOffset%32 ));
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
