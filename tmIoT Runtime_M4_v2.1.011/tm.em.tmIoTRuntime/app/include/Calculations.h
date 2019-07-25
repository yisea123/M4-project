/*===========================================================================+
|  Function : Conculation.h	                                                 |
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

#ifndef D__TASK_CALCULATIONS_H_
#define	D__TASK_CALCULATIONS_H_

/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
#ifdef __cplusplus
extern "C" {
#endif


#include    "task_Runtime.h"
#include    "Function_Timer.h"
#include    "Function_LogicFormula.h"
#include    "Function_Miscellaneous.h"
#include    "Function_Counter.h"
#include    "Function_DataBase.h"
#include    "Function_Analog.h"
#include    "Function_General.h"
#include	"CanFunctionBlock.h"
#include 	"Function_Modbus.h"
#include    "Function_ModbusTCP.h"
#include    "Function_iNet.h"

#include    "kernel/include/type.h"
#include    "kernel/include/device.h"
#include    "kernel/include/task.h"
#include    "driver/include/_ad.h"
#include    "driver/include/_output.h"
#include    "driver/include/_input.h"
#include    "driver/include/_da.h"
#include    "database.h"
#include    "driver/include/_timer.h"
/*===========================================================================+
|           Type definition                                                   |
+============================================================================*/

typedef  BOOL_T (*FUNCTIONBLOCK)( BYTE_T*  DesignTime_ObjectListData, BYTE_T*  RunTime_ObjectData );

typedef struct funcblock
{
    WORD_T              dwID;
    FUNCTIONBLOCK       DealFunc;
}  FUNCBLOCK;

/*===========================================================================+
|            Operations                                                      |
+============================================================================*/

//void       GetinputValue(void);

/*===========================================================================+
|            Attributes                                                      |
+============================================================================*/

enum    tagFUNCTIONBLOCK_INDEX
{
        BLOCK_INPUT_DIGITALPOINT,
        BLOCK_INPUT_ANALOGPOINT,
        BLOCK_OUTPUT_DIGITALPOINT,
        BLOCK_OUTPUT_ANALOGPOINT,
//		BLOCK_INPUT_DATABASE_SINGLEPOINT,
//		BLOCK_INPUT_NET_SINGLEPOINT ,
//		BLOCK_OUTPUT_DATABASE_SINGLEPOINT,
//		BLOCK_OUTPUT_NET_SINGLEPOINT,

//        BLOCK_INPUTMODULE_DOORMAGNETIC,
//        BLOCK_INPUTMODULE_HUMANBODYSENSOR,
//		BLOCK_INPUTMODULE_SWITCH,
//		BLOCK_INPUTMODEL_NETINPUT,
//		BLOCK_INPUTMODULE_DATABASESINGLEINPUT,
//		BLOCK_INPUTMODULE_DATABASEMULTIINPUT,
//		BLOCK_INPUTMODULE_ANALOGSPECIALCOLLECTOR,
//		BLOCK_INPUTMODULE_ANALOGCOLLECTOR,
		BLOCK_INPUTMODULE_DIGITAL,
		BLOCK_INPUTMODULE_ANALOG,
		BLOCK_INPUTMODULE_GLOBALVARIABLE,

//		BLOCK_OUTPUTMODULE_RELAY,
//		BLOCK_OUTPUTMODULE_ORDINARYLAMP,
//		BLOCK_OUTPUTMODULE_ALERT,
//		BLOCK_OUTPUTMODULE_NETOUTPUT,
//		BLOCK_OUTPUTMODULE_INET,
//	    BLOCK_OUTPUTMODULE_DATABASE,
		BLOCK_OUTPUTMODULE_DIGITAL,
		BLOCK_OUTPUTMODULE_ANALOG,
		BLOCK_OUTPUTMODULE_GLOBALVARIABLE,

