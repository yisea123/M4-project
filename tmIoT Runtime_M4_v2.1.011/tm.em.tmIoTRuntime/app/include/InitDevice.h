#ifndef     __INITDEVICE_H_
#define     __INITDEVICE_H_



#ifdef __cplusplus
extern "C" {
#endif

#include	"kernel/include/device.h"
#include    "driver/include/_input.h"
#include    "driver/include/_output.h"
#include    "driver/include/_led.h"
#include    "driver/include/_ad.h"
#include    "driver/include/_da.h"
#include    "driver/include/_addr.h"
#include    "driver/include/_ethernet.h"
#include    "driver/include/_eeprom.h"
//#include    "driver/include/_eeprom_at24c16.h"
#include    "driver/include/_flash.h"

/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Type definition                                                   |
+============================================================================*/
/*===========================================================================+
|           Macro definition                                                  |
+============================================================================*/
/*===========================================================================+
|            Operations                                                      |
+============================================================================*/
/*===========================================================================+
|            Attributes                                                      |
+============================================================================*/
/*===========================================================================+
|            External                                                        |
+===========================================================================*/


void        Init_Input              (          void          );
void        Init_Output             (          void          );
void        Init_Ad                 (          void          );
void        Init_Da                 (          void          );
//void        Init_CAN                (          void          );
void    	Init_led                (          void          );
void        Init_eeprom             (          void          );
//void        Init_EepromAt24c16      (          void          );
//void        Init_Addr               (          void          );
void        Init_tmIoT              (          void          );
//void        Init_Debug              (          void          );
//void        Init_Ethernet           (          void          );
//void        Init_flash              (          void          );

/*===========================================================================+
|            External Veriable                                               |
+===========================================================================*/

extern HANDLE_T      g_hInput;
extern HANDLE_T      g_hOutput;
extern HANDLE_T      g_hAd;
extern HANDLE_T      g_hDa;
//extern HANDLE_T      g_hEthernet;
extern HANDLE_T      g_hCAN0;
extern HANDLE_T      g_hCAN1;
extern HANDLE_T      g_hEeprom;
//extern HANDLE_T      g_hEepromAt24c16;
extern HANDLE_T      g_hLed;
//extern HANDLE_T      g_hAddress;


#ifdef __cplusplus
}
#endif


#endif
