#ifndef __SI24R1_H__
#define __SI24R1_H__

#include "spi.h"
#include "Com_debug.h"
#include "FreeRTOS.h"
#include "task.h"

//SI24R1 PIN DEFINITION TODO
// 拉低片选
#define SPI1_CS_LOW  HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_RESET) 
// 拉高片选
#define SPI1_CS_HIGH  HAL_GPIO_WritePin(SPI1_NSS_GPIO_Port, SPI1_NSS_Pin, GPIO_PIN_SET) 
// 拉低CE使能
#define SI24R1_CE_LOW  HAL_GPIO_WritePin(SI_EN_GPIO_Port, SI_EN_Pin, GPIO_PIN_RESET)
// 拉高CE使能
#define SI24R1_CE_HIGH  HAL_GPIO_WritePin(SI_EN_GPIO_Port, SI_EN_Pin, GPIO_PIN_SET)

#define SI24R1_TX_ADR_WIDTH   5  				// 5字节宽度的发送/接收地址
#define SI24R1_TX_PLOAD_WIDTH 18  				// 数据通道有效数据宽度

//********************************************************************************************************************//
// SPI(SI24R1) commands
#define SI24R1_READ_REG     0x00  // Define read command to register
#define SI24R1_WRITE_REG    0x20  // Define write command to register
#define SI24R1_RD_RX_PLOAD         0x61  // Define RX payload register address
#define SI24R1_WR_TX_PLOAD         0xA0  // Define TX payload register address
#define SI24R1_FLUSH_TX            0xE1  // Define flush TX register command
#define SI24R1_FLUSH_RX            0xE2  // Define flush RX register command
#define SI24R1_REUSE_TX_PL         0xE3  // Define reuse TX payload register command
#define SI24R1_NOP                 0xFF  // Define No Operation, might be used to read status register

//********************************************************************************************************************//
// SPI(SI24R1) registers(addresses)
#define SI24R1_CONFIG          0x00  // 'Config' register address
#define SI24R1_EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define SI24R1_EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SI24R1_SETUP_AW        0x03  // 'Setup address width' register address
#define SI24R1_SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define SI24R1_RF_CH           0x05  // 'RF channel' register address
#define SI24R1_RF_SETUP        0x06  // 'RF setup' register address
#define SI24R1_STATUS          0x07  // 'Status' register address
#define SI24R1_OBSERVE_TX      0x08  // 'Observe TX' register address
#define SI24R1_RSSI            0x09  // 'Received Signal Strength Indecator' register address
#define SI24R1_RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define SI24R1_RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define SI24R1_RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define SI24R1_RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define SI24R1_RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define SI24R1_RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define SI24R1_TX_ADDR         0x10  // 'TX address' register address
#define SI24R1_RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define SI24R1_RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define SI24R1_RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define SI24R1_RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define SI24R1_RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define SI24R1_RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define SI24R1_FIFO_STATUS     0x17  // 'FIFO Status Register' register address

//********************************************************************************************************************//
// STATUS Register 
#define SI24R1_RX_DR					0x40  /*接收完成*/
#define SI24R1_TX_DS					0x20  /*发送完成*/
#define SI24R1_MAX_RT					0x10  /*达到最大重发次数中断*/

//********************************************************************************************************************//
//                                        FUNCTION's PROTOTYPES                                                       //
//********************************************************************************************************************//
//SI24R1 API Functions
/********************************************************
函数功能：SI24R1写寄存器（单字节）
输入参数：reg:寄存器地址（格式：SI24R1_WRITE_REG | reg），value:要写入的值
输出参数：状态寄存器的值
*********************************************************/
uint8_t Int_SI24R1_Write_Reg(uint8_t reg, uint8_t value);
/********************************************************
函数功能：SI24R1写寄存器（多字节）
输入参数：reg:寄存器地址（格式：SI24R1_WRITE_REG | reg），pBuf:写数据的首地址，size:要写入的字节数
输出参数：状态寄存器的值
*********************************************************/
uint8_t Int_SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t size);
/********************************************************
函数功能：SI24R1读寄存器（单字节）
输入参数：reg:寄存器地址（格式：SI24R1_READ_REG | reg）
输出参数：读取到的值
*********************************************************/
uint8_t Int_SI24R1_Read_Reg(uint8_t reg);
/********************************************************
函数功能：SI24R1读寄存器（多字节）
输入参数：reg:寄存器地址（格式：SI24R1_READ_REG | reg），pBuf:读数据的首地址，size:要读取的字节数
输出参数：状态寄存器的值
*********************************************************/
uint8_t Int_SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t size);
/********************************************************
函数功能：SI24R1接收模式初始化
输入参数：无
输出参数：无
*********************************************************/
void Int_SI24R1_RX_Mode(void);
/********************************************************
函数功能：SI24R1发送模式初始化
输入参数：无
输出参数：无
*********************************************************/
void Int_SI24R1_TX_Mode(void);
/********************************************************
函数功能：SI24R1接收数据包，硬件自己接收数据保存到FIFO队列中，通过状态标志位判断队列是否有数据
输入参数：rxbuf:接收数据的缓冲区
输出参数：0:接收成功 1:接收失败
*********************************************************/
uint8_t Int_SI24R1_RxPacket(uint8_t *rxbuf);
/********************************************************
函数功能：SI24R1发送数据包
输入参数：txbuf:发送数据的缓冲区
输出参数：0:发送成功 1:发送失败
*********************************************************/
uint8_t Int_SI24R1_TxPacket(uint8_t *txbuf);


// SI24R1 初始化
void Int_SI24R1_Init(void); 
//********************************************************************************************************************//
#endif

