#include "Int_MPU6050.h"

/**
 * @brief 写入MPU6050寄存器
 * @param reg 寄存器地址
 * @param data 数据
 */
void Int_MPU6050_Write_Reg(uint8_t reg, uint8_t data)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_I2C_WRITE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
}

/**
 * @brief 读取MPU6050寄存器
 * @param reg 寄存器地址
 * @param data 数据
 */
void Int_MPU6050_Read_Reg(uint8_t reg, uint8_t *data)
{
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_I2C_READ_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, 1000);
}

/**
 * @brief 写入MPU6050寄存器
 * @param reg 寄存器地址
 * @param data 数据
 * @param size 数据长度
 */
void Int_MPU6050_Batch_Write_Reg(uint8_t reg, uint8_t data, uint16_t size)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_I2C_WRITE_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, size, 1000);
}

/**
 * @brief 读取MPU6050寄存器
 * @param reg 寄存器地址
 * @param data 数据
 * @param size 数据长度
 */
void Int_MPU6050_Batch_Read_Reg(uint8_t reg, uint8_t *data, uint16_t size)
{
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_I2C_READ_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, size, 1000);
}


/**
 * @brief 初始化MPU6050传感器
 */
void Int_MPU6050_Init(void)
{
    // 1.重置芯片，重置所有寄存器的值 => 对电源管理寄存器第7位（DEVICE_RESET）写1即可
    Int_MPU6050_Write_Reg(0x6B, 0x80);
    
    // 2.等待复位完成，重置完成后0x6B的寄存器值被重置为0x40，表示进入sleep模式
    uint8_t data = 0;
    do {
        Int_MPU6050_Read_Reg(0x6B, &data);
        HAL_Delay(10);
    } while (data != 0x40);

    // 3.唤醒MPU6050
    Int_MPU6050_Write_Reg(0x6B, 0x00);
    
    // 4.选择合适的量程
    // 4.1 加速度计量程选择（+-2g）
    Int_MPU6050_Write_Reg(0x1C, 0x00); 
    // 4.2 陀螺仪量程选择（+-250度/秒 0x00, +-2000度/秒 3<<3）
    Int_MPU6050_Write_Reg(0x1B, 0x00); 

    // 5. 关闭中断使能
    Int_MPU6050_Write_Reg(0x38, 0x00);

    // 6.用户配置寄存器，不使用队列，不使用I2C主模式，不使用FIFO，不使用DMP
    Int_MPU6050_Write_Reg(0x6A, 0x00);

    // 7.设置采样率为500Hz（默认1000Hz，设置2分频），采样率 >= 2倍使用频率
    Int_MPU6050_Write_Reg(0x19, 0x01); 

    // 8.设置低通滤波器为184Hz
    Int_MPU6050_Write_Reg(0x1A, 0x01);

    // 9.配置使用的系统时钟为添加PLL的X轴陀螺仪输出
    Int_MPU6050_Write_Reg(0x6B, 0x01);

    // 10.使能加速度和角速度传感器
    Int_MPU6050_Write_Reg(0x6C, 0x00);
}

/**
 * @brief 获取3轴角速度数据
 * @param gyro_data 角速度数据
 */
void Int_MPU6050_Get_Gyro(Gyro_Data_Type *gyro_data)
{
    // 读取0x43-0x48的6个字节，分别是x,y,z的高8位和低8位
    uint8_t data[6] = {0};
    Int_MPU6050_Batch_Read_Reg(0x43, data, 6);
    gyro_data->gyro_x = (int16_t)((data[0] << 8) | data[1]);
    gyro_data->gyro_y = (int16_t)((data[2] << 8) | data[3]);
    gyro_data->gyro_z = (int16_t)((data[4] << 8) | data[5]);
    // 根据量程选择，计算实际的角速度值
    // 0 => +-250°/s => 131 LSB/°/s
    // 1 => +-500°/s => 65.5 LSB/°/s
    // 2 => +-1000°/s => 32.8 LSB/°/s
    // 3 => +-2000°/s => 16.4 LSB/°/s
    // gyro_data->gyro_x = gyro_data->gyro_x * 131;
    // gyro_data->gyro_y = gyro_data->gyro_y * 131;
    // gyro_data->gyro_z = gyro_data->gyro_z * 131;
}

/**
 * @brief 获取3轴加速度数据
 * @param accel_data 加速度数据
 */
void Int_MPU6050_Get_Accel(Accel_Data_Type *accel_data)
{
    // 读取0x3B-0x40的6个字节，分别是x,y,z的高8位和低8位
    uint8_t data[6] = {0};
    Int_MPU6050_Batch_Read_Reg(0x3B, data, 6);
    accel_data->accel_x = (int16_t)((data[0] << 8) | data[1]);
    accel_data->accel_y = (int16_t)((data[2] << 8) | data[3]);
    accel_data->accel_z = (int16_t)((data[4] << 8) | data[5]);

    // 根据量程选择，计算实际的加速度值
    // 0 => +-2g => 16384 LSB/g
    // 1 => +-4g => 8192 LSB/g
    // 2 => +-8g => 4096 LSB/g
    // 3 => +-16g => 2048 LSB/g
    // accel_data->accel_x = accel_data->accel_x * 16384;
    // accel_data->accel_y = accel_data->accel_y * 16384;
    // accel_data->accel_z = accel_data->accel_z * 16384;
}

/**
 * @brief 获取6轴数据
 * @param data 6轴数据
 */
void Int_MPU6050_Get_Data(Gyro_Accel_Data_Type *data)
{
    Int_MPU6050_Get_Gyro(&data->gyro);
    Int_MPU6050_Get_Accel(&data->accel);
}
