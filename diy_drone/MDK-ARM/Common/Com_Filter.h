#ifndef __COM_FILTER_H__
#define __COM_FILTER_H__

#include "Com_debug.h"

// 卡尔曼滤波器结构体
typedef struct 
{
    float Last_P;  // 上一时刻的状态方差（或协方差）
    float Now_P;   // 当前时刻的状态方差（或协方差）
    float out;     // 滤波输出值，即估计的状态
    float Kg;      // 卡尔曼增益，用于调节预测值和测量值之间的权重
    float Q;       // 过程噪声的方差，反映系统模型的不确定性
    float R;       // 测量噪声的方差，反映测量数据的不确定性
} KalmanFilter_Type;


int16_t Common_Filter_LowPass(int16_t input, int16_t pre_output, float weight);

double Common_Filter_Kalman(KalmanFilter_Type *kf, double input);

#endif // __COM_FILTER_H__
