/*===========================================================================+
|  Function : Rs485 application                                              |
|  Task     : Rs485 application Source File                                  |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : 		                                                     	 |
|  Version  : V1.2                                                           |
|  Creation :                                                                |
|  Revision :                                                                |
+===========================================================================*/
#include "kernel/include/task.h"
#include "driver/include/_timer.h"
#include "taskrs485.h"
#include "comlib.h"
#include "studioprotocol.h"
/*===========================================================================+
|           Constant                                                         |
+===========================================================================*/
//BYTE_T 	ModbusSlaveNodeid = 128;
//int 	Modbus1_CliServ = -1;
//int	 	Modbus2_CliServ = -1;
/*===========================================================================+
|           Type definition                                                  |
+===========================================================================*/

/*===========================================================================+
|           Macro definition                                                 |
+===========================================================================*/

/*===========================================================================+
|           Globle Variables                                                 |
+===========================================================================*/
MODBUSOD_T 				ModbusOD_Table[MAX_RS485][MAX_MODBUSOD];
MODBUSODVALUE_T	 		ModbusODValue_Table[MAX_RS485][MAX_MODBUSOD];   		// Master Value Table
MODBUSODOLDVALUE_T 		ModbusODOldValue_Table[MAX_RS485][MAX_MODBUSOD];

MODBUSREG_T 			ModbusReg_Table[MAX_RS485][MAX_MODBUSREG];
MODBUSREGVALUE_T 		ModbusRegValue_Table[MAX_MODBUSREG];  				// Slave  Value Table
RS485STARTPARA_T 		RS485StartPara[MAX_RS485] = {{0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},
											  	  	  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}};

MODBUSPARA				g_ModbusPara;

MODBUSOFFSET_T 			ModbusOffset[MAX_RS485];
HTASK 					RS485Task[MAX_RS485];

static int 				Setp[MAX_RS485] = {0};

HANDLE_T 				g_hRS485[MAX_RS485] = {0xFF, 0xFF};
long 					u_Counter_RS485[MAX_RS485] = {0};

BYTE_T SendBuf[128];
/*===========================================================================+
|           Class implementation                                             |
+===========================================================================*/

/*===========================================================================+
|           Helpers Implementation                                           |
+===========================================================================*/
void 	Create_RS485(int Default, BYTE_T Port);
void 	RunRS485_1(void *vpParam);
void 	RunRS485_2(void *vpParam);
WORD 	ComposeData_Package(BYTE byPort, BYTE *byBuf);
void 	Tx_RS485(BYTE_T Port);
void 	Rx_RS485(BYTE_T Port);
void 	Request_ModbusOD(BYTE_T Port);
void 	Recv_ModbusRequest(BYTE_T Port);
void 	Recv_ModbusResponse(BYTE_T Port);
DBVALUE Get_ODValue_ByIndex(BYTE_T Port, WORD_T Index);
DBVALUE Get_RegValue_byDataID(BYTE_T Port, DWORD_T id);
void 	HandleData(BYTE_T Port, BYTE_T *pBuf);

void 	Set_UartPara();
void 	Setp_Modbus(BYTE_T Port);
void 	Sent_CRC(BYTE_T *pBuf, WORD_T Length);
BYTE_T	Recv_CRC(BYTE_T *pBuf, WORD_T Length);
int 	FindRegAddr(WORD_T addr);
void 	SetValueToOldODValue(BYTE byPort, WORD wIndex, DBVALUE DbValue);
/*===========================================================================+
|           Inner Globle Variable                                            |
+===========================================================================*/
RURS485MAP_T RunRS485Map [] = {
	{RS485_1, RunRS485_1},
	{RS485_2, RunRS485_2}
};
/*===========================================================================+
|           Constructor and destructor                                       |
+===========================================================================*/
void InitRS485(int Default, BYTE_T Port)
{
	int 	i;

	if (Default){
		for (i = 0; i < MAX_RS485; i++) {
			g_hRS485[i] = Device_Create(&g_RS485_Engine[i]);
		}
	} else {
		g_hRS485[Port] = Device_Create(&g_RS485_Engine[Port]);
	}

	Create_RS485(Default, Port);

}

HTASK CreateTaskRS485(BYTE_T Port)
{
	TASK    Task;
	Task.wType = TASK_WHILELOOP;
	Task.period.tCounter = 0;
	Task.period.tTimer = 1;
	Task.pRun = RunRS485Map[Port].pFun;
	Task.pParam = 0;

	return Create_Task(&Task, 0);
}

void Create_RS485(int Default, BYTE_T Port)
{
	int 		i;
	RS485CONF 	u_RS485Conf[MAX_RS485];
	DWORD_T 	User_Baud;
	DWORD_T 	User_Databit;
	DWORD_T 	User_Para;
	DWORD_T 	User_Stopbit;

	memset(u_RS485Conf, 0, sizeof(u_RS485Conf));

	u_RS485Conf[RS485_1].wChannel = RS485_1;
	u_RS485Conf[RS485_2].wChannel = RS485_2;

	if (Default) {
		for (i = 0; i < MAX_RS485; i++) {
			u_RS485Conf[i].dwBaud = 9600;
			u_RS485Conf[i].dwMode = UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE;
			Device_Open(g_hRS485[i], &u_RS485Conf[i]);
		}
	} else {
		User_Baud 			= Get_UartPaud(RS485StartPara[Port].baud);
		User_Databit 		= Get_UartDataBit(RS485StartPara[Port].databit);
		User_Para 			= Get_UartParity(RS485StartPara[Port].para);
		User_Stopbit 		= Get_UartStopBit(RS485StartPara[Port].stopbit);

		u_RS485Conf[Port].dwBaud = User_Baud;
		u_RS485Conf[Port].dwMode = User_Databit | User_Para | User_Stopbit;
		Device_Open(g_hRS485[Port], &u_RS485Conf[Port]);
	}
}

