/*===========================================================================+
|  Function : Can Driver                                                     |
|  Task     : Can Driver                                                     |
|----------------------------------------------------------------------------|
|  Compile  : CCS6.1 -                                                       |
|  Link     : CCS6.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Jia                                                            |
|  Version  : V1.00                                                          |
|  Creation : 2016/08/03                                                     |
|  Revision :                                                                |
+===========================================================================*/
#include    "driver/include/_ecan.h"
#include	"driver/include/_timer.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef    ERROR_T         (*Open  )   (   void *);
typedef    void            (*Close )   (   void  );
typedef    SIZE_T          (*Read  )   (   BYTE_T *, SIZE_T, OFFSET_T );
typedef    SIZE_T          (*Write )   (   BYTE_T *, SIZE_T, OFFSET_T );
typedef    SIZE_T          (*Control)  (   BYTE_T  , void * );
/*===========================================================================+
|           Global Variable                                                  |
+===========================================================================*/

/*===========================================================================+
|           Class implementation - RS485                                     |
+===========================================================================*/

/*---------------------------------------------------------------------------+
|           Helpers  Implement                                               |
+---------------------------------------------------------------------------*/
static      WORD_T      _eCan0_Open(ECAN* );
static      WORD_T      _eCan0_Close(void);
static      void        _eCan0_Rx(void);
static      void        _eCan0_Tx(void);
// static       WORD_T      _Write_ISReCan0(WORD_T *, WORD_T);
static      BOOL_T      _eCan0_CheckOffline(void);
static      ERROR_T     _eCan0_Control(BYTE_T, void *);
/*---------------------------------------------------------------------------+
|           Inner Global variable                                            |
+---------------------------------------------------------------------------*/
tCANBitClkParms    u_aBitClkSettings0[] =
{
    {4, 5, 4, 5},                       // CANBAUD_1M
    {4, 5, 4, 10},                      // CANBAUD_500K
    {5, 4, 4, 20},                      // CANBAUD_250K
    {5, 4, 4, 40},                      // CANBAUD_125K
    {5, 4, 4, 50},                      // CANBAUD_100k
    {5, 4, 4, 100},                     // CANBAUD_50k
    {11, 8, 4, 100},                    // CANBAUD_25k
    {11, 8, 4, 125},                    // CANBAUD_20k
    {11, 8, 4, 250},                    // CANBAUD_10k
    {11, 8, 4, 500},                    // CANBAUD_5k
    {11, 8, 4, 1000},                   // CANBAUD_2k5
};

static  volatile    BOOL_T  u_bRxLock   = FALSE;
static  volatile    BOOL_T  u_bTxLock   = FALSE;

HCAN                        HCan0;

DEV_ENGINE_T            g_CAN0_Engine  =
{
    *(Open)&_eCan0_Open,
    *(Close)&_eCan0_Close,
    *(Read)&_eCan0_Read,
    *(Write)&_eCan0_Write,
    *(Control)&_eCan0_Control,
    NULL
};
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
static  WORD_T      _eCan0_Open(ECAN* pECan)
{
    WORD_T          wResult = CAN_SUCCESS;
    int             i;
    HCAN*           pHCan0 = &HCan0;
    tCANMsgObject   MsgObjectRx;
    BYTE_T            abyBufferIn[8];
    //+++++
    //  Set Rx queue and Tx queue
    //+++++
    pHCan0->queueRx.wInput  = 0;
    pHCan0->queueRx.wOutput = 0;
    pHCan0->queueRx.wMaxSize = pECan->wRxBufferSize / sizeof(DATAFRAME);
    pHCan0->queueRx.wCount  = 0;
    pHCan0->queueRx.pQueue  = (DATAFRAME *)pECan->pRxBuffer;
    if (pHCan0->queueRx.wMaxSize <= 0)
        wResult = CAN_ERR_FAILURE;

    pHCan0->queueTx.wInput  = 0;
    pHCan0->queueTx.wOutput = 0;
    pHCan0->queueTx.wMaxSize = (pECan->wTxBufferSize / sizeof(DATAFRAME));
    pHCan0->queueTx.wCount  = 0;
    pHCan0->queueTx.pQueue  = (DATAFRAME *)pECan->pTxBuffer;
    if (pHCan0->queueTx.wMaxSize <= 0)
        wResult = CAN_ERR_FAILURE;

    //+++++
    //  Set Init State
    //+++++
    pHCan0->wMode = pECan->CanConf.wCanMode;
    pHCan0->wState = CAN_S_OPENED;
    pHCan0->wRxState = CAN_S_IDLE;
    pHCan0->wTxState = CAN_S_IDLE;

    //+++++
    //  Enable Pin
    //+++++
    //TM4C129XNCZAD平台
	#ifdef PART_TM4C129XNCZAD
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_CAN0RX);
    GPIOPinConfigure(GPIO_PA1_CAN0TX);

    GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	#endif
    
    //TM4C1230H6PM平台
    #ifdef PART_TM4C1230H6PM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB4_CAN0RX);
    GPIOPinConfigure(GPIO_PB5_CAN0TX);

    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_CR)  = 0x01;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    GPIOPinConfigure(GPIO_PF0_CAN0RX);
