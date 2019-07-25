/*===========================================================================+
|  Function : task_IO_Acquisition.h	                                         |
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

#ifndef D__TASK_IO_ACQUISITION_H
#define	D__TASK_IO_ACQUISITION_H


//#ifndef D__TASKAD
//#define D__TASKAD

#ifdef __cplusplus
extern "C" {
#endif


#include    "kernel/include/type.h"
#include    "kernel/include/device.h"
#include    "kernel/include/task.h"
#include    "driver/include/_ad.h"
#include    "driver/include/_output.h"
#include    "driver/include/_input.h"
#include    "driver/include/_da.h"
#include    "database.h"
#include    "driver/include/_timer.h"
#include    "config.h"
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
#define      PRIORITY_IOACQUISITION   3
/*===========================================================================+
|            Operations                                                      |
+============================================================================*/
/*===========================================================================+
|            Attributes                                                      |
+============================================================================*/

/*===========================================================================+
|            External Veriable                                               |
+===========================================================================*/

extern    TASK	       task_IO_Acquisition ;
extern    HTASK        g_htask_IO_Acquisition;
extern   HANDLE_T      g_hInput;
extern   HANDLE_T      g_hAddress;
extern   HANDLE_T      g_hOutput;
extern   HANDLE_T      g_hAd;
extern   HANDLE_T      g_hDa;

/*===========================================================================+
|           External                                                         |
+===========================================================================*/
void                        Create_TaskIOAcquisition (   void       );
static     void             IO_Acquisition_task( void *pParam );
static     void             input_test    ( void *pParam );
static     void             output_test   ( void *pParam );
static     void             Ad_Test       ( void *pParam );
static     void             Da_Test       ( void *pParam );
static     void             SpecileAD_Test( void *pParam );

extern     void             Set_CardInitData(void);


#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