void RunRS485_1(void *vpParam)
{
	Rx_RS485(RunRS485Map[RS485_1].RunRS485Id);

	if (g_dwSystemTick - u_Counter_RS485[RS485_1] >= 200) {
		u_Counter_RS485[RS485_1] = g_dwSystemTick;
		Tx_RS485(RunRS485Map[RS485_1].RunRS485Id);
	}
	_RS485_Rx(RunRS485Map[RS485_1].RunRS485Id);
	_RS485_Tx(RunRS485Map[RS485_1].RunRS485Id);
}

void RunRS485_2(void *vpParam)
{
	Rx_RS485(RunRS485Map[RS485_2].RunRS485Id);
	if (g_dwSystemTick - u_Counter_RS485[RS485_2] >= 200) {
		u_Counter_RS485[RS485_2] = g_dwSystemTick;
		Tx_RS485(RunRS485Map[RS485_2].RunRS485Id);

	}
	_RS485_Rx(RunRS485Map[RS485_2].RunRS485Id);
	_RS485_Tx(RunRS485Map[RS485_2].RunRS485Id);
}
/*===========================================================================+
|           Operations                                                       |
+===========================================================================*/

/*===========================================================================+/
|           Helpers                                                          |
+===========================================================================*/
void Tx_RS485(BYTE_T byPort)
{
	if(byPort >= MAX_RS485) return;

	switch (RS485StartPara[byPort].mode) {
		case MODBUS_MASTER:
			Request_ModbusOD(byPort);
			break;
		case MODBUS_SLAVE:
//			Response_ModbusOD(byPort);
			break;
		default:
			break;
	}
}

void Rx_RS485(BYTE_T byPort)
{

	if(byPort >= MAX_RS485) return;

	switch (RS485StartPara[byPort].mode) {
		case MODBUS_MASTER:
			Recv_ModbusResponse(byPort);
			break;
		case MODBUS_SLAVE:
			Recv_ModbusRequest(byPort);
			break;
		default:
			break;
	}
}

void Request_ModbusOD(BYTE_T Port)
{
	WORD wLength = 0;
	memset (SendBuf, 0, sizeof(SendBuf));

	/*
	 * 如果组包长度为0，则继续组下一组数据，保证每次进入发送状态，都有实际数据发送，提高通讯效率
	 */
	do{
		Setp_Modbus(Port);
		wLength = ComposeData_Package(Port, SendBuf);
	}while(wLength == 0);
//	if(wLength == 0) return;

	Uart_WriteDev(Uart_Type[RS485_TYPE], Port, SendBuf, wLength, 0);
}


/*
 * 作为Master时，0x01/0x02/0x03/0x04功能码组包函数
 */
WORD ComposeData_ReadHoldingRegisters(BYTE byPort, BYTE *byBuf)
{
	DBVALUE 	dbValue;
	WORD HeadIndex = ModbusOffset[byPort].start;
	WORD TailIndex = ModbusOffset[byPort].end;
	memcpy(&byBuf[0], &ModbusOD_Table[byPort][HeadIndex].id, sizeof(BYTE));
	memcpy(&byBuf[1], &ModbusOD_Table[byPort][HeadIndex].cmd, sizeof(BYTE));
	memcpy(&byBuf[2], &ModbusOD_Table[byPort][HeadIndex].addr, sizeof(WORD));
	dbValue.wData = htons(htons(ModbusOD_Table[byPort][TailIndex].addr) - htons(ModbusOD_Table[byPort][HeadIndex].addr) + 0x1);
	memcpy(&byBuf[4], &dbValue.wData, sizeof(WORD_T));
	Sent_CRC(byBuf, 8);

	return 8;
}

/*
 * 作为Master时，0x05功能码组包函数
 */
WORD ComposeData_WriteSingleCoil(BYTE byPort, BYTE *byBuf)
{
	DBVALUE 	dbValue;
	WORD HeadIndex = ModbusOffset[byPort].start;

	memcpy(&byBuf[0], &ModbusOD_Table[byPort][HeadIndex].id, sizeof(BYTE));
	memcpy(&byBuf[1], &ModbusOD_Table[byPort][HeadIndex].cmd, sizeof(BYTE));
	memcpy(&byBuf[2], &ModbusOD_Table[byPort][HeadIndex].addr, sizeof(WORD));
	dbValue = Get_ODValue_ByIndex(byPort, HeadIndex);
	memcpy(&byBuf[4], &dbValue.wData, sizeof(WORD_T));

	if(dbValue.dwData == ModbusODOldValue_Table[byPort][HeadIndex].DbValue.dwData)
		return 0;

	ModbusODOldValue_Table[byPort][HeadIndex].byUpdate = 1;
	Sent_CRC(byBuf, 8);

	ModbusODOldValue_Table[byPort][HeadIndex].bySendCount++;

	if(ModbusODOldValue_Table[byPort][HeadIndex].bySendCount >= 2) //某笔05发送3次仍没收到回复，则
	{
		SetValueToOldODValue(byPort, HeadIndex, dbValue);
		ModbusODOldValue_Table[byPort][HeadIndex].bySendCount = 0;
	}

	return 8;
}

/*
 * 作为Master时，0x06功能码组包函数
 */
