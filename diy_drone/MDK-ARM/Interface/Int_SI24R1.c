#include "Int_SI24R1.h"

// 定义静态发送地址（发送与接收地址相同）
uint8_t SI24R1_TX_ADDRESS[SI24R1_TX_ADR_WIDTH] = {0x0A,0x01,0x07,0x0E,0x69};

/**
 * @brief  SPI读写函数
 * @param  byte 要写入的字节
 * @return 读取到的字节
 */
static uint8_t SPI_RW(uint8_t byte)
{
	uint8_t rx_data;
	HAL_SPI_TransmitReceive(&hspi1, &byte, &rx_data, 1, 1000);
	return rx_data;
}

uint8_t Int_SI24R1_Write_Reg(uint8_t reg, uint8_t value)
{
	uint8_t status;

	SPI1_CS_LOW;
	status = SPI_RW(reg);				
	SPI_RW(value);
	SPI1_CS_HIGH;
	
	return(status);
}

uint8_t Int_SI24R1_Write_Buf(uint8_t reg, const uint8_t *pBuf, uint8_t size)
{
	uint8_t status,byte_ctr;

	SPI1_CS_LOW;
	status = SPI_RW(reg);
	for(byte_ctr = 0; byte_ctr < size; byte_ctr++)
	{
		SPI_RW(*pBuf++);
	}
	SPI1_CS_HIGH;

	return(status);
}

uint8_t Int_SI24R1_Read_Reg(uint8_t reg)
{
 	uint8_t value;

	SPI1_CS_LOW;
	SPI_RW(reg);
	value = SPI_RW(0);
	SPI1_CS_HIGH;
	return(value);
}

uint8_t Int_SI24R1_Read_Buf(uint8_t reg, uint8_t *pBuf, uint8_t size)
{
	uint8_t status,byte_ctr;
	SPI1_CS_LOW;
	status = SPI_RW(reg);
	for(byte_ctr = 0; byte_ctr < size; byte_ctr++)
	{
		pBuf[byte_ctr] = SPI_RW(0);
	}
	SPI1_CS_HIGH;

	return(status);
}

void Int_SI24R1_RX_Mode(void)
{
	// 拉低CE使能，进入配置模式
	SI24R1_CE_LOW;
	// 接收设备接收通道0使用和发送设备相同的发送地址
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P0, SI24R1_TX_ADDRESS, SI24R1_TX_ADR_WIDTH);
	// 使能接收通道0自动应答
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_EN_AA, 0x01);
	// 使能接收通道0
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_EN_RXADDR, 0x01);
	// 选择射频信道69（0-125个信道）
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_CH, 69);
	// 接收通道0选择和发送通道相同的有效数据宽度
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P0, SI24R1_TX_PLOAD_WIDTH);
	// 数据传输速率选择为1Mbps，发射功率为4dBm
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_SETUP, 0x06);
	// CRC使能，CRC模式为16位，开机，接收模式
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_CONFIG, 0x0f);
	// 清除所有中断标志位
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_STATUS, 0xff);
	// 拉高CE使能，进入接收模式
	SI24R1_CE_HIGH;
}

void Int_SI24R1_TX_Mode(void)
{
	// 拉低CE使能，进入配置模式
	SI24R1_CE_LOW;
	// 写入发送地址
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_TX_ADDR, SI24R1_TX_ADDRESS, SI24R1_TX_ADR_WIDTH);
	// 为了应答接收设备，接收通道0地址和发送地址相同
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P0, SI24R1_TX_ADDRESS, SI24R1_TX_ADR_WIDTH);
	// 使能接收通道0自动应答
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_EN_AA, 0x01);
	// 使能接收通道0
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_EN_RXADDR, 0x01);
	// 选择自动重发延时为250us，最大自动重发次数为10次
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_SETUP_RETR, 0x0a);
	// 选择射频信道69（0-125个信道）
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_CH, 69);
	// 数据传输速率选择为1Mbps，发射功率为4dBm
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_SETUP, 0x06);
	// CRC使能，CRC模式为16位，开机，发送模式
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_CONFIG, 0x0e);
	// 拉高CE使能，进入接收模式
	SI24R1_CE_HIGH;
}

