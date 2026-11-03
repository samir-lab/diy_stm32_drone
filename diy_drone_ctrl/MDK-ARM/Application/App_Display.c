#include "App_Display.h"
#include <stdio.h>
#include <string.h>

static void App_Display_Draw_Rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t dot)
{
    uint8_t x, y;

    if (x1 > x2)
    {
        uint8_t tmp = x1;
        x1 = x2;
        x2 = tmp;
    }
    if (y1 > y2)
    {
        uint8_t tmp = y1;
        y1 = y2;
        y2 = tmp;
    }

    if (x1 > 127) x1 = 127;
    if (x2 > 127) x2 = 127;
    if (y1 > 63) y1 = 63;
    if (y2 > 63) y2 = 63;

    for (x = x1; x <= x2; x++)
    {
        for (y = y1; y <= y2; y++)
        {
            OLED_DrawPoint(x, y, dot);
        }
    }
}

static void App_Display_Draw_Progress_Bar(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint16_t value, uint16_t max_value)
{
    uint16_t fill_width = 0;

    if (width > 128)
    {
        width = 128;
    }

    if (max_value > 0)
    {
        fill_width = (uint16_t)((uint32_t)value * width / max_value);
    }

    App_Display_Draw_Rect(x, y, x + width - 1, y + height - 1, 0);
    if (fill_width > 0)
    {
        App_Display_Draw_Rect(x, y, x + fill_width - 1, y + height - 1, 1);
    }
}

static void App_Display_Show_Startup_Screen(void)
{
    static const uint8_t *startup_lines[] =
    {
        (const uint8_t *)"  __      _ ",
        (const uint8_t *)"o'')}____//",
        (const uint8_t *)" `_/      )",
        (const uint8_t *)" (_(_/-(_/"
    };
    uint8_t i;
    uint8_t start_x;
    uint8_t start_y;
    uint8_t line_len;

    App_Display_Draw_Rect(0, 0, 127, 63, 0);

    start_y = (uint8_t)((64U - 4U * 12U) / 2U);
    for (i = 0; i < 4; i++)
    {
        line_len = (uint8_t)strlen((const char *)startup_lines[i]);
        start_x = (uint8_t)((128U - line_len * 8U) / 2U);
        OLED_ShowString(start_x, start_y + i * 12U, startup_lines[i], 12, 1);
    }
    OLED_Refresh_Gram();
    vTaskDelay(3000);
}

/**
 * 初始化OLED显示屏
 */
void App_Dispaly_Init(void)
{
    OLED_Init();
    OLED_Clear();
    App_Display_Show_Startup_Screen();
}

/**
 * 显示屏幕内容
 */
extern Remote_Ctrl_Data_Type remote_ctrl_data;
extern Flight_Ctrl_Data_Type flight_ctrl_data;
void App_Dispaly_Show(void)
{
    char buffer[32];
    int16_t yaw_value = (int16_t)remote_ctrl_data.yaw - 500;
    int16_t pitch_value = (int16_t)remote_ctrl_data.pitch - 500;
    int16_t roll_value = (int16_t)remote_ctrl_data.roll - 500;
    uint16_t throttle_value = (uint16_t)remote_ctrl_data.throttle;
    uint16_t throttle_max = 940;
    uint16_t throttle_percent;
    uint16_t battery_v = flight_ctrl_data.voltage;
    uint16_t pid_p_pitch = flight_ctrl_data.pid_p_pitch;
    uint16_t pid_p_roll = flight_ctrl_data.pid_p_roll;

    if (throttle_max == 0)
    {
        throttle_max = 1;
    }
    if (throttle_value > throttle_max)
    {
        throttle_value = throttle_max;
    }

    throttle_percent = (uint16_t)((uint32_t)throttle_value * 100U / throttle_max);

    App_Display_Draw_Rect(0, 0, 127, 63, 0);
    App_Display_Draw_Rect(0, 0, 127, 10, 1);
    OLED_ShowString(2, 0, (const uint8_t *)"Samir", 12, 0);
    snprintf(buffer, sizeof(buffer), "%d.%02dV", battery_v / 100, battery_v % 100);
    OLED_ShowString(88, 0, (const uint8_t *)buffer, 12, 0);

    App_Display_Draw_Rect(0, 12, 127, 34, 1);
    App_Display_Draw_Rect(1, 13, 126, 33, 0);
    OLED_ShowString(6, 14, (const uint8_t *)"THR", 12, 1);
    snprintf(buffer, sizeof(buffer), "%3d%%", throttle_percent);
    OLED_ShowString(72, 14, (const uint8_t *)buffer, 12, 1);
    App_Display_Draw_Progress_Bar(6, 28, 116, 6, throttle_value, throttle_max);

    App_Display_Draw_Rect(0, 37, 127, 49, 1);
    App_Display_Draw_Rect(1, 38, 126, 48, 0);
    snprintf(buffer, sizeof(buffer), "Y%+3d P%+3d R%+3d", yaw_value, pitch_value, roll_value);
    OLED_ShowString(6, 38, (const uint8_t *)buffer, 12, 1);

    App_Display_Draw_Rect(0, 52, 127, 63, 1);
    snprintf(buffer, sizeof(buffer), "P:%d.%02d", pid_p_pitch / 100, pid_p_pitch % 100);
    OLED_ShowString(2, 53, (const uint8_t *)buffer, 12, 0);
    snprintf(buffer, sizeof(buffer), "R:%d.%02d", pid_p_roll / 100, pid_p_roll % 100);
    OLED_ShowString(80, 53, (const uint8_t *)buffer, 12, 0);

    OLED_Refresh_Gram();
}
