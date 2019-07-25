/*===========================================================================+
|  Function : Flash Driver                                                     |
|  Task     : Flash Driver Header File                                         |
|----------------------------------------------------------------------------|
|  Compile  : CCS6.1 -                                                       |
|  Link     : CCS6.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   :                                                        |
|  Version  : V1.00                                                          |
|  Creation : 07/09/2016                                                     |
|  Revision :                                                                |
+===========================================================================*/
#ifndef     D___FLASH
#define     D___FLASH

#ifdef __cplusplus
extern "C" {
#endif

#include    "kernel/include/type.h"
#include    "kernel/include/device.h"
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#define		MAX_Block				31
#define     MAX_Flash               512 * 1024
#define     MAX_OFFSET              16 * 1024 - 1
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/


/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/

/*===========================================================================+
|           Externals                                                        |
+===========================================================================*/
extern      DEV_ENGINE_T            g_Flash_Engine;

extern		SIZE_T          Flash_Read         (   BYTE_T      *pbyBuf,
                                       		 	   SIZE_T      Size,
									   			   WORD_T	   Block,
                                       			   OFFSET_T    Offset  		);
extern		SIZE_T          Flash_Write        (   BYTE_T      *pbyBuf,
                                       			   SIZE_T      Size,
									   			   WORD_T	   Block,
                                      			   OFFSET_T    Offset  		);

#ifdef __cplusplus
}
#endif


#endif
