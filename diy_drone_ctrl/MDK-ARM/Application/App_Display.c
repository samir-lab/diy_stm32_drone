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

    debug_printf("voltage: %d, pitch_p: %d, roll_p: %d\n", flight_ctrl_data.voltage, remote_ctrl_data.pid_p_pitch, remote_ctrl_data.pid_p_roll);

    // 在屏幕第二到第四行展示flight_ctrl_data的voltage、pid_p_pitch、pid_p_roll
    char str[16] = {0};
    sprintf(str, "voltage: %d", flight_ctrl_data.voltage);
    OLED_ShowString(0, 16, str, 12, 1);
    sprintf(str, "pitch_p: %d", remote_ctrl_data.pid_p_pitch);
    OLED_ShowString(0, 32, str, 12, 1);
    sprintf(str, "roll_p: %d", remote_ctrl_data.pid_p_roll);
    OLED_ShowString(0, 48, str, 12, 1);


    // 刷写屏幕
    OLED_Refresh_Gram();
}
