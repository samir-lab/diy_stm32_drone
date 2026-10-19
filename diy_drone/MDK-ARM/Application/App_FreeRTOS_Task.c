#include "App_FreeRTOS_Task.h"

// STMF103C8T6 SRAM: 20KB, FreeRTOSConfig.h中设置的总堆大小为12KB，剩余8KB给其他使用

// LED控制结构体对象定义
LED_HandleTypeDef left_top_led = { .port = LED1_GPIO_Port, .pin = LED1_Pin};
LED_HandleTypeDef right_top_led = { .port = LED2_GPIO_Port, .pin = LED2_Pin};
LED_HandleTypeDef right_bottom_led = { .port = LED3_GPIO_Port, .pin = LED3_Pin};
LED_HandleTypeDef left_bottom_led = { .port = LED4_GPIO_Port, .pin = LED4_Pin};

// 当前遥控器连接状态
Remote_State remote_state = REMOTE_DISCONNECTED;
// 当前飞行状态
Flight_State flight_state = FLIGHT_IDLE;
// 定高目标值
uint16_t fixed_height_target = 0;
// 遥控数据结构体
Remote_Ctrl_Data_Type remote_ctrl_data = {
    .throttle = 0, 
    .yaw = 500, 
    .pitch = 500, 
    .roll = 500, 
    .shutdown = 0,
    .fixed_height = 0,
    .fixed_position = 0,
};

// 电源管理任务
void power_task(void *args);
TaskHandle_t power_task_handle;
#define POWER_TASK_PERIOD 10000

// 飞行控制任务
void flight_ctrl_task(void *args);
TaskHandle_t flight_ctrl_task_handle;
#define FLIGHT_CTRL_TASK_PERIOD 6

// LED灯控任务
void led_task(void *args);
TaskHandle_t led_task_handle;
#define LED_TASK_PERIOD 100

// 通讯任务
void comm_task(void *args);
TaskHandle_t comm_task_handle;
#define COMM_TASK_PERIOD 20

void App_FreeRTOS_Start(void)
{
    // 1. 创建任务
    xTaskCreate(power_task, "power_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGHEST, &power_task_handle);
    xTaskCreate(comm_task, "comm_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGHEST, &comm_task_handle);
    xTaskCreate(led_task, "led_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_HIGH, &led_task_handle);
    xTaskCreate(flight_ctrl_task, "flight_ctrl_task", DEFAULT_TASK_STACK_SIZE, NULL, TASK_PRIORITY_MEDIUM, &flight_ctrl_task_handle);


    // 2. 启动调度器
    vTaskStartScheduler();
}


void power_task(void *args)
{
    while(1){
        // 阻塞10秒等待任务通知
        uint32_t res = ulTaskNotifyTake(pdTRUE, POWER_TASK_PERIOD);
        if(res != 0)
        {
            // 收到通知，执行关机操作
            Int_IP5305T_Shutdown();
        }
        else
        {
            // 超时未收到通知，执行启动操作
            Int_IP5305T_Start();
        }
    }
}

void flight_ctrl_task(void *args)
{
    // 获取当前基准时间
    TickType_t last_wake_time = xTaskGetTickCount();
    App_Flight_Ctrl_Init();
    while(1){
        // 每6ms执行一次 => 飞行控制任务
        vTaskDelayUntil(&last_wake_time, FLIGHT_CTRL_TASK_PERIOD);
    
        // 1.获取加速度和角速度解算后的欧拉角
        App_Flight_Ctrl_Get_Euler_Angle();

        // 2.陀螺仪串级PID控制
        App_Flight_Ctrl_PID_MPU6050();

        // 3.VL53LX激光测距PID控制
        App_Flight_Ctrl_PID_VL53LX();

        // 4.PMW3901光流PID控制
        App_Flight_Ctrl_PID_PMW3901();

        // 5.电机控制
        App_Flight_Ctrl_Motor();

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

        if(remote_state == REMOTE_DISCONNECTED || flight_state == FLIGHT_FAULT) 
        {
            // 未连接/故障状态，LED灯熄灭
            Int_LED_Turn_Off(&left_top_led);
            Int_LED_Turn_Off(&right_top_led);
            Int_LED_Turn_Off(&right_bottom_led);
            Int_LED_Turn_Off(&left_bottom_led);
        }
        else if(remote_state == REMOTE_CONNECTED)
        {
            // 遥控器已连接
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
                // 快闪 => 100ms亮，100ms灭
                if(count % 1 == 0) {
                    Int_LED_Toggle(&left_top_led);
                    Int_LED_Toggle(&right_top_led);
                    Int_LED_Toggle(&right_bottom_led);
                    Int_LED_Toggle(&left_bottom_led);
                }
            } 
            else if(flight_state == FLIGHT_FIX_POSITION)
            {
                // 常亮
                Int_LED_Turn_On(&left_top_led);
                Int_LED_Turn_On(&right_top_led);
                Int_LED_Turn_On(&right_bottom_led);
                Int_LED_Turn_On(&left_bottom_led);
            }
            else if(flight_state == FLIGHT_FIX_HEIGHT)
            {
                // 中闪 => 500ms亮，500ms灭
                if(count % 5 == 0) {
                    Int_LED_Toggle(&left_top_led);
                    Int_LED_Toggle(&right_top_led);
                    Int_LED_Toggle(&right_bottom_led);
                    Int_LED_Toggle(&left_bottom_led);
                }
            }
        }
        
        // 计数清零
        if(count == 10)
        {
            count = 0;
        }
    }
}


void comm_task(void *args)
{
    // 初始化电压检测
    Int_BAT_ADC_Init();
    while(1){
        // 每20ms执行一次 => 通讯任务
        vTaskDelay(COMM_TASK_PERIOD);
        // 1.接收遥控器数据
        uint8_t res = App_Receive_Data();
        // 2.回传飞机数据
        App_Send_Data();
        // 3.检查遥控器连接状态
        App_Check_Remote_State(res);
        // 4.处理关机命令
        if(remote_ctrl_data.shutdown == 1)
        {
            // 使用freeRTOS的直接任务通知 => 通知电源任务，执行关机操作
            xTaskNotifyGive(power_task_handle);
            // 关机后不再处理数据，避免重复触发关机动作
            break;
        }
        // 5.处理飞行状态
        App_Check_Flight_State();
    }
}
