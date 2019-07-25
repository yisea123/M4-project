/*==============================================================================+
|  Function : da                                                                |
|  Task     : da Source File                                                    |
|-------------------------------------------------------------------------------|
|  Compile  :                                               	                |
|  Link     :                                                  					|
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   :  Jia                                                            	|
|  Version  : V1.00                                                             |
|  Creation : 2016/09/26                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include	"driver/include/_da.h"
#include	"driver/include/_timer.h"

//#define		test_AD_CHANEL34
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

#define	           SLOW_MODE	  			0x0000
#define	           FAST_MODE	  			0x4000

#define	           CHANNEL_DA_A  			0x8000
#define	           CHANNEL_DA_B  			0x1000

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef     	   struct tagDA_DATA        DA_DATA_T;
typedef     	   struct tagDA_HWCONF      DA_HWCONF_T;
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Da_Open      (            void *            );
static  void            _Da_Close     (            void              );
static  SIZE_T          _Da_Write     (   BYTE_T *, SIZE_T, OFFSET_T );
/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/

DEV_ENGINE_T            g_Da_Engine  =
{
    _Da_Open,
    _Da_Close,
    NULL,
	_Da_Write,
    NULL,
    NULL
};
/*------------------------------------------------------------------------------+
|                         Da_Open                                               |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Da_Open     (   void        *pArg   )
{
#ifdef S2C4111
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    GPIOPinConfigure(   GPIO_PD3_SSI2CLK    );
    GPIOPinConfigure(   GPIO_PD2_SSI2FSS    );
    GPIOPinConfigure(   GPIO_PD1_SSI2XDAT0  );
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_2);
    GPIOPinTypeSSI(GPIO_PORTD_BASE, GPIO_PIN_3);
    SSIConfigSetExpClk(SSI2_BASE, g_dwSystemClock, SSI_FRF_TI, SSI_MODE_MASTER, BITRATE_DA , DATAWIDTH_DA );
    SSIEnable(SSI2_BASE);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(   GPIO_PA2_SSI0CLK    );
    GPIOPinConfigure(   GPIO_PA3_SSI0FSS    );
    GPIOPinConfigure(   GPIO_PA4_SSI0XDAT0  );
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_2);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_3);
    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_4);
    SSIConfigSetExpClk(SSI0_BASE, g_dwSystemClock, SSI_FRF_TI, SSI_MODE_MASTER, BITRATE_DA , DATAWIDTH_DA );
    SSIEnable(SSI0_BASE);

    return  ERROR_NONE;
#endif
}
/*------------------------------------------------------------------------------+
|                         Da_Close                                               |
+------------------------------------------------------------------------------*/
static  void            _Da_Close    (   )
{

}

/*------------------------------------------------------------------------------+
|                         Da_Write                                              |
+------------------------------------------------------------------------------*/

static  SIZE_T          _Da_Write     (     BYTE_T      *pbyBuf,
                                            SIZE_T      Size,
                                            OFFSET_T    Offset  )
{
#ifdef S2C4111
    BYTE_T      i;
    WORD_T      *pwBuf  =   (WORD_T *)pbyBuf;

    if (Size + Offset > MAX_DA)
        return  0;

     for (i = Offset; i < Size+Offset; i++)
     {
          switch(i)
          {
			  case 0:
			  {
				  SSIDataPut(SSI2_BASE, FAST_MODE | CHANNEL_DA_A | (0x0fff&pwBuf[0]));
				  while(SSIBusy(SSI2_BASE));
				  break;
			  }
			  case 1:
			  {
				  SSIDataPut(SSI2_BASE, FAST_MODE | CHANNEL_DA_B | (0x0fff&pwBuf[1]));
				  while(SSIBusy(SSI2_BASE));
				  break;
			  }
			  case 2:
			  {
				  SSIDataPut(SSI0_BASE, FAST_MODE | CHANNEL_DA_A | (0x0fff&pwBuf[2]));
				  while(SSIBusy(SSI0_BASE));
				  break;
			  }
			  case 3:
			  {
				  SSIDataPut(SSI0_BASE, FAST_MODE | CHANNEL_DA_B | (0x0fff&pwBuf[3]));
				  while(SSIBusy(SSI0_BASE));
				  break;
			  }
          }
      }
    return  Size;
#endif
}

/*------------------------------------------------------------------------------+
|                             end                                               |
+------------------------------------------------------------------------------*/
