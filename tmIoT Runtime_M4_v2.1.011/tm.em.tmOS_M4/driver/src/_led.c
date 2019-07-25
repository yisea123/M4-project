/*==============================================================================+
|  Function : Led                                                               |
|  Task     : Led Source File                                                   |
|-------------------------------------------------------------------------------|
|  Compile  :                                                                   |
|  Link     :                                                                   |
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : Jia                                                               |
|  Version  : V1.00                                                             |
|  Creation : 2016/08/17                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include    "driver/include/_led.h"
#include    "driver/include/_timer.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef     struct tagLED_DATA            LED_DATA_T;
typedef     struct tagLED_HWCONF          LED_HWCONF_T;
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
long u_lCounter_LedBlink = 1000;
WORD u_wLedRunCount = 0;
/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/

/*==============================================================================+
|           Class declaration -                                      			|
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Led_Open     (   void *);
static  void            _Led_Close    (   void  );
static  SIZE_T          _Led_Write    (   BYTE_T *, SIZE_T, OFFSET_T );
static  void            _Led_Blink    (   void  );
/*------------------------------------------------------------------------------+
|           Inner Global Variable                                               |
+------------------------------------------------------------------------------*/
LED_HWCONF_T          u_aLedHwConf[MAX_LED] =
{
#ifdef M2C2132
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1},	//����ָʾ��
	{SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2}, //CANͨ��ָʾ��
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_3},	//485_1ͨ��ָʾ��
	{SYSCTL_PERIPH_GPIOA, GPIO_PORTA_BASE, GPIO_PIN_3}, //485_2ͨ��ָʾ��
#endif
#if (defined IO2C1111) || (defined IO2C1112)
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1},	//����ָʾ��
	{SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2}, //CANͨ��ָʾ��
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_3},	//485_1ͨ��ָʾ��
#endif
#ifdef IO2C1131
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1},	//����ָʾ��
	{SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2}, //CANͨ��ָʾ��
#endif
#ifdef S2C4111
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1},	//����ָʾ��
	{SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2}, //CANͨ��ָʾ��
#endif
#ifdef  IO2C1122
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_1}, //����ָʾ��
    {SYSCTL_PERIPH_GPIOF, GPIO_PORTF_BASE, GPIO_PIN_2}, //CANͨ��ָʾ��
#endif
};

DEV_ENGINE_T          g_Led_Engine  =
{
    _Led_Open,
    _Led_Close,
    NULL,
    _Led_Write,
    NULL,
    NULL
};
/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
ROUTINE     u_LedRoutine  =   {1, _Led_Blink};
/*------------------------------------------------------------------------------+
|           Helpers                                                             |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Led_Open     (   void        *pArg   )
{
    int     i;
    for (i = 0; i < MAX_LED; i++)
    {
        SysCtlPeripheralEnable(u_aLedHwConf[i].dwPeriph);
        GPIOPinTypeGPIOOutput(u_aLedHwConf[i].dwPort, u_aLedHwConf[i].byPin);
    }
    AddRoutine(&u_LedRoutine);
    AddDnCounter(&u_lCounter_LedBlink);

    return  ERROR_NONE;
}
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
static  void            _Led_Close    (   )
{

}

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
static  SIZE_T          _Led_Write   (      BYTE_T      *pbyBuf,
                                            SIZE_T      Size,
                                            OFFSET_T    Offset  )
{
    WORD_T      i;

    if (Size + Offset > MAX_LED)
        return  0;

    for (i = 0; i < Size; i++)
    {
        GPIOPinWrite(u_aLedHwConf[Offset + i].dwPort, u_aLedHwConf[Offset + i].byPin, pbyBuf[i] ); //Modify 2016/03/31
    }
    return  Size;
}

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
static  void            _Led_Blink    (   )
{
	static BYTE_T u_byLedBlink = 0xFF;

	if(u_lCounter_LedBlink > 0)
		return;

	if(u_wLedRunCount > 0){
		u_lCounter_LedBlink = 200;
		u_wLedRunCount--;
	}else{
		u_lCounter_LedBlink = 1000;
	}

	u_byLedBlink = ~u_byLedBlink;

	_Led_Write(&u_byLedBlink, 1, 0);
}

//runָʾ�ƿ�����˸
void _Led_RunFast()
{
	u_wLedRunCount = 10;
}
