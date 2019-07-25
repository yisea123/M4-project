/*==========================================================================+
|  File     : taskusb.c                                                     |
|  Function :               		                                        |
|  Task     :               		                                        |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : YW                                                      	    |
|  Version  : V1.00                                                         |
|  Creation : 2018/01/19                                                    |
|  Revision : 												                |
+==========================================================================*/
#include "taskusb.h"

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
//*****************************************************************************
//
// Defines the number of times to call to check if the attached device is
// ready.
//
//*****************************************************************************
volatile tUSBMode g_iCurrentMode = eUSBModeNone;
static volatile bool g_bUSBConfigured = false;
//*****************************************************************************
//
// The size of the host controller's memory pool in bytes.
//
//*****************************************************************************

#define COMMAND_PACKET_RECEIVED 0x00000001
#define COMMAND_STATUS_UPDATE   0x00000002

//*****************************************************************************
//
// The system tick rate expressed both as ticks per second and a millisecond
// period.
//
//*****************************************************************************
//volatile uint32_t g_ui32TxCount = 0;
//volatile uint32_t g_ui32RxCount = 0;

#define SYSTICKS_PER_SECOND     1000
#define MS_PER_SYSTICK          (1000 / SYSTICKS_PER_SECOND)

#define FLAG_STATUS_UPDATE      0
#define FLAG_USB_CONFIGURED     1
#define FLAG_SENDING_BREAK      2
static volatile uint32_t g_ui32Flags;

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
void RunTaskUSB(void *vpParam);
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
//*****************************************************************************
//
// Defines the number of times to call to check if the attached device is
// ready.
//
//*****************************************************************************
#define USBMSC_DRIVE_RETRY      4
//*****************************************************************************
//
// The size of the host controller's memory pool in bytes.
//
//*****************************************************************************
#define HCD_MEMORY_SIZE         128

//*****************************************************************************
//
// Defines the size of the buffers that hold the path, or temporary
// data from the USB disk.  There are two buffers allocated of this size.
// The buffer size must be large enough to hold the int32_test expected
// full path name, including the file name, and a trailing null character.
//
//*****************************************************************************
//#define PATH_BUF_SIZE   80

//*****************************************************************************
//
// The memory pool to provide to the Host controller driver.
//
//*****************************************************************************
uint8_t g_pHCDPool[HCD_MEMORY_SIZE];
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
tUSBDCDCDevice *g_psDevice;
/*==============================================================================+
|           Globle Variables                                                    |
+==============================================================================*/
uint32_t ui32DriveTimeout;
//*****************************************************************************
//
// Holds global flags for the system.
//
//*****************************************************************************
uint32_t g_dwFlags = 0;
FRESULT u_fresult;
//*****************************************************************************
//
// Flag indicating that some USB device is connected.
//
//*****************************************************************************
#define FLAGS_DEVICE_PRESENT    0x00000001
//*****************************************************************************
//
// The following are data structures used by FatFs.
//
//*****************************************************************************
static FATFS g_sFatFs;
static FIL g_sFileObject;
//*****************************************************************************
//
// This buffer holds the full path to the current working directory.
// Initially it is root ("/").
//
//*****************************************************************************
//static char g_cCwdBuf[PATH_BUF_SIZE] = "/";
//*****************************************************************************
//
// The instance data for the MSC driver.
//
//*****************************************************************************
tUSBHMSCInstance *g_psMSCInstance = 0;
//*****************************************************************************
//
// Declare the USB Events driver interface.
//
//*****************************************************************************
DECLARE_EVENT_DRIVER(g_sUSBEventDriver, 0, 0, USBHCDEvents);
//*****************************************************************************
//
// The global that holds all of the host drivers in use in the application.
// In this case, only the MSC class is loaded.
//
//*****************************************************************************
static tUSBHostClassDriver const * const g_ppHostClassDrivers[] =
{
    &g_sUSBHostMSCClassDriver,
    &g_sUSBEventDriver
};

//*****************************************************************************
//
// This global holds the number of class drivers in the g_ppHostClassDrivers
// list.
//
//*****************************************************************************
static const uint32_t g_ui32NumHostClassDrivers =
    sizeof(g_ppHostClassDrivers) / sizeof(tUSBHostClassDriver *);
