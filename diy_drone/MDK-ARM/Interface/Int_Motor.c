#include "Int_Motor.h"

void Int_Motor_Set_Speed(Motor_HandleTypeDef *motor)
{
    // 限制速度范围在0~1000之间
    if (motor->speed < 0) 
    {
        motor->speed = 0;
    }
    else if (motor->speed > 1000) 
    {
        motor->speed = 1000;
    }

    // 设置定时器的比较值，从而改变占空比
    __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, motor->speed);
}

void Int_Motor_Start(Motor_HandleTypeDef *motor)
{
    HAL_TIM_PWM_Start(motor->htim, motor->channel);
}
