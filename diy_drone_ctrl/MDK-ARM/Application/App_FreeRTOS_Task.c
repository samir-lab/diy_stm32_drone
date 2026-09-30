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
#define POWER_TASK_PERIOD 10000 // 10秒

// 通讯任务
void comm_task(void *args);
TaskHandle_t comm_task_handle;
#define COMM_TASK_PERIOD 6 // 6毫秒

void App_FreeRTOS_Start(void)
{
    // 1. 创建任务
    xTaskCreate(power_task, "power_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGHEST, &power_task_handle);

    // 2. 创建通讯任务
    xTaskCreate(comm_task, "comm_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGH, &comm_task_handle);

    // 3. 启动调度器
    vTaskStartScheduler();
}


void power_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    while(1){
        // 每10秒执行一次 => 启动电源 避免芯片转入低功耗模式
        vTaskDelayUntil(&last_wake_time, POWER_TASK_PERIOD);
        Int_IP5305T_Start();
    }
}


// 通讯任务发送数据缓冲区
uint8_t comm_tx_buff[SI24R1_TX_PLOAD_WIDTH] = {0}; 
void comm_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    while(1){
        // 每6毫秒执行一次
        vTaskDelayUntil(&last_wake_time, COMM_TASK_PERIOD);
        // 调用SI24R1接口，发送数据
        // 1. 进入TX模式
        Int_SI24R1_TX_Mode();
        // 2. 发送数据
        comm_tx_buff[0] = 'h';
        comm_tx_buff[1] = 'e';
        comm_tx_buff[2] = 'l';
        comm_tx_buff[3] = 'l';
        comm_tx_buff[4] = 'o';
        comm_tx_buff[5] = '!';
        Int_SI24R1_TxPacket(comm_tx_buff);
        // 3. 恢复RX模式
        Int_SI24R1_RX_Mode();
    }
}
