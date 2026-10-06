#ifndef __APP_FLIGHT_CTRL_H__
#define __APP_FLIGHT_CTRL_H__

#include "Int_MPU6050.h"
#include "Com_debug.h"
#include "Com_Filter.h"
#include "Com_IMU.h"

#define LOW_PASS_WEIGHT 0.15

/**
 * @brief 解算陀螺仪数据，获取欧拉角
 */
void App_Flight_Ctrl_Get_Euler_Angle(void);

#endif // __APP_FLIGHT_CTRL_H__
