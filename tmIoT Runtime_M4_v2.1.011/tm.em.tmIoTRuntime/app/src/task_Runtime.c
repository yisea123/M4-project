/*==========================================================================+
|  Function : task_Runtime.c                                                |
|  Task     : Source File                                                   |
|---------------------------------------------------------------------------|
|  Compile  :                                                               |
|  Link     :                                                               |
|  Call     :                                                               |
|---------------------------------------------------------------------------|
|  Author   : YW                                                            |
|  Version  : V2.00                                                         |
|  Creation : 2018/01/19                                                    |
|  Revision :                                                               |
+==========================================================================*/

#include    "task_Runtime.h"
/*===========================================================================+
|           Content                                                          |
+===========================================================================*/
/*===========================================================================+
|          Global variable                                                   |
+===========================================================================*/
RUNTIMEDATATYPE_T               g_RunTime_ObjectData[MaxTask*MAXNUM];
DESIGNTIMEDATATYPE_T  		    g_ALLObjectListData[MaxTask*MAXNUM];
DESIGNTIMEDATATYPE_T  		    g_DesignTime_ObjectListData[MaxTask][MAXNUM];

COMMDATATYPE_T  		    	g_ALLCommDataList[MaxCommData];
GLOBALVARIABLETYPE_T  		    g_ALLGlobalVariableList[MaxGlobalVariable];

WORD_T							g_DesignTime_ObjectParam[MaxTask];

HTASK                           g_htask_Calculations[MaxTask];
HTASK                           g_htask_Calculations_Debug[MaxTask];
BYTE_T                          g_RuntimeFlag[MaxTask];

BKPT_T							g_tBkptSerialNo[5];//最大允许设置5个断点
BYTE_T							g_cBKPTCnt = 0;
DWORD_T							g_dwBKPTScanCnt;
WORD_T                          g_wTotal_FunctionBlockNum = 0;

WORD_T                          g_wTotal_CommDataNum = 0;
WORD_T                          g_wTotal_GlobalVariableNum = 0;

WORD_T							u_wNow_FunctionBlockNum = 0;

WORD_T                          g_wNext_FunctionBlockNum[MaxTask];

TASK    						TaskDebug[MaxTask];
BYTE_T							g_cCurrentDebugTask;
BYTE_T							g_cStepNexttaskFalg;
DWORD_T							g_dwRunFBsCnt[MaxTask][MAXNUM];
/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void            Create_Idataconf    ( )
{
	WORD_T  i;
    TASK    Task;

	for(i = 0; i < g_cTaskTotalCount; i++)
	{
		g_DesignTime_ObjectParam[i] = i;

		Task.wType = TASK_WHILELOOP;
	    Task.period.tCounter = 0;
	    Task.period.tTimer = (TINT)TaskData[i].Cycle;
	    Task.pRun = RunFunctionBlock;
	    Task.pParam = &g_DesignTime_ObjectParam[i];
	    g_htask_Calculations[i] = Create_Task( &Task  , 0 );
	}
	Init_FunctionBlock();
}

/*---------------------------------------------------------------------------+
|           Constructor and destructor                                       |
+---------------------------------------------------------------------------*/
void            Create_Idataconf_Debug    ( )
{
	WORD_T  i;

	for(i = 0; i < g_cTaskTotalCount; i++)
	{
		g_DesignTime_ObjectParam[i] = i;

		TaskDebug[i].wType = TASK_WHILELOOP;
		TaskDebug[i].period.tCounter = 0;
		TaskDebug[i].period.tTimer = (TINT)TaskData[i].Cycle;
		TaskDebug[i].pRun = RunFunctionBlock_Debug;
		TaskDebug[i].pParam = &g_DesignTime_ObjectParam[i];
	    g_htask_Calculations[i] = Create_Task( &TaskDebug[i]  , 0 );
	}
	Init_FunctionBlock();
}

