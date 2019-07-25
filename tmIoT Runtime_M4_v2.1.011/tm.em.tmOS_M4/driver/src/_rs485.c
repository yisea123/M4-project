/*===========================================================================+
|  Function : rs485 Driver                                                   |
|  Task     : rs485 Driver                                                   |
|----------------------------------------------------------------------------|
|  Compile  : CCS6.1 -                                                       |
|  Link     : CCS6.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Jia                                                            |
|  Version  : V1.00                                                          |
|  Creation : 2016/08/15                                                     |
|  Revision :                                                                |
+===========================================================================*/
// #include    "_rs485.h"
// #include    "_timer.h"
#include    "driver/include/_rs485.h"
#include    "driver/include/_timer.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef struct tag_RS485_PARA {
        DWORD       dwPeriph;
        DWORD       dwRxPeriph;
        DWORD       dwTxPeriph;
        DWORD       dwRTPeriph;
        DWORD       dwRxPort;
        DWORD       dwTxPort;
        DWORD       dwRTPort;
        DWORD       dwRxPin;
        DWORD       dwTxPin;
        DWORD       dwRTPin;
        DWORD       dwBase;
} _RS485_PARA;
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/
BYTE u_acRxRS485Buffer1[MAX_RS485_SIZE];
BYTE u_acTxRS485Buffer1[MAX_RS485_SIZE];
BYTE u_acRxRS485Buffer2[MAX_RS485_SIZE];
BYTE u_acTxRS485Buffer2[MAX_RS485_SIZE];
/*===========================================================================+
|           Class implementation - RS485                                     |
+===========================================================================*/

/*---------------------------------------------------------------------------+
|           Helpers  Implement                                               |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
|           Inner Global variable                                            |
+---------------------------------------------------------------------------*/
#ifdef PART_TM4C1230H6PM
const _RS485_PARA u_aRs485Para[] = {
    {
        SYSCTL_PERIPH_UART0,
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_GPIOA,
        GPIO_PORTA_BASE,
        GPIO_PORTA_BASE,
        GPIO_PORTA_BASE,
        GPIO_PIN_0,
        GPIO_PIN_1,
        GPIO_PIN_2,
        UART0_BASE
    }
};
#endif

#ifdef PART_TM4C129XNCZAD
const _RS485_PARA u_aRs485Para[] = {
    {
        SYSCTL_PERIPH_UART0,
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_GPIOA,
        SYSCTL_PERIPH_GPIOB,
        GPIO_PORTA_BASE,
        GPIO_PORTA_BASE,
        GPIO_PORTB_BASE,
        GPIO_PIN_0,
        GPIO_PIN_1,
        GPIO_PIN_0,
        UART0_BASE
    }
};
#endif

#ifdef PART_TM4C1294KCPDT
const _RS485_PARA u_aRs485Para[] = {
#ifdef M2C2132
	  {
			  SYSCTL_PERIPH_UART3,
	          SYSCTL_PERIPH_GPIOJ,
	          SYSCTL_PERIPH_GPIOJ,
	          SYSCTL_PERIPH_GPIOK,
	          GPIO_PORTJ_BASE,
	          GPIO_PORTJ_BASE,
	          GPIO_PORTK_BASE,
	          GPIO_PIN_0,
	          GPIO_PIN_1,
	          GPIO_PIN_2,
	          UART3_BASE,

	  },
	  {
			  SYSCTL_PERIPH_UART4,
	          SYSCTL_PERIPH_GPIOK,
	          SYSCTL_PERIPH_GPIOK,
	          SYSCTL_PERIPH_GPIOK,
	          GPIO_PORTK_BASE,
	          GPIO_PORTK_BASE,
	          GPIO_PORTK_BASE,
	          GPIO_PIN_0,
	          GPIO_PIN_1,
	          GPIO_PIN_3,
	          UART4_BASE,
	  },
#endif
#if (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131) || (defined IO2C1122)
	  {
			  SYSCTL_PERIPH_UART4,
	          SYSCTL_PERIPH_GPIOK,
	          SYSCTL_PERIPH_GPIOK,
	          SYSCTL_PERIPH_GPIOK,
	          GPIO_PORTK_BASE,
	          GPIO_PORTK_BASE,
	          GPIO_PORTK_BASE,
	          GPIO_PIN_0,
	          GPIO_PIN_1,
	          GPIO_PIN_2,
	          UART4_BASE,

	  },
#endif
#ifdef S2C4111
	  {
			  SYSCTL_PERIPH_UART1,
	          SYSCTL_PERIPH_GPIOJ,
	          SYSCTL_PERIPH_GPIOJ,
	          SYSCTL_PERIPH_GPION,
	          GPIO_PORTJ_BASE,
	          GPIO_PORTJ_BASE,
	          GPIO_PORTN_BASE,
	          GPIO_PIN_0,
	          GPIO_PIN_1,
	          GPIO_PIN_4,
	          UART1_BASE,

	  },
#endif
};
#endif

