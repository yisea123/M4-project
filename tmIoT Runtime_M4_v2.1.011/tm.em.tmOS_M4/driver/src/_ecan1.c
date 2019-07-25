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
static      WORD_T      _eCan1_Open(ECAN* );
static      WORD_T      _eCan1_Close(void);
static      void        _eCan1_Rx(void);
static      void        _eCan1_Tx(void);
// static       WORD_T      _Write_ISReCan1(WORD_T *, WORD_T);
static      BOOL_T      _eCan1_CheckOffline(void);
static      ERROR_T     _eCan1_Control(BYTE_T, void *);
/*---------------------------------------------------------------------------+
|           Inner Global variable                                            |
+---------------------------------------------------------------------------*/
//tCANBitClkParms    u_aBitClkSettings0[] =
//{
//    {4, 5, 4, 5},                       // CANBAUD_1M
//    {4, 5, 4, 10},                      // CANBAUD_500K
//    {5, 4, 4, 20},                      // CANBAUD_250K
//    {5, 4, 4, 40},                      // CANBAUD_125K
//    {5, 4, 4, 50},                      // CANBAUD_100k
//    {5, 4, 4, 100},                     // CANBAUD_50k
//    {11, 8, 4, 100},                    // CANBAUD_25k
//    {11, 8, 4, 125},                    // CANBAUD_20k
//    {11, 8, 4, 250},                    // CANBAUD_10k
//    {11, 8, 4, 500},                    // CANBAUD_5k
//    {11, 8, 4, 1000},                   // CANBAUD_2k5
//};

static  volatile    BOOL_T  u_bRxLock   = FALSE;
static  volatile    BOOL_T  u_bTxLock   = FALSE;

HCAN                        HCAN1;

DEV_ENGINE_T            g_CAN1_Engine  =
{
    *(Open)&_eCan1_Open,
    *(Close)&_eCan1_Close,
    *(Read)&_eCan1_Read,
    *(Write)&_eCan1_Write,
    *(Control)&_eCan1_Control,
    NULL
};
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
static  WORD_T      _eCan1_Open(ECAN* pECan)
{
    WORD_T          wResult = CAN_SUCCESS;
    int             i;
    HCAN*           pHCAN1 = &HCAN1;
    tCANMsgObject   MsgObjectRx;
    BYTE_T            abyBufferIn[8];
    //+++++
    //  Set Rx queue and Tx queue
    //+++++
    pHCAN1->queueRx.wInput  = 0;
    pHCAN1->queueRx.wOutput = 0;
    pHCAN1->queueRx.wMaxSize = pECan->wRxBufferSize / sizeof(DATAFRAME);
    pHCAN1->queueRx.wCount  = 0;
    pHCAN1->queueRx.pQueue  = (DATAFRAME *)pECan->pRxBuffer;
    if (pHCAN1->queueRx.wMaxSize <= 0)
        wResult = CAN_ERR_FAILURE;

    pHCAN1->queueTx.wInput  = 0;
    pHCAN1->queueTx.wOutput = 0;
    pHCAN1->queueTx.wMaxSize = (pECan->wTxBufferSize / sizeof(DATAFRAME));
    pHCAN1->queueTx.wCount  = 0;
    pHCAN1->queueTx.pQueue  = (DATAFRAME *)pECan->pTxBuffer;
    if (pHCAN1->queueTx.wMaxSize <= 0)
        wResult = CAN_ERR_FAILURE;

    //+++++
    //  Set Init State
    //+++++
    pHCAN1->wMode = pECan->CanConf.wCanMode;
    pHCAN1->wState = CAN_S_OPENED;
    pHCAN1->wRxState = CAN_S_IDLE;
    pHCAN1->wTxState = CAN_S_IDLE;

    //+++++
    //  Enable Pin
    //+++++
    //TM4C129XNCZADƽ̨
	#ifdef PART_TM4C129XNCZAD
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    GPIOPinConfigure(GPIO_PA0_CAN1RX);
    GPIOPinConfigure(GPIO_PA1_CAN1TX);

    GPIOPinTypeCAN(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	#endif
    
    //TM4C1230H6PMƽ̨
    #ifdef PART_TM4C1230H6PM
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB4_CAN1RX);
    GPIOPinConfigure(GPIO_PB5_CAN1TX);

    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_4 | GPIO_PIN_5);

//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_CR)  = 0x01;
//    HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
//
//    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    GPIOPinConfigure(GPIO_PF0_CAN1RX);
//    GPIOPinConfigure(GPIO_PF3_CAN1TX);
//
//    GPIOPinTypeCAN(GPIO_PORTF_BASE, GPIO_PIN_0 | GPIO_PIN_3);
    #endif

	#ifdef PART_TM4C1294KCPDT