WORD ComposeData_WriteSingleRegister(BYTE byPort, BYTE *byBuf)
{
	DBVALUE 	dbValue;
	WORD HeadIndex = ModbusOffset[byPort].start;

	memcpy(&byBuf[0], &ModbusOD_Table[byPort][HeadIndex].id, sizeof(BYTE));
	memcpy(&byBuf[1], &ModbusOD_Table[byPort][HeadIndex].cmd, sizeof(BYTE));
	memcpy(&byBuf[2], &ModbusOD_Table[byPort][HeadIndex].addr, sizeof(WORD));
	dbValue = Get_ODValue_ByIndex(byPort, HeadIndex);
	memcpy(&byBuf[4], &dbValue.wData, sizeof(WORD_T));

	if(dbValue.dwData == ModbusODOldValue_Table[byPort][HeadIndex].DbValue.dwData)
		return 0;

	ModbusODOldValue_Table[byPort][HeadIndex].byUpdate = 1;
	Sent_CRC(byBuf, 8);

	ModbusODOldValue_Table[byPort][HeadIndex].bySendCount++;

	if(ModbusODOldValue_Table[byPort][HeadIndex].bySendCount >= 2) //某笔06发送3次仍没收到回复，则
	{
		SetValueToOldODValue(byPort, HeadIndex, dbValue);
		ModbusODOldValue_Table[byPort][HeadIndex].bySendCount = 0;
	}

	return 8;
}

/*
 * 作为Master时，0x0F功能码组包函数
 */
WORD CopmoseData_WriteMultipleCoils(BYTE byPort, BYTE *byBuf)
{
	DBVALUE 	dbValue;
	BYTE	byFlag = 0;
	int i = 0;
	DWORD dwValue = 0;
	WORD HeadIndex = ModbusOffset[byPort].start;
	WORD TailIndex = ModbusOffset[byPort].end;

	WORD wRegNum = TailIndex - HeadIndex + 1;
	BYTE byLength = 0;
	memcpy(&byBuf[0], &ModbusOD_Table[byPort][HeadIndex].id, sizeof(BYTE));
	memcpy(&byBuf[1], &ModbusOD_Table[byPort][HeadIndex].cmd, sizeof(BYTE));
	memcpy(&byBuf[2], &ModbusOD_Table[byPort][HeadIndex].addr, sizeof(WORD));
	WORD wValue = htons(wRegNum);
	memcpy(&byBuf[4], &wValue, sizeof(WORD));

	for(i = 0; i < wRegNum; i++)
	{
		dbValue = Get_ODValue_ByIndex(byPort, HeadIndex+i);
		if(dbValue.dwData != 0)
		{
			dwValue |= (0x1 << i);
		}
		else
		{
			dwValue &= ~(0x1 << i);
		}

		if(dbValue.dwData != ModbusODOldValue_Table[byPort][HeadIndex+i].DbValue.dwData)
		{
			byFlag = 1;
			ModbusODOldValue_Table[byPort][HeadIndex+i].byUpdate = 1;
		}
	}

	if(byFlag == 0) return 0;

	byLength = (wRegNum-1)/8 + 1;
	memcpy(&byBuf[6], &byLength, sizeof(BYTE));
	memcpy(&byBuf[7], &dwValue, byLength);

	Sent_CRC(byBuf, 9+byLength);

	ModbusODOldValue_Table[byPort][HeadIndex].bySendCount++;

	if(ModbusODOldValue_Table[byPort][HeadIndex].bySendCount >= 2) //某笔10发送3次仍没收到回复，则
	{
		for(i = 0; i < wRegNum; i++)
		{
			dbValue = Get_ODValue_ByIndex(byPort, HeadIndex+i);
			SetValueToOldODValue(byPort, HeadIndex+i, dbValue);
			ModbusODOldValue_Table[byPort][HeadIndex+i].bySendCount = 0;
		}
	}
	return 9+byLength;
}

/*
 * 作为Master时，0x10功能码组包函数
 */
WORD ComposeData_WriteMultipleRegisters(BYTE byPort, BYTE *byBuf)
{
	DBVALUE 	dbValue;
	BYTE	byFlag = 0;
	int i = 0;
	WORD HeadIndex = ModbusOffset[byPort].start;
	WORD TailIndex = ModbusOffset[byPort].end;

	WORD wRegNum = TailIndex - HeadIndex + 1;
	BYTE byLength = wRegNum*2;
	memcpy(&byBuf[0], &ModbusOD_Table[byPort][HeadIndex].id, sizeof(BYTE));
	memcpy(&byBuf[1], &ModbusOD_Table[byPort][HeadIndex].cmd, sizeof(BYTE));
	memcpy(&byBuf[2], &ModbusOD_Table[byPort][HeadIndex].addr, sizeof(WORD));
	WORD wValue = htons(wRegNum);
	memcpy(&byBuf[4], &wValue, sizeof(WORD));
	memcpy(&byBuf[6], &byLength, sizeof(BYTE));
	for(i = 0; i < wRegNum; i++)
	{
		dbValue = Get_ODValue_ByIndex(byPort, HeadIndex+i);
		memcpy(&byBuf[7+i*2], &dbValue.wData, sizeof(WORD));
		if(dbValue.dwData != ModbusODOldValue_Table[byPort][HeadIndex+i].DbValue.dwData)
		{
			byFlag = 1;
			ModbusODOldValue_Table[byPort][HeadIndex+i].byUpdate = 1;
		}
	}

	if(byFlag == 0) return 0;
	Sent_CRC(byBuf, 9+byLength);

	ModbusODOldValue_Table[byPort][HeadIndex].bySendCount++;

	if(ModbusODOldValue_Table[byPort][HeadIndex].bySendCount >= 2) //某笔10发送3次仍没收到回复，则
	{
		for(i = 0; i < wRegNum; i++)
		{
			dbValue = Get_ODValue_ByIndex(byPort, HeadIndex+i);
			SetValueToOldODValue(byPort, HeadIndex+i, dbValue);
			ModbusODOldValue_Table[byPort][HeadIndex+i].bySendCount = 0;
		}
	}

	return 9+byLength;
}

/*
 *  作为Master时，根据功能码不同，调用不同函数组包
 */
