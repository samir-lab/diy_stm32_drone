#include "App_Flight_Ctrl.h"

// MPU6050数据结构体对象
Gyro_Accel_Data_Type gyro_accel_data;
Euler_Angle_Type euler_angle;

/**
 * @brief 解算陀螺仪数据，获取欧拉角
 */
// 滤波器中间缓存数据
Gyro_Data_Type pre_gyro_data;
Accel_Data_Type pre_accel_data;
extern KalmanFilter_Type kf_init_param[3];
void App_Flight_Ctrl_Get_Euler_Angle(void)
{
    // 实现陀螺仪数据解算算法
    // 1. 获取陀螺仪和加速度计数据
    Int_MPU6050_Get_Data(&gyro_accel_data);

    // 2.应用低通滤波算法，处理角速度数据
    // 2.1 实时性要求高，对准确性要求不高，不使用傅里叶变换
    // 2.2 output = 加权系数 * pre_output + (1 - 加权系数) * 本次测量值
    gyro_accel_data.gyro.gyro_x = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_x, pre_gyro_data.gyro_x, LOW_PASS_WEIGHT);
    gyro_accel_data.gyro.gyro_y = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_y, pre_gyro_data.gyro_y, LOW_PASS_WEIGHT);
	gyro_accel_data.gyro.gyro_z = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_z, pre_gyro_data.gyro_z, LOW_PASS_WEIGHT);
    // 2.3 保存本次测量值，用于下一次滤波
    pre_gyro_data.gyro_x = gyro_accel_data.gyro.gyro_x;
	pre_gyro_data.gyro_y = gyro_accel_data.gyro.gyro_y;
	pre_gyro_data.gyro_z = gyro_accel_data.gyro.gyro_z;

    // debug_printf(": %d, %d, %d\n", 
    //     gyro_accel_data.gyro.gyro_x, 
    //     gyro_accel_data.gyro.gyro_y, 
    //     gyro_accel_data.gyro.gyro_z);

    // 3. 应用卡尔曼滤波算法，处理加速度计数据
    // 3.1 对波动比较大的数据，使用卡尔曼滤波算法
    gyro_accel_data.accel.accel_x = Common_Filter_Kalman(&kf_init_param[0], gyro_accel_data.accel.accel_x);
    gyro_accel_data.accel.accel_y = Common_Filter_Kalman(&kf_init_param[1], gyro_accel_data.accel.accel_y);
	gyro_accel_data.accel.accel_z = Common_Filter_Kalman(&kf_init_param[2], gyro_accel_data.accel.accel_z);

    // debug_printf(": %d, %d, %d\n", 
    //     gyro_accel_data.accel.accel_x, 
    //     gyro_accel_data.accel.accel_y, 
    //     gyro_accel_data.accel.accel_z);

    // 4. 姿态解算
    // 4.1 常用的解算方法
    // 4.1.1 角速度对时间积分（类似 θ = ω * t），缺点：长时间积分存在误差
    // 4.1.2 加速度解算（类似 θ = arctan(对边/邻边)），缺点：易受振动影响
    // 4.1.3 互补解算，结合上述两种方法进行计算
    // 4.1.4 四元数解算，较复杂，算法固定，有完整的数学理论，计算量小
    // 4.1.5 卡尔曼姿态评估，动态性能好，模型复杂，计算量大，理论复杂，参数难调
}
