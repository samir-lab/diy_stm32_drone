#include "Com_PID.h"

/**
 * @description: 单次PID计算
 * @param {PID_Type} *pid PID结构体指针
 */
void Com_PID_Calc(PID_Type *pid)
{
    // 1.计算误差值, 误差 = 测量值 - 目标值
    pid->error = pid->measure - pid->target;

    // 2.计算积分误差
    pid->integral += pid->error;

    // 3.计算微分误差
    if (pid->last_error == 0)
    {
        pid->last_error = pid->error;
    }
    float diff = pid->error - pid->last_error;

    // 4.计算输出
    pid->output = (pid->kp * pid->error) // 比例部分
                  + (pid->ki * pid->integral * pid->dt) // 积分部分
                  + (pid->kd * diff / pid->dt); // 微分部分

    // 5.保存上一次误差值
    pid->last_error = pid->error;
}

/**
 * @description: 串级PID计算(内环+外环)
 * @param {PID_Type} *out_pid 输出PID结构体指针
 * @param {PID_Type} *in_pid 输入PID结构体指针
 */
void Com_PID_Calc_Chain(PID_Type *out_pid, PID_Type *in_pid)
{
    // 1.先计算外环
    Com_PID_Calc(out_pid);
    // 2.将外环的输出作为内环的目标值
    in_pid->target = out_pid->output;
    // 3.计算内环
    Com_PID_Calc(in_pid);
}
