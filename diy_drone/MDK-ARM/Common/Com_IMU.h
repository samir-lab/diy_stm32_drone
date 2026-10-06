#ifndef __COM_IMU_H__
#define __COM_IMU_H__

#include "Com_debug.h"
#include "Com_config.h"
#include "math.h"

// 四元数结构体
typedef struct
{
    float q0;
    float q1;
    float q2;
    float q3;
} Quaternion_Type;

extern float RtA;
extern float Gyro_G;
extern float Gyro_Gr;

/**
 * @brief 互补解算姿态
 */
void Com_IMU_Calc_Pose_By_Complementary(Gyro_Accel_Data_Type *gyro_accel_data, Euler_Angle_Type *euler_angle);

/**
 * @brief 四元数解算姿态
 */
void Com_IMU_Get_Euler_Angle(Gyro_Accel_Data_Type *gyroAccel, Euler_Angle_Type *eulerAngle, float dt);

/**
 * @brief 获取加速度计z轴的模
 */
float Com_IMU_Get_NormAccZ(void);


#endif // __COM_IMU_H__