/*===========================================================================+
|Name:        Init_FunctionBlock                                             |
|Description: Initial the functionBlocak's functionBlockID, The index will be|
|             assigned a right Value According to the functionBlockID        |
+===========================================================================*/
void  Init_FunctionBlock( void )
{
	WORD_T     i,j,k;
	WORD_T	   wTaskFBNum;

	for(k = 0; k < g_cTaskTotalCount; k++)
	{
		wTaskFBNum = TaskData[k].EndComponentNo - TaskData[k].StartComponentNo + 1;
		for(i = 0; i < wTaskFBNum; i++)
		{
			if(0x2100 == (g_DesignTime_ObjectListData[k][i].FunctionBlockID&0xff00))   //Digital Input Point
			{
				g_RunTime_ObjectData[i + TaskData[k].StartComponentNo].index = 0;
			}
			else if(0x2200 == (g_DesignTime_ObjectListData[k][i].FunctionBlockID&0xff00))//Analog Input Point
			{
				g_RunTime_ObjectData[i + TaskData[k].StartComponentNo].index = 1;
			}
			else if(0x4100 == (g_DesignTime_ObjectListData[k][i].FunctionBlockID&0xff00))	//Digital output Point
			{
				g_RunTime_ObjectData[i + TaskData[k].StartComponentNo].index = 2;
			}
			else if(0x4200 == (g_DesignTime_ObjectListData[k][i].FunctionBlockID&0xff00))//Analog output Point
			{
				g_RunTime_ObjectData[i + TaskData[k].StartComponentNo].index = 3;
			}

			else
			{
				for( j = 0; j < BLOCK_TOTAL_NUM; j++)
				{
					if(g_DesignTime_ObjectListData[k][i].FunctionBlockID == g_FunctionBlockAttr[j].dwID) //others
					{
						g_RunTime_ObjectData[i + TaskData[k].StartComponentNo].index = j;
					}
				}
			}
//			g_RuntimeFlag[k] = 1;
		}
		g_RuntimeFlag[k] = 1;
	}
}