WORD ComposeData_Package(BYTE byPort, BYTE *byBuf)
{
	BYTE 		byFC = 0;

	if(byPort >= MAX_RS485) return 0;

	WORD HeadIndex = ModbusOffset[byPort].start;
	byFC = ModbusOD_Table[byPort][HeadIndex].cmd;
	switch (byFC) {
		case MODBUS_FC_READ_COILS:
		case MODBUS_FC_READ_DISCRETE_INPUTS:
		case MODBUS_FC_READ_HOLDING_REGISTERS:
		case MODBUS_FC_READ_INPUT_REGISTERS:
			return ComposeData_ReadHoldingRegisters(byPort, byBuf);
		case MODBUS_FC_WRITE_SINGLE_COIL:
			return ComposeData_WriteSingleCoil(byPort, byBuf);
		case MODBUS_FC_WRITE_SINGLE_REGISTER:
			return ComposeData_WriteSingleRegister(byPort, byBuf);
		case MODBUS_FC_WRITE_MULTIPLE_COILS:
			return CopmoseData_WriteMultipleCoils(byPort, byBuf);
		case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
			return ComposeData_WriteMultipleRegisters(byPort, byBuf);
		default:
			break;
	}

	return 0;
}

void SetValueToOldODValue(BYTE byPort, WORD wIndex, DBVALUE DbValue)
{
	if(byPort >= MAX_RS485 || wIndex >= MAX_MODBUSOD) return;
	if(ModbusODOldValue_Table[byPort][wIndex].byUpdate == 1)
	{
		memcpy(&ModbusODOldValue_Table[byPort][wIndex].DbValue, &DbValue, sizeof(DbValue));
		ModbusODOldValue_Table[byPort][wIndex].byUpdate = 0;
	}
}

void Sent_CRC(BYTE *pBuf, WORD Length)
{
	BYTE_T 	CRCL, CRCH;
	WORD_T 	CRC;

	CRC = CRC_check(pBuf, Length-CRC_SIZE);
	CRCL = CRC&0x00FF;
	CRCH = (CRC&0xFF00)>>8;
	memcpy(&pBuf[Length-CRCL_SIZE], &CRCL, 1);
	memcpy(&pBuf[Length-CRCH_SIZE], &CRCH, 1);
}
WORD_T glen,gcrc;
BYTE_T Recv_CRC(BYTE_T *pBuf, WORD_T Length)
{
	BYTE_T CRCL, CRCH, temp_crch, temp_crcl;
	WORD_T CRC;
	glen=Length;
	temp_crcl = pBuf[Length - 2];
	temp_crch = pBuf[Length - 1];
	gcrc=CRC = CRC_check(pBuf, Length - 2);
	CRCL = CRC&0x00FF;
	CRCH = (CRC&0xFF00)>>8;
	if((temp_crch == CRCH)&&(temp_crcl == CRCL)) {
		return 1;
	}

	return 0;
}

void Setp_Modbus(BYTE byPort)
{
	int 		HeadIndex, TailIndex, Line;

	if(byPort >= MAX_RS485) return;

	Line = g_ModbusPara.wRegNum[byPort];
	HeadIndex = Setp[byPort];
	ModbusOffset[byPort].start = HeadIndex;
	for (; HeadIndex < Line; HeadIndex++) {
		TailIndex = HeadIndex + 1;
		if (TailIndex >= Line) {
			ModbusOffset[byPort].end = HeadIndex;
			Setp[byPort] = 0;
			return;
		}
		/*Modbus 分包处理：
		 * 功能码不连续分包、通讯地址不连续分包、寄存器地址不连续分包、连续寄存器大于32个分包
		 */
		if (ModbusOD_Table[byPort][HeadIndex].cmd != ModbusOD_Table[byPort][TailIndex].cmd
		    || ModbusOD_Table[byPort][HeadIndex].id != ModbusOD_Table[byPort][TailIndex].id
			|| htons(ModbusOD_Table[byPort][HeadIndex].addr) != htons(ModbusOD_Table[byPort][TailIndex].addr)-1
			|| htons(ModbusOD_Table[byPort][TailIndex].addr) - htons(ModbusOD_Table[byPort][Setp[byPort]].addr) >= 32) {
			break;
		}
		/*
		 * 当功能码为06时，只能写单个寄存器，即使06功能码有连续寄存器，也必须进行拆分
		 */
		if(ModbusOD_Table[byPort][HeadIndex].cmd == MODBUS_FC_WRITE_SINGLE_REGISTER \
		   || ModbusOD_Table[byPort][HeadIndex].cmd == MODBUS_FC_WRITE_SINGLE_COIL)
		{
			break;
		}
	}
	ModbusOffset[byPort].end = HeadIndex;
	Setp[byPort] = HeadIndex + 1;


	return;
}

DBVALUE Get_ODValue_ByIndex(BYTE_T byPort, WORD_T Index)
{
	DBVALUE 	dbValue;

	memset(&dbValue, 0, sizeof(dbValue));

	if(byPort >= MAX_RS485 || Index >= MAX_MODBUSOD) return dbValue;

	memcpy(&dbValue, &ModbusODValue_Table[byPort][Index].writevalue, sizeof(DBVALUE));

	return dbValue;
}

void Recv_ModbusResponse(BYTE_T Port)
{
	BYTE_T Recv_Buf[128];
	WORD_T Length = 0;

	memset(Recv_Buf, 0, sizeof(Recv_Buf));

	Length = Uart_ReadDev(Uart_Type[RS485_TYPE], Port, Recv_Buf, sizeof(Recv_Buf), 0);

	if (Length > 0) {
		if(Recv_CRC(Recv_Buf, Length) == TRUE) {
			HandleData(Port, Recv_Buf);
		}
	}

	return;
}

/*
 * 作为Mater，解析0x01/0x02功能码回复数据
 */
