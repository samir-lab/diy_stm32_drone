#include "App_Flight_Ctrl.h"

// MPU6050数据结构体对象
Gyro_Accel_Data_Type gyro_accel_data;
// 解算出的欧拉角
Euler_Angle_Type euler_angle;


// 电机控制结构体对象定义
Motor_HandleTypeDef left_top_motor = { .htim = &htim3, .channel = TIM_CHANNEL_1,  .speed = 0};
Motor_HandleTypeDef left_bottom_motor = { .htim = &htim4, .channel = TIM_CHANNEL_4,  .speed = 0};
Motor_HandleTypeDef right_top_motor = { .htim = &htim2, .channel = TIM_CHANNEL_2,  .speed = 0};
Motor_HandleTypeDef right_bottom_motor = { .htim = &htim1, .channel = TIM_CHANNEL_3,  .speed = 0};


/**
 * @brief 初始化飞行控制相关的模块
 */
void App_Flight_Ctrl_Init(void)
{
    // 启动MPU6050
	Int_MPU6050_Init();
    // 启动电机
    Int_Motor_Start(&left_top_motor);
	Int_Motor_Start(&left_bottom_motor);
	Int_Motor_Start(&right_top_motor);
	Int_Motor_Start(&right_bottom_motor);
}

/**
 * @brief 角速度低通滤波
 */
// 低通滤波器中间结果
Gyro_Data_Type pre_gyro_data;
void App_Gyro_Data_Filter()
{
    // 2.1 实时性要求高，对准确性要求不高，不使用傅里叶变换
    // 2.2 output = 加权系数 * pre_output + (1 - 加权系数) * 本次测量值
    gyro_accel_data.gyro.gyro_x = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_x, pre_gyro_data.gyro_x, LOW_PASS_WEIGHT);
    gyro_accel_data.gyro.gyro_y = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_y, pre_gyro_data.gyro_y, LOW_PASS_WEIGHT);
	gyro_accel_data.gyro.gyro_z = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_z, pre_gyro_data.gyro_z, LOW_PASS_WEIGHT);
    // 2.3 保存本次测量值，用于下一次滤波
    pre_gyro_data.gyro_x = gyro_accel_data.gyro.gyro_x;
	pre_gyro_data.gyro_y = gyro_accel_data.gyro.gyro_y;
	pre_gyro_data.gyro_z = gyro_accel_data.gyro.gyro_z;

    // debug_printf(": %d, %d, %d\n", 
    //     gyro_accel_data.gyro.gyro_x, 
    //     gyro_accel_data.gyro.gyro_y, 
    //     gyro_accel_data.gyro.gyro_z);
}


/**
 * @brief 加速度卡尔曼滤波
 */
// 卡尔曼滤波器初始化参数
extern KalmanFilter_Type kf_init_param[3];
void App_Accel_Data_Filter()
{
	// 3.1 对波动比较大的数据，使用卡尔曼滤波算法
    gyro_accel_data.accel.accel_x = Common_Filter_Kalman(&kf_init_param[0], gyro_accel_data.accel.accel_x);
    gyro_accel_data.accel.accel_y = Common_Filter_Kalman(&kf_init_param[1], gyro_accel_data.accel.accel_y);
	gyro_accel_data.accel.accel_z = Common_Filter_Kalman(&kf_init_param[2], gyro_accel_data.accel.accel_z);

    // debug_printf(": %d, %d, %d\n", 
    //     gyro_accel_data.accel.accel_x, 
    //     gyro_accel_data.accel.accel_y, 
    //     gyro_accel_data.accel.accel_z);
}

/**
 * @brief 解算欧拉角
 */
void App_Calc_Euler_Angle(void)
{
    // 4.1 常用的解算方法
    // 4.1.1 角速度对时间积分（类似 θ = ω * t），缺点：长时间积分存在误差
    // 4.1.2 加速度解算（类似 θ = arctan(对边/邻边)），缺点：易受振动影响
    // 4.1.3 互补解算，结合上述两种方法进行计算
    // 4.1.4 四元数解算，较复杂，算法固定，有完整的数学理论，计算量小
    // 4.1.5 卡尔曼姿态评估，动态性能好，模型复杂，计算量大，理论复杂，参数难调
    
    // 互补解算
    // Com_IMU_Calc_Pose_By_Complementary(&gyro_accel_data, &euler_angle);

    // 四元数解算
    Com_IMU_Get_Euler_Angle(&gyro_accel_data, &euler_angle, 0.006);

    // debug_printf(": %.2f, %.2f, %.2f\n", 
    //     euler_angle.roll, 
    //     euler_angle.pitch, 
    //     euler_angle.yaw);
}

