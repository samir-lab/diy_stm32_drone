#include "App_FreeRTOS_Task.h"

// STMF103C8T6 SRAM: 20KB, FreeRTOSConfig.h中设置的总堆大小为12KB，剩余8KB给其他使用

// 最新推荐堆大小：128 => 128 * 4 = 512字节
#define DEFAULT_TASK_STACK_SIZE 128
// 任务优先级，数值越大，优先级越高，范围为0~(configMAX_PRIORITIES-1) | 不推荐使用最小优先级0(给idle任务使用)
#define TASK_PRIORITY_LOW 1
#define TASK_PRIORITY_MEDIUM 2
#define TASK_PRIORITY_HIGH 3
#define TASK_PRIORITY_HIGHEST 4

// 电源管理任务
void power_task(void *args);
TaskHandle_t power_task_handle;

void App_FreeRTOS_Start(void)
{
    // 1. 创建任务
    xTaskCreate(power_task, "power_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGHEST, &power_task_handle);

    // 2. 启动调度器
    vTaskStartScheduler();
}


void power_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    while(1){
        // 每10秒执行一次 => 启动电源 避免芯片转入低功耗模式
        vTaskDelayUntil(&last_wake_time, 10000);
        Int_IP5305T_Start();
    }
}