void ParseData_ReadCoils(BYTE byPort,  BYTE *pbyBuf)
{
	int i;
	WORD HeadIndex 	= ModbusOffset[byPort].start;
	WORD TailIndex 	= ModbusOffset[byPort].end;

	BYTE byLength = pbyBuf[2];   //数据长度
	if(byLength > sizeof(WORD))	 return;
	WORD wValue = 0;
	memcpy(&wValue, &pbyBuf[3], sizeof(BYTE)* byLength);
	if(byLength == 2) wValue = htons(wValue);

	for(i = HeadIndex; i <= TailIndex; i++)
	{
		if(wValue & (0x01 << (i - HeadIndex)))
		{
			ModbusODValue_Table[byPort][i].readvalue.dwData = 1;
		}
		else
		{
			ModbusODValue_Table[byPort][i].readvalue.dwData = 0;
		}
	}
}

/*
 * 作为Mater，解析0x03/0x04功能码回复数据
 */
void ParseData_ReadHoldingRegister(BYTE byPort, BYTE* pbyBuf)
{
	WORD HeadIndex 	= ModbusOffset[byPort].start;
	WORD TailIndex 	= ModbusOffset[byPort].end;
	WORD HeadAddr 	= ModbusOD_Table[byPort][HeadIndex].addr;

	memcpy(&ModbusODValue_Table[byPort][HeadIndex].readvalue, &pbyBuf[3], sizeof(WORD_T));
	HeadIndex++;
	for(; HeadIndex <= TailIndex; HeadIndex++) {
		memcpy(&ModbusODValue_Table[byPort][HeadIndex].readvalue, &pbyBuf[((htons(ModbusOD_Table[byPort][HeadIndex].addr) - htons(HeadAddr)) * sizeof(WORD_T) + 3)], sizeof(WORD_T));
	}
}

/*
 * 作为Master，处理slave回复数据
 */
void HandleData(BYTE_T byPort, BYTE_T *pBuf)
{
	int i = 0;
	DBVALUE DbValue;

	if(byPort >= MAX_RS485) return;

	WORD HeadIndex 	= ModbusOffset[byPort].start;
	WORD TailIndex 	= ModbusOffset[byPort].end;

	BYTE byFC = pBuf[1];

	switch (byFC) {
		case MODBUS_FC_READ_COILS:
		case MODBUS_FC_READ_DISCRETE_INPUTS:
			ParseData_ReadCoils(byPort, pBuf);
			break;
		case MODBUS_FC_READ_HOLDING_REGISTERS:
		case MODBUS_FC_READ_INPUT_REGISTERS:
			ParseData_ReadHoldingRegister(byPort, pBuf);
			break;
		case MODBUS_FC_WRITE_SINGLE_COIL:
		case MODBUS_FC_WRITE_SINGLE_REGISTER:
		case MODBUS_FC_WRITE_MULTIPLE_COILS:
		case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
			ModbusODOldValue_Table[byPort][HeadIndex].bySendCount = 0;
			for(i = HeadIndex; i <= TailIndex; i++)
			{
				DbValue = Get_ODValue_ByIndex(byPort, i);
				SetValueToOldODValue(byPort, i, DbValue);
			}
			break;
		default:
			break;
	}

	return;
}


/*
 * 作为slave时，处理0x01/0x02功能码回复,只支持output/input的寄存器读操作
 */
WORD Response_ReadDiscreteInputs(BYTE_T byPort, BYTE* pBuf, WORD wLen)
{
	int 		Offset;
	BYTE		byValue = 0;
	WORD_T 		HeadAddr;
	WORD 		wRegNum = 0;
	WORD		wLength = 0;
	BYTE		byLength = 1;
	WORD		wValue = 0;
	int			i = 0;

	if(byPort >= MAX_RS485 || wLen != 8) return 0;

	memcpy(&wValue, pBuf+2, sizeof(wValue));
	HeadAddr = htons(wValue);      //起始寄存器地址
	memcpy(&wValue, pBuf+4, sizeof(wValue));
	wRegNum = htons(wValue);    //寄存器数量
	if(wRegNum > 16) return 0;   //一次读取寄存器长度不大于16个

	memset(SendBuf, 0, sizeof(SendBuf));
	memcpy(SendBuf, pBuf, 2);   //地址、功能码
	wLength += 2;
	if(wRegNum > 8) byLength = 2;
	memcpy(SendBuf+wLength, &byLength, sizeof(byLength));  //数据长度
	wLength += 1;

	/*
	 * 数据区组包
	 */
	wValue = 0;
	for(i = 0; i < wRegNum; i++)
	{
		Offset = FindRegAddr(HeadAddr+i);
		if (Offset == -1) {
			wValue &= ~(0x1 << i);
		}
		else
		{
			if(ModbusRegValue_Table[Offset].value == 0)
			{
				wValue &= ~(0x1 << i);
			}
			else
			{
				wValue |=  (0x1 << i);
			}
		}
	}

	if(wRegNum > 8)
	{
		wValue = htons(wValue);
		memcpy(SendBuf+wLength, &wValue, sizeof(wValue));  //数据
		wLength += 2;
	}
	else
	{
		byValue = wValue & 0xFF;
		memcpy(SendBuf+wLength, &byValue, sizeof(byValue));  //数据
		wLength += 1;
	}
	wLength += 2;
	Sent_CRC(SendBuf, wLength);

	Uart_WriteDev(Uart_Type[RS485_TYPE], byPort, SendBuf, wLength, 0);
	return wLength;
}

/*
 * 作为slave时，处理0x03/0x04功能码回复
 */
