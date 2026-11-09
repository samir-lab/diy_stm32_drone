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
	// 选择射频信道110（0-125个信道）
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_CH, 0x6e);
	// 接收通道0选择和发送通道相同的有效数据宽度
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P0, SI24R1_TX_PLOAD_WIDTH);
	// 数据传输速率选择为250kbps，发射功率为7dBm
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_SETUP, 0x27);
	// CRC使能，CRC模式为16位，开机，接收模式
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_CONFIG, 0x0f);
	// 清除所有中断标志位
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_STATUS, 0x70);
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
	// 选择自动重发延时为750us，重传3次
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_SETUP_RETR, 0x23);
	// 选择射频信道110（0-125个信道）
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_CH, 0x6e);
	// 数据传输速率选择为250kbps，发射功率为7dBm
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_SETUP, 0x27);
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
	for(uint8_t i = 0; i < SI24R1_TX_ADR_WIDTH; i++)
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

void Int_SI24R1_ResetAllReg(void)
{
	// 1. 硬件复位时序：CE拉低，释放SPI片选
	SI24R1_CE_LOW;
	SPI1_CS_HIGH;
	HAL_Delay(10); 
	// 2. 清空TX/RX FIFO缓冲区
	SPI1_CS_LOW;
	SPI_RW(SI24R1_FLUSH_TX);
	SPI1_CS_HIGH;
	HAL_Delay(1); 
	SPI1_CS_LOW;
	SPI_RW(SI24R1_FLUSH_RX);
	SPI1_CS_HIGH;
	HAL_Delay(1); 
	// 关闭重复发送数据包功能
	SPI1_CS_LOW;
	SPI_RW(SI24R1_REUSE_TX_PL);
	SPI1_CS_HIGH;
	HAL_Delay(1);
	// 3. 所有寄存器恢复NRF24L01/SI24R1出厂默认值
	// CONFIG 0x00 默认 0x08：PWR_UP=0, PRIM_RX=0, 中断全部使能
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_CONFIG, 0x08);
	// EN_AA 0x01 默认 0x3F：所有通道自动应答开启
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_EN_AA, 0x3F);
	// EN_RXADDR 0x02 默认 0x03：P0/P1接收通道开启
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_EN_RXADDR, 0x03);
	// SETUP_AW 0x03 默认 0x03：5字节地址宽度
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_SETUP_AW, 0x03);
	// SETUP_RETR 0x04 默认 0x23：自动重传间隔750us，重传3次
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_SETUP_RETR, 0x23);
	// RF_CH 0x05 默认 0x02：信道2
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_CH, 0x02);
	// RF_SETUP 0x06 默认 0x07：250kbps，7dBm
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RF_SETUP, 0x27);
	// STATUS 0x07：写1清除所有中断标志
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_STATUS, 0x70);
	// OBSERVE_TX 0x08 只读，无需写
	// RSSI 0x09 只读，无需写
	// P0~P5接收地址寄存器（清空为0）
	uint8_t zero_buf[5] = {0};
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P0, zero_buf, 5);
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P1, zero_buf, 5);
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P2, zero_buf, 1);
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P3, zero_buf, 1);
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P4, zero_buf, 1);
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_RX_ADDR_P5, zero_buf, 1);
	// TX发送地址清空
	Int_SI24R1_Write_Buf(SI24R1_WRITE_REG + SI24R1_TX_ADDR, zero_buf, 5);
	// 所有通道有效载荷长度清零
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P0, 0x00);
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P1, 0x00);
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P2, 0x00);
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P3, 0x00);
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P4, 0x00);
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_RX_PW_P5, 0x00);
	// FIFO_STATUS 只读，无需写入
	// 4. 再次清空状态寄存器所有中断位
	Int_SI24R1_Write_Reg(SI24R1_WRITE_REG + SI24R1_STATUS, 0x70);
	// 5. 保持CE低电平，进入待机模式，完成全部复位
	SI24R1_CE_LOW;
	SPI1_CS_HIGH;
	HAL_Delay(20);
}

void Int_SI24R1_Init(void)
{
	// 等待SI24R1上电稳定
	HAL_Delay(200);
	Int_SI24R1_ResetAllReg();
	// 芯片检测
	uint8_t check_count = 0;
	while(Int_SI24R1_Check() != 0)
	{
		check_count++;
		HAL_Delay(10);
		if(check_count >= 100)
		{
			debug_printf("SI24R1 Check Failed!\r\n");
			break;
		}
	}

	// 初始化为接收模式
	Int_SI24R1_RX_Mode();
	debug_printf("SI24R1 Init OK!\r\n");
}


