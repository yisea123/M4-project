/*==============================================================================+
|  Function : eeprom driver                                                     |
|  Task     : eeprom driver Source File                                         |
|-------------------------------------------------------------------------------|
|  Compile  : ccs6.1                                                            |
|  Link     : ccs6.1                                                            |
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : Jia                                                               |
|  Version  : V1.00                                                             |
|  Creation : 2016/08/05                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include "driver/include/_eeprom.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
DWORD   u_dwEeprom_MemorySize = 0;
//WORD    u_wEeprom_BlockSize = 0;
/*==============================================================================+
|           Class declaration -                                      |
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/
static   ERROR_T         _Eeprom_Open       (   void *                     );
static   void            _Eeprom_Close      (   void                       );
static   SIZE_T          _Eeprom_Read       (   BYTE_T *, SIZE_T, OFFSET_T );
static   SIZE_T          _Eeprom_Write      (   BYTE_T *, SIZE_T, OFFSET_T );
/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
DEV_ENGINE_T            g_Eeprom_Engine  =
{
    _Eeprom_Open,
    _Eeprom_Close,
    _Eeprom_Read,
    _Eeprom_Write,
    NULL,
    NULL
};
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Eeprom_Close                                                          |
+------------------------------------------------------------------------------*/
static  void            _Eeprom_Close    (   )
{

}
/*------------------------------------------------------------------------------+
|           Eeprom_Open                                                          |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Eeprom_Open     (   void        *pArg   )
{	 
    //WORD    wBlockCount = 0;   
    //+++++
    //娴ｈ儻鍏樼�鐟板敶闁�EPROM閻ㄥ嫭鎼锋担锟�   //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);
    //閸掓繂顫愰崠鏈嶦PROM
    if(EEPROMInit() != EEPROM_INIT_OK)  
        return ERROR_DEV_NOTCREATE;

    u_dwEeprom_MemorySize = EEPROMSizeGet();  //閼惧嘲褰嘐EPROM 缁屾椽妫挎径褍鐨�    //wBlockCount = EEPROMBlockCountGet();   //閼惧嘲褰嘐EPROM Block閺佷即鍣�    //u_wEeprom_BlockSize = u_dwEeprom_MemorySize/wBlockCount;  //鐠侊紕鐣诲В蹇庨嚋Block缁屾椽妫块惃鍕亣鐏忥拷	return ERROR_NONE;
    return ERROR_NONE;
}

//
//  wSize: must be a multiple of 4
//  dwAddr: must be a multiple of 4
//  return Returns 0 on success or non-zero values on failure
//  
static  SIZE_T          _Eeprom_Write    (   BYTE_T      *pData,
                                             SIZE_T      wSize, 
                                             OFFSET_T    dwAddr  )
{	
    DWORD   dwErr = 0;

    if(dwAddr + wSize > u_dwEeprom_MemorySize)	return ERROR_DEV_OVERMEMORY;

    dwErr = EEPROMProgram((uint32_t *)pData, dwAddr, wSize);
 	
	return	dwErr; 
}

//
//  wSize: must be a multiple of 4
//  dwAddr: must be a multiple of 4
//
static  SIZE_T          _Eeprom_Read     (   BYTE_T      *pData,
                                             SIZE_T      wSize, 
                                             OFFSET_T    dwAddr  )
{
    DWORD   dwErr = 0;

    if(dwAddr + wSize > u_dwEeprom_MemorySize)  return ERROR_DEV_OVERMEMORY;

    EEPROMRead((uint32_t *)pData, dwAddr, wSize);

	return	dwErr;   
}
