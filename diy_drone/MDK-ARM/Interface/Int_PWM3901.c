#include "Int_PWM3901.h"

/**
 * @brief 读取PMW3901寄存器
 * @param reg 寄存器地址
 * @return 寄存器值
 */
uint8_t Int_PWM3901_Read_Reg(uint8_t reg)
{
    uint8_t tx = reg | 0x80;
    uint8_t rx;

    PMW3901_CS_LOW;
    HAL_SPI_Transmit(&hspi2, &tx, 1, 1000);
    HAL_SPI_Receive(&hspi2, &rx, 1, 1000);
    PMW3901_CS_HIGH;
    vTaskDelay(1);

    return rx;
}

/**
 * @brief 写PMW3901寄存器
 * @param reg 寄存器地址
 * @param value 寄存器值
 */
void Int_PWM3901_Write_Reg(uint8_t reg, uint8_t value)
{
    uint8_t tx[2] = {0, 0};
    tx[0] = reg & 0x7F; // 写操作地址（最高位清零）
    tx[1] = value;

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
    vTaskDelay(200);
}

/**
 * @brief 初始化PMW3901
 */
void Int_PWM3901_Init(void)
{
    // 片选拉高，释放 SPI 端口
    PMW3901_CS_HIGH;
    vTaskDelay(5);
    // Power on reset
    Int_PWM3901_Write_Reg(0x3A, 0x5A);
    vTaskDelay(10);
    Int_PWM3901_Write_Reg(0x3A, 0xA5);
    vTaskDelay(50);
    // reset
    Int_PWM3901_Reset();

    // Test the SPI communication, checking chipId and inverse chipId
    uint8_t chipId = Int_PWM3901_Read_Reg(PMW3901_REG_PRODUCT_ID);
    if (chipId != CHIP_ID)
    {
        debug_printf("[PMW3901] Chip ID read error! chipId=0x%02X\r\n", chipId);
        return;
    }


    // Reading the motion registers one time
    Int_PWM3901_Read_Reg(0x02);
    Int_PWM3901_Read_Reg(0x03);
    Int_PWM3901_Read_Reg(0x04);
    Int_PWM3901_Read_Reg(0x05);
    Int_PWM3901_Read_Reg(0x06);
    vTaskDelay(1);

    // 触发SROM下载
    Int_PWM3901_Write_Reg(0x39, 0x5A);
    vTaskDelay(10);

    // 等待SROM加载完成（轮询0x70寄存器的bit1，变为0表示完成）
    uint16_t timeout = 0;
    while ((Int_PWM3901_Read_Reg(0x70) & 0x02) != 0) {
        vTaskDelay(10);
        if (++timeout > 10) {
            debug_printf("[PMW3901] SROM load timeout!\r\n");
            break;
        }
    }
    // 启动固件
    Int_PWM3901_Write_Reg(0x13, 0x01);
    vTaskDelay(10);
    

    // 配置运动输出模式
    Int_PWM3901_Write_Reg(0x7F, 0x00);  // 切换到Bank0
    Int_PWM3901_Write_Reg(0x6C, 0x00);
    debug_printf("[PMW3901] Init OK!\r\n");
}

/**
 * @brief 获取PMW3901运动数据
 * @param data 结构体输出
 */
void Int_PWM3901_Get_Data(PMW3901_Data_Type *data)
{
    uint8_t motion = 0;
    int8_t delta_x = 0;
    int8_t delta_y = 0;
    uint8_t squal = 0;
    uint8_t shutter_l = 0;
    uint8_t shutter_h = 0;
    uint8_t raw_data = 0;

    if (data == NULL)
    {
        return;
    }

    motion = Int_PWM3901_Read_Reg(PMW3901_REG_MOTION);
    delta_x = (int8_t)Int_PWM3901_Read_Reg(PMW3901_REG_DELTA_X);
    delta_y = (int8_t)Int_PWM3901_Read_Reg(PMW3901_REG_DELTA_Y);
    squal = Int_PWM3901_Read_Reg(PMW3901_REG_SQUAL);
    shutter_l = Int_PWM3901_Read_Reg(PMW3901_REG_SHUTTER);
    shutter_h = Int_PWM3901_Read_Reg(PMW3901_REG_RAW_DATA);
    raw_data = Int_PWM3901_Read_Reg(PMW3901_REG_RAW_DATA);

    data->motion = motion;
    data->delta_x = (int16_t)delta_x;
    data->delta_y = (int16_t)delta_y;
    data->squal = squal;
    data->shutter = (uint16_t)((uint16_t)shutter_h << 8 | shutter_l);
    data->raw_data = raw_data;
}

