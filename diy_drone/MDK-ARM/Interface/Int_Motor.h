#ifndef __INT_MOTOR_H__
#define __INT_MOTOR_H__

#include "tim.h"

typedef struct {
    TIM_HandleTypeDef *htim; // 定时器句柄
    uint16_t channel;        // 定时器通道
    int8_t speed;          // 当前速度值 speed范围为0-100，可精确到小数点后一位， 默认值为20，即占空比为0.2
} Motor_HandleTypeDef;


/**
 * 设置电机速度
 */
void Int_Motor_Set_Speed(Motor_HandleTypeDef *motor);

/**
 * 启动电机
 */
void Int_Motor_Start(Motor_HandleTypeDef *motor);

#endif // __INT_MOTOR_H__