		BLOCK_LOGIC_AND,
		BLOCK_LOGIC_OR,
		BLOCK_LOGIC_NOT,
		BLOCK_LOGIC_XOR,
		BLOCK_ARITHMETIC_ADD,
		BLOCK_ARITHMETIC_MINUS,
		BLOCK_ARITHMETIC_MULTIPLY,
		BLOCK_ARITHMETIC_DEVIDE,
		BLOCK_LOGIC_DELAYTIME_ON,
		BLOCK_LOGIC_DELAYTIME_OFF,
		BLOCK_LOGIC_MISCELANEOUS_RSLATCHING,
		BLOCK_LOGIC_MISCELANEOUS_SRLATCHING,
		BLOCK_COUNTER_UPSLOPECOUNTER,
		BLOCK_COUNTER_DOWNSLOPECOUNTER,
		BLOCK_COUNTER_PULSECOUNTER,
		BLOCK_COMPARATOR_LARGER,
		BLOCK_COMPARATOR_LARGEREQUAL,
		BLOCK_COMPARATOR_EQUAL,
		BLOCK_TIMER_HIGHLEVELTIME,
		BLOCK_TIMER_LOWLEVELTIME,
		BLOCK_TIMER_PULSETIME,
		BLOCK_ANALOG_MAX,
		BLOCK_ANALOG_MIN,
		BLOCK_MISCELANEOUS_SQUAREWAVEGENERATOR,
		BLOCK_MISCELANEOUS_CONSTANT,
		BLOCK_MISCELANEOUS_RANDOM,
		BLOCK_MISCELANEOUS_GETVALUEADDRESS,
		BLOCK_HIGN_LINKER,
		BLOCK_ARITHMETIC_AND,
		BLOCK_ARITHMETIC_OR,
		BLOCK_ARITHMETIC_SHIFTLEFT,
		BLOCK_ARITHMETIC_SHIFTRIGHT,
		BLOCK_CANMASTER_INPUT,
		BLOCK_CANMASTER_OUTPUT,
		BLOCK_CANSLAVE_INPUT,
		BLOCK_CANSLAVE_OUTPUT,
		BLOCK_MODBUSRTU_M01,
		BLOCK_MODBUSRTU_M02,
		BLOCK_MODBUSRTU_M03,
		BLOCK_MODBUSRTU_M04,
		BLOCK_MODBUSRTU_S05,
		BLOCK_MODBUSRTU_S06,
		BLOCK_MODBUSRTU_S0F,
		BLOCK_MODBUSRTU_S10,
		BLOCK_MODBUSRTU_S01,
		BLOCK_MODBUSRTU_S02,
		BLOCK_MODBUSRTU_S03,
		BLOCK_MODBUSRTU_S04,
		BLOCK_MODBUSRTU_M05,
		BLOCK_MODBUSRTU_M06,
		BLOCK_MODBUSRTU_M0F,
		BLOCK_MODBUSRTU_M10,
		BLOCK_MODBUSTCP_M03,
		BLOCK_MODBUSTCP_M04,
		BLOCK_MODBUSTCP_S06,
		BLOCK_MODBUSTCP_S03,
		BLOCK_MODBUSTCP_S04,
		BLOCK_MODBUSTCP_M06,
		BLOCK_INET_INPUT,
		BLOCK_INET_OUTPUT,

		BLOCK_TOTAL_NUM

};
/*===========================================================================+
|           Macro definition                                                  |
+============================================================================*/
#define	   FUNCBLOCK_INPUTMODULE_DATABASESINGLEINPUT     0x1101
#define    FUNCBLOCK_INPUTMODULE_DATABASEMULTIINPUT      0x1102
#define    FUNCBLOCK_INPUTMODULE_NETINPUT		     	 0x1201

#define    FUNCBLOCK_INPUTMODULE_DOORMAGNETIC            0x1301
#define    FUNCBLOCK_INPUTMODULE_HUMANBODYSENSOR         0x1302
#define    FUNCBLOCK_INPUTMODULE_SWITCH           	     0x1304

