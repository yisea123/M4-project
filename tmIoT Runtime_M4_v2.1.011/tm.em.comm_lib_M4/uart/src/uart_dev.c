/*===========================================================================+
|  Function : Uart                                         					 |
|  Task     : Uart                             								 |
|----------------------------------------------------------------------------|
|  Compile  : CCS 6.1                                                        |
|  Link     : CCS 6.1                                                        |
|  Call     :                                                                |
|----------------------------------------------------------------------------|
|  Author   : jy                                                             |
|  Version  : V1.00                                                          |
|  Creation : 2018.2.9                                                       |
|  Revision :                                                                |
+===========================================================================*/
#include "uart/include/uart_dev.h"

const int Uart_Type[MAX_UART] = {RS232_TYPE, RS485_TYPE};

const UARTBAUDRATE_T 	m_Baud[] = {
	{MODBUS_BAUDRATE_1200BPS,  			1200},
	{MODBUS_BAUDRATE_2400BPS,  			2400},
	{MODBUS_BAUDRATE_4800BPS,  			4800},
	{MODBUS_BAUDRATE_9600BPS,  			9600},
	{MODBUS_BAUDRATE_14400BPS, 		   14400},
	{MODBUS_BAUDRATE_19200BPS, 		   19200},
	{MODBUS_BAUDRATE_38400BPS, 		   38400},
	{MODBUS_BAUDRATE_56000BPS, 		   56000},
	{MODBUS_BAUDRATE_57600BPS, 		   57600},
	{MODBUS_BAUDRATE_115200BPS,		  115200},
	{MODBUS_BAUDRATE_128000BPS,		  128000}
};

const UARTDATABIT_T	m_DataBit[] = {
	{MODBUS_DATA_BIT5,	UART_CONFIG_WLEN_5},
	{MODBUS_DATA_BIT6,	UART_CONFIG_WLEN_6},
	{MODBUS_DATA_BIT7,	UART_CONFIG_WLEN_7},
	{MODBUS_DATA_BIT8,	UART_CONFIG_WLEN_8}
};

const UARTPARITY_T	m_Parity[] = {
	{MODBUS_NONE_PARITY,	UART_CONFIG_PAR_NONE},
	{MODBUS_ODD_PARITY,		UART_CONFIG_PAR_ODD},
	{MODBUS_EVEN_PARITY,	UART_CONFIG_PAR_EVEN }
};


const UARTSTOPBIT_T	m_Stopbit[] = {
	{MODBUS_STOP_BIT1,	UART_CONFIG_STOP_ONE},
	{MODBUS_STOP_BIT2,	UART_CONFIG_STOP_TWO}
};

void Uart_WriteDev(int Port_Type, BYTE_T u_wChannel, BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
{
	if (Port_Type == NONE_TYPE)
		return;

	if(Port_Type == RS232_TYPE) {
//		switch(u_wChannel){
//			case RS232_1:
//				_RS232_Write1(pData, wSize, Offset);
//				break;
//			case RS232_2:
//				_RS232_Write2(pData, wSize, Offset);
//				break;
//	        default:
//	        	break;
//		}
	}

	if(Port_Type == RS485_TYPE) {
		switch(u_wChannel){
			case RS485_1:
				_RS485_Write1(pData, wSize, Offset);
				break;
			case RS485_2:
				_RS485_Write2(pData, wSize, Offset);
				break;
	        default:
	        	break;
		}
	}

	return;
}

WORD_T Uart_ReadDev(int Port_Type, BYTE_T u_wChannel, BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset)
{
	WORD_T Len;

	if (Port_Type == NONE_TYPE)
		return 0;

	if(Port_Type == RS485_TYPE) {
		switch(u_wChannel) {
			case RS485_1:
				Len = _RS485_Read1(pData, wSize, Offset);
				break;
			case RS485_2:
				Len = _RS485_Read2(pData, wSize, Offset);
				break;
		}
	}
	return Len;
}

DWORD Get_UartPaud(BYTE index)
{
	int i;

	for (i = 0; i < sizeof(m_Baud)/sizeof(UARTBAUDRATE_T); i++) {
		if(index == m_Baud[i].u_baud) {
			return m_Baud[i].g_baud;
		}
	}
	return 0;
}


DWORD Get_UartDataBit(BYTE index)
{
	int i;

	for (i = 0; i < sizeof(m_DataBit)/sizeof(UARTDATABIT_T); i++) {
		if(index == m_DataBit[i].u_databit) {
			return m_DataBit[i].g_databit;
		}
	}
	return 0;
}


DWORD Get_UartParity(BYTE index)
{
	int i;

	for (i = 0; i < sizeof(m_Parity)/sizeof(UARTPARITY_T); i++) {
		if(index == m_Parity[i].u_parity) {
			return m_Parity[i].g_parity;
		}
	}
	return 0;
}

DWORD Get_UartStopBit(BYTE index)
{
	int i;

	for (i = 0; i < sizeof(m_Stopbit)/sizeof(UARTSTOPBIT_T); i++) {
		if(index == m_Stopbit[i].u_stopbit) {
			return m_Stopbit[i].g_stopbit;
		}
	}
	return 0;
}
