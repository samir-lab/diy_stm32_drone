#ifndef __APP_FREERTOS_TASK_H__
#define __APP_FREERTOS_TASK_H__

#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"
#include "Com_config.h"
#include "Int_IP5305T.h"
#include "Int_Motor.h"
#include "Int_LED.h"
#include "Int_MPU6050.h"
#include "App_Receive_Data.h"
#include "App_Flight_Ctrl.h"

/**
 * @brief 启动FreeRTOS操作系统
 */
void App_FreeRTOS_Start(void);

#endif