//*****************************************************************************
//
// Hold the current state for the application.
//
//*****************************************************************************
volatile enum
{
    //
    // No device is present.
    //
    STATE_NO_DEVICE,

    //
    // Mass storage device is being enumerated.
    //
    STATE_DEVICE_ENUM,

    //
    // Mass storage device is ready.
    //
    STATE_DEVICE_READY,

    //
    // An unsupported device has been attached.
    //
    STATE_UNKNOWN_DEVICE,

    //
    // A mass storage device was connected but failed to ever report ready.
    //
    STATE_TIMEOUT_DEVICE,

    //
    // A power fault has occurred.
    //
    STATE_POWER_FAULT
}
g_eState;

WORD	g_wUSBFlag = 0;

//*****************************************************************************
//
// Callback function for mode changes.
//
//*****************************************************************************
void
ModeCallback(uint32_t ui32Index, tUSBMode iMode)
{
    //
    // Save the new mode.
    //
    g_iCurrentMode = iMode;

    switch(iMode)
    {
        case eUSBModeHost:
        {
            break;
        }
        case eUSBModeDevice:
        {
            break;
        }
        case eUSBModeNone:
        {
            break;
        }
        default:
        {
            break;
        }
    }
//    g_ui32NewState = 1;
}
//*****************************************************************************
//
// Initializes the file system module.
//
// \param None.
//
// This function initializes the third party FAT implementation.
//
// \return Returns \e true on success or \e false on failure.
//
//*****************************************************************************
//static bool
//FileInit(void)
//{
//    //
//    // Mount the file system, using logical disk 0.
//    //
//    if(f_mount(0, &g_sFatFs) != FR_OK)
//    {
//        return(false);
//    }
//    return(true);
//}
//*****************************************************************************
//
// This is the callback from the MSC driver.
//
// \param ps32Instance is the driver instance which is needed when communicating
// with the driver.
// \param ui32Event is one of the events defined by the driver.
// \param pvData is a pointer to data passed into the initial call to register
// the callback.
//
// This function handles callback events from the MSC driver.  The only events
// currently handled are the MSC_EVENT_OPEN and MSC_EVENT_CLOSE.  This allows
// the main routine to know when an MSC device has been detected and
// enumerated and when an MSC device has been removed from the system.
//
// \return None
//
//*****************************************************************************
static void
MSCCallback(tUSBHMSCInstance *ps32Instance, uint32_t ui32Event, void *pvData)
{
    //
    // Determine the event.
    //
    switch(ui32Event)
    {
        //
        // Called when the device driver has successfully enumerated an MSC
        // device.
        //
        case MSC_EVENT_OPEN:
        {
            //
            // Proceed to the enumeration state.
            //
            g_eState = STATE_DEVICE_ENUM;

            break;
        }

        //
        // Called when the device driver has been unloaded due to error or
        // the device is no int32_ter present.
        //
        case MSC_EVENT_CLOSE:
        {
            //
            // Go back to the "no device" state and wait for a new connection.
            //
            g_eState = STATE_NO_DEVICE;
            g_wUSBFlag = STATE_NO_DEVICE;

            //
            // Re-initialize the file system.
            //
//            FileInit();

            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// This is the generic callback from host stack.
//
// \param pvData is actually a pointer to a tEventInfo structure.
//
// This function will be called to inform the application when a USB event has
// occurred that is outside those related to the mass storage device.  At this
// point this is used to detect unsupported devices being inserted and removed.
// It is also used to inform the application when a power fault has occurred.
// This function is required when the g_USBGenerii8EventDriver is included in
// the host controller driver array that is passed in to the
// USBHCDRegisterDrivers() function.
//
// \return None.
//
//*****************************************************************************
void
USBHCDEvents(void *pvData)
{
    tEventInfo *pEventInfo;

    //
    // Cast this pointer to its actual type.
    //
    pEventInfo = (tEventInfo *)pvData;

    switch(pEventInfo->ui32Event)
    {
        //
        // An unknown device has been connected.
        //
        case USB_EVENT_UNKNOWN_CONNECTED:
        {
            //
            // An unknown device was detected.
            //
            g_eState = STATE_UNKNOWN_DEVICE;

            break;
        }

        //
        // The unknown device has been been unplugged.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // Unknown device has been removed.
            //
            g_eState = STATE_NO_DEVICE;
            g_wUSBFlag = STATE_NO_DEVICE;

            break;
        }

        //
        // A bus power fault was detected.
        //
        case USB_EVENT_POWER_FAULT:
        {
            //
            // No power means no device is present.
            //
            g_eState = STATE_POWER_FAULT;

            break;
        }

        default:
        {
            break;
        }
    }
}

//*****************************************************************************
//
// Initialize the host mode stack.
//
//*****************************************************************************
void
HostInit(void)
{
	uint32_t ui32PLLRate;

	ui32PLLRate = 480000000;
	//
    // Register the host class drivers.
    //
    USBHCDRegisterDrivers(0, g_ppHostClassDrivers, g_ui32NumHostClassDrivers);

    //
    // Open an instance of the mass storage class driver.
    //
    g_psMSCInstance = USBHMSCDriveOpen(0, MSCCallback);

    //
    // Initialize the drive timeout.
    //
    ui32DriveTimeout = USBMSC_DRIVE_RETRY;
    //
    // Initialize the power configuration. This sets the power enable signal
    // to be active high and does not enable the power fault.
    //
    USBHCDPowerConfigInit(0, USBHCD_VBUS_AUTO_HIGH | USBHCD_VBUS_FILTER);
    //
    // Tell the USB library the CPU clock and the PLL frequency.  This is a
    // new requirement for TM4C129 devices.
    //
    USBHCDFeatureSet(0, USBLIB_FEATURE_CPUCLK, &g_dwSystemClock);
    USBHCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &ui32PLLRate);
    //
    // Initialize the USB controller for host operation.
    //
    USBHCDInit(0, g_pHCDPool, HCD_MEMORY_SIZE);

    //
    // Initially wait for device connection.
    //
    g_eState = STATE_NO_DEVICE;
}

//*****************************************************************************
//
// Initialize the device mode stack.
//
//*****************************************************************************
void
DeviceInit(void)
{
	//
	// Not configured initially.
	//
	g_bUSBConfigured = false;

	USBBufferInit(&g_sTxBuffer);
	USBBufferInit(&g_sRxBuffer);

//	USBStackModeSet(0, eUSBModeDevice, 0);

//	USBDBulkInit(0, &g_sBulkDevice);
	USBDCDCInit(0, (tUSBDCDCDevice *)&g_sCDCDevice);
}

//*****************************************************************************
//
// This function returns the number of ticks since the last time this function
// was called.
//
//*****************************************************************************
uint32_t
GetTickms(void)
{
    uint32_t ui32RetVal, ui32Saved;
    static uint32_t g_ui32LastTick;
    ui32RetVal = g_dwSystemTick;
    ui32Saved = ui32RetVal;

    if(ui32Saved > g_ui32LastTick)
    {
        ui32RetVal = ui32Saved - g_ui32LastTick;
    }
    else
    {
        ui32RetVal = g_ui32LastTick - ui32Saved;
    }

    //
    // This could miss a few milliseconds but the timings here are on a
    // much larger scale.
    //
    g_ui32LastTick = ui32Saved;

    //
    // Return the number of milliseconds since the last time this was called.
    //
    return(ui32RetVal * MS_PER_SYSTICK);
}

//*****************************************************************************
//
// This is the main loop that runs the mouse device application.
//
//*****************************************************************************
void
DeviceMain(void)
{

}

//*****************************************************************************
//
// This is the main loop that runs the application.
//
//*****************************************************************************
void
HostMain(void)
{
//    //
//    //USB鏄惁宸茬粡鍑嗗濂�
//    //
//    if(g_eState == STATE_DEVICE_READY)
//    {
//        //
//        //鍒ゆ柇U鐩樻牴鐩綍涓嬫槸鍚︽湁Runtime.bin鏂囦欢
//        //
//         u_fresult =  f_open(&g_sFileObject, "/runtime.bin", FA_READ);
//         if(u_fresult == FR_OK)
//         {
//            f_close(&g_sFileObject);
//            (*((void (*)(void))(*(uint32_t *)0x2c)))();
//         }
//
//    }
    //
     // Call the USB stack to keep it running.
     //
     USBHCDMain();

     switch(g_eState)
     {
          case STATE_DEVICE_ENUM:
          {
              //
              // Take it easy on the Mass storage device if it is slow to
              // start up after connecting.
              //
              if(USBHMSCDriveReady(g_psMSCInstance) != 0)
              {
                  //
                  // Wait about 500ms before attempting to check if the
                  // device is ready again.
                  //
                  ROM_SysCtlDelay(g_dwSystemClock / (3 * 2));

                  //
                  // Decrement the retry count.
                  //
                  ui32DriveTimeout--;

                  //
                  // If the timeout is hit then go to the
                  // STATE_TIMEOUT_DEVICE state.
                  //
                  if(ui32DriveTimeout == 0)
                  {
                      g_eState = STATE_TIMEOUT_DEVICE;
                  }
                  break;
              }
                  g_eState = STATE_DEVICE_READY;
                  g_wUSBFlag = STATE_DEVICE_READY;

              //
              // Set the Device Present flag.
              //
              g_dwFlags = FLAGS_DEVICE_PRESENT;
              break;
          }

          //
          // If there is no device then just wait for one.
          //
          case STATE_NO_DEVICE:
          {
              if(g_dwFlags == FLAGS_DEVICE_PRESENT)
              {
                  //
                  // Clear the Device Present flag.
                  //
                  g_dwFlags &= ~FLAGS_DEVICE_PRESENT;
              }
              break;
          }
          //
          // An unknown device was connected.
          //
          case STATE_UNKNOWN_DEVICE:
          {
              //
              // If this is a new device then change the status.
              //
              if((g_dwFlags & FLAGS_DEVICE_PRESENT) == 0)
              {

              }
              //
              // Set the Device Present flag.
              //
              g_dwFlags = FLAGS_DEVICE_PRESENT;
              break;
          }
          //
          // The connected mass storage device is not reporting ready.
          //
          case STATE_TIMEOUT_DEVICE:
          {
              //
              // If this is the first time in this state then print a
              // message.
              //
              if((g_dwFlags & FLAGS_DEVICE_PRESENT) == 0)
              {

              }

              //
              // Set the Device Present flag.
              //
              g_dwFlags = FLAGS_DEVICE_PRESENT;
              break;
          }
          //
          // Something has caused a power fault.
          //
          case STATE_POWER_FAULT:
          {
              break;
          }
          default:
          {
              break;
          }
      }
}
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void Init_USB()
{
    uint32_t   ui32SysClock, ui32PLLRate;

    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_USB0);

	tLPMFeature sLPMFeature;

	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	//
	// PB0-1/PD6-7/PL6-7 are used for USB.
	//
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) = 0xff;
    ROM_GPIOPinConfigure(GPIO_PD6_USB0EPEN);
    ROM_GPIOPinConfigure(GPIO_PD7_USB0PFLT);
    ROM_GPIOPinTypeUSBAnalog(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_GPIOPinTypeUSBDigital(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    ROM_GPIOPinTypeUSBAnalog(GPIO_PORTL_BASE, GPIO_PIN_6 | GPIO_PIN_7);

    //
    // Set the current processor speed to provide more accurate timing
    // information to the USB library.
    //
    ui32SysClock = g_dwSystemClock;
    ui32PLLRate = 480000000;

    USBDCDFeatureSet(0, USBLIB_FEATURE_CPUCLK, &ui32SysClock);
    USBDCDFeatureSet(0, USBLIB_FEATURE_USBPLL, &ui32PLLRate);

    //
    // Initialize the USB OTG mode and pass in a mode callback.
    //
    USBStackModeSet(0, eUSBModeOTG, ModeCallback);

    sLPMFeature.ui32HIRD = 500;
    sLPMFeature.ui32Features = USBLIB_FEATURE_LPM_EN |
                               USBLIB_FEATURE_LPM_RMT_WAKE;
    USBHCDFeatureSet(0, USBLIB_FEATURE_LPM, &sLPMFeature);

    //
    // Initialize the host stack.
    //
    HostInit();

    //
    // Initialize the device stack.
    //
    DeviceInit();

    //
    // Initialize the USB controller for dual mode operation with a 2ms polling
    // rate.
    //
    USBOTGModeInit(0, 2000, g_pHCDPool, HCD_MEMORY_SIZE);

    //
    // Initialize the file system.
    //
//    FileInit();
}

/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void CreateTaskUSB()
{
	TASK	Task;
	Task.wType = TASK_WHILELOOP;
	Task.period.tCounter = 0;
	Task.period.tTimer = 1;
	Task.pRun = RunTaskUSB;
	Task.pParam = 0;
	Create_Task(&Task, 0);
}
/*---------------------------------------------------------------------------+
|  Function :                                                                |
|  Task     :                                                                |
+----------------------------------------------------------------------------+
|  Call     :                                                                |
|                                                                            |
|  Parameter:                           -                                    |
|                                                                            |
|  Return   :                           -                                    |
+---------------------------------------------------------------------------*/
void RunTaskUSB(void *vpParam)
{
	USBOTGMain(GetTickms());

    if(g_iCurrentMode == eUSBModeDevice)
    {
        DeviceMain();
    }
    else if(g_iCurrentMode == eUSBModeHost)
    {
        HostMain();
    }
}

//*****************************************************************************
//
// Receive new data and echo it back to the host.
//
// \param psDevice points to the instance data for the device whose data is to
// be processed.
// \param pi8Data points to the newly received data in the USB receive buffer.
// \param ui32NumBytes is the number of bytes of data available to be processed.
//
// This function is called whenever we receive a notification that data is
// available from the host. We read the data, byte-by-byte and swap the case
// of any alphabetical characters found then write it back out to be
// transmitted back to the host.
//
// \return Returns the number of bytes of data processed.
//
//*****************************************************************************
uint32_t
USB_WriteToHost_Studio(tUSBDCDCDevice *psDevice, uint8_t *pi8Data,
                  uint_fast32_t ui32NumBytes)
{
//    uint_fast32_t ui32Loop, ui32Space, ui32Count;
    tUSBRingBufObject sTxRing;

    //
    // Get the current buffer information to allow us to write directly to
    // the transmit buffer (we already have enough information from the
    // parameters to access the receive buffer directly).
    //
    USBBufferInfoGet(&g_sTxBuffer, &sTxRing);

    //
    // How much space is there in the transmit buffer?
    //
//    ui32Space = USBBufferSpaceAvailable(&g_sTxBuffer);

    //
    // How many characters can we process this time round?
    //
//    ui32Loop = (ui32Space < ui32NumBytes) ? ui32Space : ui32NumBytes;
//    ui32Loop = ui32NumBytes;
//    ui32Count = ui32Loop;

    //
    // Dump a debug message.
    //
//    DEBUG_PRINT("Received %d bytes\n", ui32NumBytes);

//    memcpy(g_pui8USBTxBuffer, pi8Data, ui32Count);
    //
    // We've processed the data in place so now send the processed data
    // back to the host.
    //
//    USBBufferDataWritten(&g_sTxBuffer, ui32Count);
    USBBufferWrite((tUSBBuffer *)&g_sTxBuffer,
                   (unsigned char *)pi8Data, ui32NumBytes);

 //   DEBUG_PRINT("Wrote %d bytes\n", ui32Count);

    //
    // We processed as much data as we can directly from the receive buffer so
    // we need to return the number of bytes to allow the lower layer to
    // update its read pointer appropriately.
    //
    return(ui32NumBytes);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to control and setup of the device.
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to perform control-related
// operations on behalf of the USB host.  These functions include setting
// and querying the serial communication parameters, setting handshake line
// states and sending break conditions.
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
ControlHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
               void *pvMsgData)
{
    //
    // Which event are we being asked to process?
    //
    switch(ui32Event)
    {
        //
        // We are connected to a host and communication is now possible.
        //
        case USB_EVENT_CONNECTED:
        {
            //
            // Now connected and ready for normal operation.
            //
            HWREGBITW(&g_ui32Flags, FLAG_USB_CONFIGURED) = 1;

            //
            // Flush our buffers.
            //
            USBBufferFlush(&g_sTxBuffer);
            USBBufferFlush(&g_sRxBuffer);

            //
            // Set the command status update flag.
            //
            HWREGBITW(&g_ui32Flags, FLAG_STATUS_UPDATE) = 1;

            break;
        }

        //
        // The host has disconnected.
        //
        case USB_EVENT_DISCONNECTED:
        {
            //
            // No longer connected.
            //
            HWREGBITW(&g_ui32Flags, FLAG_USB_CONFIGURED) = 0;

            //
            // Set the command status update flag.
            //
            HWREGBITW(&g_ui32Flags, FLAG_STATUS_UPDATE) = 1;

            break;
        }

        //
        // Return the current serial communication parameters.
        //
        case USBD_CDC_EVENT_GET_LINE_CODING:
        {
//            GetLineCoding(pvMsgData, UART_CLOCK);
            break;
        }

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_LINE_CODING:
        {
//            SetLineCoding(pvMsgData, UART_CLOCK);
            break;
        }

        //
        // Set the current serial communication parameters.
        //
        case USBD_CDC_EVENT_SET_CONTROL_LINE_STATE:
        {
//            SetControlLineState((uint16_t)ui32MsgValue);
            break;
        }

        //
        // Send a break condition on the serial line.
        //
        case USBD_CDC_EVENT_SEND_BREAK:
        {
//            SendBreak(true);
            break;
        }

        //
        // Clear the break condition on the serial line.
        //
        case USBD_CDC_EVENT_CLEAR_BREAK:
        {
//            SendBreak(false);
            break;
        }

        //
        // Ignore SUSPEND and RESUME for now.
        //
        case USB_EVENT_SUSPEND:
        case USB_EVENT_RESUME:
        {
            break;
        }

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
        {
#ifdef DEBUG
            while(1);
#else
            break;
#endif
        }

    }

    return(0);
}

//*****************************************************************************
//
// Handles bulk driver notifications related to the transmit channel (data to
// the USB host).
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ulEvent identifies the event we are being notified about.
// \param ulMsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the bulk driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
//uint32_t
//TxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
//          void *pvMsgData)
//{
//    //
//    // We are not required to do anything in response to any transmit event
//    // in this example. All we do is update our transmit counter.
//    //
//    if(ui32Event == USB_EVENT_TX_COMPLETE)
//    {
////        g_ui32TxCount += ui32MsgValue;
//    }
//
//    //
//    // Dump a debug message.
//    //
//  ///  DEBUG_PRINT("TX complete %d\n", ui32MsgValue);
//
//    return(0);
//}

//*****************************************************************************
//
// Handles bulk driver notifications related to the receive channel (data from
// the USB host).
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the bulk driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
//
//uint32_t
//RxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
//          void *pvMsgData)
//{
//    //
//    // Which event are we being sent?
//    //
//    switch(ui32Event)
//    {
//        //
//        // We are connected to a host and communication is now possible.
//        //
//        case USB_EVENT_CONNECTED:
//        {
//            g_bUSBConfigured = true;
//            g_ui32Flags |= COMMAND_STATUS_UPDATE;
//
//            //
//            // Flush our buffers.
//            //
//            USBBufferFlush(&g_sTxBuffer);
//            USBBufferFlush(&g_sRxBuffer);
//
//            break;
//        }
//
//        //
//        // The host has disconnected.
//        //
//        case USB_EVENT_DISCONNECTED:
//        {
//            g_bUSBConfigured = false;
//            g_ui32Flags |= COMMAND_STATUS_UPDATE;
//            break;
//        }
//
//        //
//        // A new packet has been received.
//        //
//        case USB_EVENT_RX_AVAILABLE:
//        {
//            //
//            // Get a pointer to our instance data from the callback data
//            // parameter.
//            //
//        	g_psDevice = (tUSBDBulkDevice *)pvCBData;
//        	g_RcvPortflag = 0;
//            memcpy(g_byRecv_Buffer, pvMsgData, ui32MsgValue);
//            DealRcvProtocol(ui32MsgValue);
//        }
//
//        //
//        // Ignore SUSPEND and RESUME for now.
//        //
//        case USB_EVENT_SUSPEND:
//        case USB_EVENT_RESUME:
//            break;
//
//        //
//        // Ignore all other events and return 0.
//        //
//        default:
//            break;
//    }
//
//    return(0);
//}

//*****************************************************************************
//
// Handles CDC driver notifications related to the transmit channel (data to
// the USB host).
//
// \param pvCBData is the client-supplied callback pointer for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the transmit data channel (the IN channel carrying
// data to the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
TxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
    //
    // Which event have we been sent?
    //
    switch(ui32Event)
    {
        case USB_EVENT_TX_COMPLETE:
        {
            //
            // Since we are using the USBBuffer, we don't need to do anything
            // here.
            //
            break;
        }

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
        {
#ifdef DEBUG
            while(1);
#else
            break;
#endif
        }
    }
    return(0);
}

