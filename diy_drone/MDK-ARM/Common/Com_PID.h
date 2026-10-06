#ifndef __COM_PID_H__
#define __COM_PID_H__

// PID结构体，如果CPU性能足够好，推荐用double类型
// kp,ki,kd需要在初始化时确定，目标值和测量值需要在计算时传入
typedef struct
{
    float kp; // 比例部分，值越大，响应越快
    float ki; // 积分部分，解决稳态误差
    float kd; // 微分部分，解决过调震荡，值越大，抑制效果越好
    float error; // 误差值
    float last_error; // 上一次误差值
    float integral; // 积分累计
    float output; // 输出结果
    float target; // 目标值
    float measure; // 测量值
    float dt; // 采样周期
} PID_Type;

/**
 * @description: 单次PID计算
 * @param {PID_Type} *pid PID结构体指针
 */
void Com_PID_Calc(PID_Type *pid);

/**
 * @description: 串级PID计算(内环+外环)
 * @param {PID_Type} *out_pid 输出PID结构体指针
 * @param {PID_Type} *in_pid 输入PID结构体指针
 */
void Com_PID_Calc_Chain(PID_Type *out_pid, PID_Type *in_pid);

#endif // __COM_PID_H__
