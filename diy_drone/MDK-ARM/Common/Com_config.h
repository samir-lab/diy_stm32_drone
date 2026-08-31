#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

// 最新推荐堆大小：128 => 128 * 4 = 512字节
#define DEFAULT_TASK_STACK_SIZE 128
// 任务优先级，数值越大，优先级越高，范围为0~(configMAX_PRIORITIES-1) | 不推荐使用最小优先级0(给idle任务使用)
#define TASK_PRIORITY_LOW 1
#define TASK_PRIORITY_MEDIUM 2
#define TASK_PRIORITY_HIGH 3
#define TASK_PRIORITY_HIGHEST 4

// 遥控器连接状态枚举
typedef enum {
    REMOTE_CONNECTED = 0, // 遥控器已连接
    REMOTE_DISCONNECTED,  // 遥控器未连接
} Remote_State;

// 飞行状态枚举
typedef enum {
    FLIGHT_IDLE = 0, // 空闲状态
    FLIGHT_TAKEOFF,  // 起飞状态
    FLIGHT_HOVER,   // 悬停状态
    FLIGHT_FAULT,  // 故障状态
} Flight_State;

#endif // __COM_CONFIG_H__