/*===========================================================================+
|Name:         RunFunctionBlock                                              |
|Description:  Runtime Processor,  Scan all the FunctionBlock and refresh the|
|              data of functionBlocks                                        |
+===========================================================================*/
void RunFunctionBlock(void *pParam)
{
	WORD_T					i;
	WORD_T					scanObjectCount = 0,inputSourceID;
	RUNTIMEDATATYPE_T	    inputSource;
	BYTE_T					m;
	WORD_T  *p;
	p = pParam;
	WORD_T	index = *p;
	WORD_T	wTotal_FunctionBlockNum = TaskData[index].EndComponentNo - TaskData[index].StartComponentNo + 1;

	if(g_RuntimeFlag[index] == 1)
	{
	    InitObjectListStatus(index); //initial the runtime status
		while(scanObjectCount < wTotal_FunctionBlockNum)
		{
			for(i = 0; i < wTotal_FunctionBlockNum; i++)
			{
				if(g_RunTime_ObjectData[i + TaskData[index].StartComponentNo].scanStatus == 0)
				{
					g_RunTime_ObjectData[i + TaskData[index].StartComponentNo].inputScanCount = 0;
					for(m = 0; m < g_DesignTime_ObjectListData[index][i].InputCounts; m++)
					{
						inputSourceID = g_DesignTime_ObjectListData[index][i].InputsID[m];
						inputSource =  g_RunTime_ObjectData[inputSourceID];
						if(inputSource.scanStatus == 1)
						{
//							g_RunTime_ObjectData[index][i].inputStatus[m] = 1;
							g_RunTime_ObjectData[i + TaskData[index].StartComponentNo].inputScanCount++;
						}
					}
				}
			    if(g_DesignTime_ObjectListData[index][i].InputCounts == g_RunTime_ObjectData[i + TaskData[index].StartComponentNo].inputScanCount)
			    {
					g_FunctionBlockAttr[ g_RunTime_ObjectData[i + TaskData[index].StartComponentNo].index ].DealFunc( (BYTE_T*)&g_DesignTime_ObjectListData[index][i], (BYTE*)&g_RunTime_ObjectData[0]);
					scanObjectCount++;
					g_RunTime_ObjectData[i + TaskData[index].StartComponentNo].scanStatus = 1;
			    }
//				scanTimes++;
			}
		}
    }

	if(TaskData[index].Class == InitTask)
	{
		Destroy_Task(g_htask_Calculations[index]);
	}
}
/*===========================================================================+
|Name:         RunFunctionBlock_Debug                                        |
|Description:  Runtime Processor in Debug Mode,  Scan all the FunctionBlock  |
|              and refresh the data of functionBlocks,And Deal the Debug Info|
+===========================================================================*/
void RunFunctionBlock_Debug(void *pParam)
{
//	WORD_T					i;
//	WORD_T					scanObjectCount = 0,inputSourceID;
//	RUNTIMEDATATYPE_T	    inputSource;
//	BYTE_T					m,j;
//	BYTE_T					scanBreakFlag = 0;
//	WORD_T  *p;
//	p = pParam;
//	WORD_T	index = *p;
//	WORD_T	wTotal_FunctionBlockNum = TaskData[index].EndComponentNo - TaskData[index].StartComponentNo + 1;
//
//	g_cCurrentDebugTask = index;
//
//	if(g_RuntimeFlag[index] == 1)
//	{
//		if(g_cSendStartDebugFBNum == 0) //刚进入debug模式，应该向工具发送第一个元件的地址
//		{
//			SendScanSerialNo(TaskData[index].StartComponentNo);
//			g_cSendStartDebugFBNum = 1;
//		}
//
//		if(g_wNext_FunctionBlockNum[index] == 0)
//			InitObjectListStatus(index);//initial the runtime status
//
//		//不论是全速还是单步，都至少执行一个FB
//		if(g_RunTime_ObjectData[index][g_wNext_FunctionBlockNum[index]].scanStatus == 0)
//		{
//			g_RunTime_ObjectData[index][g_wNext_FunctionBlockNum[index]].inputScanCount = 0;
//			for(m = 0; m < g_DesignTime_ObjectListData[index][g_wNext_FunctionBlockNum[index]].InputCounts; m++)
//			{
//				inputSourceID = g_DesignTime_ObjectListData[index][g_wNext_FunctionBlockNum[index]].InputsID[m];
//				inputSource =  g_RunTime_ObjectData[index][inputSourceID];
//				if(inputSource.scanStatus == 1)
//				{
//					g_RunTime_ObjectData[index][g_wNext_FunctionBlockNum[index]].inputScanCount++;
//				}
//			}
//		}
//		if(g_DesignTime_ObjectListData[index][g_wNext_FunctionBlockNum[index]].InputCounts == g_RunTime_ObjectData[index][g_wNext_FunctionBlockNum[index]].inputScanCount)
//		{
//			g_FunctionBlockAttr[g_RunTime_ObjectData[index][g_wNext_FunctionBlockNum[index]].index].DealFunc( (BYTE_T*)&g_DesignTime_ObjectListData[index][g_wNext_FunctionBlockNum[index]], (BYTE*)&g_RunTime_ObjectData[index]);
//			scanObjectCount++;
//			g_RunTime_ObjectData[index][g_wNext_FunctionBlockNum[index]].scanStatus = 1;
//			g_dwRunFBsCnt[index][g_wNext_FunctionBlockNum[index]]++;
//		}
//
//		g_wNext_FunctionBlockNum[index]++;
//
//		//如果是单步调试
//		if(g_wStepRunflag)
//		{
//			if(g_wNext_FunctionBlockNum[index] != wTotal_FunctionBlockNum)
//			{
//				g_wStepRunflag = 1;
//			}
//			else
//			{
//				g_wStepRunflag = 0;
//				g_cSendStartDebugFBNum = 0;
//				g_wNext_FunctionBlockNum[index] = 0;
//				g_cStepNexttaskFalg = 1;
//				return;
//			}
//			SendScanSerialNo(TaskData[index].StartComponentNo + g_wNext_FunctionBlockNum[index]);
//		}
//
//		//如果是全速调试
//		if(g_wRunFlag)
//		{
//			//至少执行完一个FB后，再判断是否有断点
//			for(i = g_wNext_FunctionBlockNum[index]; i < wTotal_FunctionBlockNum; i++)
//			{
//				//判断该FB是否有设置有断点，FB运行次数大于断点初值
//				for( j = 0; j < g_cBKPTCnt; j++)
//				{
//					if((i == g_tBkptSerialNo[j].SerialNo) && (g_dwRunFBsCnt[index][i] - g_tBkptSerialNo[j].ScanCnt >= g_tBkptSerialNo[j].ScanCntOffset))//modify 20170408
//					{
//						g_wNext_FunctionBlockNum[index] = i;
//						scanBreakFlag  = 1;
//						break;
//					}
//				}
//				//有断点则直接退出runtime task，并发送FB serial number给工具
//				if(scanBreakFlag)
//				{
//					g_wStepFlag = 0;
//					g_wRunFlag = 0;
////					g_wNext_FunctionBlockNum[index] %= g_wTotal_FunctionBlockNum;
//					SendScanSerialNo(TaskData[index].StartComponentNo + g_wNext_FunctionBlockNum[index]);
//					break;
//				}
//
//				//无断点则执行FB
//				if(g_RunTime_ObjectData[index][i].scanStatus == 0)
//				{
//					g_RunTime_ObjectData[index][i].inputScanCount = 0;
//					for(m = 0; m < g_DesignTime_ObjectListData[index][i].InputCounts; m++)
//					{
//						inputSourceID = g_DesignTime_ObjectListData[index][i].InputsID[m];
//						inputSource =  g_RunTime_ObjectData[index][inputSourceID];
//						if(inputSource.scanStatus == 1)
//						{
//							g_RunTime_ObjectData[index][i].inputScanCount++;
//						}
//					}
//				}
//				if(g_DesignTime_ObjectListData[index][i].InputCounts == g_RunTime_ObjectData[index][i].inputScanCount)
//				{
//					g_FunctionBlockAttr[g_RunTime_ObjectData[index][g_wNext_FunctionBlockNum[index]].index].DealFunc( (BYTE_T*)&g_DesignTime_ObjectListData[index][g_wNext_FunctionBlockNum[index]], (BYTE*)&g_RunTime_ObjectData[index]);
//					scanObjectCount++;
//					g_RunTime_ObjectData[index][i].scanStatus = 1;
//					g_dwRunFBsCnt[index][g_wNext_FunctionBlockNum[index]]++;
//				}
//			}
//		}
//
//		if(g_wNext_FunctionBlockNum[index] == wTotal_FunctionBlockNum)
//		{
//			g_wNext_FunctionBlockNum[index] = 0;
//			g_wStepRunflag = 0;
//		}
//    }
}
/*===========================================================================+
| Name:        InitObjectListStatus                                          |
| Description: Initial all the functionBlock status during the executive of   |
|              Runtime                                                       |
+===========================================================================*/
void InitObjectListStatus( BYTE_T number )
{
	WORD_T i;

	for(i = 0; i < TaskData[number].EndComponentNo - TaskData[number].StartComponentNo + 1; i++)
	{
	  g_RunTime_ObjectData[TaskData[number].StartComponentNo + i].scanStatus = 0;
	  g_RunTime_ObjectData[TaskData[number].StartComponentNo + i].inputScanCount = 0;
	}
}

