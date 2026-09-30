#ifndef __APP_FREERTOS_TASK_H__
#define __APP_FREERTOS_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"
#include "Int_IP5305T.h"
#include "Int_SI24R1.h"

/**
 * @brief 启动FreeRTOS操作系统
 */
void App_FreeRTOS_Start(void);

#endif
