/*==========================================================================+
|  File     : Function_iNet.h                                               |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : ZZYuan                                                        |
|  Version  : v1.00                                                         |
|  Creation : 2018Äê11ÔÂ05ÈÕ                                                                                                                                                                               |
|  Revision :                                                               |
+==========================================================================*/
#ifndef FUNCTION_INET_H_
#define FUNCTION_INET_H_
#include  "studioprotocol.h"
#include  "taskinet.h"

extern BOOL_T  Funcblock_iNet_In(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern BOOL_T  Funcblock_iNet_Out(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);

#endif /* FUNCTION_INET_H_ */