#ifdef M2C2133
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinConfigure(GPIO_PB0_CAN1RX);
    GPIOPinConfigure(GPIO_PB1_CAN1TX);

    GPIOPinTypeCAN(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
#endif
	#endif
    //+++++
    //  Enable Can 0
    //+++++
    SysCtlPeripheralEnable(SYSCTL_PERIPH_CAN1);

    CANInit(CAN1_BASE);
    //CANSetBitTiming(CAN1_BASE, &u_aBitClkSettings0[pECan->CanConf.BaudrateConf.wBaudrate]);
    CANBitRateSet(CAN1_BASE, g_dwSystemClock, pECan->CanConf.BaudrateConf.dwBaudrate);
    CANEnable(CAN1_BASE);

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

        CANMessageSet(CAN1_BASE, i, &MsgObjectRx, MSG_OBJ_TYPE_RX);
    }

    MsgObjectRx.ui32Flags         = MSG_OBJ_USE_ID_FILTER;
    MsgObjectRx.ui32MsgID         = 0; // (pECan->CanConf.wNodeID & CAN_STANDARD_MASK) << 18;
    MsgObjectRx.ui32MsgIDMask     = 0; // CAN_STANDARD_MASK << 18;
    MsgObjectRx.ui32MsgLen        = 8;
    MsgObjectRx.pui8MsgData      = abyBufferIn;

    CANMessageSet(CAN1_BASE, CAN_RX_MAILBOX_ENDNO, &MsgObjectRx, MSG_OBJ_TYPE_RX);

    return  wResult;
}


static  WORD_T      _eCan1_Close()
{
    HCAN*       pHCAN1 = &HCAN1;

    pHCAN1->wState = CAN_S_CLOSED;

    return  CAN_SUCCESS;
}

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/
static      void        _eCan1_Rx()
{
    int             i;
    WORD_T            wInput              = 0;
    HCAN*           pHCAN1               = &HCAN1;
    DATAFRAME*      pDataFrame;
    tCANMsgObject   MsgObjectBuf;
    DWORD_T           dwNewDateStatus     = CANStatusGet(CAN1_BASE, CAN_STS_NEWDAT);

//  DRV_DWORD_T dwMaskReg0 = CANRegRead(CAN1_BASE + CAN_O_IF2MSK1);
    //+++++
    //  No New Data
    //+++++
    _eCan1_CheckOffline();
    if ((dwNewDateStatus & 0xFFFF) == 0)    return;

    if (!u_bRxLock)
    {
        u_bRxLock   = TRUE;

        for (i = CAN_RX_MAILBOX_STARTNO; i <= CAN_RX_MAILBOX_ENDNO; i++)
        {
            if (!(dwNewDateStatus & 1 << (i - 1)))  continue;

            wInput = pHCAN1->queueRx.wInput + 1;
            if (wInput == pHCAN1->queueRx.wMaxSize)      wInput = 0;

            if (pHCAN1->queueRx.wCount != pHCAN1->queueRx.wMaxSize)
            {
                pDataFrame                  = pHCAN1->queueRx.pQueue + pHCAN1->queueRx.wInput;
                MsgObjectBuf.pui8MsgData      = (BYTE_T *)&pDataFrame->CanData;

                CANMessageGet(CAN1_BASE, i, &MsgObjectBuf, 1);

                if (MsgObjectBuf.ui32Flags & MSG_OBJ_EXTENDED_ID)
                    pDataFrame->MsgID.dwAll = MsgObjectBuf.ui32MsgID & 0x1FFFFFFF;
                else
                    pDataFrame->MsgID.ID.wStdID = MsgObjectBuf.ui32MsgID & 0x7FF;

                if (MsgObjectBuf.ui32Flags & MSG_OBJ_REMOTE_FRAME)  // modify 2016.3.9
                    pDataFrame->MsgID.ID.wRTR = 1;

                pDataFrame->wLength  =  MsgObjectBuf.ui32MsgLen;

                pHCAN1->queueRx.wInput = wInput;
                pHCAN1->queueRx.wCount++;
            }
            else
            {
                pHCAN1->wRxState = CAN_S_ERROR;
                break;
            }
        }

        u_bRxLock   = FALSE;
    }
}

