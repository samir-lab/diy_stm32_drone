#ifndef __INT_VL53L1X_H__
#define __INT_VL53L1X_H__

#include "vl53l1_platform.h"
#include "VL53L1X_api.h"
#include "VL53L1X_calibration.h"
#include "FreeRTOS.h"
#include "task.h"
#include "Com_debug.h"

/**
 * @brief 初始化VL53L1X传感器
 */
void Int_VL53L1X_Init(void);

/**
 * @brief 获取VL53L1X传感器的距离值
 */
uint16_t Int_VL53L1X_Get_Distance(void);

#endif // __INT_VL53L1X_H__
