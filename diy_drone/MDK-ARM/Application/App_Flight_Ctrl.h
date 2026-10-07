#ifndef __APP_FLIGHT_CTRL_H__
#define __APP_FLIGHT_CTRL_H__

#include "Int_MPU6050.h"
#include "Com_debug.h"
#include "Com_Filter.h"
#include "Com_IMU.h"
#include "Com_PID.h"
#include "Com_tool.h"
#include "Int_Motor.h"
#include "Int_VL53L1X.h"
#include "Int_PWM3901.h"

#define LOW_PASS_WEIGHT 0.15

/**
 * @brief 初始化飞行控制相关的模块
 */
void App_Flight_Ctrl_Init(void);

/**
 * @brief 解算陀螺仪数据，获取欧拉角
 */
void App_Flight_Ctrl_Get_Euler_Angle(void);

/**
 * @brief PID控制，根据欧拉角计算出PID目标值
 */
void App_Flight_Ctrl_PID_Ctrl(void);

/**
 * @brief 电机控制
 */
void App_Flight_Ctrl_Motor_Ctrl(void);

#endif // __APP_FLIGHT_CTRL_H__
