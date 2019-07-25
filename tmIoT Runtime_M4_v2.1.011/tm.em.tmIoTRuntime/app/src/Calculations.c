/*==============================================================================+
|  Function : calculation.c                                                     |
|  Task     : Source File                                                       |
|-------------------------------------------------------------------------------|
|  Compile  :                                                                   |
|  Link     :                                                                   |
|  Call     :                                                                   |
|-------------------------------------------------------------------------------|
|  Author   : YW                                                                |
|  Version  : V2.00                                                             |
|  Creation : 2018/01/19                                                        |
|  Revision :                                                                   |
+==============================================================================*/
#include    "Calculations.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/
/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
/*===========================================================================+
|           Helpers  Implement                                               |
+===========================================================================*/
/*===========================================================================+
|           Global variable                                                  |
+===========================================================================*/
DBVALUE               DbValue[8];
/*===========================================================================+
|           Global variable                                                  |
+===========================================================================*/
const   FUNCBLOCK		g_FunctionBlockAttr[] =
{
//输入点
     {FUNCBLOCK_INPUT_DIGITALPOINT                                      ,Funcblock_Input_DigitalPoint                  },       //0x21xx
     {FUNCBLOCK_INPUT_ANALOGPOINT                                       ,Funcblock_Input_AnalogPoint                   },       //0x22xx
//输出点
     {FUNCBLOCK_OUTPUT_DIGITALPOINT                                     ,Funcblock_Output_DigitalPoint                 },       //ox41xx
     {FUNCBLOCK_OUTPUT_ANALOGPOINT                                      ,Funcblock_Output_AnalogPoint                  },       //ox42xx
//Data输入点
//	 {FUNCBLOCK_INPUT_DATABASE_SINGLEPOINT	                     	    ,Funcblock_Input_DataBase_SinglePoint          },  	    //0x2301
//	 {FUNCBLOCK_INPUT_NET_SINGLEPOINT	                    	        ,Funcblock_Input_Net_SinglePoint               },		//0x2401
//Data输出点
//	 {FUNCBLOCK_OUTPUT_DATABASE_SINGLEPOINT                             ,Funcblock_Output_DataBase_SinglePoint         },       //0x4301
//	 {FUNCBLOCK_OUTPUT_NET_SINGLEPOINT    	                    	    ,Funcblock_Output_Net_SinglePoint              },		//0x4401
//输入模块
//   {FUNCBLOCK_INPUTMODULE_DOORMAGNETIC                                ,Funcblock_InputModule_DoorMagnetic            },       //0x1302
//   {FUNCBLOCK_INPUTMODULE_HUMANBODYSENSOR                             ,Funcblock_InputModule_HumanBodySensor         },       //0x1302
//	 {FUNCBLOCK_INPUTMODULE_SWITCH   	                    			,Funcblock_InputModule_Switch                  },		//0x1304
//	 {FUNCBLOCK_INPUTMODULE_NETINPUT	                    	    	,Funcblock_InputModule_NetInput        	   	   },		//0x1201
//	 {FUNCBLOCK_INPUTMODULE_DATABASESINGLEINPUT	                    	,Funcblock_InputModule_DataBase_SingleInput    },		//0x1101
//	 {FUNCBLOCK_INPUTMODULE_DATABASEMULTIINPUT	                    	,Funcblock_InputModule_DataBase_MultiInput     },		//0x1102
	 {FUNCBLOCK_INPUTMODULE_DIGITAL                                     ,Funcblock_InputModule_Digital                 },		//0x1701
	 {FUNCBLOCK_INPUTMODULE_ANALOG                                      ,Funcblock_InputModule_Analog                  },		//0x1702
	 {FUNCBLOCK_INPUTMODULE_GLOBALVARIABLE                              ,Funcblock_InputModule_GlobalVariable          },		//0x1801
//输出模块
//	 {FUNCBLOCK_OUTPUTMODULE_REPLY       	                    	    ,Funcblock_OutputModule_Reply                  },		//0x5101
//	 {FUNCBLOCK_OUTPUTMODULE_ORDINARYLAMP       	                    ,Funcblock_OutputModule_OrdinaryLamp           },		//0x5501
//	 {FUNCBLOCK_OUTPUTMODULE_ALERT                                      ,Funcblock_OutputModule_Alert                  },       //0x5503
//	 {FUNCBLOCK_OUTPUTMODULE_NETOUTPUT    	                    	    ,Funcblock_OutputModule_NetOutput              },		//0x5102
//	 {FUNCBLOCK_OUTPUTMODULE_INET       	                    	    ,Funcblock_OutputModule_iNet                   },		//0x5203
//	 {FUNCBLOCK_INPUTMODULE_ANALOGSPECIALCOLLECTOR	                    ,Funcblock_InputModule_AnalogSpecialCollector  },  	    //0x1601
//	 {FUNCBLOCK_INPUTMODULE_ANALOGCOLLECTOR	                            ,Funcblock_InputModule_AnalogCollector         },  	    //0x1602
//	 {FUNCBLOCK_OUTPUTMODULE_DATABASE    	                    	    ,Funcblock_OutputModule_DataBase               },		//0x5101
	 {FUNCBLOCK_OUTPUTMODULE_DIGITAL    	                    	    ,Funcblock_OutputModule_Digital                },		//0x5801
	 {FUNCBLOCK_OUTPUTMODULE_ANALOG    	                    	        ,Funcblock_OutputModule_Analog                 },		//0x5802
	 {FUNCBLOCK_OUTPUTMODULE_GLOBALVARIABLE                             ,Funcblock_OutputModule_GlobalVariable         },		//0x5901
//逻辑功能模块
	 {FUNCBLOCK_LOGIC_AND	                    				        ,Funcblock_Logic_And                           },  	    //ox3101
	 {FUNCBLOCK_LOGIC_OR	                    				        ,Funcblock_Logic_Or             			   },		//0x3102
	 {FUNCBLOCK_LOGIC_NOT	                     				        ,Funcblock_Logic_Not           		           },       //0x3103
	 {FUNCBLOCK_LOGIC_XOR	                     				        ,Funcblock_Logic_Xor           		           },       //0x3104

	 {FUNCBLOCK_ARITHMETIC_ADD	                     				    ,Funcblock_Arithmetic_Add           		   },       //0x3201
	 {FUNCBLOCK_ARITHMETIC_MINUS	                     				,Funcblock_Arithmetic_Minus           		   },       //0x3202
	 {FUNCBLOCK_ARITHMETIC_MULTIPLY	                     				,Funcblock_Arithmetic_Multiply           	   },       //0x3203
	 {FUNCBLOCK_ARITHMETIC_DEVIDE	                     				,Funcblock_Arithmetic_Devide           		   },       //0x3204

	 {FUNCBLOCK_TIMER_DELAYTIME_ON	                    				,Funcblock_Timer_DelayTime_On                  },		//0x3404
	 {FUNCBLOCK_TIMER_DELAYTIME_OFF	                    				,Funcblock_Timer_DelayTime_Off                 },		//0x3405
	 {FUNCBLOCK_LOGIC_MISCELANEOUS_RSLATCHING	                    	,Funcblock_Miscellaneous_RSLatching            },		//0x3501
	 {FUNCBLOCK_LOGIC_MISCELANEOUS_SRLATCHING	                    	,Funcblock_Miscellaneous_SRLatching            },		//0x3501

	 {FUNCBLOCK_COUNTER_UPSLOPECOUNTER                    	            ,Funcblock_Counter_UpSlopeCounter              },		//0x3301
	 {FUNCBLOCK_COUNTER_DOWNSLOPECOUNTER                    	        ,Funcblock_Counter_DownSlopeCounter            },		//0x3302
	 {FUNCBLOCK_COUNTER_PULSECOUNTER                    	            ,Funcblock_Counter_PulseCounter                },		//0x3303

	 {FUNCBLOCK_COMPARATOR_LARGER                        	            ,Funcblock_Comparator_Large                    },		//0x3205
	 {FUNCBLOCK_COMPARATOR_LARGEREQUAL                    	            ,Funcblock_Comparator_lagerEqual               },		//0x3206
	 {FUNCBLOCK_COMPARATOR_EQUAL                    	                ,Funcblock_Comparator_Equal                    },		//0x3207

	 {FUNCBLOCK_TIMER_HIGHLEVELTIME                   	                ,Funcblock_Timer_HighLevelTime                 },	    //0x3401
	 {FUNCBLOCK_TIMER_LOWLEVELTIME                                      ,Funcblock_Timer_LowLevelTime                  },       //0x3402
	 {FUNCBLOCK_TIMER_PULSETIME                   	                    ,Funcblock_Timer_PulseTime                     },	    //0x3403

	 {FUNCBLOCK_SORT_MAX                   	                        	,Funcblock_Sort_Max                          },	    //0x3208
	 {FUNCBLOCK_SORT_MIN                  	                        	,Funcblock_Sort_Min                          },	    //0x3209

	 {FUNCBLOCK_MISCELANEOUS_SQUAREWAVEGENERATOR                        ,Funcblock_Miscellaneous_SquareWaveGenertor    },		//0x3702
	 {FUNCBLOCK_MISCELANEOUS_CONSTANT                                   ,Funcblock_Miscellaneous_Constant              },		//0x3701
	 {FUNCBLOCK_MISCELANEOUS_RANDOM                                     ,Funcblock_Miscellaneous_Random                },		//0x3703
	 {FUNCBLOCK_MISCELANEOUS_GETVALUEADDRESS                            ,Funcblock_Miscellaneous_GetValueAddress       },		//0x3704
	 {FUNCBLOCK_HIGN_LINKER                                             ,Funcblock_High_Linker                         },       //0x3705

	 {FUNCBLOCK_ARITHMETIC_AND                        					,Funcblock_Arithmetic_And    				   },		//0x3801
	 {FUNCBLOCK_ARITHMETIC_OR                                   		,Funcblock_Arithmetic_Or              		   },		//0x3802
	 {FUNCBLOCK_ARITHMETIC_SHIFTLEFT                                    ,Funcblock_Arithmetic_ShiftLeft                },		//0x3803
	 {FUNCBLOCK_ARITHMETIC_SHIFTRIGHT                            		,Funcblock_Arithmetic_ShiftRight       		   },		//0x3804

	 {FUNCBLOCK_CANMASTER_INPUT											,Funcblock_Canopen_MasterIn					   },		//0x1202
	 {FUNCBLOCK_CANMASTER_OUTPUT										,Funcblock_Canopen_MasterOut				   },		//0x5204
	 {FUNCBLOCK_CANSLAVE_INPUT											,Funcblock_Canopen_SlaveIn					   },		//0x1203
	 {FUNCBLOCK_CANSLAVE_OUTPUT											,Funcblock_Canopen_SlaveOut					   },		//0x5205

	 {FUNCBLOCK_MODBUSRTU_M01											,Funcblock_ModbusRTU_M01					   },		//0x1211
	 {FUNCBLOCK_MODBUSRTU_M02											,Funcblock_ModbusRTU_M02					   },		//0x1212
	 {FUNCBLOCK_MODBUSRTU_M03											,Funcblock_ModbusRTU_M03					   },		//0x1204
	 {FUNCBLOCK_MODBUSRTU_M04											,Funcblock_ModbusRTU_M04					   },		//0x1205
	 {FUNCBLOCK_MODBUSRTU_S05											,Funcblock_ModbusRTU_S05					   },		//0x1215
	 {FUNCBLOCK_MODBUSRTU_S06											,Funcblock_ModbusRTU_S06					   },		//0x1206
	 {FUNCBLOCK_MODBUSRTU_S0F											,Funcblock_ModbusRTU_S0F					   },		//0x120F
	 {FUNCBLOCK_MODBUSRTU_S10											,Funcblock_ModbusRTU_S10					   },		//0x1210

	 {FUNCBLOCK_MODBUSRTU_S01											,Funcblock_ModbusRTU_S01					   },		//0x5211
	 {FUNCBLOCK_MODBUSRTU_S02											,Funcblock_ModbusRTU_S02					   },		//0x5212
	 {FUNCBLOCK_MODBUSRTU_S03											,Funcblock_ModbusRTU_S03					   },		//0x5206
	 {FUNCBLOCK_MODBUSRTU_S04											,Funcblock_ModbusRTU_S04					   },		//0x5207
	 {FUNCBLOCK_MODBUSRTU_M05											,Funcblock_ModbusRTU_M05					   },		//0x5215
	 {FUNCBLOCK_MODBUSRTU_M06											,Funcblock_ModbusRTU_M06					   },		//0x5208
	 {FUNCBLOCK_MODBUSRTU_M0F											,Funcblock_ModbusRTU_M0F					   },		//0x520F
	 {FUNCBLOCK_MODBUSRTU_M10											,Funcblock_ModbusRTU_M10					   },		//0x5210

	 {FUNCBLOCK_MODBUSTCP_M03											,Funcblock_ModbusTCP_M03					   },		//0x1305
	 {FUNCBLOCK_MODBUSTCP_M04											,Funcblock_ModbusTCP_M04					   },		//0x1306
	 {FUNCBLOCK_MODBUSTCP_S06											,Funcblock_ModbusTCP_S06					   },		//0x1307
	 {FUNCBLOCK_MODBUSTCP_S03											,Funcblock_ModbusTCP_S03					   },		//0x5306
	 {FUNCBLOCK_MODBUSTCP_S04											,Funcblock_ModbusTCP_S04					   },		//0x5307
	 {FUNCBLOCK_MODBUSTCP_M06											,Funcblock_ModbusTCP_M06					   },		//0x5308

	 {FUNCBLOCK_INET_INPUT												,Funcblock_iNet_In					   		   },		//0x5308
     {FUNCBLOCK_INET_OUTPUT												,Funcblock_iNet_Out					   		   },		//0x5308
};
/*===========================================================================================+
|           InputPoint and OutPutPoint Definition                                            |
|			输入点、输出点、输入模块、输出模块定义				                                 		 |
+===========================================================================================*/

