/*==========================================================================+
|  File     : task_Runtime.h                                                |
|  Function :                                                               |
|  Task     :                                                               |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : zhao                                                          |
|  Version  : V1.00                                                         |
|  Creation : 2015/11/13                                                    |
|  Revision :                                                               |
+==========================================================================*/

#ifndef D__TASK_RUNTIME_H_
#define D__TASK_RUNTIME_H_

#include    "string.h"
#include	<stdlib.h>
#include    "driver/include/_output.h"
#include    "driver/include/_input.h"
#include    "kernel/include/task.h"
#include    "studioprotocol.h"
#include    "database.h"
#include    "Calculations.h"


/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#define         MAXNUM         50
#define         COMPONENTCNTS  15

#define			InitTask		0
#define			NormalTask		1
/*---------------------------------------------------------------------------+
|           Type Definition	                                                 |
+---------------------------------------------------------------------------*/

typedef	struct	tagIDB_DATA
{
	WORD_T	IdbValue;
} IDB_DATA;

typedef	struct	BKPT_Type
{
	WORD_T	SerialNo;
	WORD_T	ScanCntOffset;
	DWORD_T ScanCnt;
} BKPT_T;

typedef union	tagIDBVALUE
{
    DWORD_T			dwData;
	char 			acData[2];
	WORD_T			wData;
	char 			cData;
	int				nData;
	long			lData;
	float 			fData;
	char			*pcData;
} IDBVALUE;

typedef struct inputData_Type
{
	 WORD_T  inputID[8];
	 WORD_T	 inputValue[8];
}INPUT_T ;

typedef struct inputCount_Type
{
	BYTE_T  inputcount;
	BYTE_T  inputscanStatus;

}INPUTCOUNT_T ;

#pragma pack(push)
#pragma pack(1)
typedef struct Cell_Type
{
	BYTE_T  byColumn;
	WORD_T  wRow;

}CELLTYPE_T ;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
typedef   struct    RuntimeData_Type
{
//  BYTE_T			     inputStatus[8];
//	BYTE_T  		     outputStatus;
	DBVALUE  		     outputValue;
	BYTE_T			     scanStatus;
	BYTE_T               inputScanCount;
	DWORD_T              Parameter[2];
	WORD_T				 DataType; //对应DBVALUE编号
	WORD_T               index;
}RUNTIMEDATATYPE_T;
#pragma pack(pop)


#pragma pack(push)
#pragma pack(1)
typedef   struct   DesigntimeData_Type
{
	WORD_T			     serialNo;			//表示在studio工具上扫描的序号
	WORD_T			     FunctionBlockID;	//表示每个FB特定的ID
	BYTE_T	             InputCounts;		//表示该FB有几路input
    WORD_T 		         InputsID[5];		//表示各路input连接的前一FB的serialNo
	CELLTYPE_T			 CellType;			//行列属性
	BYTE_T               ComponentNo;		//表示在studio中该FB重复使用的次数
	DWORD				 PrivateParam;		//私有属性           (WORD个数+WORD起始)
}DESIGNTIMEDATATYPE_T;
#pragma pack(pop)

typedef   struct   CommData_Type
{
	DWORD				 DataID;			//用户选中的DataID
	DBVALUE  		     Value;				//DataID对应的值
}COMMDATATYPE_T;

#pragma pack(push)
#pragma pack(1)
typedef   struct   GlobalVariable_Type
{
	WORD				 GVID;				//全局变量ID
	BYTE  		     	 GVType;			//全局变量类型
	DBVALUE  		     Value;				//全局变量值
}GLOBALVARIABLETYPE_T;
#pragma pack(pop)

typedef  struct tagIOBJECT_DATA
{
	WORD_T  IobjectValue;
} IOBJECT_DATA;

/*---------------------------------------------------------------------------+
|           external  Variable                                               |
+---------------------------------------------------------------------------*/
extern		RUNTIMEDATATYPE_T    					g_RunTime_ObjectData[];
extern		DESIGNTIMEDATATYPE_T  		    		g_ALLObjectListData[];
extern      DESIGNTIMEDATATYPE_T					g_DesignTime_ObjectListData[][MAXNUM];
extern      COMMDATATYPE_T  		    			g_ALLCommDataList[];
extern      GLOBALVARIABLETYPE_T  		    		g_ALLGlobalVariableList[];
extern      WORD_T									g_DesignTime_ObjectParam[];
extern      WORD_T                                  InputValue[MAXNUM];
extern      BYTE_T                                  g_RuntimeFlag[];

extern      BKPT_T									g_tBkptSerialNo[5];
extern      BYTE_T									g_cBKPTCnt;
extern      WORD_T									u_wNow_FunctionBlockNum;
extern      DWORD_T									g_dwBKPTScanCnt;
extern      WORD_T                                  g_wTotal_FunctionBlockNum;
extern      WORD_T                                  g_wNext_FunctionBlockNum[];
extern      DWORD_T									g_dwRunFBsCnt[][MAXNUM];

extern      TASK        task_CaptureData;
extern      HTASK       g_htask_Calculations[];
extern      HTASK       g_htask_Calculations_Debug[];
extern      TASK    	TaskDebug[];
extern      BYTE_T		g_cCurrentDebugTask;
extern      BYTE_T		g_cStepNexttaskFalg;
extern      WORD_T      g_wTotal_CommDataNum;
extern      WORD_T      g_wTotal_GlobalVariableNum;
/*---------------------------------------------------------------------------+
|           external                                                         |
+---------------------------------------------------------------------------*/
void        Create_Idataconf    ( void      );
void        RunFunctionBlock    (void *pParam);
void        Create_Idataconf_Debug    ( void      );
void        RunFunctionBlock_Debug    (void *pParam);
void 		InitObjectListStatus( BYTE_T number );
void        Init_FunctionBlock   (void);
void        InitObjectListData(void);

/*============================================================================================+
|                                     end                                                     |
+============================================================================================*/

#endif