//    GPIOPinConfigure(GPIO_PF3_CAN0TX);
//
//    GPIOPinTypeCAN(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_3);
    #endif

#ifdef PART_TM4C1294KCPDT
#if (defined M2C2132) || (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131) || (defined S2C4111) || (defined IO2C1122)
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_CAN0RX);
    GPIOPinConfigure(GPIO_PA1_CAN0TX);

    GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
#endif
#endif
    //+++++
    //  Enable Can 0
    //+++++
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN0);

    CANInit(CAN0_BASE);
    //CANSetBitTiming(CAN0_BASE, &u_aBitClkSettings0[pECan->CanConf.BaudrateConf.wBaudrate]);
    CANBitRateSet(CAN0_BASE, g_dwSystemClock, pECan->CanConf.BaudrateConf.dwBaudrate);
    CANEnable(CAN0_BASE);

    //+++++
    //  Configure Rx MailBox
    //+++++
    for (i = CAN_RX_MAILBOX_STARTNO; i < CAN_RX_MAILBOX_ENDNO; i++)
    {
        MsgObjectRx.ui32Flags         = MSG_OBJ_USE_ID_FILTER | MSG_OBJ_FIFO;
        MsgObjectRx.ui32MsgID         = 0; // (pECan->CanConf.wNodeID & CAN_STANDARD_MASK) << 18;
        MsgObjectRx.ui32MsgIDMask     = 0; // CAN_STANDARD_MASK << 18;
        MsgObjectRx.ui32MsgLen        = 8;
        MsgObjectRx.pui8MsgData      = abyBufferIn;

        CANMessageSet(CAN0_BASE, i, &MsgObjectRx, MSG_OBJ_TYPE_RX);
    }

    MsgObjectRx.ui32Flags         = MSG_OBJ_USE_ID_FILTER;
    MsgObjectRx.ui32MsgID         = 0; // (pECan->CanConf.wNodeID & CAN_STANDARD_MASK) << 18;
    MsgObjectRx.ui32MsgIDMask     = 0; // CAN_STANDARD_MASK << 18;
    MsgObjectRx.ui32MsgLen        = 8;
    MsgObjectRx.pui8MsgData      = abyBufferIn;

    CANMessageSet(CAN0_BASE, CAN_RX_MAILBOX_ENDNO, &MsgObjectRx, MSG_OBJ_TYPE_RX);

    return  wResult;
}


static  WORD_T      _eCan0_Close()
{
    HCAN*       pHCan0 = &HCan0;

    pHCan0->wState = CAN_S_CLOSED;

    return  CAN_SUCCESS;
}

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
static      void        _eCan0_Rx()
{
    int             i;
    WORD_T            wInput              = 0;
    HCAN*           pHCan0               = &HCan0;
    DATAFRAME*      pDataFrame;
    tCANMsgObject   MsgObjectBuf;
    DWORD_T           dwNewDateStatus     = CANStatusGet(CAN0_BASE, CAN_STS_NEWDAT);

//  DRV_DWORD_T dwMaskReg0 = CANRegRead(CAN0_BASE + CAN_O_IF2MSK1);
    //+++++
    //  No New Data
    //+++++
    _eCan0_CheckOffline();
    if ((dwNewDateStatus & 0xFFFF) == 0)    return;

    if (!u_bRxLock)
    {
        u_bRxLock   = TRUE;

        for (i = CAN_RX_MAILBOX_STARTNO; i <= CAN_RX_MAILBOX_ENDNO; i++)
        {
            if (!(dwNewDateStatus & 1 << (i - 1)))  continue;

            wInput = pHCan0->queueRx.wInput + 1;
            if (wInput == pHCan0->queueRx.wMaxSize)      wInput = 0;

            if (pHCan0->queueRx.wCount != pHCan0->queueRx.wMaxSize)
            {
                pDataFrame                  = pHCan0->queueRx.pQueue + pHCan0->queueRx.wInput;
                MsgObjectBuf.pui8MsgData      = (BYTE_T *)&pDataFrame->CanData;

                CANMessageGet(CAN0_BASE, i, &MsgObjectBuf, 1);

                if (MsgObjectBuf.ui32Flags & MSG_OBJ_EXTENDED_ID)
                    pDataFrame->MsgID.dwAll = MsgObjectBuf.ui32MsgID & 0x1FFFFFFF;
                else
                    pDataFrame->MsgID.ID.wStdID = MsgObjectBuf.ui32MsgID & 0x7FF;

                if (MsgObjectBuf.ui32Flags & MSG_OBJ_REMOTE_FRAME)  // modify 2016.3.9
                    pDataFrame->MsgID.ID.wRTR = 1;

                pDataFrame->wLength  =  MsgObjectBuf.ui32MsgLen;

                pHCan0->queueRx.wInput = wInput;
                pHCan0->queueRx.wCount++;
            }
            else
            {
                pHCan0->wRxState = CAN_S_ERROR;
                break;
            }
        }

        u_bRxLock   = FALSE;
    }
}

