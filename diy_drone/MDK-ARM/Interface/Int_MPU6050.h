#ifndef __INT_MPU6050_H__
#define __INT_MPU6050_H__

#include <stdlib.h>
#include "i2c.h"
#include "Com_config.h"
#include "Com_debug.h"
#include "FreeRTOS.h"
#include "task.h"

// 从设备地址
#define MPU6050_I2C_ADDRESS 0x68
// 读写地址
#define MPU6050_I2C_READ_ADDRESS 0xD1
#define MPU6050_I2C_WRITE_ADDRESS 0xD0

/**
 * @brief 初始化MPU6050传感器
 */
void Int_MPU6050_Init(void);

/**
 * @brief 获取3轴角速度数据
 * @param gyro_data 角速度数据
 */
void Int_MPU6050_Get_Gyro(Gyro_Data_Type *gyro_data);

/**
 * @brief 获取3轴加速度数据
 * @param accel_data 加速度数据
 */
void Int_MPU6050_Get_Accel(Accel_Data_Type *accel_data);

/**
 * @brief 获取6轴数据
 * @param data 6轴数据
 */
void Int_MPU6050_Get_Data(Gyro_Accel_Data_Type *data);

#endif // __INT_MPU6050_H__
