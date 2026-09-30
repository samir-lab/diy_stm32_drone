#include "App_FreeRTOS_Task.h"

// STMF103C8T6 SRAM: 20KB, FreeRTOSConfig.h中设置的总堆大小为12KB，剩余8KB给其他使用

// LED控制结构体对象定义
LED_HandleTypeDef left_top_led = { .port = LED1_GPIO_Port, .pin = LED1_Pin};
LED_HandleTypeDef right_top_led = { .port = LED2_GPIO_Port, .pin = LED2_Pin};
LED_HandleTypeDef right_bottom_led = { .port = LED3_GPIO_Port, .pin = LED3_Pin};
LED_HandleTypeDef left_bottom_led = { .port = LED4_GPIO_Port, .pin = LED4_Pin};

// 电机控制结构体对象定义
Motor_HandleTypeDef left_top_motor = { .htim = &htim3, .channel = TIM_CHANNEL_1,  .speed = 1};
Motor_HandleTypeDef left_bottom_motor = { .htim = &htim4, .channel = TIM_CHANNEL_4,  .speed = 1};
Motor_HandleTypeDef right_top_motor = { .htim = &htim2, .channel = TIM_CHANNEL_2,  .speed = 1};
Motor_HandleTypeDef right_bottom_motor = { .htim = &htim1, .channel = TIM_CHANNEL_3,  .speed = 1};


// 当前遥控器连接状态
Remote_State remote_state = REMOTE_CONNECTED;
// 当前飞行状态
Flight_State flight_state = FLIGHT_IDLE;

// 电源管理任务
void power_task(void *args);
TaskHandle_t power_task_handle;
#define POWER_TASK_PERIOD 10000

// 电机控制任务
void motor_task(void *args);
TaskHandle_t motor_task_handle;
#define MOTOR_TASK_PERIOD 6

// LED灯控任务
void led_task(void *args);
TaskHandle_t led_task_handle;
#define LED_TASK_PERIOD 100

// 通讯任务
void comm_task(void *args);
TaskHandle_t comm_task_handle;
#define COMM_TASK_PERIOD 6 // 6毫秒

void App_FreeRTOS_Start(void)
{
    // 1. 创建任务
    xTaskCreate(power_task, "power_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGHEST, &power_task_handle);
    xTaskCreate(motor_task, "motor_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGH, &motor_task_handle);
    xTaskCreate(led_task, "led_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_LOW, &led_task_handle);
    xTaskCreate(comm_task, "comm_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_MEDIUM, &comm_task_handle);

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

void motor_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    while(1){
        // 每6ms执行一次 => 电机控制任务
        vTaskDelayUntil(&last_wake_time, MOTOR_TASK_PERIOD);
    

        // 启动电机
        // Int_Motor_Start(&left_top_motor);
        // Int_Motor_Start(&left_bottom_motor);
        // Int_Motor_Start(&right_top_motor);
        // Int_Motor_Start(&right_bottom_motor);
    }
}

void led_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    uint8_t count = 0;
    while(1){
        // 每100ms执行一次 => LED灯控任务
        vTaskDelayUntil(&last_wake_time, LED_TASK_PERIOD);
        count++;

        if(remote_state == REMOTE_CONNECTED)
        {
            // 遥控器已连接，LED灯常亮
            Int_LED_Turn_On(&left_top_led);
            Int_LED_Turn_On(&right_top_led);
            Int_LED_Turn_On(&right_bottom_led);
            Int_LED_Turn_On(&left_bottom_led);
            
            if(flight_state == FLIGHT_IDLE)
            {
                // 慢闪 => 1000ms亮，1000ms灭
                if(count == 10) {
                    Int_LED_Toggle(&left_top_led);
                    Int_LED_Toggle(&right_top_led);
                    Int_LED_Toggle(&right_bottom_led);
                    Int_LED_Toggle(&left_bottom_led);
                }
            } 
            else if(flight_state == FLIGHT_TAKEOFF)
            {
                // 快闪 => 200ms亮，200ms灭
                if(count % 2 == 0) {
                    Int_LED_Toggle(&left_top_led);
                    Int_LED_Toggle(&right_top_led);
                    Int_LED_Toggle(&right_bottom_led);
                    Int_LED_Toggle(&left_bottom_led);
                }
            } 
            else if(flight_state == FLIGHT_HOVER)
            {
                // 中闪 => 500ms亮，500ms灭
                if(count % 5 == 0) {
                    Int_LED_Toggle(&left_top_led);
                    Int_LED_Toggle(&right_top_led);
                    Int_LED_Toggle(&right_bottom_led);
                    Int_LED_Toggle(&left_bottom_led);
                }
            } 
            else if(flight_state == FLIGHT_FAULT)
            {
                // 故障状态，LED灯熄灭
                Int_LED_Turn_Off(&left_top_led);
                Int_LED_Turn_Off(&right_top_led);
                Int_LED_Turn_Off(&right_bottom_led);
                Int_LED_Turn_Off(&left_bottom_led);
            }
        } 
        else if(remote_state == REMOTE_DISCONNECTED) 
        {
            // 遥控器未连接，LED灯闪烁
            Int_LED_Toggle(&left_top_led);
            Int_LED_Toggle(&right_top_led);
            Int_LED_Toggle(&right_bottom_led);
            Int_LED_Toggle(&left_bottom_led);
        }
        
        // 计数清零
        if(count == 10)
        {
            count = 0;
        }
    }
}


// 通讯任务接收数据缓冲区
uint8_t comm_rx_buffer[SI24R1_TX_PLOAD_WIDTH] = {0};
void comm_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    while(1){
        // 每6ms执行一次 => 通讯任务
        vTaskDelayUntil(&last_wake_time, COMM_TASK_PERIOD);
        // 1. 接收数据到缓冲区
        uint8_t res = Int_SI24R1_RxPacket(comm_rx_buffer);
        if(res == 0)
        {
            debug_printf("Received data: %s\n", comm_rx_buffer);
            // 解析数据
        }
    }
}
