/*==============================================================================+
|  Function : Input                                                             |
|  Task     : Input Source File                                                 |
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
#include    "driver/include/_input.h"
#include    "driver/include/_timer.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef     struct tagINPUT_DATA            INPUT_DATA_T;
typedef     struct tagINPUT_HWCONF          INPUT_HWCONF_T;
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/

/*==============================================================================+
|           Class declaration -                                      |
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Helpers Implementation                                              |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Input_Open     (   void *);
static  void            _Input_Close    (   void  );
static  SIZE_T          _Input_Read     (   BYTE_T *, SIZE_T, OFFSET_T );
static  ERROR_T         _Input_Control  (   BYTE_T  , void * );
static  void            _Input_Scan     (   void    );
/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
INPUT_HWCONF_T          u_aInputHwConf[MAX_INPUT] =
{
#ifdef	M2C2132
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_7},
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_6},
#endif
#if (defined IO2C1111) || (defined IO2C1112)
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_7},
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_6},
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_5},
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_4},
	{SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_0},
#endif
#ifdef IO2C1131
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_7},
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_6},
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_5},
	{SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_4},
	{SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_0},
	{SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_1},
	{SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_2},
	{SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_3},
#endif
#ifdef IO2C1122
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_7},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_6},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_5},
    {SYSCTL_PERIPH_GPIOC, GPIO_PORTC_BASE, GPIO_PIN_4},
    {SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_0},
    {SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_1},
    {SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_2},
    {SYSCTL_PERIPH_GPIOH, GPIO_PORTH_BASE, GPIO_PIN_3},
#endif
};


INPUT_DATA_T            u_Input_Data;

DEV_ENGINE_T            g_Input_Engine  =
{
    _Input_Open,
    _Input_Close,
    _Input_Read,
    NULL,
    _Input_Control,
    &u_Input_Data
};

ROUTINE     u_InputRoutine  =   {1, _Input_Scan};
/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Input_Open                                                          |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Input_Open     (   void        *pArg   )
{
    int     i;

    for (i = 0; i < MAX_INPUT; i++)
    {
        SysCtlPeripheralEnable(u_aInputHwConf[i].dwPeriph);
        GPIOPinTypeGPIOInput(u_aInputHwConf[i].dwPort, u_aInputHwConf[i].byPin);
    }
    u_Input_Data.wSetFilterCount = 4;     //Add 2016/03/31
    AddRoutine(&u_InputRoutine);

    return  ERROR_NONE;
}
/*------------------------------------------------------------------------------+
|           Input_Close                                                         |
+------------------------------------------------------------------------------*/
static  void            _Input_Close    (   )
{
    DeleteRoutine(&u_InputRoutine);
}
/*------------------------------------------------------------------------------+
|           Input_Read                                                          |
+------------------------------------------------------------------------------*/
static  SIZE_T          _Input_Read     (   BYTE_T      *pbyBuf,
                                            SIZE_T      Size,
                                            OFFSET_T    Offset  )
{
    WORD_T      i;

    BYTE_T      *pwBuf  =   pbyBuf;
    if (Size + Offset > MAX_INPUT)
        return  0;

    for (i = 0; i < Size; i++)
        pwBuf[i]    =   u_Input_Data.abyValue[Offset + i];

    return  Size;
}
/*------------------------------------------------------------------------------+
|           Input_Control                                                       |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Input_Control  (   BYTE_T      byCmd,
        void        *pArg   )
{
    switch (byCmd)
    {
        case    INPUT_CMD_SETSCANTIME:
        {
            DeleteRoutine(&u_InputRoutine);
            u_InputRoutine.tSlice   =   *(WORD_T *)pArg;
            AddRoutine(&u_InputRoutine);
            return INPUT_CMD_SETSCANTIME;
        }
        case    INPUT_CMD_SETFILTERCOUNT:
            u_Input_Data.wSetFilterCount    =   *(WORD_T *)pArg;
            return INPUT_CMD_SETFILTERCOUNT;
        default:
            return ERROR_NONE;
    }
}
/*------------------------------------------------------------------------------+
|           Input_Scan                                                          |
+------------------------------------------------------------------------------*/

static   void       _Input_Scan()
{
    WORD_T          i;
    BYTE_T          byValue ;

    for (i = 0; i < MAX_INPUT; i++)
    {
        byValue  =   (GPIOPinRead(u_aInputHwConf[i].dwPort, u_aInputHwConf[i].byPin) == 0) ?  0X00 : ~(0X00) ;

        if (byValue != u_Input_Data.abyValue[i])
        {
            u_Input_Data.awRealFilterCount[i]++;

            if (u_Input_Data.awRealFilterCount[i] > u_Input_Data.wSetFilterCount)
                u_Input_Data.abyValue[i] =   byValue;
        }
        else
            u_Input_Data.awRealFilterCount[i]   =   0;
    }
}
/*------------------------------------------------------------------------------+
|           end                                                             |
+------------------------------------------------------------------------------*/
