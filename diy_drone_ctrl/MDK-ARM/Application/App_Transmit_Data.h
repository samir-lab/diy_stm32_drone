#ifndef __App_Transmit_Data_H__
#define __App_Transmit_Data_H__

#include "Int_SI24R1.h"

typedef struct {
    int16_t throttle; // 油门值
    int16_t yaw;      // 偏航值
    int16_t pitch;    // 俯仰值
    int16_t roll;     // 横滚值
    uint8_t shutdown;  // 关机标志位   1:关机 0:不关机
    uint8_t fixed_height; // 定高标志位   切换为1表示定高，切换为0表示不定高
    uint8_t fixed_position; // 悬停标志位   切换为1表示悬停，切换为0表示不悬停
    uint16_t pid_p_pitch; // 俯仰PID比例系数（*100后的整数值）
    uint16_t pid_p_roll;  // 横滚PID比例系数（*100后的整数值）
} Remote_Ctrl_Data_Type;

typedef struct {
    uint16_t voltage; // 电池电压，voltage*100后的整数值
    uint16_t pid_p_pitch; // 俯仰PID比例系数（*100后的整数值）
    uint16_t pid_p_roll;  // 横滚PID比例系数（*100后的整数值）
} Flight_Ctrl_Data_Type;


// 定义帧头校验3字节
#define FRAME_HEADER_CHECK_1 'G'
#define FRAME_HEADER_CHECK_2 'K'
#define FRAME_HEADER_CHECK_3 'Q'

#define MAX_RETRY_COUNT 10

/**
 * @brief 自动切换SI24R1模式，发送数据
 */
void App_Transmit_Data(void);

/**
 * @brief 接收数据
 */
void App_Receive_Data(void);

#endif // __App_Transmit_Data_H__
