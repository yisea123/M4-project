/*==============================================================================+
|  Function : Device                                                            |
|  Task     : Device Source File                                                |
|-------------------------------------------------------------------------------|
|  Compile  :                                               	|
|  Link     :                                                  	|
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : Wain                                                              |
|  Version  : V1.00                                                             |
|  Creation : 2013/07/10                                                        |
|  Revision :                                                                   |
+==============================================================================*/

#include    "kernel/include/device.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/

/*==============================================================================+
|           Class declaration - Device                                          |
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
DEVICE_T                    u_aDeviceTable[DEVICE_MAX_NBR + 1];
WORD_T                      u_wDeviceCount      =   0;
/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/
HANDLE_T                    Device_Create           (   DEV_ENGINE_T            *pEngine     )
{  
    HANDLE_T      i;
	if (u_wDeviceCount == 0)
	{
		memset ( u_aDeviceTable, 0 , sizeof(u_aDeviceTable) );
	}   	

    for (i = 1; i <= DEVICE_MAX_NBR; i++ )
    {
        if (!(u_aDeviceTable[i].byFlag & DEVICE_CREATED) )
            break;
    }
    
    if (i <= DEVICE_MAX_NBR)
    {
        u_wDeviceCount++;
        u_aDeviceTable[u_wDeviceCount].byFlag       =   DEVICE_CREATED;
        u_aDeviceTable[u_wDeviceCount].pEngine      =   pEngine ;
        
        return  i;
    }
    
    return  DEVICE_HANDLE_NULL;
        
}


ERROR_T                     Device_Destroy          (   HANDLE_T                Handle      )
{
    if (Handle > DEVICE_MAX_NBR)
        return  ERROR_DEV_HANDLE_NOTEXIST;
        
    u_aDeviceTable[Handle].byFlag           =   0;
    
    return      ERROR_NONE;
}
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
ERROR_T                     Device_Open             (   HANDLE_T                Handle,
                                                        void                    *pArg       )
{
    if ( !(u_aDeviceTable[Handle].byFlag & DEVICE_CREATED ) )
        return ERROR_DEV_NOTCREATE;
        
    u_aDeviceTable[Handle].byFlag     |=  DEVICE_OPENED;
    
    if (u_aDeviceTable[Handle].pEngine->Open)
       return   u_aDeviceTable[Handle].pEngine->Open(pArg);
    
    return  ERROR_NONE;
}


void                        Device_Close            (   HANDLE_T                Handle      )
{
    if (u_aDeviceTable[Handle].byFlag & DEVICE_OPENED )
    {
        u_aDeviceTable[Handle].byFlag   &=   ~DEVICE_OPENED;
        
        if (u_aDeviceTable[Handle].pEngine->Close)
            u_aDeviceTable[Handle].pEngine->Close();
    }
}

SIZE_T                      Device_Read             (   HANDLE_T                Handle,
                                                        BYTE_T                  *pbyBuf,
                                                        SIZE_T                  Size,
                                                        OFFSET_T                Offset      )
{
    if (u_aDeviceTable[Handle].byFlag & DEVICE_OPENED &&
        u_aDeviceTable[Handle].pEngine->Read)
    {
        return  u_aDeviceTable[Handle].pEngine->Read    (   pbyBuf,
                                                            Size,
                                                            Offset  );
    }
    
    return  0;
}


SIZE_T                      Device_Write            (   HANDLE_T                Handle,
                                                        BYTE_T                  *pbyBuf,
                                                        SIZE_T                  Size,
                                                        OFFSET_T                Offset      )
{
    if (u_aDeviceTable[Handle].byFlag & DEVICE_OPENED &&
        u_aDeviceTable[Handle].pEngine->Write)
    {
        return  u_aDeviceTable[Handle].pEngine->Write   (   pbyBuf,
                                                                    Size,
                                                                    Offset  );
    }
    
    return     0;
}

ERROR_T                     Device_Control          (   HANDLE_T                Handle,
                                                        BYTE_T                  byCmd,
                                                        void                    *pArg       )
{
    if (u_aDeviceTable[Handle].byFlag & DEVICE_CREATED &&
        u_aDeviceTable[Handle].pEngine->Control)
    {
        return  u_aDeviceTable[Handle].pEngine->Control  (  byCmd,
                                                                    pArg    );
    }
    
    return  ERROR_NONE;
}
/*------------------------------------------------------------------------------+
|           Helpers                                                             |
+------------------------------------------------------------------------------*/

