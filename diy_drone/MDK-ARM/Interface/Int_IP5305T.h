#ifndef _INT_IP5305T_H_
#define _INT_IP5305T_H_

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"

/**
 * @brief 定期启动IP5305T，避免自动进入低功耗状态
 */
void Int_IP5305T_Start(void);

#endif // _INT_IP5305T_H_
