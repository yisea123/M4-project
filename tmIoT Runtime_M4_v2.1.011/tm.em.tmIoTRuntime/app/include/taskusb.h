/*==========================================================================+
|  File     : taskusb.h                                                     |
|  Function :               		                                        |
|  Task     :               		                                        |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : Jia                                                           |
|  Version  : V1.00                                                         |
|  Creation : 2016/05/22                                                    |
|  Revision : 												                |
+==========================================================================*/
#ifndef	D_TASKUSB_H
#define	D_TASKUSB_H

#include "kernel/include/type.h"
#include "M4_lib/usblib/usblib.h"
#include "M4_lib/usblib/host/usbhost.h"
#include "M4_lib/usblib/host/usbhmsc.h"
//#include "M4_lib/utils/ustdlib.h"
#include "M4_lib/fatfs/src/ff.h"
#include "M4_lib/fatfs/src/diskio.h"
#include "driver/include/_timer.h"
#include "kernel/include/task.h"


#include <stdbool.h>
#include <stdint.h>
#include "M4_lib/inc/hw_memmap.h"
#include "M4_lib/inc/hw_types.h"
#include "M4_lib/inc/hw_usb.h"
#include "M4_lib/driverlib/rom.h"
#include "M4_lib/driverlib/rom_map.h"
#include "M4_lib/driverlib/sysctl.h"
#include "M4_lib/grlib/grlib.h"
//#include "M4_lib/usblib/usblib.h"
#include "M4_lib/usblib/device/usbdevice.h"
//#include "M4_lib/usblib/host/usbhost.h"


#include "M4_lib/usblib/usbcdc.h"
#include "M4_lib/usblib/device/usbdcomp.h"
#include "M4_lib/usblib/device/usbdcdc.h"

//#include "M4_lib/usblib/usblib.h"
//#include "M4_lib/usblib/usbcdc.h"
#include "M4_lib/usblib/usb-ids.h"
//#include "M4_lib/usblib/device/usbdevice.h"
//#include "M4_lib/usblib/device/usbdcdc.h"

#include "M4_lib/inc/hw_ints.h"
#include "M4_lib/driverlib/interrupt.h"
#include "M4_lib/driverlib/sysctl.h"
#include "M4_lib/driverlib/systick.h"
#include "M4_lib/driverlib/rom.h"
#include "M4_lib/driverlib/rom_map.h"
#include "M4_lib/grlib/grlib.h"
//#include "M4_lib/usblib/usb-ids.h"
//#include "M4_lib/usblib/device/usbdevice.h"
//#include "M4_lib/usblib/device/usbdbulk.h"

//#include "usb/usb_bulk/usb_bulk_structs.h"
#include "usb/usb_serial/usb_serial_structs.h"

#include  "studioprotocol.h"
/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
extern	tUSBDCDCDevice *g_psDevice;
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/

/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
void Init_USB();
void CreateTaskUSB();
uint32_t USB_WriteToHost_Studio(tUSBDCDCDevice *psDevice, uint8_t *pi8Data, uint_fast32_t ui32NumBytes);
/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/
extern WORD	g_wUSBFlag;
#endif