/*===========================================================================+
| Name:        InitObjectListData                                            |
| Description: Initial all the functionBlock output Value                    |
+===========================================================================*/

void InitObjectListData( void )
{
    WORD_T  i;

    for(i = 0; i < MaxTask*MAXNUM; i++)
    {
    	g_RunTime_ObjectData[i].outputValue.dwData = 0;
    }
}

/*===========================================================================+
|Name:         ComponenttoTaskData                                           |
|Description:  Copy g_ALLObjectListData to g_DesignTime_ObjectListData, 	 |
|			   g_ALLObjectListData is Component Data			             |
|			   g_DesignTime_ObjectListData is Task Data			             |
+===========================================================================*/
void	ComponenttoTaskData( void )
{
	WORD_T     i,j;
	WORD_T	   wTaskFBNum;

	for(i = 0; i < g_cTaskTotalCount; i++)
	{
		wTaskFBNum = TaskData[i].EndComponentNo - TaskData[i].StartComponentNo + 1;
		for(j = 0; j < wTaskFBNum; j++)
		{
			memcpy(&g_DesignTime_ObjectListData[i][j], &g_ALLObjectListData[TaskData[i].StartComponentNo + j], sizeof(DESIGNTIMEDATATYPE_T));
		}
	}
}
/*--------------------------------------------------------------------------------------------+
|                              END                                                             |
+---------------------------------------------------------------------------------------------*/
