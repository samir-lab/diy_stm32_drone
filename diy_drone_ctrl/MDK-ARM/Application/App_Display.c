#include "App_Display.h"
#include <stdio.h>

static void App_Display_Draw_Progress_Bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value, uint16_t max_value)
{
    uint16_t fill_width = 0;
    if (max_value > 0)
    {
        fill_width = (uint16_t)((uint32_t)value * width / max_value);
    }

    if (width > 118)
    {
        width = 118;
    }

    OLED_Fill(x, y, x + width, y + height, 0);
    if (fill_width > 0)
    {
        OLED_Fill(x, y, x + fill_width, y + height, 1);
    }
}

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
    char buffer[32];
    uint16_t throttle_percent = (uint16_t)((uint32_t)remote_ctrl_data.throttle * 100U / 1000U);
    uint16_t battery_v = flight_ctrl_data.voltage;
    uint16_t pid_p_pitch = flight_ctrl_data.pid_p_pitch;
    uint16_t pid_p_roll = flight_ctrl_data.pid_p_roll;

    /* 只清理各区域，避免整屏清空造成明显闪烁 */
    OLED_Fill(0, 0, 127, 10, 0);
    OLED_Fill(0, 11, 127, 54, 0);
    OLED_Fill(0, 55, 127, 63, 0);

    /* 顶部标题栏 */
    OLED_Fill(0, 0, 127, 10, 1);
    OLED_ShowString(0, 0, (const uint8_t *)"Samir", 12, 0);
    snprintf(buffer, sizeof(buffer), "%d.%02dV", battery_v / 100, battery_v % 100);
    OLED_ShowString(72, 0, (const uint8_t *)buffer, 12, 0);

    /* 中间区域：油门进度条 */
    OLED_ShowString(0, 15, (const uint8_t *)"THR", 12, 1);
    snprintf(buffer, sizeof(buffer), "%3d%%", throttle_percent);
    OLED_ShowString(66, 15, (const uint8_t *)buffer, 12, 1);
    App_Display_Draw_Progress_Bar(0, 28, 118, 6, (uint16_t)remote_ctrl_data.throttle, 1000);

    /* 中间区域：偏航/俯仰/横滚 */
    snprintf(buffer, sizeof(buffer), "Y:%4d", remote_ctrl_data.yaw);
    OLED_ShowString(0, 38, (const uint8_t *)buffer, 12, 1);

    snprintf(buffer, sizeof(buffer), "P:%4d", remote_ctrl_data.pitch);
    OLED_ShowString(0, 50, (const uint8_t *)buffer, 12, 1);

    snprintf(buffer, sizeof(buffer), "R:%4d", remote_ctrl_data.roll);
    OLED_ShowString(60, 50, (const uint8_t *)buffer, 12, 1);

    /* 底部状态栏 */
    OLED_Fill(0, 55, 127, 63, 1);
    snprintf(buffer, sizeof(buffer), "P:%d.%02d", pid_p_pitch / 100, pid_p_pitch % 100);
    OLED_ShowString(0, 56, (const uint8_t *)buffer, 12, 0);
    snprintf(buffer, sizeof(buffer), "R:%d.%02d", pid_p_roll / 100, pid_p_roll % 100);
    OLED_ShowString(64, 56, (const uint8_t *)buffer, 12, 0);

    OLED_Refresh_Gram();
}
