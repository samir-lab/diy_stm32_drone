#include "App_Data_Mixer.h"

// 遥控数据
extern Remote_Ctrl_Data_Type remote_ctrl_data;

// 摇杆数据
Joystick_Type joystick;

// 摇杆零偏数据
int16_t throttle_offset = 0; // 油门零偏
int16_t yaw_offset = 0;      // 偏航零偏
int16_t pitch_offset = 0;    // 俯仰零偏
int16_t roll_offset = 0;     // 横滚零偏
/**
 * @brief 校准摇杆(计算平均误差)
 */
void App_Joystick_Calibrate(void)
{
    // 多次读取误差取平均值
    int16_t throttle_offset_sum = 0;
    int16_t yaw_offset_sum = 0;
    int16_t pitch_offset_sum = 0;
    int16_t roll_offset_sum = 0;
    for(uint8_t i = 0; i < 10; i++)
    {
        App_Joystick_Data_Process();
        throttle_offset_sum += joystick.throttle - 0;
        yaw_offset_sum += joystick.yaw - 500;
        pitch_offset_sum += joystick.pitch - 500;
        roll_offset_sum += joystick.roll - 500;
        // 延迟10ms
        vTaskDelay(10); 
    }

    throttle_offset += throttle_offset_sum / 10;
    yaw_offset += yaw_offset_sum / 10;
    pitch_offset += pitch_offset_sum / 10;
    roll_offset += roll_offset_sum / 10;
}

/**
 * @brief 处理按键数据
 */
void App_Key_Data_Process(void)
{
    // 调用按键接口，获取按键状态
    Key_Type key = Int_Key_GetKey();
    if(key == KEY_NONE)
    {
        // 没有按键按下，继续等待
        return;
    }
    // 处理按键事件
    switch(key){
        case KEY_UP:
            // 处理KEY_UP事件
            remote_ctrl_data.pid_p_pitch += 2;
            break;
        case KEY_DOWN:
            // 处理KEY_DOWN事件
            if (remote_ctrl_data.pid_p_roll - 2 < 0)
            {
                remote_ctrl_data.pid_p_pitch = 0;
            }
            else
            {
                remote_ctrl_data.pid_p_pitch -= 2;
            }
            break;
        case KEY_LEFT:
            // 处理KEY_LEFT事件
            if (remote_ctrl_data.pid_p_roll - 2 < 0)
            {
                remote_ctrl_data.pid_p_roll = 0;
            }
            else
            {
                remote_ctrl_data.pid_p_roll -= 2;
            }
            break;
        case KEY_RIGHT:
            // 处理KEY_RIGHT事件
            remote_ctrl_data.pid_p_roll += 2;
            break;
        case KEY_LEFT_X:
            // 处理KEY_LEFT_X事件（短按切换悬停）
            remote_ctrl_data.fixed_position = (uint8_t)!remote_ctrl_data.fixed_position;
            break;
        case KEY_LEFT_X_LONG:
            // 处理KEY_LEFT_X_LONG事件（长按开关机）
            remote_ctrl_data.shutdown = (uint8_t)!remote_ctrl_data.shutdown;
            break;
        case KEY_RIGHT_X:
            // 处理KEY_RIGHT_X事件（短按切换定高）
            remote_ctrl_data.fixed_height = (uint8_t)!remote_ctrl_data.fixed_height; 
            break;
        case KEY_RIGHT_X_LONG:
            // 处理KEY_RIGHT_X_LONG事件（长按摇杆校准）
            App_Joystick_Calibrate();
            break;
        default:
            break;
    }
}


/**
 * @brief 初始化摇杆
 */
void App_Joystick_Init(void)
{
    // 调用摇杆接口，初始化摇杆
    Int_Joystick_Init();
}

/**
 * @brief 处理摇杆数据
 */
void App_Joystick_Data_Process(void)
{
    // 进入临界区
    taskENTER_CRITICAL();

    // 1.调用摇杆接口，获取摇杆状态
    Int_Joystick_Get(&joystick);

    // 2.处理范围和极性 => 想要的值为0-1000 ADC的范围是0-4095
    joystick.throttle = joystick.throttle * 1000 / 4095;
    joystick.yaw = 1000 - joystick.yaw * 1000 / 4095;
    joystick.pitch = joystick.pitch * 1000 / 4095;
    joystick.roll = joystick.roll * 1000 / 4095;
    
    // 3.处理零偏校准
    joystick.throttle -= throttle_offset;
    joystick.yaw -= yaw_offset;
    joystick.pitch -= pitch_offset;
    joystick.roll -= roll_offset;

    // 4.限制范围
    joystick.throttle = Com_Tool_Limit(joystick.throttle, 0, 1000);
    joystick.yaw = Com_Tool_Limit(joystick.yaw, 0, 1000);
    joystick.pitch = Com_Tool_Limit(joystick.pitch, 0, 1000);
    joystick.roll = Com_Tool_Limit(joystick.roll, 0, 1000);

    remote_ctrl_data.throttle = joystick.throttle;
    remote_ctrl_data.yaw = joystick.yaw;
    remote_ctrl_data.pitch = joystick.pitch;
    remote_ctrl_data.roll = joystick.roll;
    // 退出临界区
    taskEXIT_CRITICAL();
}
