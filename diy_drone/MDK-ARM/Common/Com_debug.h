#ifndef __COM_DEBUG_H__
#define __COM_DEBUG_H__ 

#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

// 添加日志打印开关
#define DEBUG_PRINTF_ON 1

#if DEBUG_PRINTF_ON == 1
// 只打印文件名
#define __FILE_NAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
// 添加文件名和行号
#define debug_printf(format, ...) printf("[%s:%d] " format, __FILE_NAME__, __LINE__, ##__VA_ARGS__)

#else
#define debug_printf(format, ...)
#endif

#endif // __COM_DEBUG_H__
