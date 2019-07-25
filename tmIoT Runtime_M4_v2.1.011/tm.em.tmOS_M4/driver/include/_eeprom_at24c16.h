///*===========================================================================+
//|  Function : eeprom Driver                                                  |
//|  Task     : eeprom Driver Header File                                      |
//|----------------------------------------------------------------------------|
//|  Compile  : CCS6.1 -                                                       |
//|  Link     : CCS6.1 -                                                       |
//|  Call     :                                                                |
//|----------------------------------------------------------------------------|
//|  Author   : jia                                                            |
//|  Version  : V1.00                                                          |
//|  Creation : 2016/08/17                                                     |
//|  Revision :                                                                |
//+===========================================================================*/
//#ifndef     D_EEPROM_AT24C16_H
//#define     D_EEPROM_AT24C16_H
//
//
//#ifdef __cplusplus
//extern "C" {
//#endif
//
//#include "kernel/include/type.h"
//#include "kernel/include/device.h"
//#include "config.h"
///*==============================================================================+
//|           Macro definition                                                    |
//+==============================================================================*/
//#define     MAX_EEPROM                           16
//
////+++++
////  Eeprom peripheral、port and pin
////+++++
//#if (defined M2C2132) || (defined IO2C1111) || (defined IO2C1112) || (defined IO2C1131)
//	#define     E2PROM_I2C_PERIPH                  SYSCTL_PERIPH_I2C7
//	#define     E2PROM_I2C_BASE                    I2C7_BASE
//	#define     E2PROM_I2C_SCL_PERIPH              SYSCTL_PERIPH_GPIOA
//	#define     E2PROM_I2C_SCL_PORT                GPIO_PORTA_BASE
//	#define     E2PROM_I2C_SCL_PIN                 GPIO_PIN_4
//	#define     E2PROM_I2C_SDA_PERIPH              SYSCTL_PERIPH_GPIOA
//	#define     E2PROM_I2C_SDA_PORT                GPIO_PORTA_BASE
//	#define     E2PROM_I2C_SDA_PIN                 GPIO_PIN_5
//	#define		E2PROM_I2C_GPIO_SCL				   GPIO_PA4_I2C7SCL
//	#define		E2PROM_I2C_GPIO_SDA				   GPIO_PA5_I2C7SDA
//#endif
//#ifdef S2C41111
//	#define     E2PROM_I2C_PERIPH                  SYSCTL_PERIPH_I2C0
//	#define     E2PROM_I2C_BASE                    I2C0_BASE
//	#define     E2PROM_I2C_SCL_PERIPH              SYSCTL_PERIPH_GPIOB
//	#define     E2PROM_I2C_SCL_PORT                GPIO_PORTB_BASE
//	#define     E2PROM_I2C_SCL_PIN                 GPIO_PIN_2
//	#define     E2PROM_I2C_SDA_PERIPH              SYSCTL_PERIPH_GPIOB
//	#define     E2PROM_I2C_SDA_PORT                GPIO_PORTB_BASE
//	#define     E2PROM_I2C_SDA_PIN                 GPIO_PIN_3
//	#define		E2PROM_I2C_GPIO_SCL				   GPIO_PB2_I2C0SCL
//	#define		E2PROM_I2C_GPIO_SDA				   GPIO_PB3_I2C0SDA
//#endif
////+++++
////  EEprom parameter
////+++++
//#define     E2PROM_AT24C16_ADDRESS               (0xA0>>1)
//
//#define     E2PROM_AT24C16_BLOCKSIZE             16
//#define     E2PROM_AT24C16_BLOCKMARK             0xF
///*==============================================================================+
//|           Type definition                                                     |
//+==============================================================================*/
///*---------------------------------------------------------------------------+
//|           Operations                                                       |
//+---------------------------------------------------------------------------*/
//
///*===========================================================================+
//|           Externals                                                        |
//+===========================================================================*/
//extern  DEV_ENGINE_T            g_EepromAt24c16_Engine;
//#ifdef __cplusplus
//}
//#endif
//
//#endif
//
