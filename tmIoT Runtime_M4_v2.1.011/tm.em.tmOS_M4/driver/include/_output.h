/*===========================================================================+
|  Function : Output Driver                                                  |
|  Task     : Output Driver Header File                                      |
|----------------------------------------------------------------------------|
|  Compile  : CCS6.1 -                                                       |
|  Link     : CCS6.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Jia                                                            |
|  Version  : V1.00                                                          |
|  Creation : 2016/08/17                                                     |
|  Revision :                                                                |
+===========================================================================*/
#ifndef     D_OUTPUT_H
#define     D_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include    "kernel/include/type.h"
#include    "kernel/include/device.h"
#include	"config.h"
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#ifdef		M2C2132
	#define     MAX_OUTPUT                          2
#endif
#if (defined IO2C1111) || (defined IO2C1112)
	#define     MAX_OUTPUT                          5
#endif
#ifdef		IO2C1131
	#define     MAX_OUTPUT                          6
#endif
#ifdef		S2C4111
	#define     MAX_OUTPUT                          0
#endif
#ifdef      IO2C1122
    #define     MAX_OUTPUT                          8
#endif
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
struct      tagOUTPUT_HWCONF
{
    DWORD_T         dwPeriph;
    DWORD_T         dwPort;
    BYTE_T          byPin;
};

/*---------------------------------------------------------------------------+
|           Operations                                                       |
+---------------------------------------------------------------------------*/


/*===========================================================================+
|           Externals                                                        |
+===========================================================================*/
extern      DEV_ENGINE_T            g_Output_Engine;

#ifdef __cplusplus
}
#endif


#endif