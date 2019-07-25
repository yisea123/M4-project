///*===========================================================================+
//|  Function : rs232 Driver                                                   |
//|  Task     : rs232 Driver                                                   |
//|----------------------------------------------------------------------------|
//|  Compile  : CCS6.1 -                                                       |
//|  Link     : CCS6.1 -                                                       |
//|  Call     :                                                                |
//|----------------------------------------------------------------------------|
//|  Author   : Jia                                                            |
//|  Version  : V1.00                                                          |
//|  Creation : 2016/08/15                                                     |
//|  Revision :                                                                |
//+===========================================================================*/
//#include    "driver/include/_rs232.h"
//#include    "driver/include/_timer.h"
///*===========================================================================+
//|           Constant                                                         |
//+===========================================================================*/
//
///*===========================================================================+
//|           Type definition                                                  |
//+===========================================================================*/
//typedef struct tag_RS232_PARA {
//        DWORD       dwPeriph;
//        DWORD       dwRxPeriph;
//        DWORD       dwTxPeriph;
//        DWORD       dwRxPort;
//        DWORD       dwTxPort;
//        DWORD       dwRxPin;
//        DWORD       dwTxPin;
//        DWORD       dwBase;
//} _RS232_PARA;
///*===========================================================================+
//|           Global Variable                                                  |
//+===========================================================================*/
//BYTE u_acRxRS232Buffer[MAX_RS232_SIZE];
//BYTE u_acTxRS232Buffer[MAX_RS232_SIZE];
//BYTE u_byRS232RecvTimeOut = 3;
///*===========================================================================+
//|           Class implementation - RS485                                     |
//+===========================================================================*/
//
///*---------------------------------------------------------------------------+
//|           Helpers  Implement                                               |
//+---------------------------------------------------------------------------*/
//static ERROR_T  _RS232_Open(void  *pArg);
//static void     _RS232_Close();
//static WORD_T   _RS232_Write(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);
//static WORD_T   _RS232_Read(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);
//static  ERROR_T _RS232_Control(BYTE_T byCmd, void *pArg);
///*---------------------------------------------------------------------------+
//|           Inner Global variable                                            |
//+---------------------------------------------------------------------------*/
//#ifdef PART_TM4C1230H6PM
//const _RS232_PARA u_aRs232Para[] = {
//        {
//                SYSCTL_PERIPH_UART1,
//                SYSCTL_PERIPH_GPIOC,
//                SYSCTL_PERIPH_GPIOC,
//                GPIO_PORTC_BASE,
//                GPIO_PORTC_BASE,
//                GPIO_PIN_4,
//                GPIO_PIN_5,
//                UART1_BASE
//        }
//};
//#endif
//
//#ifdef PART_TM4C129XNCZAD
//const _RS232_PARA u_aRs232Para[] = {
//        {
//                SYSCTL_PERIPH_UART0,
//                SYSCTL_PERIPH_GPIOA,
//                SYSCTL_PERIPH_GPIOA,
//                GPIO_PORTA_BASE,
//                GPIO_PORTA_BASE,
//                GPIO_PIN_0,
//                GPIO_PIN_1,
//                UART0_BASE
//        }
//};
//#endif
//
//#ifdef PART_TM4C1294KCPDT
//const _RS232_PARA u_aRs232Para[] = {
//        {
//                SYSCTL_PERIPH_UART7,
//                SYSCTL_PERIPH_GPIOC,
//                SYSCTL_PERIPH_GPIOC,
//                GPIO_PORTC_BASE,
//                GPIO_PORTC_BASE,
//                GPIO_PIN_4,
//                GPIO_PIN_5,
//                UART7_BASE
//        }
//};
//#endif
//static	WORD    u_wChannel = 0;
//HRS232 u_ahRs232[MAX_RS232];
//
//DEV_ENGINE_T            g_RS232_Engine  =
//{
//    _RS232_Open,
//    _RS232_Close,
//    _RS232_Read,
//    _RS232_Write,
//    _RS232_Control,
//    NULL
//};
///*---------------------------------------------------------------------------+
//|           Constructor and destructor                                       |
//+---------------------------------------------------------------------------*/
//
///*---------------------------------------------------------------------------+
//|           Operations                                                       |
//+---------------------------------------------------------------------------*/
//static  void _RS232_Rx();
//static  void _RS232_Tx();
///*------------------------------------------------------------------------------+
//|           _RS485_Open                                                         |
//+------------------------------------------------------------------------------*/
//static  ERROR_T  _RS232_Open( void  *pArg )
//{
//    _RS232* pRs232 = pArg;
//    HRS232 *phRs232;
//    u_wChannel = pRs232->wChannel;
//    phRs232 = &u_ahRs232[u_wChannel];
//
//    // Enable the peripherals
//    SysCtlPeripheralEnable(u_aRs232Para[u_wChannel].dwPeriph);
//    SysCtlPeripheralEnable(u_aRs232Para[u_wChannel].dwRxPeriph);
//    SysCtlPeripheralEnable(u_aRs232Para[u_wChannel].dwTxPeriph);
//	/*UART Configure*/
//	#ifdef PART_TM4C1230H6PM
//    GPIOPinConfigure(GPIO_PC4_U1RX);
//    GPIOPinConfigure(GPIO_PC5_U1TX);
//	#endif
//	#ifdef PART_TM4C129XNCZAD
//    GPIOPinConfigure(GPIO_PA0_U0RX);
//    GPIOPinConfigure(GPIO_PA1_U0TX);
//    #endif
//	#ifdef PART_TM4C1294KCPDT
//    GPIOPinConfigure(GPIO_PC4_U7RX);
//    GPIOPinConfigure(GPIO_PC5_U7TX);
//	#endif
//    GPIOPinTypeUART(u_aRs232Para[u_wChannel].dwRxPort, u_aRs232Para[u_wChannel].dwRxPin);
//    GPIOPinTypeUART(u_aRs232Para[u_wChannel].dwTxPort, u_aRs232Para[u_wChannel].dwTxPin);
//
//    // Configure the UART
//    UARTConfigSetExpClk(u_aRs232Para[u_wChannel].dwBase, g_dwSystemClock, pRs232->dwBaud, pRs232->dwConfig);
//
//    //
//    //鏍规嵁娉㈢壒鐜囧ぇ灏忚瀹氫笉鍚岃秴鏃舵椂闂磎s
//    //
//    if(pRs232->dwBaud >= 4800) u_byRS232RecvTimeOut = 5;
//    else u_byRS232RecvTimeOut = 10;
//
//    // Enable the UART operation.
//    UARTEnable(u_aRs232Para[u_wChannel].dwBase);
//
//    memset(u_acRxRS232Buffer, 0, sizeof(u_acRxRS232Buffer));
//    memset(u_acTxRS232Buffer, 0, sizeof(u_acTxRS232Buffer));
//    phRs232->queueRx.pQueue     = u_acRxRS232Buffer;
//    phRs232->queueRx.wEnd       = sizeof(u_acRxRS232Buffer);
//    phRs232->queueRx.wInput     =
//    phRs232->queueRx.wOutput    = 0;
//    phRs232->queueTx.pQueue     = u_acTxRS232Buffer;
//    phRs232->queueTx.wEnd       = sizeof(u_acTxRS232Buffer);
//    phRs232->queueTx.wInput     =
//    phRs232->queueTx.wOutput    = 0;
//    return ERROR_NONE;
//}
//
///*------------------------------------------------------------------------------+
//|           _RS485_Write                                                         |
//+------------------------------------------------------------------------------*/
//static WORD_T   _RS232_Write(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
//{
//    int i = 0;
//
//    HRS232* phRs232 = &u_ahRs232[u_wChannel];
//
//    if(wSize > MAX_RS232_SIZE) return COM_E_TX_FULL;
//    for(i = 0; i < wSize; i++)
//    {
//        *(phRs232->queueTx.pQueue + i) = pData[i];
//    }
//    phRs232->queueTx.wOutput = wSize;
//    return COM_SUCCESS;
//}
///*------------------------------------------------------------------------------+
//|           _RS232_Tx                                                           |
//+------------------------------------------------------------------------------*/
//static  void _RS232_Tx()
//{
//	int i = 0;
//    HRS232 *phRs232 = &u_ahRs232[u_wChannel];
//
//    if(phRs232->queueTx.wOutput == 0) return;
//
//    for(i = 0; i < phRs232->queueTx.wOutput; i++)
//    {
//        UARTCharPut(u_aRs232Para[u_wChannel].dwBase,*(phRs232->queueTx.pQueue + i));
//    }
//    phRs232->queueTx.wOutput = 0;
//}
///*------------------------------------------------------------------------------+
//|           _RS232_Read                                                         |
//+------------------------------------------------------------------------------*/
//static WORD_T   _RS232_Read(BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
//{
//   WORD    wLen = 0;
//   HRS232* phRs232 = &u_ahRs232[u_wChannel];
//
//   if(phRs232->queueRx.wInput == 0 || phRs232->wRxState != FALSE)
//       return wLen;
//
//   for(wLen = 0; wLen < phRs232->queueRx.wInput; wLen++)
//   {
//       pData[wLen] = *(phRs232->queueRx.pQueue + wLen);
//   }
//   phRs232->queueRx.wInput = 0;
//   return wLen;
//}
//
///*------------------------------------------------------------------------------+
//|           _RS232_Rx                                                           |
//+------------------------------------------------------------------------------*/
//static  void _RS232_Rx()
//{
//    WORD    wInput = 0;
//    HRS232* phRs232 = &u_ahRs232[u_wChannel];
//
//    while(UARTCharsAvail(u_aRs232Para[u_wChannel].dwBase))
//    {
//        wInput = phRs232->queueRx.wInput + 1;
//        if(wInput == phRs232->queueRx.wEnd)
//            break;
//
//        *(phRs232->queueRx.pQueue + phRs232->queueRx.wInput) = UARTCharGetNonBlocking(u_aRs232Para[u_wChannel].dwBase);
//        phRs232->queueRx.wInput = wInput;
//
//        phRs232->dwRxTick = g_dwSystemTick;
//        phRs232->wRxState = TRUE;
//    }
//
//    //濡傛灉瓒呮椂3ms娌℃湁鏀跺埌鏁版嵁璇存槑涓�寘鏁版嵁宸茬粡鏀跺畬
//    if(g_dwSystemTick - phRs232->dwRxTick > u_byRS232RecvTimeOut && phRs232->wRxState == TRUE)
//    {
//    	phRs232->wRxState = FALSE;
//    }
//}
///*------------------------------------------------------------------------------+
//|           _RS232_Close                                                         |
//+------------------------------------------------------------------------------*/
//static void     _RS232_Close()
//{
//    UARTDisable(u_aRs232Para[u_wChannel].dwBase);
//}
//
///*------------------------------------------------------------------------------+
//|           _RS232_Control                                                           |
//+------------------------------------------------------------------------------*/
//static  ERROR_T _RS232_Control(BYTE_T byCmd, void *pArg)
//{
//    switch (byCmd)
//    {
//        case RS232_CMD_TX:
//            _RS232_Tx();
//            break;
//        case RS232_CMD_RX:
//            _RS232_Rx();
//            break;
//        default:
//            break;
//    }
//    return  ERROR_NONE;
//}