WORD Response_ReadHoldingRegisters(BYTE_T byPort, BYTE* pBuf, WORD wLen)
{
	int 		Offset;
	WORD_T		Value = 0;
	WORD_T 		HeadAddr;
	int 		wRegNum = 0;
	BYTE		byLength = 0;
	WORD		wLength = 0;
	WORD		wValue = 0;

	if(byPort >= MAX_RS485) return 0;

	memcpy(&wValue, pBuf+2, sizeof(wValue));
	HeadAddr = htons(wValue);      //起始寄存器地址
	memcpy(&wValue, pBuf+4, sizeof(wValue));
	wRegNum = htons(wValue);    //寄存器数量
	byLength = wRegNum*2;       //数据长度

	memset(SendBuf, 0, sizeof(SendBuf));
	memcpy(SendBuf, pBuf, 2);   //地址、功能码
	wLength += 2;
	memcpy(SendBuf+wLength, &byLength, sizeof(byLength));  //数据长度
	wLength += 1;

	/*
	 * 数据区组包
	 */
	while (wRegNum--) {
		Offset = FindRegAddr(HeadAddr);
		if(wLength >= sizeof(SendBuf)) return 0;
		if (Offset == -1) {
			memcpy(SendBuf+wLength, &Value, sizeof(WORD_T));
			wLength += 2;
		}else {
			memcpy(SendBuf+wLength, &ModbusRegValue_Table[Offset].value, sizeof(WORD_T));
			wLength += 2;
		}
		HeadAddr++;
	}

	wLength += 2;
	Sent_CRC(SendBuf, wLength);

	Uart_WriteDev(Uart_Type[RS485_TYPE], byPort, SendBuf, wLength, 0);
	return wLength;
}

/*
 * 作为slave时，处理0x05功能码回复,只支持output的寄存器写功能
 */
WORD Response_WiriteSingleCoil(BYTE_T byPort, BYTE* pBuf, WORD wLen)
{
	int 		Offset;
	WORD_T 		HeadAddr;
	WORD		wValue = 0;

	if(byPort >= MAX_RS485 || wLen != 8) return 0;

	memcpy(&wValue, pBuf+2, sizeof(wValue));
	HeadAddr = htons(wValue);      //起始寄存器地址
	memcpy(&wValue, pBuf+4, sizeof(wValue));
	wValue = htons(wValue);       //数值value
	Offset = FindRegAddr(HeadAddr);
	if(Offset == -1) return 0;

	if(wValue == 0xFF00)
	{
		ModbusRegValue_Table[Offset].value = 0xFF;
	}
	else if(wValue == 0x0)
	{
		ModbusRegValue_Table[Offset].value = 0x0;
	}
	else
	{
		return 0;
	}

	memcpy(SendBuf, pBuf, wLen);   //回复数据与接收数据一致
	Uart_WriteDev(Uart_Type[RS485_TYPE], byPort, SendBuf, wLen, 0);
	return wLen;
}

/*
 * 作为slave时，处理0x06功能码回复
 */
WORD Response_WriteSingleRegister(BYTE_T byPort, BYTE* pBuf, WORD wLen)
{
	int 		Offset;
	WORD_T 		HeadAddr;
	WORD		wValue = 0;

	if(byPort >= MAX_RS485 || wLen != 8) return 0;

	memcpy(&wValue, pBuf+2, sizeof(wValue));
	HeadAddr = htons(wValue);      //起始寄存器地址
	memcpy(&wValue, pBuf+4, sizeof(wValue));
	wValue = htons(wValue);       //数值value
	Offset = FindRegAddr(HeadAddr);
	if(Offset == -1) return 0;
	memcpy(&ModbusRegValue_Table[Offset].value, &wValue, sizeof(WORD_T));

	memcpy(SendBuf, pBuf, wLen);   //回复数据与接收数据一致
	Uart_WriteDev(Uart_Type[RS485_TYPE], byPort, SendBuf, wLen, 0);
	return wLen;
}

/*
 * 作为slave时，处理0x10功能码及回复
 */
WORD Response_WriteMultipleRegister(BYTE_T byPort, BYTE* pBuf, WORD wLen)
{
	int 		Offset;
	WORD_T 		HeadAddr;
	WORD		wValue = 0;
	WORD		wRegNum= 0;
	BYTE		byLength = 0;
	int			i = 0;

	if(byPort >= MAX_RS485 && wLen < 8) return 0;

	memcpy(&wValue, pBuf+2, sizeof(wValue));
	HeadAddr = htons(wValue);      //起始寄存器地址
	memcpy(&wValue, pBuf+4, sizeof(wValue));
	wRegNum = htons(wValue);       //寄存去个数
	memcpy(&byLength, pBuf+6, sizeof(byLength));   //数据长度

	if(byLength != wRegNum*2) return 0;
	for(i = 0; i < wRegNum; i++)
	{
		Offset = FindRegAddr(HeadAddr+i);
		if(Offset == -1) continue;

		memcpy(&wValue, pBuf+7+2*i, sizeof(wValue));
		wValue = htons(wValue);
		memcpy(&ModbusRegValue_Table[Offset].value, &wValue, sizeof(WORD_T));
	}

	memcpy(SendBuf, pBuf, 6);
	Sent_CRC(SendBuf, 8);
	Uart_WriteDev(Uart_Type[RS485_TYPE], byPort, SendBuf, 8, 0);
	return 8;
}

/*
 * 作为slave时，处理0xF功能码及回复,只支持output的寄存器写功能
 */
