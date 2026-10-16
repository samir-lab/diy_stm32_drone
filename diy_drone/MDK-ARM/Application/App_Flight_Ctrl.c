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

// 遥控器数据
extern Mixed_Ctrl_Data_Type mixed_ctrl_data;
// 飞行状态
extern Flight_State flight_state;
// 定高目标值
extern uint16_t fixed_height_target;

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
    // 初始化VL53L1X
	Int_VL53L1X_Init();
    // 初始化PWM3901
    Int_PMW3901_Init();
}

/**
 * @brief 角速度低通滤波
 */
// 低通滤波器中间结果
Gyro_Data_Type pre_gyro_data;
uint16_t gyro_data_threshold = 2000;
void App_Gyro_Data_Filter()
{
    // 过滤非连续的大于跳跃阈值异常数据
    if(abs(gyro_accel_data.gyro.gyro_x - pre_gyro_data.gyro_x) > gyro_data_threshold)
    {
        gyro_accel_data.gyro.gyro_x = pre_gyro_data.gyro_x;
    }
    if(abs(gyro_accel_data.gyro.gyro_y - pre_gyro_data.gyro_y) > gyro_data_threshold)
    {
        gyro_accel_data.gyro.gyro_y = pre_gyro_data.gyro_y;
    }
    if(abs(gyro_accel_data.gyro.gyro_z - pre_gyro_data.gyro_z) > gyro_data_threshold)
    {
        gyro_accel_data.gyro.gyro_z = pre_gyro_data.gyro_z;
    }
    // 2.1 实时性要求高，对准确性要求不高，不使用傅里叶变换
    // 2.2 output = 加权系数 * pre_output + (1 - 加权系数) * 本次测量值
    gyro_accel_data.gyro.gyro_x = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_x, pre_gyro_data.gyro_x, LOW_PASS_WEIGHT);
    gyro_accel_data.gyro.gyro_y = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_y, pre_gyro_data.gyro_y, LOW_PASS_WEIGHT);
	gyro_accel_data.gyro.gyro_z = Common_Filter_LowPass(gyro_accel_data.gyro.gyro_z, pre_gyro_data.gyro_z, LOW_PASS_WEIGHT);
    // 2.3 保存本次测量值，用于下一次滤波
    pre_gyro_data.gyro_x = gyro_accel_data.gyro.gyro_x;
	pre_gyro_data.gyro_y = gyro_accel_data.gyro.gyro_y;
	pre_gyro_data.gyro_z = gyro_accel_data.gyro.gyro_z;

    // debug_printf(": %d, %d, %d\r\n", 
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

    // debug_printf(":%.2f, %.2f, %.2f\n",
    //         euler_angle.roll, 
    //         euler_angle.pitch, 
    //         euler_angle.yaw);

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

// 俯仰角 => 外环PID参数
PID_Type pid_pitch_out = {.kp = 0.12, .ki = 0.0, .kd = 0.3};
// 俯仰角 => 内环PID参数
PID_Type pid_pitch_in = {.kp = 0.8, .ki = 0.0, .kd = 15};

// 横滚角 => 外环PID参数
PID_Type pid_roll_out = {.kp = 0.12, .ki = 0.0, .kd = 0.3};
// 横滚角 => 内环PID参数
PID_Type pid_roll_in = {.kp = 0.8, .ki = 0.0, .kd = 15};