//*****************************************************************************
//
// Handles CDC driver notifications related to the receive channel (data from
// the USB host).
//
// \param pvCBData is the client-supplied callback data value for this channel.
// \param ui32Event identifies the event we are being notified about.
// \param ui32MsgValue is an event-specific value.
// \param pvMsgData is an event-specific pointer.
//
// This function is called by the CDC driver to notify us of any events
// related to operation of the receive data channel (the OUT channel carrying
// data from the USB host).
//
// \return The return value is event-specific.
//
//*****************************************************************************
uint32_t
RxHandler(void *pvCBData, uint32_t ui32Event, uint32_t ui32MsgValue,
          void *pvMsgData)
{
	static uint16_t pvMsgPacket=0;
	static uint16_t pvLength=0;
	static uint8_t pvPacket=0;
    //
    // Which event are we being sent?
    //
    switch(ui32Event)
    {
        //
        // A new packet has been received.
        //
        case USB_EVENT_RX_AVAILABLE:
        {
            //
            // Feed some characters into the UART TX FIFO and enable the
            // interrupt so we are told when there is more space.
            //
        	g_psDevice = (tUSBDCDCDevice *)pvCBData;
        	g_RcvPortflag = 0;
        	if(pvMsgPacket == 0)//绗竴鍖卽sb鏁版嵁鍒欏垽鏂暟鎹暱搴︽槸鍚﹁秴杩噓sb fullspeed妯″紡涓嬬殑鏈�ぇ闀垮害64瀛楄妭
        	{
        		pvPacket = 0;
        		memcpy(&pvLength, pvMsgData, 2);
            	pvLength = htons(pvLength);
				if(pvLength > 64 && ui32MsgValue == 64)
				{
					pvMsgPacket = pvLength / 64 + 1;
				}
				if(pvLength <= 64)
				{
					USBBufferRead((tUSBBuffer *)&g_sRxBuffer, g_byRecv_Buffer, ui32MsgValue);
					DealRcvProtocol(pvLength);
					memset(g_byRecv_Buffer, 0, pvLength);
					break;
				}
        	}
			USBBufferRead((tUSBBuffer *)&g_sRxBuffer, g_byRecv_Buffer+pvPacket*64, ui32MsgValue);
			pvMsgPacket--;
			pvPacket++;
			if(pvMsgPacket == 0)
			{
				DealRcvProtocol(pvLength);
				memset(g_byRecv_Buffer, 0, pvLength);
			}
            break;
        }

        //
        // We are being asked how much unprocessed data we have still to
        // process. We return 0 if the UART is currently idle or 1 if it is
        // in the process of transmitting something. The actual number of
        // bytes in the UART FIFO is not important here, merely whether or
        // not everything previously sent to us has been transmitted.
        //
        case USB_EVENT_DATA_REMAINING:
        {
            //
            // Get the number of bytes in the buffer and add 1 if some data
            // still has to clear the transmitter.
            //
        }

        //
        // We are being asked to provide a buffer into which the next packet
        // can be read. We do not support this mode of receiving data so let
        // the driver know by returning 0. The CDC driver should not be sending
        // this message but this is included just for illustration and
        // completeness.
        //
        case USB_EVENT_REQUEST_BUFFER:
        {
            return(0);
        }

        //
        // We don't expect to receive any other events.  Ignore any that show
        // up in a release build or hang in a debug build.
        //
        default:
#ifdef DEBUG
            while(1);
#else
            break;
#endif
    }

    return(0);
}