WORD Response_WriteMultipleCoils(BYTE_T byPort, BYTE* pBuf, WORD wLen)
{
	int 		Offset;
	WORD_T 		HeadAddr;
	WORD 		wRegNum = 0;
	BYTE		byLength = 0;
	WORD		wValue = 0;
	int			i = 0;

	if(byPort >= MAX_RS485 || wLen < 8) return 0;

	memcpy(&wValue, pBuf+2, sizeof(wValue));
	HeadAddr = htons(wValue);      //起始寄存器地址
	memcpy(&wValue, pBuf+4, sizeof(wValue));
	wRegNum = htons(wValue);    //寄存器数量
	if(wRegNum > 16) return 0;   //一次写寄存器长度不大于16个
	byLength = pBuf[6];
	if(byLength > 2) return 0;  //数据长度不大于2BYTE

	if(byLength == 1 && wRegNum <= 8)
	{
		wValue = pBuf[7];
	}
	else if(byLength == 2 && wRegNum > 8)
	{
		memcpy(&wValue, pBuf+7, sizeof(wValue));
		wValue = htons(wValue);
	}
	else return 0;

	for(i = 0; i < wRegNum; i++)
	{
		Offset = FindRegAddr(HeadAddr+i);
		if(Offset == -1) continue;
		if(wValue & (0x1 << i))
		{
			ModbusRegValue_Table[Offset].value = 0xFF;
		}
		else
		{
			ModbusRegValue_Table[Offset].value = 0x0;
		}
	}

	memset(SendBuf, 0, sizeof(SendBuf));
	memcpy(SendBuf, pBuf, 6);   //回复数据与接收数据前6字节相同
	Sent_CRC(SendBuf, 8);
	Uart_WriteDev(Uart_Type[RS485_TYPE], byPort, SendBuf, 8, 0);
	return 8;
}

void Recv_ModbusRequest(BYTE_T byPort)
{
	BYTE_T 		Recv_Buf[128];
	WORD_T 		Recv_Len = 0;
	BYTE		byFC;
	BYTE		byAddr;

	memset(Recv_Buf,	0, 	sizeof(Recv_Buf));

	if(byPort >= MAX_RS485) return;

	Recv_Len = Uart_ReadDev(Uart_Type[RS485_TYPE], byPort, Recv_Buf, sizeof(Recv_Buf), 0);

	if (Recv_Len == 0) return;

	if (Recv_CRC(Recv_Buf, Recv_Len))
	{
		byAddr = Recv_Buf[0];   //通讯地址
		byFC = Recv_Buf[1];		//功能码
		if(byAddr == RS485StartPara[byPort].nodeid) {
			switch (byFC)
			{
				case MODBUS_FC_READ_COILS:
				case MODBUS_FC_READ_DISCRETE_INPUTS:
					Response_ReadDiscreteInputs(byPort, Recv_Buf, Recv_Len);
					break;
				case MODBUS_FC_READ_HOLDING_REGISTERS:
				case MODBUS_FC_READ_INPUT_REGISTERS:
					Response_ReadHoldingRegisters(byPort, Recv_Buf, Recv_Len);
					break;
				case MODBUS_FC_WRITE_SINGLE_COIL:
					Response_WiriteSingleCoil(byPort, Recv_Buf, Recv_Len);
					break;
				case MODBUS_FC_WRITE_SINGLE_REGISTER:
					Response_WriteSingleRegister(byPort, Recv_Buf, Recv_Len);
					break;
				case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
					Response_WriteMultipleRegister(byPort, Recv_Buf, Recv_Len);
					break;
				case MODBUS_FC_WRITE_MULTIPLE_COILS:
					Response_WriteMultipleCoils(byPort, Recv_Buf, Recv_Len);
					break;
				default:
					break;
			}
		}
	}
	return;
}

/******************************************外部使用********************************************/
void InitMulRS485(void)
{
	if (RS485StartPara[RS485_1].port == RS485_1) {
		InitRS485(RS485_DEFAULT_DISABLE, RS485StartPara[RS485_1].port);
	}
#ifdef M2C2132
	if (RS485StartPara[RS485_2].port == RS485_2) {
		InitRS485(RS485_DEFAULT_DISABLE, RS485StartPara[RS485_2].port);
	}
#endif
	return;
}

void CreateMulTaskRS485(void)
{
	if (RS485StartPara[RS485_1].port == RS485_1) {
		RS485Task[RS485_1] = CreateTaskRS485(RS485StartPara[RS485_1].port);
	}
#ifdef M2C2132
	if (RS485StartPara[RS485_2].port == RS485_2) {
		RS485Task[RS485_2] = CreateTaskRS485(RS485StartPara[RS485_2].port);
	}
#endif
	return;
}

void DestroyMulTaskRS485(void)
{
	if (RS485StartPara[RS485_1].port == RS485_1) {
		Destroy_Task(RS485Task[RS485_1]);
	}
#ifdef M2C2132
	if (RS485StartPara[RS485_2].port == RS485_2) {
		Destroy_Task(RS485Task[RS485_2]);
	}
#endif
	return;
}

void  Get_RegValue_byDataID_2(DWORD_T id, DBVALUE *dbValue)   //zzy   06 S    2019/01/03
{
	int i, j;
	int Offset;

	for (i = 0; i < MAX_RS485; i++) {
		for (j = 0; j < MAX_MODBUSREG; j++) {
			if (htonl(ModbusReg_Table[i][j].regdataid) == id) {
				Offset = FindRegAddr(htons(ModbusReg_Table[i][j].regaddr));
				if (Offset == -1) {
					return;
				}
				memcpy(dbValue, &ModbusRegValue_Table[Offset].value, sizeof(WORD_T));
				return;
			}
		}
	}
	return;
}

void Set_RegValue_byDataID_2(DWORD_T id, WORD_T value)    //zzy     03/04 S     2019/01/03
{
	int i,j;
	int Offset;

	for (i = 0; i < MAX_RS485; i++) {
		for (j = 0; j < MAX_MODBUSREG; j++) {
			if(htonl(ModbusReg_Table[i][j].regdataid) == id) {
				Offset = FindRegAddr(htons(ModbusReg_Table[i][j].regaddr));
				if (Offset == -1) {
					return;
				}
				value = htons(value);
				memcpy(&ModbusRegValue_Table[Offset].value, &value, sizeof(WORD_T));
				return;
			}
		}
	}
	return;
}

