#include "uart/include/uart_dev.h"
#include "database.h"

#define 	MODBUS_SLAVE 		1
#define 	MODBUS_MASTER 		2

#define 	MAX_MODBUSOD 		200
#define 	MODBUS_CLIENT		0
#define 	MODBUS_SERVER 		1
#define 	MAX_MODBUSREG		125    // 1 -- 126


#define 	MODBUSDATA_OFFSET			0x0
#define 	MODBUSPARA_OFFSET 			0xA
#define     MODBUSOD1_OFFSET			0x400			//1K位置起始
#define     MODBUSOD2_OFFSET			0x2000			//8K位置起始

typedef struct tag_MODBUSPARA
{
	BYTE	byEnable;
	WORD	wRegNum[MAX_RS485];       //studio总共配置的寄存器数量
}MODBUSPARA;

typedef 	struct tag_MODBUSREGVALUE {
	WORD_T addr;
	WORD_T value;
} MODBUSREGVALUE_T;

typedef struct tag_RS485STARTPARA {
		BYTE_T port;
		BYTE_T mode;
		BYTE_T nodeid;
		BYTE_T baud;
		BYTE_T databit;
		BYTE_T para;
		BYTE_T stopbit;
} RS485STARTPARA_T;

#pragma pack(push)
#pragma pack(1)
typedef struct tag_MODBUSOD {
	DWORD_T 	dataid;
	BYTE_T 	id;
	BYTE_T 	cmd;
	WORD_T 	addr;
} MODBUSOD_T;
#pragma pack(pop)

typedef struct tag_MODBUSODOLDVALUE {
	DBVALUE DbValue;
	BYTE	bySendCount;
	BYTE	byUpdate;
} MODBUSODOLDVALUE_T;

typedef struct tag_MODBUSODVALUE {
	DWORD_T dataid;
	DBVALUE readvalue;
	DBVALUE writevalue;
} MODBUSODVALUE_T;

#pragma pack(push)
#pragma pack(1)
typedef struct tag_MODBUSREG {
	DWORD			    regdataid;
	WORD 				regaddr;
}MODBUSREG_T;
#pragma pack(pop)

typedef struct tag_RunRS485Map {
    int    	   RunRS485Id;
    void       (*pFun)(void *);
}RURS485MAP_T;

#pragma pack(push)
#pragma pack(1)
typedef struct tag_MODBUSCMD {
	BYTE_T id;
	BYTE_T cmd;
	WORD_T addr;
	WORD_T count;
	WORD_T value;
} MODBUSCMD_T;
#pragma pack(pop)

typedef struct tag_MODBUSOFFSET {
	WORD_T start;
	WORD_T end;
} MODBUSOFFSET_T;

void  InitRS485(int Default, BYTE_T Port);
void  InitMulRS485(void);
HTASK CreateTaskRS485(BYTE_T Port);
void  CreateMulTaskRS485(void);
void  DestroyMulTaskRS485(void);
void  Get_ODValueByDataID_2(DWORD_T id, DBVALUE *dbValue);
void  Set_ODValueByDataID_2(DWORD_T id, DWORD_T value);
void  Get_RegValue_byDataID_2(DWORD_T id, DBVALUE *dbValue);
void  Set_RegValue_byDataID_2(DWORD_T id, WORD_T value);
BOOL  SetRs485Para(BYTE *byData, WORD wLen);

extern RS485STARTPARA_T 	RS485StartPara[MAX_RS485];
extern HTASK 				RS485Task[MAX_RS485];
extern MODBUSPARA			g_ModbusPara;
extern MODBUSOD_T 			ModbusOD_Table[MAX_RS485][MAX_MODBUSOD];
extern MODBUSREG_T 			ModbusReg_Table[MAX_RS485][MAX_MODBUSREG];
