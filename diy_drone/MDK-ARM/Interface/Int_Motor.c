#include "Int_Motor.h"

void Int_Motor_Set_Speed(Motor_HandleTypeDef *motor)
{
    // 限制速度范围在0~100之间
    if (motor->speed < 0) {
        motor->speed = 0;
    } else if (motor->speed > 100) {
        motor->speed = 100;
    }

    // 将速度值映射到定时器的计数范围
    // 定时器的计数范围为1000，则占空比为speed/1000
    uint16_t compare_value = (motor->speed * 1000) / 100;

    // 设置定时器的比较值，从而改变占空比
    __HAL_TIM_SET_COMPARE(motor->htim, motor->channel, compare_value);
}

void Int_Motor_Start(Motor_HandleTypeDef *motor)
{
    HAL_TIM_PWM_Start(motor->htim, motor->channel);
}