// 偏航角 => 外环PID参数
PID_Type pid_yaw_out = {.kp = 0.0, .ki = 0.0, .kd = 0.0};
// 偏航角 => 内环PID参数
PID_Type pid_yaw_in = {.kp = -0.8, .ki = 0.0, .kd = 15};
void App_Flight_Ctrl_PID_MPU6050(void)
{
    // PID控制算法实现
    // 1. 俯仰角
    // 1.1 外环，需要目标角度和测量角度
    // 外环的目标值 => 数值转换 => 遥控器数值范围是0-1000，需要转换为[-500,500]，并控制俯仰角范围在[-10,10]之间，单位为度°
    pid_pitch_out.target = (mixed_ctrl_data.pitch - 500) / 50.0;
    // 外环的测量值 => 当前的俯仰角
	pid_pitch_out.measure = euler_angle.pitch;

    // 1.2 内环，需要测量角速度
    // 内环的测量值 => 当前的角速度，单位转换为度/s
	pid_pitch_in.measure = (gyro_accel_data.gyro.gyro_y * 2000) / 32768.0;

    // 1.3 计算PID输出值
    Com_PID_Calc_Chain(&pid_pitch_out, &pid_pitch_in);

    // 2.横滚角
    // 2.1 外环，需要目标角度和测量角度
	pid_roll_out.target = (mixed_ctrl_data.roll - 500) / 50.0;
	pid_roll_out.measure = euler_angle.roll;

    // 2.2 内环，需要测量角速度
    // 内环的测量值 => 当前的角速度，单位转换为度/s
	pid_roll_in.measure = (gyro_accel_data.gyro.gyro_x * 2000) / 32768.0;

    // 2.3 计算PID输出值
    Com_PID_Calc_Chain(&pid_roll_out, &pid_roll_in);

    // 3.偏航角
    // 3.1 外环，需要目标角度和测量角度
	pid_yaw_out.target = (mixed_ctrl_data.yaw - 500) / 50.0;
	pid_yaw_out.measure = euler_angle.yaw;
	
    // 3.2 内环，需要测量角速度
    // 内环的测量值 => 当前的角速度，单位转换为度/s
	pid_yaw_in.measure = (gyro_accel_data.gyro.gyro_z * 2000) / 32768.0;

    // 3.3 计算PID输出值
    Com_PID_Calc_Chain(&pid_yaw_out, &pid_yaw_in);
    //debug_printf(":%d,%.2f,%d,%.2f\r\n", mixed_ctrl_data.yaw, euler_angle.yaw,gyro_accel_data.gyro.gyro_z,pid_yaw_in.output);
}

/**
 * @brief PID控制，根据VL53LX激光测距计算出PID目标值
 */
PID_Type pid_fixed_height = {.kp = -0.4, .ki = 0.0, .kd = 15};
void App_Flight_Ctrl_PID_VL53LX(void)
{
	if(flight_state != FLIGHT_FIX_HEIGHT && flight_state != FLIGHT_FIX_POSITION)
	{
        pid_fixed_height.output = 0;
        return;
    }
    uint8_t is_data_ready = Int_VL53L1X_Is_Data_Ready();
    if (is_data_ready == 0)
    {
        pid_fixed_height.output = 0;
        return;
    }
    
    // 目标值（按下按钮时的高度）测量值（实时测试到的数据）
    uint16_t curr_distance = Int_VL53L1X_Get_Distance();
    pid_fixed_height.target = fixed_height_target;
	pid_fixed_height.measure = curr_distance;

    Com_PID_Calc(&pid_fixed_height);

    // debug_printf(":%d, %d, %.2f\n", fixed_height_target, curr_distance, pid_fixed_height.output);
}


/**
 * @brief PID控制，根据PMW3901光流数据计算出PID目标值
 */
// x轴
PID_Type pid_fixed_pos_x = {.kp = 0.8, .ki = 0.0, .kd = 15};
PID_Type pid_fixed_pos_y = {.kp = 0.8, .ki = 0.0, .kd = 15};
PMW3901_Data_Type position_data = {0};
void App_Flight_Ctrl_PID_PMW3901(void)
{
    if (flight_state != FLIGHT_FIX_POSITION)
	{
        pid_fixed_pos_x.output = 0;
        pid_fixed_pos_y.output = 0;
        return;
    }

    // 读取光流数据，使用 delta_x / delta_y 作为位置漂移误差
    Int_PMW3901_Get_Data(&position_data);

    if(position_data.delta_x == 0 && position_data.delta_y == 0)
    {
        pid_fixed_pos_x.output = 0;
        pid_fixed_pos_y.output = 0;
		return;
    }

    pid_fixed_pos_x.target = 0.0f;
    pid_fixed_pos_x.measure = position_data.delta_x;
    Com_PID_Calc(&pid_fixed_pos_x);

    pid_fixed_pos_y.target = 0.0f;
    pid_fixed_pos_y.measure = position_data.delta_y;
    Com_PID_Calc(&pid_fixed_pos_y);

    // debug_printf(":%d, %d, %.2f, %.2f\n", position_data.delta_x, position_data.delta_y, pid_fixed_pos_x.output, pid_fixed_pos_y.output);
}


