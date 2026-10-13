#include "Int_PWM3901.h"

/**
 * @brief 读取PMW3901寄存器
 * @param reg 寄存器地址
 * @return 寄存器值
 */
uint8_t Int_PWM3901_Read_Reg(uint8_t reg)
{
    uint8_t tx[2] = {reg & 0x7F, 0x00};
    uint8_t rx[2] = {0, 0};
    PMW3901_CS_LOW;
    HAL_SPI_TransmitReceive(&hspi2, tx, rx, 2, 1000);
    PMW3901_CS_HIGH;
    vTaskDelay(1);
    return rx[1];
}

/**
 * @brief 写PMW3901寄存器
 * @param reg 寄存器地址
 * @param value 寄存器值
 */
void Int_PWM3901_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = {reg | 0x80, value};

    PMW3901_CS_LOW;
    HAL_SPI_Transmit(&hspi2, tx, 2, 1000);
    PMW3901_CS_HIGH;
    vTaskDelay(1);
}

/**
 * @brief 软复位PMW3901
 */
void Int_PWM3901_Reset(void)
{
    PMW3901_RESET_LOW;
    vTaskDelay(20);
    PMW3901_RESET_HIGH;
    vTaskDelay(50);
}

/**
 * @brief 初始化PMW3901
 */
void Int_PW3901_InitReg(void);
void Int_PWM3901_Init(void)
{
	// 上电稳定
    vTaskDelay(50);
	// 软复位
	Int_PWM3901_Reset();
	vTaskDelay(10);

    // Read from registers 0x02, 0x03, 0x04, 0x05 and 0x06 one time regardless of the motion pin state
	Int_PWM3901_Read_Reg(0x02);
    Int_PWM3901_Read_Reg(0x03);
	Int_PWM3901_Read_Reg(0x04);
	Int_PWM3901_Read_Reg(0x05);
	Int_PWM3901_Read_Reg(0x06);


    // 配置寄存器
    Int_PW3901_InitReg();
    vTaskDelay(50);

    // 读取ProductId验证初始化成功
    uint8_t product_id = Int_PWM3901_Read_Reg(PW3901_PRODUCT_ID);
    if (product_id != CHIP_ID)
    {
        debug_printf("[PMW3901] Init failed! ProductId: 0x%02X\r\n", product_id);
        return;
    }

    debug_printf("[PMW3901] Init OK!\r\n");
}

/**
 * @brief 获取PMW3901运动数据
 * @param data 结构体输出
 */
void Int_PWM3901_Get_Data(PMW3901_Data_Type *data)
{
    if (data == NULL)
    {
        return;
    }

    uint8_t motion = Int_PWM3901_Read_Reg(PW3901_MOTION);
    uint8_t delta_x_l = Int_PWM3901_Read_Reg(PW3901_DELTA_X_L);
    uint8_t delta_x_h = Int_PWM3901_Read_Reg(PW3901_DELTA_X_H);
    uint8_t delta_y_l = Int_PWM3901_Read_Reg(PW3901_DELTA_Y_L);
    uint8_t delta_y_h = Int_PWM3901_Read_Reg(PW3901_DELTA_Y_H);
    uint8_t squal = Int_PWM3901_Read_Reg(PW3901_SQUAL);
    uint8_t raw_data = Int_PWM3901_Read_Reg(PW3901_RAW_DATA_SUM);
    uint8_t shutter_l = Int_PWM3901_Read_Reg(PW3901_SHUTTER_LOWER);
    uint8_t shutter_h = Int_PWM3901_Read_Reg(PW3901_SHUTTER_UPPER);

    data->motion = motion;
    data->delta_x = (int16_t)((uint16_t)delta_x_h << 8 | delta_x_l);
    data->delta_y = (int16_t)((uint16_t)delta_y_h << 8 | delta_y_l);
    data->squal = squal;
    data->raw_data = raw_data;
    data->shutter = (uint16_t)(((uint16_t)(shutter_h & 0x1F) << 8) | shutter_l);
}

