/*==============================================================================+
|  Function : ad                                                                |
|  Task     : ad Source File                                                    |
|-------------------------------------------------------------------------------|
|  Compile  :                                                                   |
|  Link     :                                                                   |
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : Jia                                                               |
|  Version  : V1.00                                                             |
|  Creation : 2016/09/26                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include	"driver/include/_ad.h"
#include	"driver/include/_timer.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
DWORD      u_dwAdValueBuf[MAX_AD][AD_FILTER_MAX];
BOOL	   u_bFilterFlag[MAX_AD] = {FALSE,FALSE,FALSE,FALSE};

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef     struct tagAD_DATA            AD_DATA_T;
typedef     struct tagAD_HWCONF          AD_HWCONF_T;
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Ad_Open     (            void *            );
static  void            _Ad_Close    (            void              );
static  SIZE_T          _Ad_Read     (   BYTE_T *, SIZE_T, OFFSET_T );
static  ERROR_T         _Ad_Control  (   BYTE_T  , void *           );
static  void            _Ad_Scan     (            void              );
static  WORD_T  GetFilterADValue(WORD_T wAdChannel);
/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
AD_HWCONF_T          u_aAdHwConf[SELECT] =
{
#ifdef S2C4111
    {SYSCTL_PERIPH_GPIOG, GPIO_PORTG_BASE, GPIO_PIN_1},
    {SYSCTL_PERIPH_GPIOG, GPIO_PORTG_BASE, GPIO_PIN_0},
#endif
};

/********************************************************************/

AD_DATA_T               u_Ad_Data;

DEV_ENGINE_T            g_Ad_Engine  =
{
    _Ad_Open,
    _Ad_Close,
    _Ad_Read,
    NULL,
    _Ad_Control,
    &u_Ad_Data
};

ROUTINE     u_AdRoutine  =   {1, _Ad_Scan};
/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|                               Ad_Open                                    |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Ad_Open     (   void        *pArg   )
{
#ifdef S2C4111

    int     i;

    for (i = 0; i < SELECT; i++)
    {
        SysCtlPeripheralEnable(u_aAdHwConf[i].dwPeriph);
        GPIOPinTypeGPIOOutput(u_aAdHwConf[i].dwPort, u_aAdHwConf[i].byPin);
		GPIOPadConfigSet(u_aAdHwConf[i].dwPort,u_aAdHwConf[i].byPin,GPIO_STRENGTH_4MA,GPIO_PIN_TYPE_STD);
    }

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI1);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);

    GPIOPinConfigure( GPIO_PB5_SSI1CLK );
    GPIOPinConfigure( GPIO_PE5_SSI1XDAT1 );

    GPIOPinTypeSSI(GPIO_PORTB_BASE, GPIO_PIN_5);
    GPIOPinTypeSSI(GPIO_PORTE_BASE, GPIO_PIN_5);

	SSIConfigSetExpClk(SSI1_BASE, g_dwSystemClock, SSI_FRF_MOTO_MODE_3,
						SSI_MODE_MASTER, BITRATE_AD,  DATAWIDTH  );
    SSIEnable(SSI1_BASE);
    GPIOPinTypeGPIOOutput   ( GPIO_PORTB_BASE,  GPIO_PIN_4 );
    u_Ad_Data.wSetFilterCount = AD_FILTER_MAX;
    memset(u_dwAdValueBuf, 0, sizeof(u_dwAdValueBuf));

    AddRoutine(&u_AdRoutine);
#endif
    return  ERROR_NONE;
}
/*------------------------------------------------------------------------------+
|                             Ad_Close                                          |
+------------------------------------------------------------------------------*/
static  void            _Ad_Close    (   )
{
#ifdef S2C4111
    DeleteRoutine(&u_AdRoutine);
#endif
}

/*------------------------------------------------------------------------------+
|                               Ad_Read                                          |
+------------------------------------------------------------------------------*/
static  SIZE_T          _Ad_Read     (      BYTE_T      *pbyBuf,
                                            SIZE_T      Size,
                                            OFFSET_T    Offset  )
{
#ifdef S2C4111
    WORD_T      i=0;
    WORD_T      *pwBuf  =   (WORD_T *)pbyBuf;

    if (Size + Offset > MAX_AD)
        return  0;

    for (i = 0; i < Size; i++)
    {
       pwBuf[i] = GetFilterADValue(Offset + i);
//       pwBuf[i] = pwBuf[i]>>4;   //16位AD，右移4位保证12位精度
	}

#endif
    return  Size;
}