static	WORD    RS485Channel_1 = 0;
static  WORD    RS485Channel_2 = 1;

HRS485 u_ahRs485[MAX_RS485];

DEV_ENGINE_T            g_RS485_Engine[]  =
{
    {
        _RS485_Open,
        _Rs485_Close1,
        _RS485_Read1,
        _RS485_Write1,
        _RS485_Control,
        NULL
    },
    {
        _RS485_Open,
        _Rs485_Close2,
        _RS485_Read2,
        _RS485_Write2,
        _RS485_Control,
        NULL
    }
};
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           _Open_RS485                                                         |
+------------------------------------------------------------------------------*/
ERROR_T _RS485_Open(void  *pArg)
{
    RS485CONF* pRs485Conf = pArg;
    HRS485 *phRs485;

    WORD u_wChannel = pRs485Conf->wChannel;
    phRs485 = &u_ahRs485[u_wChannel];
    
    SysCtlPeripheralEnable(u_aRs485Para[u_wChannel].dwPeriph);
    SysCtlPeripheralEnable(u_aRs485Para[u_wChannel].dwRxPeriph);
    SysCtlPeripheralEnable(u_aRs485Para[u_wChannel].dwTxPeriph);
    SysCtlPeripheralEnable(u_aRs485Para[u_wChannel].dwRTPeriph);

	#ifdef PART_TM4C1294KCPDT
    switch(u_wChannel) {
    	 case RS485_1:
//    		 GPIOPinConfigure(GPIO_PA0_U0RX);
//    		 GPIOPinConfigure(GPIO_PA1_U0TX);
//     		 GPIOPinConfigure(GPIO_PC6_U5RX);
//     		 GPIOPinConfigure(GPIO_PC7_U5TX);
#if (defined M2C2132) || (defined S2C4111)
     	    GPIOPinConfigure(GPIO_PJ0_U3RX);
     	    GPIOPinConfigure(GPIO_PJ1_U3TX);
#endif
#if (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131) || (defined IO2C1122)
     	    GPIOPinConfigure(GPIO_PK0_U4RX);
     	    GPIOPinConfigure(GPIO_PK1_U4TX);
#endif
    		 break;
    	 case RS485_2:
//    		 GPIOPinConfigure(GPIO_PD4_U2RX);
//    		 GPIOPinConfigure(GPIO_PD5_U2TX);
#ifdef M2C2132
    		 GPIOPinConfigure(GPIO_PK0_U4RX);
    		 GPIOPinConfigure(GPIO_PK1_U4TX);
#endif
        	break;
    	 default:
    		 break;
    }
	#endif

    GPIOPinTypeUART(u_aRs485Para[u_wChannel].dwRxPort, u_aRs485Para[u_wChannel].dwRxPin);
    GPIOPinTypeUART(u_aRs485Para[u_wChannel].dwTxPort, u_aRs485Para[u_wChannel].dwTxPin);
    GPIOPinTypeGPIOOutput(u_aRs485Para[u_wChannel].dwRTPort, u_aRs485Para[u_wChannel].dwRTPin);

    switch (u_wChannel) {
    	case RS485_1:
    	    memset(u_acRxRS485Buffer1, 0, sizeof(u_acRxRS485Buffer1));
    	    memset(u_acTxRS485Buffer1, 0, sizeof(u_acTxRS485Buffer1));
    	    phRs485->queueRx.pQueue     = u_acRxRS485Buffer1;
    	    phRs485->queueRx.wEnd       = sizeof(u_acRxRS485Buffer1);
    	    phRs485->queueRx.wInput     =
    	    phRs485->queueRx.wOutput    = 0;
    	    phRs485->queueTx.pQueue     = u_acTxRS485Buffer1;
    	    phRs485->queueTx.wEnd       = sizeof(u_acTxRS485Buffer1);
    	    phRs485->queueTx.wInput     =
    	    phRs485->queueTx.wOutput    = 0;
    		break;
    	case RS485_2:
    	    memset(u_acRxRS485Buffer2, 0, sizeof(u_acRxRS485Buffer2));
    	    memset(u_acTxRS485Buffer2, 0, sizeof(u_acTxRS485Buffer2));
    	    phRs485->queueRx.pQueue     = u_acRxRS485Buffer2;
    	    phRs485->queueRx.wEnd       = sizeof(u_acRxRS485Buffer2);
    	    phRs485->queueRx.wInput     =
    	    phRs485->queueRx.wOutput    = 0;
    	    phRs485->queueTx.pQueue     = u_acTxRS485Buffer2;
    	    phRs485->queueTx.wEnd       = sizeof(u_acTxRS485Buffer2);
    	    phRs485->queueTx.wInput     =
    	    phRs485->queueTx.wOutput    = 0;
    		break;
    	default:
    		break;
    }

    _Set_RS485_Mode(u_wChannel,  RS485_READ);
    UARTConfigSetExpClk(u_aRs485Para[u_wChannel].dwBase, g_dwSystemClock, pRs485Conf->dwBaud, pRs485Conf->dwMode);
    UARTEnable(u_aRs485Para[u_wChannel].dwBase);

    return ERROR_NONE;
}

