#ifndef __COM_CONFIG_H__
#define __COM_CONFIG_H__

#include "main.h"

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
    FLIGHT_FIX_HEIGHT,   // 定高状态
    FLIGHT_FIX_POSITION,   // 悬停状态
    FLIGHT_FAULT,  // 故障状态
} Flight_State;

typedef struct {
    int16_t throttle; // 油门值
    int16_t yaw;      // 偏航值
    int16_t pitch;    // 俯仰值
    int16_t roll;     // 横滚值
    uint8_t shutdown;  // 关机标志位   1:关机 0:不关机
    uint8_t fixed_height; // 定高标志位   切换为1表示定高，切换为0表示不定高
    uint8_t fixed_position; // 悬停标志位   切换为1表示悬停，切换为0表示不悬停
    uint8_t pid_p_pitch; // 俯仰PID比例系数
    uint8_t pid_p_roll;  // 横滚PID比例系数
} Remote_Ctrl_Data_Type;

typedef struct {
    uint8_t voltage;
    uint8_t pid_p_pitch; 
    uint8_t pid_p_roll;
} Flight_Ctrl_Data_Type;

// 油门解锁状态
typedef enum {
    FREE = 0,
    MAX,
    LEAVE_MAX,
    MIN,
    UNLOCK,
} Throttle_State;

// 陀螺仪数据-角速度
typedef struct {
    int16_t gyro_x; // 往左为负，往右为正
    int16_t gyro_y; // 往前为正，往后为负
    int16_t gyro_z; // 往左转为正，往右转为负
} Gyro_Data_Type;

// 陀螺仪数据-加速度
typedef struct {
    int16_t accel_x; // 往前为正，往后为负
    int16_t accel_y; // 往左为正，往右为负
    int16_t accel_z; // 往上为正，往下为负
} Accel_Data_Type;

// 陀螺仪数据
typedef struct {
    Gyro_Data_Type gyro;
    Accel_Data_Type accel;
} Gyro_Accel_Data_Type;

// 解算出的欧拉角
typedef struct {
    float roll;
    float pitch;
    float yaw;
} Euler_Angle_Type;


#endif // __COM_CONFIG_H__
