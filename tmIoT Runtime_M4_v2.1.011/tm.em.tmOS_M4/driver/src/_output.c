/*==============================================================================+
|  Function : Output                                                            |
|  Task     : Output Source File                                                |
|-------------------------------------------------------------------------------|
|  Compile  :                                                                   |
|  Link     :                                                                   |
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : Jia                                                              |
|  Version  : V1.00                                                             |
|  Creation : 2016/08/17                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include    "kernel/include/device.h"
#include 	"driver/include/_output.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/

/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef     struct tagOUTPUT_DATA            OUTPUT_DATA_T;
typedef     struct tagOUTPUT_HWCONF          OUTPUT_HWCONF_T;
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
static  ERROR_T         _Output_Open     (   void *);
static  void            _Output_Close    (   void  );
static  SIZE_T          _Output_Write     (   BYTE_T *, SIZE_T, OFFSET_T );
/*------------------------------------------------------------------------------+
|           Inner Globle Variable                                               |
+------------------------------------------------------------------------------*/
OUTPUT_HWCONF_T          u_aOutputHwConf[MAX_OUTPUT] =
{
#ifdef	M2C2132
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_3},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_2},
#endif
#if (defined IO2C1111) || (defined IO2C1112)
	{SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, GPIO_PIN_0},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_0},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_1},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_2},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_3},
#endif
#ifdef	IO2C1131
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_3},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_2},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_1},
	{SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_0},
	{SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, GPIO_PIN_0},
	{SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, GPIO_PIN_1},
#endif
#ifdef  IO2C1122
    {SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_3},
    {SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_2},
    {SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_1},
    {SYSCTL_PERIPH_GPIOM, GPIO_PORTM_BASE, GPIO_PIN_0},
    {SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, GPIO_PIN_0},
    {SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, GPIO_PIN_1},
    {SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, GPIO_PIN_2},
    {SYSCTL_PERIPH_GPIOL, GPIO_PORTL_BASE, GPIO_PIN_3},
#endif
};

DEV_ENGINE_T            g_Output_Engine  =
{
    _Output_Open,
    _Output_Close,
    NULL,
    _Output_Write,
    NULL,
    NULL,
};

/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Helpers                                                             |
+------------------------------------------------------------------------------*/
static  ERROR_T         _Output_Open     (   void        *pArg   )
{
    int     i;

    for (i = 0; i < MAX_OUTPUT; i++)
    {
        SysCtlPeripheralEnable(u_aOutputHwConf[i].dwPeriph);
        GPIOPinTypeGPIOOutput(u_aOutputHwConf[i].dwPort, u_aOutputHwConf[i].byPin);
    }

    return  ERROR_NONE;
}

/*------------------------------------------------------------------------------+
|           Output_Close                                                        |
+------------------------------------------------------------------------------*/
static  void            _Output_Close    (   )
{

}
/*------------------------------------------------------------------------------+
|           Input_Write                                                         |
+------------------------------------------------------------------------------*/
static  SIZE_T          _Output_Write     ( BYTE_T      *pbyBuf,
                                            SIZE_T      Size,
                                            OFFSET_T    Offset  )
{
    WORD_T      i;

    if (Size + Offset > MAX_OUTPUT)
        return  0;

    for (i = 0; i < Size; i++)
    {
        GPIOPinWrite(u_aOutputHwConf[Offset + i].dwPort, u_aOutputHwConf[Offset + i].byPin, (pbyBuf[i]==0)?0:0xFF);    //Modify 2018/09/25
    }

    return  Size;
}