static      void        _eCan1_Tx()
{
    int             i;
    HCAN*           pHCAN1               = &HCAN1;
    DATAFRAME*      pDataFrame;
    tCANMsgObject   MsgObjectTx;
    DWORD_T           dwNewDateStatus     = CANStatusGet(CAN1_BASE, CAN_STS_TXREQUEST);  //��ȡ����ݿ��õı��Ķ��������32λ����

    _eCan1_CheckOffline();


    if (!u_bTxLock)
    {
        u_bTxLock = TRUE;

        for (i = CAN_TX_MAILBOX_STARTNO; i <= CAN_TX_MAILBOX_ENDNO; i++)
        {
            if (dwNewDateStatus & 1 << (i - 1))  continue;

            if (pHCAN1->queueTx.wCount > 0)
            {
                pDataFrame                  = pHCAN1->queueTx.pQueue + pHCAN1->queueTx.wOutput;

                MsgObjectTx.ui32MsgLen        = pDataFrame->wLength;
                MsgObjectTx.pui8MsgData      = (BYTE_T *)&pDataFrame->CanData;
                MsgObjectTx.ui32MsgID         = pDataFrame->MsgID.ID.wStdID & 0x7FF;
                MsgObjectTx.ui32Flags         = MSG_OBJ_NO_FLAGS;

                //CANMessageSet(CAN1_BASE, i, &MsgObjectTx, MSG_OBJ_TYPE_TX);
                CANMessageSet(CAN1_BASE, i, &MsgObjectTx, (pDataFrame->MsgID.ID.wRTR) ? MSG_OBJ_TYPE_TX_REMOTE : MSG_OBJ_TYPE_TX); // Modify 2016.4.1   ����Զ��֡

                if (++pHCAN1->queueTx.wOutput == pHCAN1->queueTx.wMaxSize)
                    pHCAN1->queueTx.wOutput  = 0;
                pHCAN1->queueTx.wCount--;
#ifdef M2C2133
                GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, ~(uint8_t)GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_2));//can����ָʾ��
#endif
            }
            else
                break;
        }

        u_bTxLock = FALSE;
    }
}


WORD_T      _eCan1_Read(WORD_T *pw, WORD_T wSize, WORD_T* pwLength)
{
    HCAN*           pHCAN1   =   &HCAN1;

    //without this, keil4 has a warning about pwLength was set but not used, Very ODD!!!!!!!!
    //memset((void *)&pwLength, 0, sizeof(WORD_T));

    if (!u_bRxLock)
    {
        u_bRxLock   =   TRUE;
        if (pHCAN1->queueRx.wCount == 0)
        {
            u_bRxLock   =   FALSE;
            return CAN_ERR_RX_EMPTY;
        }
        if (wSize < sizeof(DATAFRAME))
        {
            u_bRxLock   =   FALSE;
            return CAN_ERR_RX_SMALL;
        }

        *(DATAFRAME *)pw = *(pHCAN1->queueRx.pQueue + pHCAN1->queueRx.wOutput++);
        *pwLength   =   ((DATAFRAME *)pw)->wLength;

        pHCAN1->queueRx.wCount--;
        if (pHCAN1->queueRx.wOutput >= pHCAN1->queueRx.wMaxSize)
            pHCAN1->queueRx.wOutput = 0;
        u_bRxLock   =   FALSE;
        return CAN_SUCCESS;
    }
    return CAN_ERR_RX_BUSY;
}

WORD_T      _eCan1_Write(WORD_T *pw, WORD_T wLength)
{
    WORD_T        wInput;
    HCAN*       pHCAN1 = &HCAN1;

    if (!u_bTxLock)
    {
        u_bTxLock   =   TRUE;

        if (pHCAN1->queueTx.wCount == pHCAN1->queueTx.wMaxSize)
        {
            u_bTxLock   =   FALSE;
            return CAN_ERR_TX_FULL;
        }

        if (wLength < sizeof(DATAFRAME))
        {
            u_bTxLock   =   FALSE;
            return CAN_ERR_TX_SMALL;
        }
        wInput = pHCAN1->queueTx.wInput + 1;
        if (wInput == pHCAN1->queueTx.wMaxSize) wInput = 0;
        *(pHCAN1->queueTx.pQueue + pHCAN1->queueTx.wInput) = *(DATAFRAME *)pw;
        pHCAN1->queueTx.wInput = wInput;
        pHCAN1->queueTx.wCount++;
        u_bTxLock   =   FALSE;
        return CAN_SUCCESS;
    }
    return CAN_ERR_TX_BUSY;
}


static      BOOL_T      _eCan1_CheckOffline()
{
    DWORD_T dwNewDateStatus     = CANStatusGet(CAN1_BASE, CAN_STS_CONTROL);

    if (dwNewDateStatus & CAN_STATUS_BUS_OFF)
        CANEnable(CAN1_BASE);

    return  FALSE;
}


static      ERROR_T     _eCan1_Control(BYTE_T byCmd, void *pArg)
{
    switch (byCmd)
    {
        case    CAN_CMD_TX:
        {
            _eCan1_Tx();
            break;
        }

        case    CAN_CMD_RX:
        {
            _eCan1_Rx();
            break;
        }

        default:
            break;
    }
    return  ERROR_NONE;
}
