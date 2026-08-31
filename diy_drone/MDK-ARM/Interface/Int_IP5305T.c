#include "Int_IP5305T.h"


void Int_IP5305T_Start(void)
{
    // 拉低电源按键引脚，模拟按下电源键，启动IP5305T
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_RESET);
    // 延时100ms时间（30ms - 2s之间），确保按键被识别
    vTaskDelay(100);
    // 拉高电源按键引脚，模拟松开电源键
    HAL_GPIO_WritePin(POWER_KEY_GPIO_Port, POWER_KEY_Pin, GPIO_PIN_SET);
}
