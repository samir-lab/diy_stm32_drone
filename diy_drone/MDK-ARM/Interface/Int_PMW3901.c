#include "Int_PMW3901.h"

/**
 * @brief 读取PMW3901寄存器
 * @param reg 寄存器地址
 * @return 寄存器值
 */
uint8_t Int_PMW3901_Read_Reg(uint8_t reg)
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
void Int_PMW3901_Write_Reg(uint8_t reg, uint8_t value)
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
void Int_PMW3901_Reset(void)
{
    PMW3901_RESET_LOW;
    vTaskDelay(20);
    PMW3901_RESET_HIGH;
    vTaskDelay(50);
}

/**
 * 零偏校准，可以不做，实测下来offset都是0
 */
float g_pmw3901_offset_x = 0;
float g_pmw3901_offset_y = 0;
void Int_PMW3901_Calibration(void)
{
    int32_t sum_x = 0;
    int32_t sum_y = 0;
    uint16_t sample_count = 0;

    /* 将传感器放置在静止平面，连续采样若干次，计算静止时的平均偏置 */
    for (uint8_t i = 0; i < 100; i++)
    {
		uint8_t motion = Int_PMW3901_Read_Reg(PMW3901_MOTION);
		// 只采样有效数据
		if(motion == 48)
		{
			i--;
			continue;
		}
        uint8_t delta_x_l = Int_PMW3901_Read_Reg(PMW3901_DELTA_X_L);
		uint8_t delta_x_h = Int_PMW3901_Read_Reg(PMW3901_DELTA_X_H);
		uint8_t delta_y_l = Int_PMW3901_Read_Reg(PMW3901_DELTA_Y_L);
		uint8_t delta_y_h = Int_PMW3901_Read_Reg(PMW3901_DELTA_Y_H);

		int16_t raw_x = (int16_t)((uint16_t)delta_x_h << 8 | delta_x_l);
		int16_t raw_y = (int16_t)((uint16_t)delta_y_h << 8 | delta_y_l);

        /* 只统计静止状态下的基线值，避免被明显抖动污染 */
        if ((raw_x >= -20 && raw_x <= 20) && (raw_y >= -20 && raw_y <= 20))
        {
            sum_x += raw_x;
            sum_y += raw_y;
            sample_count++;
        }

        vTaskDelay(5);
    }
    if (sample_count > 0)
    {
        g_pmw3901_offset_x = (float)(sum_x / sample_count);
        g_pmw3901_offset_y = (float)(sum_y / sample_count);
    }
    else
    {
        g_pmw3901_offset_x = 0;
        g_pmw3901_offset_y = 0;
    }

    debug_printf("[PMW3901] Zero offset: x=%.2f, y=%.2f\r\n", g_pmw3901_offset_x, g_pmw3901_offset_y);
}


/**
 * @brief 初始化PMW3901
 */