static      void        _eCan0_Tx()
{
    int             i;
    HCAN*           pHCan0               = &HCan0;
    DATAFRAME*      pDataFrame;
    tCANMsgObject   MsgObjectTx;
    DWORD_T           dwNewDateStatus     = CANStatusGet(CAN0_BASE, CAN_STS_TXREQUEST);  //读取新数据可用的报文对象的完整32位屏蔽

    _eCan0_CheckOffline();


    if (!u_bTxLock)
    {
        u_bTxLock = TRUE;

        for (i = CAN_TX_MAILBOX_STARTNO; i <= CAN_TX_MAILBOX_ENDNO; i++)
        {
            if (dwNewDateStatus & 1 << (i - 1))  continue;

            if (pHCan0->queueTx.wCount > 0)
            {
                pDataFrame                  = pHCan0->queueTx.pQueue + pHCan0->queueTx.wOutput;

                MsgObjectTx.ui32MsgLen        = pDataFrame->wLength;
                MsgObjectTx.pui8MsgData      = (BYTE_T *)&pDataFrame->CanData;
                MsgObjectTx.ui32MsgID         = pDataFrame->MsgID.ID.wStdID & 0x7FF;
                MsgObjectTx.ui32Flags         = MSG_OBJ_NO_FLAGS;

                //CANMessageSet(CAN0_BASE, i, &MsgObjectTx, MSG_OBJ_TYPE_TX);
                CANMessageSet(CAN0_BASE, i, &MsgObjectTx, (pDataFrame->MsgID.ID.wRTR) ? MSG_OBJ_TYPE_TX_REMOTE : MSG_OBJ_TYPE_TX); // Modify 2016.4.1   增加远程帧

                if (++pHCan0->queueTx.wOutput == pHCan0->queueTx.wMaxSize)
                    pHCan0->queueTx.wOutput  = 0;
                pHCan0->queueTx.wCount--;
#if (defined M2C2132) || (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131) || (defined S2C4111) || (defined IO2C1122)
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ~(uint8_t)GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2));//can发送指示灯
#endif
            }
            else
                break;
        }

        u_bTxLock = FALSE;
    }
}


WORD_T      _eCan0_Read(WORD_T *pw, WORD_T wSize, WORD_T* pwLength)
{
    HCAN*           pHCan0   =   &HCan0;

    //without this, keil4 has a warning about pwLength was set but not used, Very ODD!!!!!!!!
    //memset((void *)&pwLength, 0, sizeof(WORD_T));

    if (!u_bRxLock)
    {
        u_bRxLock   =   TRUE;
        if (pHCan0->queueRx.wCount == 0)
        {
            u_bRxLock   =   FALSE;
            return CAN_ERR_RX_EMPTY;
        }
        if (wSize < sizeof(DATAFRAME))
        {
            u_bRxLock   =   FALSE;
            return CAN_ERR_RX_SMALL;
        }

        *(DATAFRAME *)pw = *(pHCan0->queueRx.pQueue + pHCan0->queueRx.wOutput++);
        *pwLength   =   ((DATAFRAME *)pw)->wLength;

        pHCan0->queueRx.wCount--;
        if (pHCan0->queueRx.wOutput >= pHCan0->queueRx.wMaxSize)
            pHCan0->queueRx.wOutput = 0;
        u_bRxLock   =   FALSE;
        return CAN_SUCCESS;
    }
    return CAN_ERR_RX_BUSY;
}

WORD_T      _eCan0_Write(WORD_T *pw, WORD_T wLength)
{
    WORD_T        wInput;
    HCAN*       pHCan0 = &HCan0;

    if (!u_bTxLock)
    {
        u_bTxLock   =   TRUE;

        if (pHCan0->queueTx.wCount == pHCan0->queueTx.wMaxSize)
        {
            u_bTxLock   =   FALSE;
            return CAN_ERR_TX_FULL;
        }

        if (wLength < sizeof(DATAFRAME))
        {
            u_bTxLock   =   FALSE;
            return CAN_ERR_TX_SMALL;
        }
        wInput = pHCan0->queueTx.wInput + 1;
        if (wInput == pHCan0->queueTx.wMaxSize) wInput = 0;
        *(pHCan0->queueTx.pQueue + pHCan0->queueTx.wInput) = *(DATAFRAME *)pw;
        pHCan0->queueTx.wInput = wInput;
        pHCan0->queueTx.wCount++;
        u_bTxLock   =   FALSE;
        return CAN_SUCCESS;
    }
    return CAN_ERR_TX_BUSY;
}


static      BOOL_T      _eCan0_CheckOffline()
{
    DWORD_T dwNewDateStatus     = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL);

    if (dwNewDateStatus & CAN_STATUS_BUS_OFF)
        CANEnable(CAN0_BASE);

    return  FALSE;
}

static      ERROR_T     _eCan0_Control(BYTE_T byCmd, void *pArg)
{
    switch (byCmd)
    {
        case    CAN_CMD_TX:
        {
            _eCan0_Tx();
            break;
        }

        case    CAN_CMD_RX:
        {
            _eCan0_Rx();
            break;
        }

        default:
            break;
    }
    return  ERROR_NONE;
}
