#include "Int_Key.h"

/**
 * @brief 消抖后获取按键状态
 * @return Key_Type 按键类型
 */
Key_Type Int_Key_GetKey(void)
{
    GPIO_PinState key_state = HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin);
    // 若按下KEY_UP键
    if (key_state == GPIO_PIN_RESET) {
        // 消抖延迟10ms
        vTaskDelay(10);
        key_state = HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin);
        if (key_state == GPIO_PIN_RESET) {
            // 等待按键抬起
            while (HAL_GPIO_ReadPin(KEY_UP_GPIO_Port, KEY_UP_Pin) == GPIO_PIN_RESET);
            return KEY_UP;
        }
    }

    // 若按下KEY_DOWN键
    key_state = HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin);
    if (key_state == GPIO_PIN_RESET) {
        // 消抖延迟10ms
        vTaskDelay(10);
        key_state = HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin);
        if (key_state == GPIO_PIN_RESET) {
            // 等待按键抬起
            while (HAL_GPIO_ReadPin(KEY_DOWN_GPIO_Port, KEY_DOWN_Pin) == GPIO_PIN_RESET);
            return KEY_DOWN;
        }
    }

    // 若按下KEY_LEFT键
    key_state = HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin);
    if (key_state == GPIO_PIN_RESET) {
        // 消抖延迟10ms
        vTaskDelay(10);
        key_state = HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin);
        if (key_state == GPIO_PIN_RESET) {
            // 等待按键抬起
            while (HAL_GPIO_ReadPin(KEY_LEFT_GPIO_Port, KEY_LEFT_Pin) == GPIO_PIN_RESET);
            return KEY_LEFT;
        }
    }

    // 若按下KEY_RIGHT键
    key_state = HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin);
    if (key_state == GPIO_PIN_RESET) {
        // 消抖延迟10ms
        vTaskDelay(10);
        key_state = HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin);
        if (key_state == GPIO_PIN_RESET) {
            // 等待按键抬起
            while (HAL_GPIO_ReadPin(KEY_RIGHT_GPIO_Port, KEY_RIGHT_Pin) == GPIO_PIN_RESET);
            return KEY_RIGHT;
        }
    }

    // 若按下KEY_LEFT_X键
    key_state = HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin);
    if (key_state == GPIO_PIN_RESET) {
        // 记录按下时间
        TickType_t press_time = xTaskGetTickCount();
        // 消抖延迟10ms
        vTaskDelay(10);
        key_state = HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin);
        if (key_state == GPIO_PIN_RESET) {
            // 等待按键抬起
            while (HAL_GPIO_ReadPin(KEY_LEFT_X_GPIO_Port, KEY_LEFT_X_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            // 检查是否长按
            if ((xTaskGetTickCount() - press_time) >= pdMS_TO_TICKS(2000)) {
                // 长按超过2秒，返回KEY_LEFT_X_LONG
                return KEY_LEFT_X_LONG;
            }
            return KEY_LEFT_X;
        }
    }

    // 若按下KEY_RIGHT_X键
    key_state = HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin);
    if (key_state == GPIO_PIN_RESET) {
        // 记录按下时间
        TickType_t press_time = xTaskGetTickCount();
        // 消抖延迟10ms
        vTaskDelay(10);
        key_state = HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin);
        if (key_state == GPIO_PIN_RESET) {
            // 等待按键抬起
            while (HAL_GPIO_ReadPin(KEY_RIGHT_X_GPIO_Port, KEY_RIGHT_X_Pin) == GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
            // 检查是否长按
            if ((xTaskGetTickCount() - press_time) >= pdMS_TO_TICKS(2000)) {
                // 长按超过2秒，返回KEY_RIGHT_X_LONG
                return KEY_RIGHT_X_LONG;
            }
            return KEY_RIGHT_X;
        }
    }

    return KEY_NONE; // 没有按键按下
}
