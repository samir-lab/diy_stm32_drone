#ifndef __APP_DATA_MIXER_H__
#define __APP_DATA_MIXER_H__

#include "Int_Key.h"
#include "Int_Joystick.h"
#include "Com_debug.h"
#include "Com_tool.h"

typedef struct {
    int16_t throttle; // 油门值
    int16_t yaw;      // 偏航值
    int16_t pitch;    // 俯仰值
    int16_t roll;     // 横滚值
    uint8_t shutdown;  // 关机标志位   1:关机 0:不关机
    uint8_t fixed_height; // 定高标志位   切换为1表示定高，切换为0表示不定高
    uint8_t fixed_position; // 悬停标志位   切换为1表示悬停，切换为0表示不悬停
} Mixed_Ctrl_Data_Type;


/**
 * @brief 处理按键数据
 */
void App_Key_Data_Process(void);

/**
 * @brief 初始化摇杆
 */
void App_Joystick_Init(void);
/**
 * @brief 处理摇杆数据
 */
void App_Joystick_Data_Process(void);

#endif // __APP_DATA_MIXER_H__
