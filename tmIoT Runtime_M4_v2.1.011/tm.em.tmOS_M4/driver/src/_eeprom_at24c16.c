///*==============================================================================+
//|  Function : eeprom driver                                                     |
//|  Task     : eeprom driver Source File                                         |
//|-------------------------------------------------------------------------------|
//|  Compile  : ccs6.1                                                            |
//|  Link     : ccs6.1                                                            |
//|  Call     :                                                                   |
//|-------------------------------------------------------------------------------|
//|  Author   : Jia                                                               |
//|  Version  : V1.00                                                             |
//|  Creation : 2016/08/05                                                        |
//|  Revision :                                                                   |
//+==============================================================================*/
//#include "driver/include/_eeprom_at24c16.h"
//#include "driver/include/_timer.h"
///*==============================================================================+
//|           Constant                                                            |
//+==============================================================================*/
//
///*------------------------------------------------------------------------------+
//|           Inner Globle Variable                                               |
//+------------------------------------------------------------------------------*/
///*==============================================================================+
//|           Type definition                                                     |
//+==============================================================================*/
//
///*==============================================================================+
//|           Macro definition                                                    |
//+==============================================================================*/
///*------------------------------------------------------------------------------+
//|           Helpers Implementation                                              |
//+------------------------------------------------------------------------------*/
//static  WORD        _SendI2CByte(DWORD dwBase, BYTE byData, WORD wCmd);
//static  WORD        _RecvI2CByte(DWORD dwBase, BYTE *pbyData, WORD wCmd);
//static  WORD        _Write_At24c16(WORD wAddr,  BYTE *pData,  WORD wSize);
//static  WORD        _Read_AT24c16(WORD wAddr,  BYTE *pData,  WORD wSize);
//static  ERROR_T     _EepromAt24c16_Open(void *pArg);
//static  SIZE_T      _EepromAt24c16_Read(BYTE_T *pData, SIZE_T wSize, OFFSET_T dwAddr);
//static  SIZE_T      _EepromAt24c16_Write(BYTE_T *pData, SIZE_T  wSize, OFFSET_T dwAddr);
///*==============================================================================+
//|           Globle Variables                                                    |
//+==============================================================================*/
//DEV_ENGINE_T            g_EepromAt24c16_Engine  =
//{
//    _EepromAt24c16_Open,
//    NULL,
//    _EepromAt24c16_Read,
//    _EepromAt24c16_Write,
//    NULL,
//    NULL
//};
//
///*==============================================================================+
//|           Class declaration -                                                 |
//+==============================================================================*/
///*------------------------------------------------------------------------------+
//|           Constructor and destructor                                          |
//+------------------------------------------------------------------------------*/
//static  ERROR_T     _EepromAt24c16_Open(void *pArg)
//{
//    //+++++
//    //  Enable Module
//    //+++++
//    SysCtlPeripheralEnable(E2PROM_I2C_PERIPH);
//    SysCtlPeripheralEnable(E2PROM_I2C_SCL_PERIPH);
//    SysCtlPeripheralEnable(E2PROM_I2C_SDA_PERIPH);
//    //+++++
//    //  Configure GPIO Pins
//    //+++++
//    GPIOPinConfigure(E2PROM_I2C_GPIO_SCL);
//    GPIOPinConfigure(E2PROM_I2C_GPIO_SDA);
//
//    GPIOPinTypeI2CSCL(E2PROM_I2C_SCL_PORT, E2PROM_I2C_SCL_PIN);
//    GPIOPinTypeI2C(E2PROM_I2C_SDA_PORT, E2PROM_I2C_SDA_PIN);
//
//    //+++++
//    //  Configure I2C0 Baud and then enable I2C0
//    //+++++
//    //I2CMasterInit(E2PROM_I2C_BASE, TRUE);
//    I2CMasterInitExpClk(E2PROM_I2C_BASE, SysCtlClockGet(), false);
//    I2CMasterEnable(E2PROM_I2C_BASE);
//    return ERROR_NONE;
//}
///*------------------------------------------------------------------------------+
//|           _EepromAt24c16_Write                                                |
//+------------------------------------------------------------------------------*/
//static  SIZE_T _EepromAt24c16_Write(BYTE_T *pData, SIZE_T  wSize, OFFSET_T dwAddr)
//{
//    WORD    wErr = I2C_MASTER_ERR_NONE;
//    WORD    wSendSize   = 0;
//
//    if ((dwAddr&E2PROM_AT24C16_BLOCKMARK) + wSize <= E2PROM_AT24C16_BLOCKSIZE)
//    {
//        if ((wErr = _Write_At24c16(dwAddr, pData, wSize)) != I2C_MASTER_ERR_NONE)     return  wErr;
//    }
//    else
//    {
//        while (wSize +  (dwAddr&E2PROM_AT24C16_BLOCKMARK) > E2PROM_AT24C16_BLOCKSIZE)
//        {
//            wSendSize   =   E2PROM_AT24C16_BLOCKSIZE - (dwAddr&E2PROM_AT24C16_BLOCKMARK);
//
//            if ((wErr = _Write_At24c16(dwAddr, pData, wSendSize)) != I2C_MASTER_ERR_NONE)      return  wErr;
//
//            pData      +=   wSendSize;
//            dwAddr     +=   wSendSize;
//            wSize      -=   wSendSize;
//        }
//
//        if ((wErr = _Write_At24c16(dwAddr, pData, wSize)) != I2C_MASTER_ERR_NONE)     return  wErr;
//    }
//
//    return  wErr;
//}
//
///*------------------------------------------------------------------------------+
//|           _EepromAt24c16_Read                                                 |
//+------------------------------------------------------------------------------*/
//static  SIZE_T  _EepromAt24c16_Read(BYTE_T *pData, SIZE_T wSize, OFFSET_T dwAddr  )
//{
//    WORD    wErr = I2C_MASTER_ERR_NONE;
//    WORD    wRecvSize   = 0;
//
//    if ((dwAddr&E2PROM_AT24C16_BLOCKMARK) + wSize <= E2PROM_AT24C16_BLOCKSIZE)
//    {
//        if ((wErr = _Read_AT24c16(dwAddr, pData, wSize)) != I2C_MASTER_ERR_NONE)     return  wErr;
//    }
//    else
//    {
//        while (wSize +  (dwAddr&E2PROM_AT24C16_BLOCKMARK) > E2PROM_AT24C16_BLOCKSIZE)
//        {
//            wRecvSize   =   E2PROM_AT24C16_BLOCKSIZE - (dwAddr&E2PROM_AT24C16_BLOCKMARK);
//
//            if ((wErr = _Read_AT24c16(dwAddr, pData, wRecvSize)) != I2C_MASTER_ERR_NONE)     return  wErr;
//            pData      +=   wRecvSize;
//            dwAddr     +=   wRecvSize;
//            wSize      -=   wRecvSize;
//        }
//
//        if ((wErr = _Read_AT24c16(dwAddr, pData, wSize)) != I2C_MASTER_ERR_NONE)     return  wErr;
//
//    }
//    return  wErr;
//}
///*------------------------------------------------------------------------------+
//|           Helpers                                                             |
//+------------------------------------------------------------------------------*/
//WORD        _Write_At24c16(WORD wAddr,  BYTE *pData,  WORD wSize)
//{
//    BYTE        byAddr;
//    WORD        wErr            = I2C_MASTER_ERR_NONE;
//    WORD        wSentCount;
//
//    //while (g_dwSystemTick - u_dwWriteStartTime < E2PROM_24LC16_WRITECYCLE);
//    SysCtlDelay(SysCtlClockGet()/100);
//    //+++++
//    //  Set Salve Address and Write
//    //+++++
//    byAddr = (wAddr>>8)&0x7;
//
//    I2CMasterSlaveAddrSet(E2PROM_I2C_BASE, E2PROM_AT24C16_ADDRESS|byAddr, FALSE);
//
//    //+++++
//    //  Set Address
//    //+++++
//    byAddr = wAddr & 0xFF;
//    if ((wErr = _SendI2CByte(E2PROM_I2C_BASE, byAddr, I2C_MASTER_CMD_BURST_SEND_START)) != I2C_MASTER_ERR_NONE)   return  wErr;
//
//
//    //+++++
//    //  Send Data
//    //+++++
//    for (wSentCount = 0; wSentCount < wSize-1; wSentCount++)
//    {
//        if ((wErr = _SendI2CByte(E2PROM_I2C_BASE, *(pData+wSentCount), I2C_MASTER_CMD_BURST_SEND_CONT)) != I2C_MASTER_ERR_NONE)    return  wErr;
//    }
//    if ((wErr = _SendI2CByte(E2PROM_I2C_BASE, *(pData+wSentCount), I2C_MASTER_CMD_BURST_SEND_FINISH)) != I2C_MASTER_ERR_NONE)       return  wErr;
//
//    //u_dwWriteStartTime = g_dwSystemTick;
//
//    return  wErr;
//}
//
//
//WORD        _Read_AT24c16(WORD wAddr,  BYTE *pData,  WORD wSize)
//{
//    BYTE        byAddr;
//    WORD        wErr = I2C_MASTER_ERR_NONE;
//    WORD        wRecvCount = 0;
//
//    //while (g_dwSystemTick - u_dwWriteStartTime < E2PROM_24LC16_WRITECYCLE);
//    SysCtlDelay(SysCtlClockGet()/100);
//    //+++++
//    //  Set Salve Address and Write
//    //+++++
//    byAddr = (wAddr>>8)&0x7;
//    I2CMasterSlaveAddrSet(E2PROM_I2C_BASE, E2PROM_AT24C16_ADDRESS|byAddr, FALSE);
//
//    //+++++
//    //  Set Address
//    //+++++
//    byAddr = wAddr & 0xFF;
//    if ((wErr = _SendI2CByte(E2PROM_I2C_BASE, byAddr, I2C_MASTER_CMD_BURST_SEND_START)) != I2C_MASTER_ERR_NONE)   return    wErr;
//
//    I2CMasterSlaveAddrSet(E2PROM_I2C_BASE, E2PROM_AT24C16_ADDRESS, TRUE);
//    //+++++
//    //  Receive Data
//    //+++++
//    if (wSize == 1)
//    {
//        if ((wErr = _RecvI2CByte(E2PROM_I2C_BASE, pData, I2C_MASTER_CMD_SINGLE_RECEIVE)) != I2C_MASTER_ERR_NONE)    return   wErr;
//    }
//    else
//    {
//        if ((wErr = _RecvI2CByte(E2PROM_I2C_BASE,  pData+wRecvCount, I2C_MASTER_CMD_BURST_RECEIVE_START)) != I2C_MASTER_ERR_NONE)  return  wErr;
//
//        for (wRecvCount = 1; wRecvCount < wSize-1; wRecvCount++)
//        {
//            if ((wErr = _RecvI2CByte(E2PROM_I2C_BASE, pData+wRecvCount, I2C_MASTER_CMD_BURST_RECEIVE_CONT)) != I2C_MASTER_ERR_NONE)  return  wErr;
//        }
//
//        if ((wErr = _RecvI2CByte(E2PROM_I2C_BASE, pData+wRecvCount, I2C_MASTER_CMD_BURST_RECEIVE_FINISH)) != I2C_MASTER_ERR_NONE)   return  wErr;
//    }
//
//    return  wErr;
//}
//
//WORD        _SendI2CByte(DWORD dwBase, BYTE byData, WORD wCmd)
//{
//    WORD    wErr;
//
//    I2CMasterDataPut(dwBase, byData);
//    I2CMasterControl(dwBase, wCmd);
//    while (I2CMasterBusy(dwBase));
//
//    wErr = I2CMasterErr(dwBase);
//    return      wErr;
//
//}
//
//
//WORD        _RecvI2CByte(DWORD dwBase, BYTE *pbyData, WORD wCmd)
//{
//    WORD    wErr;
//    I2CMasterControl(dwBase, wCmd);
//
//    while (I2CMasterBusy(dwBase));
//    wErr = I2CMasterErr(dwBase);
//
//    if (wErr == I2C_MASTER_ERR_NONE)
//    {
//        *pbyData    =   I2CMasterDataGet(dwBase);
//    }
//
//    return      wErr;
//}