/*------------------------------------------------------------------------------+
|           _Close_RS485                                                        |
+------------------------------------------------------------------------------*/
void _Rs485_Close1()
{
    UARTDisable(u_aRs485Para[RS485Channel_1].dwBase);
}
void _Rs485_Close2()
{
    UARTDisable(u_aRs485Para[RS485Channel_2].dwBase);
}

/*------------------------------------------------------------------------------+
|           _RS485_Write                                                        |
+------------------------------------------------------------------------------*/
WORD_T _RS485_Write1(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
{
    int i = 0;
    HRS485* phRs485 = &u_ahRs485[RS485Channel_1];

    if(wSize > MAX_RS485_SIZE) return COM_E_TX_FULL;
    for(i = 0; i < wSize; i++)
    {
        *(phRs485->queueTx.pQueue + i) = pData[i];
    }
    phRs485->queueTx.wOutput = wSize;
    return COM_SUCCESS;
}

WORD_T _RS485_Write2(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
{
    int i = 0;
    HRS485* phRs485 = &u_ahRs485[RS485Channel_2];

    if(wSize > MAX_RS485_SIZE) return COM_E_TX_FULL;
    for(i = 0; i < wSize; i++)
    {
        *(phRs485->queueTx.pQueue + i) = pData[i];
    }
    phRs485->queueTx.wOutput = wSize;
    return COM_SUCCESS;
}
/*------------------------------------------------------------------------------+
|           _RS485_Tx                                                           |
+------------------------------------------------------------------------------*/
void _RS485_Tx(WORD  u_wChannel)
{
    int i = 0;
    HRS485 *phRs485 = &u_ahRs485[u_wChannel];


    if(phRs485->queueTx.wOutput == 0) return;

    if(phRs485->wState != RS485_WRITE)
    {
        phRs485->wState = RS485_WRITE;
        _Set_RS485_Mode(u_wChannel, RS485_WRITE); 
    }

    for(i = 0; i < phRs485->queueTx.wOutput; i++)
    {
        UARTCharPut(u_aRs485Para[u_wChannel].dwBase,*(phRs485->queueTx.pQueue + i));
    }
    phRs485->queueTx.wOutput = 0;
}
/*------------------------------------------------------------------------------+
|           _Read_RS485                                                         |
+------------------------------------------------------------------------------*/
WORD _RS485_Read1(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
{
    WORD    wLen = 0;
    HRS485* phRs485 = &u_ahRs485[RS485Channel_1];

    if(phRs485->queueRx.wInput == 0 || phRs485->wRxState != FALSE)
        return wLen;

    for(wLen = 0; wLen < phRs485->queueRx.wInput; wLen++)
    {
       pData[wLen] = *(phRs485->queueRx.pQueue + wLen);
    }
#if (defined M2C2132) ||(defined IO2C1111) || (defined IO2C1112)
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, ~(uint8_t)GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_3));//RS485_1����ָʾ��
#endif
    phRs485->queueRx.wInput = 0;
    return wLen;
}

WORD _RS485_Read2(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
{
    WORD    wLen = 0;
    HRS485* phRs485 = &u_ahRs485[RS485Channel_2];

    if(phRs485->queueRx.wInput == 0 || phRs485->wRxState != FALSE)
        return wLen;

    for(wLen = 0; wLen < phRs485->queueRx.wInput; wLen++)
    {
       pData[wLen] = *(phRs485->queueRx.pQueue + wLen);
    }
#ifdef M2C2132
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, ~(uint8_t)GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_3));//RS485_2����ָʾ��
#endif
    phRs485->queueRx.wInput = 0;
    return wLen;
}

