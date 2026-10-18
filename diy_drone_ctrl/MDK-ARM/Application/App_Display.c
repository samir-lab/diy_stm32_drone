#include "App_Display.h"


/**
 * 初始化OLED显示屏
 */
void App_Dispaly_Init(void)
{
	OLED_Init();
}

/**
 * 显示屏幕内容
 */
void App_Dispaly_Show(void)
{
    // x坐标，y坐标，显示内容，字体大小
    OLED_ShowString(0, 0, "Hello World", 16, 1);

    // 刷写屏幕
    OLED_Refresh_Gram();
}
