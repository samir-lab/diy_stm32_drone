#include "Int_LED.h"

void Int_LED_Turn_On(LED_HandleTypeDef *led)
{
    // 拉低GPIO引脚电平，点亮LED
    HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_RESET);
}


void Int_LED_Turn_Off(LED_HandleTypeDef *led)
{
    // 拉高GPIO引脚电平，熄灭LED
    HAL_GPIO_WritePin(led->port, led->pin, GPIO_PIN_SET);
}

void Int_LED_Toggle(LED_HandleTypeDef *led)
{
    // 切换GPIO引脚电平，切换LED状态
    HAL_GPIO_TogglePin(led->port, led->pin);
}
