#ifndef __INT_KEY_H__
#define __INT_KEY_H__

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"

typedef enum {
    KEY_NONE = 0,
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_LEFT_X,
    KEY_LEFT_X_LONG,
    KEY_RIGHT_X,
    KEY_RIGHT_X_LONG,
} Key_Type;


/**
 * @brief 获取按键状态
 * @return Key_Type 按键类型
 */
Key_Type Int_Key_GetKey(void);

#endif /* __INT_KEY_H__ */