/*------------------------------------------------------------------------------+
|           _RS485_Rx                                                           |
+------------------------------------------------------------------------------*/
void _RS485_Rx(WORD u_wChannel)
{
    WORD    wInput = 0;
    HRS485* phRs485 = &u_ahRs485[u_wChannel];

    if(phRs485->wState != RS485_READ)
    {
        if(UARTBusy(u_aRs485Para[u_wChannel].dwBase) != true)
        {
            phRs485->wState = RS485_READ;
            _Set_RS485_Mode(u_wChannel, RS485_READ);
        }
    }

    if(phRs485->wState != RS485_READ )	return;

    while(UARTCharsAvail(u_aRs485Para[u_wChannel].dwBase))
    {
        wInput = phRs485->queueRx.wInput + 1;
        if(wInput == phRs485->queueRx.wEnd)
            break;
        *(phRs485->queueRx.pQueue + phRs485->queueRx.wInput) = UARTCharGetNonBlocking(u_aRs485Para[u_wChannel].dwBase);
        phRs485->queueRx.wInput = wInput;

        phRs485->dwRxTick = g_dwSystemTick;
        phRs485->wRxState = TRUE;
    }
    //濡傛灉瓒呮椂5ms娌℃湁鏀跺埌鏁版嵁璇存槑涓�寘鏁版嵁宸茬粡鏀跺畬
    if(g_dwSystemTick - phRs485->dwRxTick > 5 && phRs485->wRxState == TRUE)
    {
        phRs485->wRxState = FALSE;
    }
}
/*------------------------------------------------------------------------------+
|           _RS485_Control                                                           |
+------------------------------------------------------------------------------*/
ERROR_T _RS485_Control(BYTE_T byCmd, void *pArg)
{
    WORD Port = *((WORD*)pArg);
    printf("Port = %d\n", Port);
    
    switch (byCmd)
    {
        case RS485_CMD_TX:
            _RS485_Tx(Port);
            break;
        case RS485_CMD_RX:
            _RS485_Rx(Port);
            break;
        default:
            break;
    }
    return  ERROR_NONE;
}
/*------------------------------------------------------------------------------+
|           _Set_RS485_Mode                                                     |
+------------------------------------------------------------------------------*/
void _Set_RS485_Mode(WORD wChannel, WORD wMode)
{
    BYTE byValue = 0x0;

    if (wMode == RS485_WRITE)
        byValue = 0xFF;

    GPIOPinWrite(u_aRs485Para[wChannel].dwRTPort, u_aRs485Para[wChannel].dwRTPin, byValue);
}