uint8_t Int_SI24R1_RxPacket(uint8_t *rxbuf)
{
	uint8_t state;
	// 读取状态寄存器的值
	state = Int_SI24R1_Read_Reg(SI24R1_STATUS);
	// 清除RX_DS中断标志
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_STATUS, state);

	// 如果接收完成
	if(state & SI24R1_RX_DR)
	{
		Int_SI24R1_Read_Buf(SI24R1_RD_RX_PLOAD, rxbuf, SI24R1_TX_PLOAD_WIDTH);
		Int_SI24R1_Write_Reg(SI24R1_FLUSH_RX, 0xff);
		return 0;
	}
	return 1;
}

uint8_t Int_SI24R1_TxPacket(uint8_t *txbuf)
{
	uint8_t state;
	// 拉低CE使能，进入配置模式
	SI24R1_CE_LOW;
	// 写数据到TX FIFO
  	Int_SI24R1_Write_Buf(SI24R1_WR_TX_PLOAD, txbuf, SI24R1_TX_PLOAD_WIDTH);
	// 拉高CE使能，开始发送数据
 	SI24R1_CE_HIGH;
	
	// 不使用中断方式，轮询状态寄存器的值
	state = Int_SI24R1_Read_Reg(SI24R1_STATUS);
	while((state & (SI24R1_TX_DS | SI24R1_MAX_RT)) == 0)
	{
		state = Int_SI24R1_Read_Reg(SI24R1_STATUS);
		vTaskDelay(1); // 延迟1ms，避免占用过多CPU资源
	}
	// 清除TX_DS或MAX_RT中断标志
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_STATUS, state);
	if(state & SI24R1_MAX_RT)
	{
		// 达到最大重发次数，清除TX FIFO寄存器
		Int_SI24R1_Write_Reg(SI24R1_FLUSH_TX,0xff);
		// 发送失败
		return 1;
	}
	if(state & SI24R1_TX_DS)
	{
		// 发送成功
		return 0;
	}
	// 发送失败
	return 1;
}

/**
 * @brief  检查SI24R1模块是否正常工作
 * @return 0:检查成功 1:检查失败
 */

uint8_t check_tx_address[SI24R1_TX_ADR_WIDTH];
uint8_t Int_SI24R1_Check(void)
{
	// 先读一次发送地址，避免第一次写入时读取到旧的值
	Int_SI24R1_Read_Buf(SI24R1_READ_REG + SI24R1_TX_ADDR, check_tx_address, SI24R1_TX_ADR_WIDTH);
	// 写入发送地址
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_TX_ADDR, SI24R1_TX_ADDRESS, SI24R1_TX_ADR_WIDTH);
	// 读取发送地址
	Int_SI24R1_Read_Buf(SI24R1_READ_REG + SI24R1_TX_ADDR, check_tx_address, SI24R1_TX_ADR_WIDTH);
	// 比较写入和读取的地址是否一致
	for(int i = 0; i < SI24R1_TX_ADR_WIDTH; i++)
	{
		if(check_tx_address[i] != SI24R1_TX_ADDRESS[i])
		{
			// 检查失败
			return 1; 
		}
	}
	// 检查成功
	return 0; 
}

void Int_SI24R1_Init(void)
{
	// 等待SI24R1上电稳定
	HAL_Delay(200);
	// 芯片检测
	while(Int_SI24R1_Check() != 0)
	{
		HAL_Delay(10);
	}

	// 初始化为接收模式 => 发送数据时切换到发送模式
	Int_SI24R1_RX_Mode();
	debug_printf("SI24R1 Init OK!\r\n");
}


