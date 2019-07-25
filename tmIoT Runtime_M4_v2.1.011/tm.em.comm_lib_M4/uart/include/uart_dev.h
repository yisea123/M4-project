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
//#include "driver/include/_rs232.h"
#include "driver/include/_rs485.h"

#define MAX_UART 	2
#define RS232_TYPE  0
#define RS485_TYPE  1
#define NONE_TYPE 	-1
#define CRC_SIZE 	2
#define CRCL_SIZE 	2
#define CRCH_SIZE   1
#define RS485_DEFAULT_ENABLE 	1
#define RS485_DEFAULT_DISABLE	0

/* Modbus function codes */
#define MODBUS_FC_READ_COILS                    0x01
#define MODBUS_FC_READ_DISCRETE_INPUTS          0x02
#define MODBUS_FC_READ_HOLDING_REGISTERS        0x03
#define MODBUS_FC_READ_INPUT_REGISTERS          0x04
#define MODBUS_FC_WRITE_SINGLE_COIL             0x05
#define MODBUS_FC_WRITE_SINGLE_REGISTER         0x06
#define MODBUS_FC_READ_EXCEPTION_STATUS         0x07
#define MODBUS_FC_WRITE_MULTIPLE_COILS          0x0F
#define MODBUS_FC_WRITE_MULTIPLE_REGISTERS      0x10
#define MODBUS_FC_REPORT_SLAVE_ID               0x11
#define MODBUS_FC_MASK_WRITE_REGISTER           0x16
#define MODBUS_FC_WRITE_AND_READ_REGISTERS      0x17

/********************** According to the studio agreement ***********************/
#define     MODBUS_BAUDRATE_1200BPS            1
#define     MODBUS_BAUDRATE_2400BPS            2
#define     MODBUS_BAUDRATE_4800BPS            3
#define     MODBUS_BAUDRATE_9600BPS            4
#define     MODBUS_BAUDRATE_14400BPS           5
#define     MODBUS_BAUDRATE_19200BPS           6
#define     MODBUS_BAUDRATE_38400BPS           7
#define     MODBUS_BAUDRATE_56000BPS           8
#define     MODBUS_BAUDRATE_57600BPS           9
#define     MODBUS_BAUDRATE_115200BPS          10
#define     MODBUS_BAUDRATE_128000BPS          11

#define 	MODBUS_DATA_BIT5				   1
#define 	MODBUS_DATA_BIT6				   2
#define 	MODBUS_DATA_BIT7				   3
#define 	MODBUS_DATA_BIT8				   4

#define 	MODBUS_NONE_PARITY				   1
#define 	MODBUS_ODD_PARITY				   2
#define 	MODBUS_EVEN_PARITY				   3

#define 	MODBUS_STOP_BIT1				   1
#define 	MODBUS_STOP_BIT2				   2
/********************** According to the studio agreement ***********************/

typedef struct tag_UARTBAUDRATE {
	BYTE u_baud;
	DWORD g_baud;
}UARTBAUDRATE_T;

typedef struct tag_UARTDATABIT {
	BYTE u_databit;
	DWORD g_databit;
} UARTDATABIT_T;

typedef struct tag_UARTPARITY {
	BYTE u_parity;
	DWORD g_parity;
}UARTPARITY_T;

typedef struct tag_UARTSTOPBIT {
	BYTE u_stopbit;
	DWORD g_stopbit;
}UARTSTOPBIT_T;

void Uart_WriteDev(int Port_Type, BYTE_T u_wChannel, BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);
WORD_T Uart_ReadDev(int Port_Type, BYTE_T u_wChannel, BYTE_T *pData, SIZE_T  wSize, OFFSET_T Offset);
DWORD Get_UartPaud(BYTE index);
DWORD Get_UartDataBit(BYTE index);
DWORD Get_UartParity(BYTE index);
DWORD Get_UartStopBit(BYTE index);

extern const int Uart_Type[MAX_UART];