void Int_PW3901_InitReg(void)
{
	PMW3901_CS_LOW;
	Int_PWM3901_Write_Reg(0x7F, 0x00);
	Int_PWM3901_Write_Reg(0x55, 0x01);
	Int_PWM3901_Write_Reg(0x50, 0x07);
	Int_PWM3901_Write_Reg(0x7F, 0x0E);
	Int_PWM3901_Write_Reg(0x43, 0x10);
	Int_PWM3901_Write_Reg(0x48, 0x02);
	
	Int_PWM3901_Write_Reg(0x7F, 0x00);
	Int_PWM3901_Write_Reg(0x51, 0x7B);
	Int_PWM3901_Write_Reg(0x50, 0x00);
	Int_PWM3901_Write_Reg(0x55, 0x00);
	Int_PWM3901_Write_Reg(0x7F, 0x0E);
	
	Int_PWM3901_Write_Reg(0x7F, 0x00);
	Int_PWM3901_Write_Reg(0x61, 0xAD);
	Int_PWM3901_Write_Reg(0x7F, 0x03);
	Int_PWM3901_Write_Reg(0x40, 0x00);
	Int_PWM3901_Write_Reg(0x7F, 0x05);
	Int_PWM3901_Write_Reg(0x41, 0xB3);
	Int_PWM3901_Write_Reg(0x43, 0xF1);
	Int_PWM3901_Write_Reg(0x45, 0x14);
	Int_PWM3901_Write_Reg(0x5B, 0x32);
	Int_PWM3901_Write_Reg(0x5F, 0x34);
	Int_PWM3901_Write_Reg(0x7B, 0x08);
	Int_PWM3901_Write_Reg(0x7F, 0x06);
	Int_PWM3901_Write_Reg(0x44, 0x1B);
	Int_PWM3901_Write_Reg(0x40, 0xBF);
	Int_PWM3901_Write_Reg(0x4E, 0x3F);
	Int_PWM3901_Write_Reg(0x7F, 0x08);
	Int_PWM3901_Write_Reg(0x65, 0x20);
	Int_PWM3901_Write_Reg(0x6A, 0x18);
	Int_PWM3901_Write_Reg(0x7F, 0x09);
	Int_PWM3901_Write_Reg(0x4F, 0xAF);
	Int_PWM3901_Write_Reg(0x5F, 0x40);
	Int_PWM3901_Write_Reg(0x48, 0x80);
	Int_PWM3901_Write_Reg(0x49, 0x80);
	Int_PWM3901_Write_Reg(0x57, 0x77);
	Int_PWM3901_Write_Reg(0x60, 0x78);
	Int_PWM3901_Write_Reg(0x61, 0x78);
	Int_PWM3901_Write_Reg(0x62, 0x08);
	Int_PWM3901_Write_Reg(0x63, 0x50);
	Int_PWM3901_Write_Reg(0x7F, 0x0A);
	Int_PWM3901_Write_Reg(0x45, 0x60);
	Int_PWM3901_Write_Reg(0x7F, 0x00);
	Int_PWM3901_Write_Reg(0x4D, 0x11);
	Int_PWM3901_Write_Reg(0x55, 0x80);
	Int_PWM3901_Write_Reg(0x74, 0x1F);
	Int_PWM3901_Write_Reg(0x75, 0x1F);
	Int_PWM3901_Write_Reg(0x4A, 0x78);
	Int_PWM3901_Write_Reg(0x4B, 0x78);
	Int_PWM3901_Write_Reg(0x44, 0x08);
	Int_PWM3901_Write_Reg(0x45, 0x50);
	Int_PWM3901_Write_Reg(0x64, 0xFF);
	Int_PWM3901_Write_Reg(0x65, 0x1F);
	Int_PWM3901_Write_Reg(0x7F, 0x14);
	Int_PWM3901_Write_Reg(0x65, 0x67);
	Int_PWM3901_Write_Reg(0x66, 0x08);
	Int_PWM3901_Write_Reg(0x63, 0x70);
	Int_PWM3901_Write_Reg(0x7F, 0x15);
	Int_PWM3901_Write_Reg(0x48, 0x48);
	Int_PWM3901_Write_Reg(0x7F, 0x07);
	Int_PWM3901_Write_Reg(0x41, 0x0D);
	Int_PWM3901_Write_Reg(0x43, 0x14);
	Int_PWM3901_Write_Reg(0x4B, 0x0E);
	Int_PWM3901_Write_Reg(0x45, 0x0F);
	Int_PWM3901_Write_Reg(0x44, 0x42);
	Int_PWM3901_Write_Reg(0x4C, 0x80);
	Int_PWM3901_Write_Reg(0x7F, 0x10);
	Int_PWM3901_Write_Reg(0x5B, 0x02);
	Int_PWM3901_Write_Reg(0x7F, 0x07);
	Int_PWM3901_Write_Reg(0x40, 0x41);
	Int_PWM3901_Write_Reg(0x70, 0x00);
	vTaskDelay(10);
	
	Int_PWM3901_Write_Reg(0x32, 0x44);
	Int_PWM3901_Write_Reg(0x7F, 0x07);
	Int_PWM3901_Write_Reg(0x40, 0x40);
	Int_PWM3901_Write_Reg(0x7F, 0x06);
	Int_PWM3901_Write_Reg(0x62, 0xF0);
	Int_PWM3901_Write_Reg(0x63, 0x00);
	Int_PWM3901_Write_Reg(0x7F, 0x0D);
	Int_PWM3901_Write_Reg(0x48, 0xC0);
	Int_PWM3901_Write_Reg(0x6F, 0xD5);
	Int_PWM3901_Write_Reg(0x7F, 0x00);
	Int_PWM3901_Write_Reg(0x5B, 0xA0);
	Int_PWM3901_Write_Reg(0x4E, 0xA8);
	Int_PWM3901_Write_Reg(0x5A, 0x50);
	Int_PWM3901_Write_Reg(0x40, 0x80);
	
	PMW3901_CS_HIGH;
}