static void App_Flight_Locked_Posture(void)
{
    left_top_motor.speed = 0;
    left_bottom_motor.speed = 0;
    right_top_motor.speed = 0;
    right_bottom_motor.speed = 0;
}

/**
 * @brief 平衡姿态控制
 */
static void App_Flight_Balanced_Posture(void)
{
	// 起飞状态
    left_top_motor.speed = mixed_ctrl_data.throttle 
                            + pid_pitch_in.output 
                            - pid_roll_in.output
                            - pid_yaw_in.output;
    left_bottom_motor.speed = mixed_ctrl_data.throttle 
                                - pid_pitch_in.output 
                                - pid_roll_in.output
                                + pid_yaw_in.output;
    right_top_motor.speed = mixed_ctrl_data.throttle 
                                + pid_pitch_in.output 
                                + pid_roll_in.output
                                + pid_yaw_in.output;
    right_bottom_motor.speed = mixed_ctrl_data.throttle 
                                - pid_pitch_in.output 
                                + pid_roll_in.output
                                - pid_yaw_in.output;
}

/**
 * @brief 定高姿态控制
 */
static void App_Flight_Fixed_Height_Posture(void)
{
    App_Flight_Balanced_Posture();
	left_top_motor.speed += pid_fixed_height.output;
    left_bottom_motor.speed += pid_fixed_height.output;
    right_top_motor.speed += pid_fixed_height.output;
    right_bottom_motor.speed += pid_fixed_height.output;
}

/**
 * @brief 悬停姿态控制
 */
static void App_Flight_Fixed_Position_Posture(void)
{
    App_Flight_Fixed_Height_Posture();
	left_top_motor.speed = left_top_motor.speed + pid_fixed_pos_x.output + pid_fixed_pos_y.output;
    left_bottom_motor.speed = left_bottom_motor.speed + pid_fixed_pos_x.output - pid_fixed_pos_y.output;
    right_top_motor.speed = right_top_motor.speed - pid_fixed_pos_x.output + pid_fixed_pos_y.output;
    right_bottom_motor.speed = right_bottom_motor.speed -pid_fixed_pos_x.output - pid_fixed_pos_y.output;
}


/**
 * @brief 电机控制
 */
void App_Flight_Ctrl_Motor(void)
{
    // 1.判断飞机状态
    switch (flight_state)
    {
        case FLIGHT_IDLE:
            // 加锁状态，电机速度置为0
            App_Flight_Locked_Posture();
            break;
        case FLIGHT_TAKEOFF:
            // 起飞状态
            App_Flight_Balanced_Posture();
            break;
        case FLIGHT_FIX_POSITION:
            // 定点悬停
            App_Flight_Fixed_Position_Posture();
            break;
        case FLIGHT_FIX_HEIGHT:
            // 定高状态
            App_Flight_Fixed_Height_Posture();
            break;
        case FLIGHT_FAULT:
            left_top_motor.speed -= 1;
			left_bottom_motor.speed -= 1;
			right_top_motor.speed -= 1;
			right_bottom_motor.speed -= 1;
            break;
        default:
            break;
    }

    // 2.设置电机速度, 现在转速范围
    // 2.1 安全限制 => 当油门小于50时 => 电机上锁，速度置为0
    if (mixed_ctrl_data.throttle < 50)
    {
        App_Flight_Locked_Posture();
    }
    // 2.3 设置占空比
	Int_Motor_Set_Speed(&left_top_motor);
	Int_Motor_Set_Speed(&left_bottom_motor);
	Int_Motor_Set_Speed(&right_top_motor);
	Int_Motor_Set_Speed(&right_bottom_motor);
    // debug_printf(":%d,%d,%d,%d\r\n",left_top_motor.speed,right_top_motor.speed,left_bottom_motor.speed,right_bottom_motor.speed);
}
