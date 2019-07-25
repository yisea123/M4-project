/*===========================================================================+
|  Function : Input Driver                                                   |
|  Task     : Input Driver Header File                                       |
|----------------------------------------------------------------------------|
|  Compile  : CCS6.1 -                                                       |
|  Link     : CCS6.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Jia                                                            |
|  Version  : V1.00                                                          |
|  Creation : 28/03/2011                                                     |
|  Revision :                                                                |
+===========================================================================*/
#ifndef     D_INPUT_H
#define     D_INPUT_H

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
	#define     MAX_INPUT                           2
#endif

#if (defined IO2C1111) || (defined IO2C1112)
	#define     MAX_INPUT                           5
#endif

#ifdef		IO2C1131
	#define     MAX_INPUT                           8
#endif

#ifdef		S2C4111
	#define     MAX_INPUT                           0
#endif
#ifdef   IO2C1122
    #define     MAX_INPUT                           8
#endif

#define     INPUT_CMD_SETSCANTIME               1
#define     INPUT_CMD_SETFILTERCOUNT            2

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
struct      tagINPUT_DATA
{
    WORD_T          wSetFilterCount;
    BYTE_T          abyValue[MAX_INPUT];
    WORD_T          awRealFilterCount[MAX_INPUT];
};

struct      tagINPUT_HWCONF
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
extern      DEV_ENGINE_T            g_Input_Engine;

#ifdef __cplusplus
}
#endif


#endif
