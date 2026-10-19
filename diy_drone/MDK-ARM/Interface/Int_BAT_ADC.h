#ifndef __INT_BAT_ADC_H__
#define __INT_BAT_ADC_H__

#include "adc.h"

/**
 * 启动ADC采样
 */
void Int_BAT_ADC_Init(void);

/**
 * 读取电池电压
 */
float Int_BAT_ADC_Read(void);

#endif // __INT_BAT_ADC_H__