#define    FUNCBLOCK_INPUT_NET_SINGLEPOINT                  0x2401
#define    FUNCBLOCK_INPUT_DIGITALPOINT	                    0x2101
#define    FUNCBLOCK_INPUT_ANALOGPOINT                      0x2201
#define    FUNCBLOCK_INPUT_DATABASE_SINGLEPOINT             0x2301
#define    FUNCBLOCK_INPUTMODULE_ANALOGSPECIALCOLLECTOR	    0x1601
#define    FUNCBLOCK_INPUTMODULE_ANALOGCOLLECTOR            0x1602
#define	   FUNCBLOCK_INPUTMODULE_DIGITAL			        0x1701
#define    FUNCBLOCK_INPUTMODULE_ANALOG  			        0x1702
#define    FUNCBLOCK_INPUTMODULE_GLOBALVARIABLE				0x1801


#define    FUNCBLOCK_LOGIC_AND	             	    0x3101	//3101 change for
#define    FUNCBLOCK_LOGIC_OR	             	    0x3102
#define    FUNCBLOCK_LOGIC_NOT	             	    0x3103
#define    FUNCBLOCK_LOGIC_XOR                      0x3104

#define    FUNCBLOCK_ARITHMETIC_ADD					0x3201
#define    FUNCBLOCK_ARITHMETIC_MINUS               0x3202
#define    FUNCBLOCK_ARITHMETIC_MULTIPLY            0x3203
#define    FUNCBLOCK_ARITHMETIC_DEVIDE              0x3204
#define    FUNCBLOCK_COMPARATOR_LARGER              0x3205
#define	   FUNCBLOCK_COMPARATOR_LARGEREQUAL         0x3206
#define    FUNCBLOCK_COMPARATOR_EQUAL               0x3207

#define	   FUNCBLOCK_COUNTER_UPSLOPECOUNTER         0x3301
#define    FUNCBLOCK_COUNTER_DOWNSLOPECOUNTER		0x3302
#define    FUNCBLOCK_COUNTER_PULSECOUNTER           0x3303

#define	   FUNCBLOCK_TIMER_HIGHLEVELTIME			0x3401
#define    FUNCBLOCK_TIMER_LOWLEVELTIME	            0x3402 //3101
#define    FUNCBLOCK_TIMER_PULSETIME                0x3403 //3101
#define    FUNCBLOCK_TIMER_DELAYTIME_ON				0x3404
#define	   FUNCBLOCK_TIMER_DELAYTIME_OFF            0x3405

#define	   FUNCBLOCK_LOGIC_MISCELANEOUS_RSLATCHING			0x3501
#define    FUNCBLOCK_LOGIC_MISCELANEOUS_SRLATCHING			0x3502

#define    FUNCBLOCK_SORT_MAX								0x3602
#define    FUNCBLOCK_SORT_MIN                             	0x3603
#define    FUNCBLOCK_MISCELANEOUS_SQUAREWAVEGENERATOR		0x3702
#define    FUNCBLOCK_MISCELANEOUS_CONSTANT                  0x3701
#define    FUNCBLOCK_MISCELANEOUS_RANDOM	                0x3703
#define    FUNCBLOCK_MISCELANEOUS_GETVALUEADDRESS			0x3704
#define    FUNCBLOCK_HIGN_LINKER                    0x3705

#define    FUNCBLOCK_ARITHMETIC_AND					0x3801
#define    FUNCBLOCK_ARITHMETIC_OR					0x3802
#define    FUNCBLOCK_ARITHMETIC_SHIFTLEFT			0x3803
#define    FUNCBLOCK_ARITHMETIC_SHIFTRIGHT			0x3804

#define    FUNCBLOCK_OUTPUT_DIGITALPOINT	        0x4101
#define    FUNCBLOCK_OUTPUT_ANALOGPOINT             0x4201
#define    FUNCBLOCK_OUTPUTMODULE_REPLY	            0x5101
#define    FUNCBLOCK_OUTPUTMODULE_ORDINARYLAMP      0x5501
#define    FUNCBLOCK_OUTPUTMODULE_ALERT             0x5503

