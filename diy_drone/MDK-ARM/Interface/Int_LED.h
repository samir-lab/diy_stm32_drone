#ifndef __INT_LED_H__
#define __INT_LED_H__

#include "main.h"

typedef struct {
    GPIO_TypeDef *port; // LED所在的GPIO端口
    uint16_t pin;       // LED所在的GPIO引脚
} LED_HandleTypeDef;


/**
 * @brief 点亮LED
 */
void Int_LED_Turn_On(LED_HandleTypeDef *led);

/**
 * @brief 熄灭LED
 */
void Int_LED_Turn_Off(LED_HandleTypeDef *led);

/**
 * @brief 切换LED状态
 */
void Int_LED_Toggle(LED_HandleTypeDef *led);

#endif
