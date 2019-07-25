/*===========================================================================+
|  Function : AD Driver                                                     |
|  Task     : AD Driver Header File                                         |
|----------------------------------------------------------------------------|
|  Compile  : CCS6.1 -                                                       |
|  Link     : CCS6.1 -                                                       |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : Wain.Wei                                                       |
|  Version  : V1.00                                                          |
|  Creation : 28/03/2011                                                     |
|  Revision :                                                                |
+===========================================================================*/
#ifndef     D___AD
#define     D___AD

#ifdef __cplusplus
extern "C" {
#endif
#include    "kernel/include/device.h"
#include	"config.h"
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/
#define     MAX_AD                          4
#define     AD_CMD_SETSCANTIME              1
#define     AD_CMD_SETFILTERCOUNT           2
#define     AD_FILTER_MAX                   50
#ifdef	S2C4111
	#define     SELECT                          2
#else
	#define     SELECT                          0
#endif
#define     DATAWIDTH                       16
#define     BITRATE_AD                      10000000
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
struct      tagAD_DATA
{
    DWORD_T          wSetFilterCount;
    DWORD_T          awValue[MAX_AD];
    DWORD_T          awRealFilterCount[MAX_AD];
};


struct      tagAD_HWCONF
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
extern      DEV_ENGINE_T            g_Ad_Engine;



#ifdef __cplusplus
}
#endif 


#endif