#define    FUNCBLOCK_OUTPUTMODULE_DATABASE 		    0x5101
#define    FUNCBLOCK_OUTPUTMODULE_NETOUTPUT			0x5202
#define    FUNCBLOCK_OUTPUTMODULE_INET			    0x5203
#define	   FUNCBLOCK_OUTPUTMODULE_DIGITAL			0x5801
#define    FUNCBLOCK_OUTPUTMODULE_ANALOG			0x5802
#define    FUNCBLOCK_OUTPUTMODULE_GLOBALVARIABLE	0x5901

#define    FUNCBLOCK_OUTPUT_DATABASE_SINGLEPOINT    0x4301
#define    FUNCBLOCK_OUTPUT_NET_SINGLEPOINT         0x4401

#define    FUNCBLOCK_CANMASTER_INPUT				0x1202
#define    FUNCBLOCK_CANMASTER_OUTPUT				0x5204

#define    FUNCBLOCK_CANSLAVE_INPUT					0x1203
#define    FUNCBLOCK_CANSLAVE_OUTPUT				0x5205

/**********************************Input******************************/
#define    FUNCBLOCK_MODBUSRTU_M01 					0x1211
#define    FUNCBLOCK_MODBUSRTU_M02 					0x1212
#define    FUNCBLOCK_MODBUSRTU_M03 					0x1204
#define    FUNCBLOCK_MODBUSRTU_M04					0x1205
#define    FUNCBLOCK_MODBUSRTU_S05					0x1215
#define    FUNCBLOCK_MODBUSRTU_S06					0x1206
#define    FUNCBLOCK_MODBUSRTU_S0F					0x120F
#define    FUNCBLOCK_MODBUSRTU_S10					0x1210
/**********************************Output******************************/
#define    FUNCBLOCK_MODBUSRTU_S01					0x5211
#define    FUNCBLOCK_MODBUSRTU_S02					0x5212
#define    FUNCBLOCK_MODBUSRTU_S03					0x5206
#define    FUNCBLOCK_MODBUSRTU_S04					0x5207
#define    FUNCBLOCK_MODBUSRTU_M05					0x5215
#define    FUNCBLOCK_MODBUSRTU_M06					0x5208
#define    FUNCBLOCK_MODBUSRTU_M0F					0x520F
#define    FUNCBLOCK_MODBUSRTU_M10					0x5210
/**********************************Input******************************/
#define    FUNCBLOCK_MODBUSTCP_M03 					0x1305
#define    FUNCBLOCK_MODBUSTCP_M04					0x1306
#define    FUNCBLOCK_MODBUSTCP_S06					0x1307
/**********************************Output******************************/
#define    FUNCBLOCK_MODBUSTCP_S03					0x5306
#define    FUNCBLOCK_MODBUSTCP_S04					0x5307
#define    FUNCBLOCK_MODBUSTCP_M06					0x5308
/**********************************iNet******************************/
#define    FUNCBLOCK_INET_INPUT						0x1301
#define    FUNCBLOCK_INET_OUTPUT					0x5301

#define   FUNC_DATA_NUM                             5

/*===========================================================================+
|            Extern                                                          |
+===========================================================================*/
extern	BOOL_T 	Funcblock_Input_DigitalPoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Output_DigitalPoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern  BOOL_T 	Funcblock_Input_AnalogPoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
extern	BOOL_T 	Funcblock_Output_AnalogPoint(BYTE_T* DesignTime_ObjectListData, BYTE_T* RunTime_ObjectData);
/*===========================================================================+
|            Extern Veriable                                                 |
+===========================================================================*/
extern	const        FUNCBLOCK		    g_FunctionBlockAttr[];

#ifdef __cplusplus
}
#endif /* extern "C" */

#endif