void Get_ODValueByDataID_2(DWORD_T id, DBVALUE *dbValue)  //zzy  03/04 M     2019/01/03
{
	int i, j;

	for (i = 0; i < MAX_RS485; i++) {
		for (j = 0; j < MAX_MODBUSOD; j++) {
			if (ModbusODValue_Table[i][j].dataid == id){
				dbValue->wData = htons(ModbusODValue_Table[i][j].readvalue.wData);
				return;
			}
		}
	}
	return;
}

void Set_ODValueByDataID_2(DWORD_T id, DWORD_T value)  //zzy 06 M    2019/01/03
{
	int i, j;

	for (i = 0; i < MAX_RS485; i++) {
		for (j = 0; j < MAX_MODBUSOD; j++) {
			if (ModbusODValue_Table[i][j].dataid == id) {
				value = htons(value);
				memcpy(&ModbusODValue_Table[i][j].writevalue, &value, sizeof(DWORD_T));
				return;
			}
		}
	}
	return;
}

int FindRegAddr(WORD_T addr)
{
	int i;

	for (i = 0; i < MAX_MODBUSREG; i++) {
		if (htons(ModbusRegValue_Table[i].addr) == addr) {
			return i;
		}
	}
	return -1;
}

/*===========================================================================+
|Name:         InitOD_Table    				                             	 |
|Description:  													             |
+===========================================================================*/
void InitModbusTable()
{
	int i = 0;
	BYTE byPort = 0;
	WORD wTempBuf[MAX_MODBUSREG*2];
	WORD wRegNum = 0;
	DWORD dwValue = 0;

	//初始化
	memset(&ModbusODOldValue_Table, -1, sizeof(ModbusODOldValue_Table));
	memset(wTempBuf, 0, sizeof(wTempBuf));

	for(byPort = 0; byPort < MAX_RS485; byPort++)
	{
		wRegNum = g_ModbusPara.wRegNum[byPort];
		switch (RS485StartPara[byPort].mode) {
			case MODBUS_SLAVE:
				Reorder(wTempBuf);
				for (i = 0; i < sizeof(ModbusRegValue_Table)/sizeof(ModbusRegValue_Table[0]); i++) {
					memcpy(&ModbusRegValue_Table[i].addr, &wTempBuf[i], sizeof(WORD_T));
				}
				break;
			case MODBUS_MASTER:
				for (i = 0; i < wRegNum ; i++) {
					dwValue = htonl(ModbusOD_Table[byPort][i].dataid);
					memcpy(&ModbusODValue_Table[byPort][i].dataid, &dwValue, sizeof(DWORD_T));
				}
				break;
			default:
				break;
		}
	}
}
/*===========================================================================+
|Name:         Read_ModbusData_FromFlash                                 	 |
|Description:  													             |
+===========================================================================*/
void 	Read_ModbusData_FromFlash(void)
{
	memset(&g_ModbusPara, 0, sizeof(g_ModbusPara));
	memset(RS485StartPara, 0, sizeof(RS485StartPara));
	memset(ModbusOD_Table, 0, sizeof(ModbusOD_Table));
	memset(ModbusReg_Table, 0, sizeof(ModbusReg_Table));
	Flash_Read((BYTE*)&g_ModbusPara, sizeof(g_ModbusPara), ModbusOD_Block, MODBUSDATA_OFFSET);
	Flash_Read((BYTE*)RS485StartPara,sizeof(RS485StartPara), ModbusOD_Block, MODBUSPARA_OFFSET);
	Flash_Read((BYTE*)ModbusOD_Table,sizeof(ModbusOD_Table), ModbusOD_Block, MODBUSOD1_OFFSET);
	Flash_Read((BYTE*)ModbusReg_Table,sizeof(ModbusReg_Table), ModbusOD_Block, MODBUSOD2_OFFSET);

	if (g_ModbusPara.byEnable == 1) {
		InitModbusTable();
	}
}
/*===========================================================================+
|Name:         Read_ModbusData_FromStudio                                    |
|Description:  										                         |
+===========================================================================*/
void Read_ModbusData_FromStudio(void)
{
	InitModbusTable();
}
/*===========================================================================+
|Name:         Reorder                                    					 |
|Description:  去重和升序功能					                         		 |
+===========================================================================*/
void Reorder(WORD_T *dest)
{
	int i,j;
	int index = 1;
	WORD_T src[MAX_MODBUSREG*2];
	WORD_T tmp;


	memset(&src,  0x0, sizeof(src));

	for (i = 0; i < MAX_MODBUSREG; i++) {
		memcpy(&src[i], &ModbusReg_Table[RS485_1][i].regaddr, sizeof(WORD_T));
		memcpy(&src[125+i],	&ModbusReg_Table[RS485_2][i].regaddr, sizeof(WORD_T));
	}

//	for(i = 125; i < MAX_MODBUSREG*2; i++) {
//		memcpy(&src[i],	&ModbusReg_Table[RS485_2][i].regaddr, sizeof(WORD_T));
//	}

	for(i = 0; i < MAX_MODBUSREG*2-1; i++) {
		for(j = 0; j < MAX_MODBUSREG*2-1-i; j++) {
			if(src[j] > src[j+1]) {
				tmp = src[j];
				src[j] = src[j+1];
				src[j+1] = tmp;
			}
		}
	}

	dest[0] = src[0];

	for(i = 1; i < MAX_MODBUSREG*2; i++) {
		if(src[i] != src[i-1]){
			dest[index++] = src[i];
        }
	}
}

/*===========================================================================+
|Name:         Read_ModbusData_FromStudio                                    |
|Description:  										                         |
+===========================================================================*/
BOOL SetRs485Para(BYTE *byData, WORD wLen)
{
	if(wLen != sizeof(RS485StartPara)) return false;
	memcpy(RS485StartPara, byData, wLen);
	return true;
}
