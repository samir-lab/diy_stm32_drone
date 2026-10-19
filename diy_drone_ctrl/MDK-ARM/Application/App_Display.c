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
extern Remote_Ctrl_Data_Type remote_ctrl_data;
extern Flight_Ctrl_Data_Type flight_ctrl_data;
void App_Dispaly_Show(void)
{
    // x坐标，y坐标，显示内容，字体大小
    OLED_ShowString(0, 0, "Samir's MoonLab", 12, 1);
    OLED_ShowNum(0, 12, flight_ctrl_data.voltage, 12, 1);
    OLED_ShowNum(0, 24, flight_ctrl_data.pid_p_pitch, 12, 1);
    OLED_ShowNum(0, 36, flight_ctrl_data.pid_p_roll, 12, 1);

    // 刷写屏幕
    OLED_Refresh_Gram();
}
