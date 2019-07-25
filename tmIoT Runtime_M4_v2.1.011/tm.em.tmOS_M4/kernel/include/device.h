/*==============================================================================+
|  Function : Device                                                            |
|  Task     : Device Header File                                                |
|-------------------------------------------------------------------------------|
|  Compile  :                                                   |
|  Link     :                                               	|
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : Wain                                                              |
|  Version  : V1.00                                                             |
|  Creation : 2013/07/23                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#ifndef     D__DEVICE
#define     D__DEVICE

#ifdef __cplusplus
extern "C" {
#endif

#include    "type.h"

/*==============================================================================+
|           Constant                                                            |
+==============================================================================*/
// Device max number
#define     DEVICE_MAX_NBR                      48

// Device flag
#define     DEVICE_CREATED                      0x1
#define     DEVICE_OPENED                       0x2

// Device null handle
#define     DEVICE_HANDLE_NULL                  0
/*==============================================================================+
|           Type definition                                                     |
+==============================================================================*/
typedef     struct tagDEV_ENGINE                DEV_ENGINE_T;
typedef     struct tagDEVICE                    DEVICE_T;

struct  tagDEV_ENGINE
{
    ERROR_T         (*Open  )   (   void *);
    void            (*Close )   (   void  );
    SIZE_T          (*Read  )   (   BYTE_T *, SIZE_T, OFFSET_T );
    SIZE_T          (*Write )   (   BYTE_T *, SIZE_T, OFFSET_T );
    SIZE_T          (*Control)  (   BYTE_T  , void * );
    
    void            *pDevData;
};


struct  tagDEVICE
{
    BYTE_T                  byFlag;
    DEV_ENGINE_T            *pEngine;
    void                    *pDevData;
};
/*==============================================================================+
|           Macro definition                                                    |
+==============================================================================*/

/*==============================================================================+
|           Class declaration -                                   		    |
+==============================================================================*/
/*------------------------------------------------------------------------------+
|           Attributes                                                          |
+------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------+
|           Constructor and destructor                                          |
+------------------------------------------------------------------------------*/
HANDLE_T                    Device_Create           (   DEV_ENGINE_T            *pEngine    );
ERROR_T                     Device_Destroy          (   HANDLE_T                Handle      );
/*------------------------------------------------------------------------------+
|           Operations                                                          |
+------------------------------------------------------------------------------*/
ERROR_T                     Device_Open             (   HANDLE_T                Handle,
                                                        void                    *pArg       );
void                        Device_Close            (   HANDLE_T                Handle      );

SIZE_T                      Device_Read             (   HANDLE_T                Handle,
                                                        BYTE_T                  *pbyBuf,
                                                        SIZE_T                  Size,
                                                        OFFSET_T                Offset      );
SIZE_T                      Device_Write            (   HANDLE_T                Handle,
                                                        BYTE_T                  *pbyBuf,
                                                        SIZE_T                  Size,
                                                        OFFSET_T                Offset      );                                                
ERROR_T                     Device_Control          (   HANDLE_T                Handle,
                                                        BYTE_T                  byCmd,
                                                        void                    *pArg       );


/*==============================================================================+
|           Externs                                                             |
+==============================================================================*/




#ifdef __cplusplus
}
#endif 


#endif
