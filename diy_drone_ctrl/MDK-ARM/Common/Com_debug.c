#include "Com_debug.h"

// 重写fputc函数
int fputc(int ch, FILE *f)
{
    // 通过串口1发送数据
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 1000);
    return ch;
}
