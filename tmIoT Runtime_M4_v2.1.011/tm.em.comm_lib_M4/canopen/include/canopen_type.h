/*===========================================================================+
|  Function : CANOPEN TYPE                                                   |
|  Task     : CANOPEN TYPE Header File                                       |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   :                                                                |
|  Version  : V1.00                                                          |
|  Creation : 2016.3.5                                                       |
|  Revision :                                                                |
+===========================================================================*/
#ifndef D__CANOPEN_TYPE
#define D__CANOPEN_TYPE

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define CANOPEN_BYTE_SIZE       1

/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
typedef char                    CANOPEN_INT8_T;
typedef unsigned char           CANOPEN_UINT8_T;
typedef short                   CANOPEN_INT16_T;
typedef unsigned short          CANOPEN_UINT16_T;
typedef long                    CANOPEN_INT32_T;
typedef unsigned long           CANOPEN_UINT32_T;

typedef int                     CANOPEN_INT_T;
typedef float                   CANOPEN_FLOAT_T;

typedef CANOPEN_INT8_T          CANOPEN_CHAR_T;
typedef CANOPEN_UINT8_T         CANOPEN_BYTE_T;
typedef CANOPEN_INT16_T         CANOPEN_SHORT_T;
typedef CANOPEN_UINT16_T        CANOPEN_WORD_T;
typedef CANOPEN_UINT16_T        CANOPEN_BOOL_T;
typedef CANOPEN_INT32_T         CANOPEN_LONG_T;
typedef CANOPEN_UINT32_T        CANOPEN_DWORD_T;

typedef CANOPEN_WORD_T          CANOPEN_OFFSET_T;
typedef CANOPEN_WORD_T          CANOPEN_SIZE_T;
typedef CANOPEN_DWORD_T         CANOPEN_ERR_T;

typedef CANOPEN_DWORD_T         CANOPEN_TICK_T;
typedef CANOPEN_DWORD_T         CANOPEN_DEVICE_T;

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/
#define Canopen_Memset memset
#define Canopen_Memcpy memcpy

#define CANOPEN_ASSERT(Cond) while (!(Cond))

#define CANOPEN_NULL            (void *)0

#define CANOPEN_TRUE            (1==1)
#define CANOPEN_FALSE           (1==0)

#define CANOPEN_INLINE          static __inline

/*===========================================================================+
|           Class declaration                                                |
+===========================================================================*/

/*===========================================================================+
|           Attributes                                                       |
+===========================================================================*/

/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/

/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/

/*===========================================================================+
|           Externs                                                          |
+===========================================================================*/


#ifdef __cplusplus
}
#endif


#endif
