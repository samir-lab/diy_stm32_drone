#ifndef __APP_DATA_MIXER_H__
#define __APP_DATA_MIXER_H__

#include "Int_Key.h"
#include "Int_Joystick.h"
#include "Com_debug.h"
#include "Com_tool.h"
#include "App_Transmit_Data.h"

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
