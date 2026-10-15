#include "Int_Joystick.h"

/**
 * @brief 初始化摇杆接口
 */
// 用于存储ADC采集的4个通道数据的缓冲区
uint16_t adc_buffer[4] = {0}; 
void Int_Joystick_Init(void)
{
    // 开启ADC1的DMA传输，采集4个通道的数据
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, 4);
}


/**
 * @brief 获取摇杆的当前状态
 * @param joystick 指向Joystick_Type结构体的指针，用于存储摇杆的当前状态
 */
void Int_Joystick_Get(Joystick_Type *joystick)
{
    // 将ADC采集的原始数据转换为摇杆的油门、偏航、俯仰和横滚值
    joystick->throttle = (int16_t)adc_buffer[0]; // 油门值
    joystick->yaw      = (int16_t)adc_buffer[1]; // 偏航值
    joystick->roll     = (int16_t)adc_buffer[2]; // 横滚值
    joystick->pitch    = (int16_t)adc_buffer[3]; // 俯仰值
}
