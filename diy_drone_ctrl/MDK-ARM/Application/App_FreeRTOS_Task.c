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

// 按键任务
void key_task(void *args);
TaskHandle_t key_task_handle;
#define KEY_TASK_PERIOD 20 // 20毫秒

// 摇杆任务
void joystick_task(void *args);
TaskHandle_t joystick_task_handle;
#define JOYSTICK_TASK_PERIOD 20 // 20毫秒

// OLED任务
void oled_task(void *args);
TaskHandle_t oled_task_handle;
#define OLED_TASK_PERIOD 100 // 100毫秒

void App_FreeRTOS_Start(void)
{
    // 1. 创建任务
    xTaskCreate(power_task, "power_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGHEST, &power_task_handle);
    xTaskCreate(comm_task, "comm_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGH, &comm_task_handle);
    xTaskCreate(key_task, "key_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_MEDIUM, &key_task_handle);
    xTaskCreate(joystick_task, "joystick_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_MEDIUM, &joystick_task_handle);
    xTaskCreate(oled_task, "oled_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_MEDIUM, &oled_task_handle);

    // 2. 启动调度器
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


void comm_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    while(1){
        // 每6毫秒执行一次
        vTaskDelayUntil(&last_wake_time, COMM_TASK_PERIOD);
        App_Transmit_Data();
    }
}

void key_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    while(1){
        // 每20毫秒执行一次
        vTaskDelayUntil(&last_wake_time, KEY_TASK_PERIOD);
        // 处理按键数据
        App_Key_Data_Process();
    }
}

void joystick_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    // 摇杆ADC初始化
    App_Joystick_Init();
    while(1){
        // 每20毫秒执行一次
        vTaskDelayUntil(&last_wake_time, JOYSTICK_TASK_PERIOD);
        // 处理摇杆数据
        App_Joystick_Data_Process();
    }
}

void oled_task(void *args)
{
    TickType_t last_wake_time = xTaskGetTickCount();
    App_Dispaly_Init();
    while(1){
        vTaskDelayUntil(&last_wake_time, OLED_TASK_PERIOD);
        App_Dispaly_Show();
    }
}
