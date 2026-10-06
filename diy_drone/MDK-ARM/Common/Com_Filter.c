#include "Com_Filter.h"

/**
 * @brief 一阶低通滤波算法，加权平均滤波器
 * @param input 输入值
 * @param pre_output 上一次输出值
 * @param weight 加权系数
 */
int16_t Common_Filter_LowPass(int16_t input, int16_t pre_output, float weight)
{
    return (int16_t)(weight * input + (1 - weight) * pre_output);
}


/**
 * 卡尔曼滤波算法介绍：
 * https://www.mwrf.net/tech/basic/2023/30081.html
 * https://kalmanfilter.net/CN/default_cn.aspx
 * @brief 卡尔曼滤波算法
 * @param kf 卡尔曼滤波器结构体指针
 * @param input 输入值
 */
KalmanFilter_Type kf_init_param[3] = {
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543},
    {0.02, 0, 0, 0, 0.001, 0.543}
};
double Common_Filter_Kalman(KalmanFilter_Type *kf, double input)
{
    kf->Now_P = kf->Last_P + kf->Q;
    kf->Kg = kf->Now_P / (kf->Now_P + kf->R);
    kf->out = kf->out + kf->Kg * (input - kf->out);
    kf->Last_P = (1 - kf->Kg) * kf->Now_P;
    return kf->out;
}