/**
 * @brief 解算陀螺仪数据，获取欧拉角
 */
void App_Flight_Ctrl_Get_Euler_Angle(void)
{
    // 实现陀螺仪数据解算算法
    // 1. 获取陀螺仪和加速度计数据
    Int_MPU6050_Get_Data(&gyro_accel_data);

    // 2. 应用低通滤波算法，处理角速度数据
    App_Gyro_Data_Filter();

    // 3. 应用卡尔曼滤波算法，处理加速度计数据
    App_Accel_Data_Filter();

    // 4. 姿态解算
    App_Calc_Euler_Angle();
}


/**
 * @brief PID控制，根据欧拉角计算出PID目标值
 * @note PID调参是先调内环，再调外环
 */
// 遥控器数据
extern Mixed_Ctrl_Data_Type mixed_ctrl_data;
// 俯仰角 => 外环PID参数
PID_Type pid_pitch_out = {.kp = 0, .ki = 0, .kd = 0};
// 俯仰角 => 内环PID参数
PID_Type pid_pitch_in = {.kp = 1, .ki = 0, .kd = 0};

void App_Flight_Ctrl_PID_Ctrl(void)
{
    // PID控制算法实现
    // 1. 俯仰角
    // 1.1 外环，需要目标角度和测量角度
    // 数值转换 => 遥控器数值范围是0-1000，需要转换为[-500,500]，并控制俯仰角范围在[-10,10]之间
    pid_pitch_out.target = (mixed_ctrl_data.pitch - 500) / 50.0;
    // 外环的测量值 => 当前的俯仰角
	pid_pitch_out.measure = euler_angle.pitch;

    // 1.2 内环，需要测量角速度
	pid_pitch_in.target = pid_pitch_out.output;
    // 内环的测量值 => 当前的角速度
	pid_pitch_in.measure = (gyro_accel_data.gyro.gyro_y * 2000) / 32768.0;

    // 1.3 计算PID输出值
    Com_PID_Calc_Chain(&pid_pitch_out, &pid_pitch_in);

    // 观察内环
    // debug_printf(": %.2f, %.2f\n", pid_pitch_in.error, pid_pitch_in.output);

}

/**
 * @brief 电机控制
 */
extern Flight_State flight_state;
void App_Flight_Ctrl_Motor_Ctrl(void)
{
    // 1.判断飞机状态
    switch (flight_state)
    {
        case FLIGHT_IDLE:
            // 加锁状态，电机速度置为0
            left_top_motor.speed = 0;
			left_bottom_motor.speed = 0;
			right_top_motor.speed = 0;
			right_bottom_motor.speed = 0;
            break;
        case FLIGHT_TAKEOFF:
            // 起飞状态
            // 俯仰角 => 向前飞角速度 => 正误差 => 需要一个向后的反馈，前两个电机转得快，后两个转得慢
            left_top_motor.speed = mixed_ctrl_data.throttle + pid_pitch_in.output;
			right_top_motor.speed = mixed_ctrl_data.throttle + pid_pitch_in.output;
            left_bottom_motor.speed = mixed_ctrl_data.throttle - pid_pitch_in.output;
			right_bottom_motor.speed = mixed_ctrl_data.throttle - pid_pitch_in.output;
            break;
        case FLIGHT_FIX_POSITION:
            break;
        case FLIGHT_FIX_HEIGHT:
            break;
        case FLIGHT_FAULT:
            break;
        default:
            break;
    }

    // 2.设置电机速度, 现在转速范围
	Int_Motor_Set_Speed(&left_top_motor);
	Int_Motor_Set_Speed(&left_bottom_motor);
	Int_Motor_Set_Speed(&right_top_motor);
	Int_Motor_Set_Speed(&right_bottom_motor);

}
