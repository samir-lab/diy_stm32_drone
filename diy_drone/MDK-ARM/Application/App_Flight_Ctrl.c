#include "App_Flight_Ctrl.h"

Gyro_Accel_Data_Type gyro_accel_data;
Euler_Angle_Type euler_angle;

/**
 * @brief 解算陀螺仪数据，获取欧拉角
 */
int16_t log_count = 0;
void App_Flight_Ctrl_Get_Euler_Angle(void)
{
    // 实现陀螺仪数据解算算法
    // 1. 获取陀螺仪和加速度计数据
    Int_MPU6050_Get_Data(&gyro_accel_data);
    log_count++;
    if(log_count % 100 == 0)
    {
        debug_printf("\ngyro_x: %d, gyro_y: %d, gyro_z: %d\n", 
            gyro_accel_data.gyro.gyro_x, gyro_accel_data.gyro.gyro_y, gyro_accel_data.gyro.gyro_z);
        debug_printf("\naccel_x: %d, accel_y: %d, accel_z: %d\n", 
            gyro_accel_data.accel.accel_x, gyro_accel_data.accel.accel_y, gyro_accel_data.accel.accel_z);
    }
    
    // 2. 应用卡尔曼滤波或其他算法
    // 3. 计算欧拉角
}