void Int_PMW3901_InitReg(void);
void Int_PMW3901_Init(void)
{
	// 上电稳定
    vTaskDelay(50);
	// 软复位
	Int_PMW3901_Reset();
	vTaskDelay(10);

    // Read from registers 0x02, 0x03, 0x04, 0x05 and 0x06 one time regardless of the motion pin state
	Int_PMW3901_Read_Reg(0x02);
    Int_PMW3901_Read_Reg(0x03);
	Int_PMW3901_Read_Reg(0x04);
	Int_PMW3901_Read_Reg(0x05);
	Int_PMW3901_Read_Reg(0x06);


    // 配置寄存器
    Int_PMW3901_InitReg();
    vTaskDelay(50);

    // 读取ProductId验证初始化成功
    uint8_t product_id = Int_PMW3901_Read_Reg(PMW3901_PRODUCT_ID);
	debug_printf("[PMW3901] Init ProductId: 0x%02X\r\n", product_id);
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
uint8_t PMW3901_DEAD_ZONE = 12;
void Int_PMW3901_Get_Data(PMW3901_Data_Type *data)
{
    if (data == NULL)
    {
        return;
    }

    uint8_t motion = Int_PMW3901_Read_Reg(PMW3901_MOTION);
    data->motion = motion;

	// 根据motion判断是否发生运动
	// motion=48 表示数据有效，Run Mode 正常，但未检测到运动
	// motion=176 表示 Motion 发生，数据有效，Run Mode 正常
	if(motion == 48)
	{
		data->delta_x = 0;
		data->delta_y = 0;
		return;
	}

	/**
	 * SQUAL是PMW3901光流传感器的“表面质量”寄存器值，范围0-255，代表当前地面纹理是否足够丰富、光流数据是否可信‌
	 * SQUAL（Surface Quality）就是芯片对“脚下地面好不好认”的打分。
	 * 数值越高，说明地面纹理越清晰，光流算出来的位移越可靠；
	 * 数值太低，意味着地面太光滑、太暗或反光，光流数据基本是噪声，不能直接用
	 * 
	 * 60-100‌：正常范围，数据比较可信，可以放心用。
	 * ‌低于20‌：纹理不理想，数据会乱跳甚至静止不动，基本不可信。
	 * ‌个位数‌：地面太光滑（如瓷砖、白墙），光流输出基本是噪声，必须靠其他传感器兜底。‌‌
	 */
    uint8_t squal = Int_PMW3901_Read_Reg(PMW3901_SQUAL);
    data->squal = squal;
	if(squal < 60)
	{
		data->delta_x = 0;
		data->delta_y = 0;
		return;
	}


	/**
	 * 0x08 寄存器就是 RawData_Sum（原始数据总和），它反映的是传感器成像质量。
	 * 含义‌：RawData_Sum 是当前帧图像像素灰度值的累加和，数值越大说明图像越亮、纹理越丰富，追踪质量越好。
	 * 可以根据实际设置一个门限值，用于过滤噪声
	 */
    uint8_t raw_data = Int_PMW3901_Read_Reg(PMW3901_RAW_DATA_SUM);
    data->raw_data = raw_data;

	
	/**
	 * Shutter 值代表曝光时间‌，光线暗时芯片会自动拉长曝光，它是一个反映进光量和镜头状态的指标
	 * 如果异常偏大，说明进光量不足，可能是镜头脏了、表面纹理太差或离地高度不合适。
	 */
    uint8_t shutter_l = Int_PMW3901_Read_Reg(PMW3901_SHUTTER_LOWER);
    uint8_t shutter_h = Int_PMW3901_Read_Reg(PMW3901_SHUTTER_UPPER);
    data->shutter = (uint16_t)(((uint16_t)(shutter_h & 0x1F) << 8) | shutter_l);

    uint8_t delta_x_l = Int_PMW3901_Read_Reg(PMW3901_DELTA_X_L);
    uint8_t delta_x_h = Int_PMW3901_Read_Reg(PMW3901_DELTA_X_H);
    uint8_t delta_y_l = Int_PMW3901_Read_Reg(PMW3901_DELTA_Y_L);
    uint8_t delta_y_h = Int_PMW3901_Read_Reg(PMW3901_DELTA_Y_H);

    data->delta_x = (int16_t)((uint16_t)delta_x_h << 8 | delta_x_l);
    data->delta_y = (int16_t)((uint16_t)delta_y_h << 8 | delta_y_l);

	// 过滤死区数据
	if (data->delta_x > PMW3901_DEAD_ZONE * -1 && data->delta_x < PMW3901_DEAD_ZONE)
	{
		data->delta_x = 0;
	}
	if (data->delta_y > PMW3901_DEAD_ZONE * -1 && data->delta_y < PMW3901_DEAD_ZONE)
	{
		data->delta_y = 0;
	}
}

void Int_PMW3901_InitReg(void)
{
	PMW3901_CS_LOW;
	Int_PMW3901_Write_Reg(0x7F, 0x00);
	Int_PMW3901_Write_Reg(0x55, 0x01);
	Int_PMW3901_Write_Reg(0x50, 0x07);
	Int_PMW3901_Write_Reg(0x7F, 0x0E);
	Int_PMW3901_Write_Reg(0x43, 0x10);
	Int_PMW3901_Write_Reg(0x48, 0x02);
	
	Int_PMW3901_Write_Reg(0x7F, 0x00);
	Int_PMW3901_Write_Reg(0x51, 0x7B);
	Int_PMW3901_Write_Reg(0x50, 0x00);
	Int_PMW3901_Write_Reg(0x55, 0x00);
	Int_PMW3901_Write_Reg(0x7F, 0x0E);
	
	Int_PMW3901_Write_Reg(0x7F, 0x00);
	Int_PMW3901_Write_Reg(0x61, 0xAD);
	Int_PMW3901_Write_Reg(0x7F, 0x03);
	Int_PMW3901_Write_Reg(0x40, 0x00);
	Int_PMW3901_Write_Reg(0x7F, 0x05);
	Int_PMW3901_Write_Reg(0x41, 0xB3);
	Int_PMW3901_Write_Reg(0x43, 0xF1);
	Int_PMW3901_Write_Reg(0x45, 0x14);
	Int_PMW3901_Write_Reg(0x5B, 0x32);
	Int_PMW3901_Write_Reg(0x5F, 0x34);
	Int_PMW3901_Write_Reg(0x7B, 0x08);
	Int_PMW3901_Write_Reg(0x7F, 0x06);
	Int_PMW3901_Write_Reg(0x44, 0x1B);
	Int_PMW3901_Write_Reg(0x40, 0xBF);
	Int_PMW3901_Write_Reg(0x4E, 0x3F);
	Int_PMW3901_Write_Reg(0x7F, 0x08);
	Int_PMW3901_Write_Reg(0x65, 0x20);
	Int_PMW3901_Write_Reg(0x6A, 0x18);
	Int_PMW3901_Write_Reg(0x7F, 0x09);
	Int_PMW3901_Write_Reg(0x4F, 0xAF);
	Int_PMW3901_Write_Reg(0x5F, 0x40);
	Int_PMW3901_Write_Reg(0x48, 0x80);
	Int_PMW3901_Write_Reg(0x49, 0x80);
	Int_PMW3901_Write_Reg(0x57, 0x77);
	Int_PMW3901_Write_Reg(0x60, 0x78);
	Int_PMW3901_Write_Reg(0x61, 0x78);
	Int_PMW3901_Write_Reg(0x62, 0x08);
	Int_PMW3901_Write_Reg(0x63, 0x50);
	Int_PMW3901_Write_Reg(0x7F, 0x0A);
	Int_PMW3901_Write_Reg(0x45, 0x60);
	Int_PMW3901_Write_Reg(0x7F, 0x00);
	Int_PMW3901_Write_Reg(0x4D, 0x11);
	Int_PMW3901_Write_Reg(0x55, 0x80);
	Int_PMW3901_Write_Reg(0x74, 0x1F);
	Int_PMW3901_Write_Reg(0x75, 0x1F);
	Int_PMW3901_Write_Reg(0x4A, 0x78);
	Int_PMW3901_Write_Reg(0x4B, 0x78);
	Int_PMW3901_Write_Reg(0x44, 0x08);
	Int_PMW3901_Write_Reg(0x45, 0x50);
	Int_PMW3901_Write_Reg(0x64, 0xFF);
	Int_PMW3901_Write_Reg(0x65, 0x1F);
	Int_PMW3901_Write_Reg(0x7F, 0x14);
	Int_PMW3901_Write_Reg(0x65, 0x67);
	Int_PMW3901_Write_Reg(0x66, 0x08);
	Int_PMW3901_Write_Reg(0x63, 0x70);
	Int_PMW3901_Write_Reg(0x7F, 0x15);
	Int_PMW3901_Write_Reg(0x48, 0x48);
	Int_PMW3901_Write_Reg(0x7F, 0x07);
	Int_PMW3901_Write_Reg(0x41, 0x0D);
	Int_PMW3901_Write_Reg(0x43, 0x14);
	Int_PMW3901_Write_Reg(0x4B, 0x0E);
	Int_PMW3901_Write_Reg(0x45, 0x0F);
	Int_PMW3901_Write_Reg(0x44, 0x42);
	Int_PMW3901_Write_Reg(0x4C, 0x80);
	Int_PMW3901_Write_Reg(0x7F, 0x10);
	Int_PMW3901_Write_Reg(0x5B, 0x02);
	Int_PMW3901_Write_Reg(0x7F, 0x07);
	Int_PMW3901_Write_Reg(0x40, 0x41);
	Int_PMW3901_Write_Reg(0x70, 0x00);
	vTaskDelay(10);
	
	Int_PMW3901_Write_Reg(0x32, 0x44);
	Int_PMW3901_Write_Reg(0x7F, 0x07);
	Int_PMW3901_Write_Reg(0x40, 0x40);
	Int_PMW3901_Write_Reg(0x7F, 0x06);
	Int_PMW3901_Write_Reg(0x62, 0xF0);
	Int_PMW3901_Write_Reg(0x63, 0x00);
	Int_PMW3901_Write_Reg(0x7F, 0x0D);
	Int_PMW3901_Write_Reg(0x48, 0xC0);
	Int_PMW3901_Write_Reg(0x6F, 0xD5);
	Int_PMW3901_Write_Reg(0x7F, 0x00);
	Int_PMW3901_Write_Reg(0x5B, 0xA0);
	Int_PMW3901_Write_Reg(0x4E, 0xA8);
	Int_PMW3901_Write_Reg(0x5A, 0x50);
	Int_PMW3901_Write_Reg(0x40, 0x80);
	
	PMW3901_CS_HIGH;
}
