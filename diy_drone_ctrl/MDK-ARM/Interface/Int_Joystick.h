#ifndef __INT_JOYSTICK_H__
#define __INT_JOYSTICK_H__

#include "adc.h"

typedef struct {
    int16_t throttle; // 油门值
    int16_t yaw;      // 偏航值
    int16_t pitch;    // 俯仰值
    int16_t roll;     // 横滚值
} Joystick_Type;

/**
 * @brief 初始化摇杆接口
 */
void Int_Joystick_Init(void);


/**
 * @brief 获取摇杆的当前状态
 * @param joystick 指向Joystick_Type结构体的指针，用于存储摇杆的当前状态
 */
void Int_Joystick_Get(Joystick_Type *joystick);

#endif /* __INT_JOYSTICK_H__ */
