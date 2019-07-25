/*===========================================================================+
|  Function : Header	                                                     |
|  Task     : 			                                                     |
|----------------------------------------------------------------------------|
|  Compile  : 			                                                   	 |
|  Link     : 			                                                   	 |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : 			                                                     |
|  Version  : V1.00                                                          |
|  Creation : 			                                                     |
|  Revision :                                                          	     |
+===========================================================================*/

#ifndef     D__TYPE
#define     D__TYPE

#ifdef __cplusplus
extern "C" {
#endif

#include	<string.h>
#include    <stdint.h>
#include    <stdbool.h>
#include 	<stdlib.h>
#include	<stdio.h>

#include    "M4_lib/inc/hw_ints.h"
#include    "M4_lib/inc/hw_memmap.h"

#include    "M4_lib/inc/hw_uart.h"
#include    "M4_lib/inc/hw_gpio.h"
#include	"M4_lib/inc/hw_epi.h"
#include	"M4_lib/inc/hw_can.h"
#include 	"M4_lib/inc/hw_flash.h"
#include    "M4_lib/inc/hw_types.h"
#include    "M4_lib/driverlib/debug.h"
#include    "M4_lib/driverlib/gpio.h"
#include    "M4_lib/driverlib/interrupt.h"
#include    "M4_lib/driverlib/sysctl.h"
#include    "M4_lib/driverlib/systick.h"
#include    "M4_lib/driverlib/uart.h"
#include    "M4_lib/driverlib/can.h"
#include    "M4_lib/driverlib/i2c.h"
#include    "M4_lib/driverlib/ssi.h"
#include	"M4_lib/driverlib/epi.h"
#include    "M4_lib/driverlib/rom.h"
#include    "M4_lib/driverlib/rom_map.h"
#include 	"M4_lib/driverlib/udma.h"
#include 	"M4_lib/driverlib/pin_map.h"
#include    "M4_lib/driverlib/eeprom.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define		ERROR_NONE							0
#define		ERROR_DEV_HANDLE_NOTEXIST			1
#define		ERROR_DEV_NOTCREATE					2
#define		ERROR_DEV_OVERMEMORY				3
/**
 * ...
 */
#define     FALSE               (1 == 0)
#define     TRUE                (1 == 1)
#define     NO                  FALSE
#define     YES                 TRUE

/**
 * define ASCII Control code
 */
#ifndef	    NULL
#define     NULL                        (void *)0
#endif

#define     NUL                 0x00    /* Null                 */
#define     SOH                 0x01    /* Start Of Head        */
#define     MD1                 0x01    /*                      */
#define     STX                 0x02    /* Start of TeXt        */
#define     MD2                 0x02    /*                      */
#define     ETX                 0x03    /* End of TeXt          */
#define     MD3                 0x03    /*                      */
#define     BEL                 0x07    /* BELl                 */
#define     BS                  0x08    /* BackSpace            */
#define     HT                  0x09    /* Horizontal Tabulator */
#define     LF                  0x0A    /* Line Feed            */
#define     VT                  0x0B    /* Vertical Tabulator   */
#define     HOM                 0x0B    /*                      */
#define     FF                  0x0C    /* Form Feed            */
#define     CLR                 0x0C    /*                      */
#define     CR                  0x0D    /* Carriage Return      */
#define     CAN                 0x18    /*                      */
#define     ESC                 0x1B    /* ESCape               */
#define     US                  0x1F    /*                      */

/**
 * ...
 */
#define     MASK_NULL           0x0000
#define     MASK_BIT00          0x01
#define     MASK_BIT01          0x02
#define     MASK_BIT02          0x04
#define     MASK_BIT03          0x08
#define     MASK_BIT04          0x10
#define     MASK_BIT05          0x20
#define     MASK_BIT06          0x40
#define     MASK_BIT07          0x80
#define     MASK_BIT08          0x0100
#define     MASK_BIT09          0x0200
#define     MASK_BIT10          0x0400
#define     MASK_BIT11          0x0800
#define     MASK_BIT12          0x1000
#define     MASK_BIT13          0x2000
#define     MASK_BIT14          0x4000
#define     MASK_BIT15          0x8000

/**
 * ...
 */
#define     ID_NULL             0
#define     NULL_ID             ID_NULL
#define     HANDLE_NULL         0
#define     NULL_HANDLE         HANDLE_NULL

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef     unsigned char       BYTE;   /* 16-Bit */
typedef     unsigned short 		WORD;   /* 16-Bit */
typedef     int                	INT;    /* 32-Bit */
typedef     bool                BOOL;   /* 8-Bit */
typedef		unsigned  short		USHORT;
typedef     long                LONG;   /* 32-Bit */
typedef     unsigned long       DWORD;  /* 32-Bit */
typedef		float				FLOAT;	/* 32-Bit */
typedef		double				DOUBLE;	/* 32-Bit */

typedef     WORD                RTTI;   /* Run Time Type Information       */
typedef     WORD                HANDLE; /* Handle (ID) of object           */

typedef     long                TINT;   /* Timer unit, must be signed type */


typedef     char                        INT8_T;
typedef     unsigned char               UINT8_T;
typedef     short                       INT16_T;
typedef     unsigned short              UINT16_T;
typedef     long                        INT32_T;
typedef     unsigned long               UINT32_T;
typedef     long                        INT_T;

typedef		INT8_T					    CHAR_T;
typedef     UINT8_T                     BYTE_T;
typedef     INT16_T                     SHORT_T;
typedef     UINT16_T                    WORD_T;
typedef		UINT8_T						BOOL_T;
typedef     INT32_T                     LONG_T;
typedef     UINT32_T                    DWORD_T;

typedef     INT32_T                     TINT_T;
typedef		WORD_T					    HANDLE_T;
typedef     WORD_T                      SIZE_T;
typedef     DWORD_T                     OFFSET_T;    // WORD_T --> DWORD_T  2016/08/04
typedef		WORD_T						ERROR_T;

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/
#define     LOBYTE(w)           ((BYTE)((w) & 0x00FF))
#define     HIBYTE(w)           ((BYTE)((WORD)(w) >> 8))
#define     LOWORD(l)           ((WORD)(l))
#define     HIWORD(l)           ((WORD)((DWORD)(l) >> 16))

#define		SWAPWORD(w)			((WORD)(((WORD)(w) >> 8) | (((WORD)(w) & 0x00FF) << 8)))
#define		SWAPDWORD(dw)		((DWORD)(((DWORD)(dw) >> 16) | ((DWORD)(dw) & 0x0000FFFF) << 16))

#define     ABS(n)              (((n) >= 0) ? (n) : -(n))
	
#define		_inPortW(a)			(*(volatile WORD*)(a))
#define		_outPortW(a, v)		(*(volatile WORD*)(a) = (WORD)(v))


#ifndef     sizeofB
#define		sizeofB(a)			((sizeof(a)/sizeof(WORD))<<1)
#endif

#define     sizeofW(a)			(sizeof(a)/sizeof(WORD))

#define		CHANGEW(w)			((WORD)(((WORD)(w) >> 8) | (((WORD)(w) & 0x00FF) << 8)))
/*===========================================================================+
|           Function and Class prototype                                     |
+===========================================================================*/


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
