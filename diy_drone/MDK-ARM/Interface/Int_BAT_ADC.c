#include "Int_BAT_ADC.h"

/**
 * 启动ADC采样
 */
void Int_BAT_ADC_Init(void)
{
    // 1.打开使能引脚
    HAL_GPIO_WritePin(BAT_ADC_EN_GPIO_Port, BAT_ADC_EN_Pin, GPIO_PIN_RESET);

    // 2.完成ADC初始化
    HAL_ADC_Start(&hadc1);
}

/**
 * 读取电池电压
 */
float Int_BAT_ADC_Read(void)
{
    uint32_t adc_value = HAL_ADC_GetValue(&hadc1);
    float voltage = (adc_value * 3.3f / 4095.0f) * 2.0f;
    return voltage;
}

