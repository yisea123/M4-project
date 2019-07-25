///*==============================================================================+
//|  Function : ethernet                                                          |
//|  Task     : ethernet Source File                                              |
//|-------------------------------------------------------------------------------|
//|  Compile  :                                                                   |
//|  Link     :                                                  	                |
//|  Call     :                                                                   |
//|-------------------------------------------------------------------------------|
//|  Author   : jia                                                               |
//|  Version  : V1.00                                                             |
//|  Creation : 2016/07/23                                                        |
//|  Revision :                                                                   |
//+==============================================================================*/
//#include 	<stdbool.h>
//#include 	<stdint.h>
//#include    "kernel/include/device.h"
//#include	"driver/include/_timer.h"
//#include 	"driver/include/_ethernet.h"
//#include 	"M4_lib/inc/hw_emac.h"
//#include    "M4_lib/driverlib/emac.h"
///*==============================================================================+
//|           Constant                                                            |
//+==============================================================================*/
//
///*==============================================================================+
//|           Type definition                                                     |
//+==============================================================================*/
//typedef     struct tagEthernet_DATA            	Ethernet_DATA_T;
//
//struct      tagEthernet_DATA
//{
//    WORD_T          wSetFilterCount;
//    WORD_T          WriteValue[MAX_ETHERNET];
//	WORD_T          ReadValue[MAX_ETHERNET];
//    WORD_T          awRealFilterCount[MAX_ETHERNET];
//};
///**
// * Interrupt counters (for debug purposes).
// */
//volatile uint32_t g_ui32NormalInts;
//volatile uint32_t g_ui32AbnormalInts;
//
//
////*****************************************************************************
////
//// Ethernet DMA descriptors.
////
//// Although uIP uses a single buffer, the MAC hardware needs a minimum of
//// 3 receive descriptors to operate.
////
////*****************************************************************************
//#define NUM_TX_DESCRIPTORS 3
//#define NUM_RX_DESCRIPTORS 3
//tEMACDMADescriptor g_psRxDescriptor[NUM_TX_DESCRIPTORS];
//tEMACDMADescriptor g_psTxDescriptor[NUM_RX_DESCRIPTORS];
//
//uint32_t g_ui32RxDescIndex;
//uint32_t g_ui32TxDescIndex;
//
////*****************************************************************************
////
//// Transmit and receive buffers.
////
////*****************************************************************************
//#define RX_BUFFER_SIZE 1536
//#define TX_BUFFER_SIZE 1536
//uint8_t g_pui8RxBuffer[RX_BUFFER_SIZE];
//uint8_t g_pui8TxBuffer[TX_BUFFER_SIZE];
////*****************************************************************************
////
//// A set of flags.  The flag bits are defined as follows:
////
////     0 -> An indicator that a SysTick interrupt has occurred.
////     1 -> An RX Packet has been received.
////     2 -> A TX packet DMA transfer is pending.
////     3 -> A RX packet DMA transfer is pending.
////
////*****************************************************************************
//#define FLAG_SYSTICK            0
//#define FLAG_RXPKT              1
//#define FLAG_TXPKT              2
//#define FLAG_RXPKTPEND          3
//volatile uint32_t g_ui32Flags;
///*==============================================================================+
//|           Macro definition                                                    |
//+==============================================================================*/
//
///*==============================================================================+
//|           Globle Variables                                                    |
//+==============================================================================*/
//
//
///*==============================================================================+
//|           Class declaration -                                      |
//+==============================================================================*/
///*------------------------------------------------------------------------------+
//|           Helpers Implementation                                              |
//+------------------------------------------------------------------------------*/
//ERROR_T         _Ethernet_Open     (   void *);
//void            _Ethernet_Close    (   void  );
//SIZE_T			_Ethernet_Read	   (   BYTE_T *, SIZE_T, OFFSET_T );
//SIZE_T			_Ethernet_Write	   (   BYTE_T *, SIZE_T, OFFSET_T );
//ERROR_T         _Ethernet_Control  (   BYTE_T ,  void  * );
//void InitDescriptors(uint32_t ui32Base);
//int32_t PacketReceive(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen);
//int32_t PacketTransmit(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen);
///*------------------------------------------------------------------------------+
//|           Inner Globle Variable                                               |
//+------------------------------------------------------------------------------*/
//Ethernet_DATA_T         u_Ethernet_Data;
//
//DEV_ENGINE_T            g_Ethernet_Engine  =
//{
//    _Ethernet_Open,
//    _Ethernet_Close,
//    _Ethernet_Read,
//    _Ethernet_Write,
//    _Ethernet_Control,
//    &u_Ethernet_Data
//};
///*------------------------------------------------------------------------------+
//|           Constructor and destructor                                          |
//+------------------------------------------------------------------------------*/
///*------------------------------------------------------------------------------+
//|          _Ethernet_Open                                                       |
//+------------------------------------------------------------------------------*/
//  ERROR_T         _Ethernet_Open     (   void      *pArg   )
//{
//
//    //
//    // Get the MAC address from the flash user registers.  If it has not been
//    // programmed, then use the boot loader default MAC address.
//    //
//    //
//    // Enable and reset the Ethernet modules.
//    //
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EMAC0);
//    MAP_SysCtlPeripheralEnable(SYSCTL_PERIPH_EPHY0);
//    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EMAC0);
//    MAP_SysCtlPeripheralReset(SYSCTL_PERIPH_EPHY0);
//
//    //
//    // PF1/PK4/PK6 are used for Ethernet LEDs.
//    //
//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
//    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
//    ROM_GPIOPinConfigure(GPIO_PF1_EN0LED2);
//    ROM_GPIOPinConfigure(GPIO_PK4_EN0LED0);
//    ROM_GPIOPinConfigure(GPIO_PK6_EN0LED1);
//    GPIOPinTypeEthernetLED(GPIO_PORTF_BASE, GPIO_PIN_1);
//    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_4);
//    GPIOPinTypeEthernetLED(GPIO_PORTK_BASE, GPIO_PIN_6);
//
//    //
//    // Wait for the MAC to be ready.
//    //
//
//    while(!MAP_SysCtlPeripheralReady(SYSCTL_PERIPH_EMAC0))
//    {
//    }
//    //
//    // Configure for use with the internal PHY.
//    //
//
//    MAP_EMACPHYConfigSet(EMAC0_BASE, (EMAC_PHY_TYPE_INTERNAL | EMAC_PHY_INT_MDIX_EN |
//		                               EMAC_PHY_AN_100B_T_FULL_DUPLEX));
//
//    //
//    // Reset the MAC. to latch the PHY configuration
//    //
//    MAP_EMACReset(EMAC0_BASE);
//    //
//    // Initialize the MAC and set the DMA mode.
//    //
//
//    MAP_EMACInit(EMAC0_BASE, g_dwSystemClock,
//                     EMAC_BCONFIG_MIXED_BURST | EMAC_BCONFIG_PRIORITY_FIXED, 4, 4,
//                     0);
//
//    //
//    // Set MAC configuration options.
//    //
//    MAP_EMACConfigSet(EMAC0_BASE,
//                      (EMAC_CONFIG_FULL_DUPLEX | EMAC_CONFIG_CHECKSUM_OFFLOAD |
//                       EMAC_CONFIG_7BYTE_PREAMBLE | EMAC_CONFIG_IF_GAP_96BITS |
//                       EMAC_CONFIG_USE_MACADDR0 |
//                       EMAC_CONFIG_SA_FROM_DESCRIPTOR |
//                       EMAC_CONFIG_BO_LIMIT_1024),
//                      (EMAC_MODE_RX_STORE_FORWARD |
//                       EMAC_MODE_TX_STORE_FORWARD |
//                       EMAC_MODE_TX_THRESHOLD_64_BYTES |
//                       EMAC_MODE_RX_THRESHOLD_64_BYTES), 0);
//    //
//    // Initialize the Ethernet DMA descriptors.
//    //
//    InitDescriptors(EMAC0_BASE);
//    //+++++
//    // Program the hardware with its MAC address (for filtering).
//    //++++
//    MAP_EMACAddrSet(EMAC0_BASE, 0, (uint8_t *)pArg);
//
//    //
//    // Wait for the link to become active.  If we are using an external
//    // RMII PHY, we already waited for link to be acquired prior to
//    // configuring the MAC so don't wait again here.
//    //
//
//    //while((EMACPHYRead(EMAC0_BASE, 0, EPHY_BMSR) &
//    //       EPHY_BMSR_LINKSTAT) == 0)
//    //{
//    //}
//
//    //
//    // Set MAC filtering options.  We receive all broadcast and multicast
//    // packets along with those addressed specifically for us.
//    //
//    MAP_EMACFrameFilterSet(EMAC0_BASE, (EMAC_FRMFILTER_SADDR |
//                                        EMAC_FRMFILTER_PASS_MULTICAST |
//                                        EMAC_FRMFILTER_PASS_NO_CTRL));
//
//    //
//    // Clear any pending interrupts.
//    //
//    MAP_EMACIntClear(EMAC0_BASE, EMACIntStatus(EMAC0_BASE, false));
//    //
//    // Mark the receive descriptors as available to the DMA to start
//    // the receive processing.
//    //
//    g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus |= DES0_RX_CTRL_OWN;
//    //
//    // Enable the Ethernet MAC transmitter and receiver.
//    //
//    MAP_EMACTxEnable(EMAC0_BASE);
//    MAP_EMACRxEnable(EMAC0_BASE);
//
//    return  ERROR_NONE;
//}
//
///*------------------------------------------------------------------------------+
//|           _Ethernet_Close                                                      |
//+------------------------------------------------------------------------------*/
//
//  void            _Ethernet_Close    (   )
//{
//
//}
///*------------------------------------------------------------------------------+
//|           _Ethernet_Read                                                      |
//+------------------------------------------------------------------------------*/
//
// SIZE_T          _Ethernet_Read     (   BYTE_T      *pbyBuf,
//                                        SIZE_T      Size,
//                                        OFFSET_T    Offset  )
//{
//	long 					Length;
//
//    if (Size + Offset > MAX_ETHERNET )  return  0;
//
//	Length = PacketReceive(EMAC0_BASE, pbyBuf, Size);
//
//    return  Length;
//}
//
///*------------------------------------------------------------------------------+
//|           _Ethernet_Write                                                     |
//+------------------------------------------------------------------------------*/
//
//  SIZE_T          _Ethernet_Write     (   BYTE_T      *pbyBuf,
//                                          SIZE_T      Size,
//                                          OFFSET_T    Offset  )
//{
//	long     				Length;
//
//    if (Size + Offset > MAX_ETHERNET)
//        return  0;
//
//	Length = PacketTransmit(EMAC0_BASE,(unsigned char *)pbyBuf, Size);
//
//    return  Length;
//}
//
///*------------------------------------------------------------------------------+
//|           _Ethernet_Control                                                   |
//+------------------------------------------------------------------------------*/
//
//ERROR_T         _Ethernet_Control  (   BYTE_T      byCmd,
//                                       void       *pArg   )
//{
//
//     switch (byCmd)
//    {
//        case    ETHERNET_CMD_SETMCADDRESS:
//        {
//			MAP_EMACAddrSet(EMAC0_BASE,0, pArg);  //MAC地址，對進入的包進行過濾
//            //
//            // Set MAC filtering options.  We receive all broadcast and multicast
//            // packets along with those addressed specifically for us.
//            //
//            MAP_EMACFrameFilterSet(EMAC0_BASE, (EMAC_FRMFILTER_SADDR |
//                                   EMAC_FRMFILTER_PASS_MULTICAST |
//                                   EMAC_FRMFILTER_PASS_NO_CTRL));
//
//            break;
//        }
//        default:
//            break;
//    }
//
//    return  ERROR_NONE;
//}
////*****************************************************************************
////
//// Initialize the transmit and receive DMA descriptors.  We apparently need
//// a minimum of 3 descriptors in each chain.
////
////*****************************************************************************
//void
//InitDescriptors(uint32_t ui32Base)
//{
//    uint32_t ui32Loop;
//
//    //
//    // Initialize each of the transmit descriptors.  Note that we leave the buffer
//    // pointer and size empty and the OWN bit clear here since we have
//    // not set up any transmissions yet.
//    //
//    for(ui32Loop = 0; ui32Loop < NUM_TX_DESCRIPTORS; ui32Loop++)
//    {
//        g_psTxDescriptor[ui32Loop].ui32Count =
//            (DES1_TX_CTRL_SADDR_INSERT |
//             (TX_BUFFER_SIZE << DES1_TX_CTRL_BUFF1_SIZE_S));
//        g_psTxDescriptor[ui32Loop].pvBuffer1 = g_pui8TxBuffer;
//        g_psTxDescriptor[ui32Loop].DES3.pLink =
//            (ui32Loop == (NUM_TX_DESCRIPTORS - 1)) ?
//            g_psTxDescriptor : &g_psTxDescriptor[ui32Loop + 1];
//        g_psTxDescriptor[ui32Loop].ui32CtrlStatus =
//            (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
//             DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_CHAINED |
//             DES0_TX_CTRL_IP_ALL_CKHSUMS);
//    }
//
//    //
//    // Initialize each of the receive descriptors.  We clear the OWN bit here
//    // to make sure that the receiver doesn't start writing anything
//    // immediately.
//    //
//    for(ui32Loop = 0; ui32Loop < NUM_RX_DESCRIPTORS; ui32Loop++)
//    {
//        g_psRxDescriptor[ui32Loop].ui32CtrlStatus = 0;
//        g_psRxDescriptor[ui32Loop].ui32Count =
//            (DES1_RX_CTRL_CHAINED |
//             (RX_BUFFER_SIZE << DES1_RX_CTRL_BUFF1_SIZE_S));
//        g_psRxDescriptor[ui32Loop].pvBuffer1 = g_pui8RxBuffer;
//        g_psRxDescriptor[ui32Loop].DES3.pLink =
//            (ui32Loop == (NUM_RX_DESCRIPTORS - 1)) ?
//            g_psRxDescriptor : &g_psRxDescriptor[ui32Loop + 1];
//    }
//
//    //
//    // Set the descriptor pointers in the hardware.
//    //
//    MAP_EMACRxDMADescriptorListSet(ui32Base, g_psRxDescriptor);
//    MAP_EMACTxDMADescriptorListSet(ui32Base, g_psTxDescriptor);
//
//    //
//    // Start from the beginning of both descriptor chains.  We actually set
//    // the transmit descriptor index to the last descriptor in the chain
//    // since it will be incremented before use and this means the first
//    // transmission we perform will use the correct descriptor.
//    //
//    g_ui32RxDescIndex = 0;
//    g_ui32TxDescIndex = NUM_TX_DESCRIPTORS - 1;
//}
//
////*****************************************************************************
////
//// Read a packet from the DMA receive buffer into the uIP packet buffer.
////
////*****************************************************************************
//int32_t
//PacketReceive(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen)
//{
//    int_fast32_t i32FrameLen, i32Loop;
//
//    //
//    // Check the arguments.
//    //
//    ASSERT(ui32Base == EMAC0_BASE);
//    ASSERT(pui8Buf != 0);
//    ASSERT(i32BufLen > 0);
//
//    //
//    // By default, we assume we got a bad frame.
//    //
//    i32FrameLen = 0;
//
//    //
//    // Make sure that we own the receive descriptor.
//    //
//    if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus & DES0_RX_CTRL_OWN))
//    {
//        //
//        // We own the receive descriptor so check to see if it contains a valid
//        // frame.  Look for a descriptor error, indicating that the incoming
//        // packet was truncated or, if this is the last frame in a packet,
//        // the receive error bit.
//        //
//        if(!(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
//             DES0_RX_STAT_ERR))
//        {
//            //
//            // We have a valid frame so copy the content to the supplied
//            // buffer. First check that the "last descriptor" flag is set.  We
//            // sized the receive buffer such that it can always hold a valid
//            // frame so this flag should never be clear at this point but...
//            //
//            if(g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
//               DES0_RX_STAT_LAST_DESC)
//            {
//                i32FrameLen =
//                    ((g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus &
//                      DES0_RX_STAT_FRAME_LENGTH_M) >>
//                     DES0_RX_STAT_FRAME_LENGTH_S);
//
//                //
//                // Sanity check.  This shouldn't be required since we sized the
//                // uIP buffer such that it's the same size as the DMA receive
//                // buffer but, just in case...
//                //
//                if(i32FrameLen > i32BufLen)
//                {
//                    i32FrameLen = i32BufLen;
//                }
//
//                //
//                // Copy the data from the DMA receive buffer into the provided
//                // frame buffer.
//                //
//                for(i32Loop = 0; i32Loop < i32FrameLen; i32Loop++)
//                {
//                    pui8Buf[i32Loop] = g_pui8RxBuffer[i32Loop];
//                }
//            }
//        }
//
//        //
//        // Move on to the next descriptor in the chain.
//        //
//        g_ui32RxDescIndex++;
//        if(g_ui32RxDescIndex == NUM_RX_DESCRIPTORS)
//        {
//            g_ui32RxDescIndex = 0;
//        }
//
//        //
//        // Mark the next descriptor in the ring as available for the receiver
//        // to write into.
//        //
//        g_psRxDescriptor[g_ui32RxDescIndex].ui32CtrlStatus = DES0_RX_CTRL_OWN;
//    }
//
//    //
//    // Return the Frame Length
//    //
//    return(i32FrameLen);
//}
//
////*****************************************************************************
////
//// Transmit a packet from the supplied buffer.
////
////*****************************************************************************
//int32_t
//PacketTransmit(uint32_t ui32Base, uint8_t *pui8Buf, int32_t i32BufLen)
//{
//    int_fast32_t i32Loop;
//    WORD_T i=0;
//    //
//    // Indicate that a packet is being sent.
//    //
//    HWREGBITW(&g_ui32Flags, FLAG_TXPKT) = 1;
//
//    //
//    // Wait for the previous packet to be transmitted.
//    //
//    while((g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus &
//          DES0_TX_CTRL_OWN) & (i<300))
//    {
//        //
//        // Spin and waste time.
//        //
//    	i++;
//    }
//
//    //
//    // Check that we're not going to overflow the transmit buffer.  This
//    // shouldn't be necessary since the uIP buffer is smaller than our DMA
//    // transmit buffer but, just in case...
//    //
//    if(i32BufLen > TX_BUFFER_SIZE)
//    {
//        i32BufLen = TX_BUFFER_SIZE;
//    }
//
//    //
//    // Copy the packet data into the transmit buffer.
//    //
//    for(i32Loop = 0; i32Loop < i32BufLen; i32Loop++)
//    {
//        g_pui8TxBuffer[i32Loop] = pui8Buf[i32Loop];
//    }
//
//    //
//    // Move to the next descriptor.
//    //
//    g_ui32TxDescIndex++;
//    if(g_ui32TxDescIndex == NUM_TX_DESCRIPTORS)
//    {
//        g_ui32TxDescIndex = 0;
//    }
//
//    //
//    // Fill in the packet size and tell the transmitter to start work.
//    //
//    g_psTxDescriptor[g_ui32TxDescIndex].ui32Count = (uint32_t)i32BufLen;
//    g_psTxDescriptor[g_ui32TxDescIndex].ui32CtrlStatus =
//        (DES0_TX_CTRL_LAST_SEG | DES0_TX_CTRL_FIRST_SEG |
//         DES0_TX_CTRL_INTERRUPT | DES0_TX_CTRL_IP_ALL_CKHSUMS |
//         DES0_TX_CTRL_CHAINED | DES0_TX_CTRL_OWN);
//
//    //
//    // Tell the DMA to reacquire the descriptor now that we've filled it in.
//    //
//    MAP_EMACTxDMAPollDemand(EMAC0_BASE);
//
//    //
//    // Return the number of bytes sent.
//    //
//    return(i32BufLen);
//}
///*------------------------------------------------------------------------------+
//|           end                                                         |
//+------------------------------------------------------------------------------*/