/*------------------------------------------------------------------------------+
|                              Ad_Control                                        |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Ad_Control  (     BYTE_T      byCmd,
                                           void        *pArg   )
{
#ifdef S2C4111
    switch (byCmd)
    {
        case    AD_CMD_SETSCANTIME:
        {
            DeleteRoutine(&u_AdRoutine);
            u_AdRoutine.tSlice   =   *(WORD_T *)pArg;
            AddRoutine(&u_AdRoutine);
            return AD_CMD_SETSCANTIME;
        }
        case    AD_CMD_SETFILTERCOUNT:
              u_Ad_Data.wSetFilterCount    =   *(WORD_T *)pArg;
              if(u_Ad_Data.wSetFilterCount > AD_FILTER_MAX)
                  u_Ad_Data.wSetFilterCount = AD_FILTER_MAX;
              return AD_CMD_SETFILTERCOUNT;
        default:
              return ERROR_NONE;
    }
#endif
}
/*------------------------------------------------------------------------------+
|                                   Ad_CHS                                      |
+------------------------------------------------------------------------------*/
static  void  Ad_ChannelSelection(WORD_T CHS_Data )
{
#ifdef S2C4111
	WORD_T i;
	WORD_T CHS_IO[SELECT];
	for(i = 0; i < SELECT; i++)
	{
		CHS_IO[i]   =    CHS_Data % SELECT;
		CHS_Data	=	 CHS_Data / SELECT;
		if( CHS_IO[i] == 1)
		{
			GPIOPinWrite(u_aAdHwConf[i].dwPort, u_aAdHwConf[i].byPin,u_aAdHwConf[i].byPin);
		}
		else
			GPIOPinWrite(u_aAdHwConf[i].dwPort, u_aAdHwConf[i].byPin,~u_aAdHwConf[i].byPin);
	}
#endif
}

/*------------------------------------------------------------------------------+
|                                   Ad_SCAN                                     |
+------------------------------------------------------------------------------*/
static   void       _Ad_Scan()
{
#ifdef S2C4111
    WORD_T          AD_ID ;

    for( AD_ID = 0 ; AD_ID < MAX_AD;  AD_ID++ )
    {
       Ad_ChannelSelection( AD_ID );
       SysCtlDelay(150);
       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0x0);
       SysCtlDelay(150);
       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0xff);
       SysCtlDelay(150);
       GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0x0);

       SSIDataPut(SSI1_BASE, 0xABAB);  //  Generate Clock

       while(SSIBusy(SSI1_BASE));
           SSIDataGet(SSI1_BASE, (uint32_t*)(&u_dwAdValueBuf[AD_ID][u_Ad_Data.awRealFilterCount[AD_ID]]));

       u_Ad_Data.awRealFilterCount[AD_ID]++;
       if(u_Ad_Data.awRealFilterCount[AD_ID] >= u_Ad_Data.wSetFilterCount)
       {
           u_Ad_Data.awRealFilterCount[AD_ID] = 0;
           u_bFilterFlag[AD_ID] = TRUE;
       }
    }
#endif
}

/*------------------------------------------------------------------------------+
|                             GetFilterADValue                                     |
+------------------------------------------------------------------------------*/
WORD_T  GetFilterADValue(WORD_T wAdChannel)
{
#ifdef S2C4111
    DWORD   dwSumAdValue = 0;
    WORD    wFilterAdValue = 0;
    int i = 0;

    while( u_bFilterFlag[wAdChannel] == FALSE );

    for(i = 0; i < u_Ad_Data.wSetFilterCount; i++)
    {
        dwSumAdValue += u_dwAdValueBuf[wAdChannel][i];
    }

    wFilterAdValue = dwSumAdValue/u_Ad_Data.wSetFilterCount;

    return wFilterAdValue;
#endif
}

/*------------------------------------------------------------------------------+
|               end                                                             |
+------------------------------------------------------------------------------*/