/*===========================================================================+
|Name:         Input Point                                                   |
|Description:  Read the inputValue form DataBase According to the input ID   |
+===========================================================================*/
BOOL_T 	Funcblock_Input_DigitalPoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T                subID;

	if(RuntimeData[DesigntimeData->serialNo].scanStatus == 0)
	{
	 	subID = (DesigntimeData->FunctionBlockID&0x00ff) + ENUM_CARD_0_INTERFACE_IO_INPUTG1 - 1 ;
		RuntimeData[DesigntimeData->serialNo].outputValue.dwData = (DWORD_T)g_DBData[ subID ].dbValue.cData;
	}
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*===========================================================================+
|Name:         output Point                                                  |
|Description:  Write the OutputValue to Database According to the Output     |
+===========================================================================*/
BOOL_T    Funcblock_Output_DigitalPoint(BYTE_T*  DesignTime_ObjectListData, BYTE_T*  RunTime_ObjectData )
{
	RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T                wIndex;
	WORD_T                subID;

	wIndex = (DesigntimeData->FunctionBlockID&0x00ff) + ENUM_CARD_0_INTERFACE_IO_OUTPUTG1 - 1;
    subID = DesigntimeData->FunctionBlockID&0x00ff ;
	DbValue[subID].cData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.cData;
	RuntimeData[DesigntimeData->serialNo].outputValue.cData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.cData;
	DBSetValueByOffset(wIndex,DbValue[subID]);	 // write output value to DataBase
	RuntimeData[DesigntimeData->serialNo].DataType = 1;

	return TRUE;
}
/*--------------------------------------------------------------------------------------------+
| name:         Analog Point										                          |
| description:  Read Data From the Database According to the Analog Input ID                  |
+--------------------------------------------------------------------------------------------*/
BOOL_T 	Funcblock_Input_AnalogPoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
	RUNTIMEDATATYPE_T  *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
	DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
	WORD_T         subID;

	subID = (DesigntimeData->FunctionBlockID&0x00ff) + ENUM_CARD_0_INTERFACE_IO_AD1 -1 ;
	RuntimeData[DesigntimeData->serialNo].outputValue.dwData = g_DBData[ subID ].dbValue.dwData;  //Read input Data from function block outputValue
	RuntimeData[DesigntimeData->serialNo].DataType = 2;

	return TRUE;
}
/*===========================================================================+
|Name:         Analog output Point                                           |
|Description:  Write the OutputValue to Database According to the Input ID   |
+===========================================================================*/
BOOL_T 	Funcblock_Output_AnalogPoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData)
{
    RUNTIMEDATATYPE_T     *RuntimeData = (RUNTIMEDATATYPE_T*)RunTime_ObjectData;
    DESIGNTIMEDATATYPE_T  *DesigntimeData = (DESIGNTIMEDATATYPE_T*)DesignTime_ObjectListData;
    WORD_T                wIndex;
    WORD_T                subID;

    wIndex = (DesigntimeData->FunctionBlockID&0x00ff) + ENUM_CARD_0_INTERFACE_IO_DA1 - 1;
    subID = DesigntimeData->FunctionBlockID&0x00ff;
    DbValue[subID].dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
    RuntimeData[DesigntimeData->serialNo].outputValue.dwData = RuntimeData[DesigntimeData->InputsID[0]].outputValue.dwData;
    DBSetValueByOffset(wIndex,DbValue[subID]);   // write output value to DataBase
    RuntimeData[DesigntimeData->serialNo].DataType = 2;

    return TRUE;
}
/*-------------------------------------------------------------------------------------------------+
|                              END                                                                 |
+-------------------------------------------------------------------------------------------------*/
