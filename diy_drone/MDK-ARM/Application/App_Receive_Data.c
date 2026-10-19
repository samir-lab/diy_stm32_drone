#include "App_Receive_Data.h"

/**
 * @brief 接收遥控器数据
 * @return 0:接收成功 1:校验/接收失败
 */
// 遥控数据结构体
extern Remote_Ctrl_Data_Type remote_ctrl_data;
// 通讯任务接收数据缓冲区
uint8_t comm_rx_buffer[SI24R1_TX_PLOAD_WIDTH] = {0};
uint8_t App_Receive_Data(void)
{
    // 1. 清空接收缓冲区
    memset(comm_rx_buffer, 0, SI24R1_TX_PLOAD_WIDTH);
    // 2. 接收数据到缓冲区
    uint8_t res = 1;
    uint8_t retry_count = 0;
    do
    {
        retry_count++;
        res = Int_SI24R1_RxPacket(comm_rx_buffer);
        vTaskDelay(10);
    } while (res == 1 && retry_count < MAX_RETRY_COUNT);

    if(res == 1)
    {
        return 1; // 接收失败
    }
    // 解析数据
    // 1.帧头校验
    if(comm_rx_buffer[0] != FRAME_HEADER_CHECK_1 
        || comm_rx_buffer[1] != FRAME_HEADER_CHECK_2 
        || comm_rx_buffer[2] != FRAME_HEADER_CHECK_3)
    {
        return 1; // 校验失败
    }
    // 2. 校验和
    uint32_t checksum = 0;
    // 高位在前
    uint32_t recived_checksum = (comm_rx_buffer[16] << 24) 
                                | (comm_rx_buffer[17] << 16) 
                                | (comm_rx_buffer[18] << 8) 
                                | comm_rx_buffer[19];
    for(uint8_t i = 0; i < 16; i++)
    {
        checksum += comm_rx_buffer[i];
    }
    if(checksum != recived_checksum)
    {
        return 1; // 校验失败
    }

    // 3. 解析数据到结构体
    remote_ctrl_data.throttle = (comm_rx_buffer[3] << 8) | comm_rx_buffer[4];
    remote_ctrl_data.yaw = (comm_rx_buffer[5] << 8) | comm_rx_buffer[6];
    remote_ctrl_data.pitch = (comm_rx_buffer[7] << 8) | comm_rx_buffer[8];
    remote_ctrl_data.roll = (comm_rx_buffer[9] << 8) | comm_rx_buffer[10];
    remote_ctrl_data.shutdown = comm_rx_buffer[11];
    remote_ctrl_data.fixed_height = comm_rx_buffer[12];
    remote_ctrl_data.fixed_position = comm_rx_buffer[13];
    remote_ctrl_data.pid_p_pitch = comm_rx_buffer[14];
    remote_ctrl_data.pid_p_roll = comm_rx_buffer[15];

    // debug_printf(": %d, %d, %d, %d, %d, %d, %d\n", 
    //     remote_ctrl_data.throttle, 
    //     remote_ctrl_data.yaw, 
    //     remote_ctrl_data.pitch, 
    //     remote_ctrl_data.roll, 
    //     remote_ctrl_data.shutdown, 
    //     remote_ctrl_data.fixed_height, 
    //     remote_ctrl_data.fixed_position);
    return 0; // 接收成功
}

/**
 * 回传飞行参数
 */
extern PID_Type pid_pitch_out;
extern PID_Type pid_roll_out;
Flight_Ctrl_Data_Type flight_ctrl_data;
uint8_t comm_tx_buffer[SI24R1_TX_PLOAD_WIDTH] = {0};
void App_Send_Data(void)
{
    // 电池电压
    float voltage_f = Int_BAT_ADC_Read();
    float pid_p_pitch_f = pid_pitch_out.kp;
    float pid_p_roll_f = pid_roll_out.kp;
    // 转换为uint8_t类型
    uint8_t voltage = (uint8_t)(voltage_f * 100.0f);
    uint8_t pid_p_pitch = (uint8_t)(pid_p_pitch_f * 100.0f);
    uint8_t pid_p_roll = (uint8_t)(pid_p_roll_f * 100.0f);
    
    flight_ctrl_data.pid_p_pitch = pid_p_pitch;
    flight_ctrl_data.pid_p_roll = pid_p_roll;
    flight_ctrl_data.voltage = voltage;

    // 清空发送缓冲区
    memset(comm_tx_buffer, 0, SI24R1_TX_PLOAD_WIDTH);
    // 进入发送模式
	Int_SI24R1_TX_Mode();
    // 调用SI24R1接口，发送数据
    // 1. 发送数据 => 唯一性和可靠性
    // 1.1 帧头校验，指定发送给对应的设备，唯一性
    // 1.2 末尾添加校验和，可靠性

    // 数据本身11字节，帧头设置3字节，校验和设置4字节，总共18字节
    // 帧头
    comm_tx_buffer[0] = FRAME_HEADER_CHECK_1;
    comm_tx_buffer[1] = FRAME_HEADER_CHECK_2;
    comm_tx_buffer[2] = FRAME_HEADER_CHECK_3;
    // 数据本身
    comm_tx_buffer[3] = flight_ctrl_data.voltage;
    comm_tx_buffer[4] = flight_ctrl_data.pid_p_pitch;
    comm_tx_buffer[5] = flight_ctrl_data.pid_p_roll;
    // 校验和
    uint32_t checksum = 0;
    for(uint8_t i = 0; i < 6; i++)
    {
        checksum += comm_tx_buffer[i];
    }
    comm_tx_buffer[6] = (checksum >> 24) & 0xFF;
    comm_tx_buffer[7] = (checksum >> 16) & 0xFF;
    comm_tx_buffer[8] = (checksum >> 8) & 0xFF;
    comm_tx_buffer[9] = checksum & 0xFF;

    uint8_t res = 1;
    uint8_t retry_count = 0;
    do
    {
        retry_count++;
        res = Int_SI24R1_TxPacket(comm_tx_buffer);
        vTaskDelay(10);
    } while (res == 1 && retry_count < MAX_RETRY_COUNT);
    
    // 发送完成，转为接收模式
	Int_SI24R1_RX_Mode();
}


/**
 * @brief 检查遥控器连接状态
 * @param conn_result 连接结果，0:接收成功 1:接收失败
 */
extern Remote_State remote_state;
void App_Check_Remote_State(uint8_t conn_result)
{
    if(conn_result == 0)
    {
        remote_state = REMOTE_CONNECTED;
    }
    else if(conn_result == 1)
    {
        remote_state = REMOTE_DISCONNECTED;
    }
}


/**
 * @brief 处理解锁逻辑
 * @return 0:解锁成功 1:解锁失败
 */
// 油门解锁状态
Throttle_State throttle_state = FREE;
uint32_t max_enter_time = 0;
uint32_t min_enter_time = 0;
uint8_t App_Flight_Unlock(void)
{
    // 1.考虑安全问题，未解锁油门应为0，解锁逻辑为：油门拉到最大持续1秒，再拉到最小持续1秒，则完成解锁
    switch (throttle_state)
    {
        case FREE:
            if(remote_ctrl_data.throttle > 900)
            {
                throttle_state = MAX;
                // 记录进入时间
                max_enter_time = xTaskGetTickCount();
            }
            break;
        case MAX:
            if(remote_ctrl_data.throttle < 900)
            {
                // 保持MAX超过1秒，则进入LEAVE_MAX状态
                if(xTaskGetTickCount() - max_enter_time > 1000)
                {
                    throttle_state = LEAVE_MAX;
                }
                else
                {
                    // 否则重新解锁
                    throttle_state = FREE;
                }
            }
            break;
        case LEAVE_MAX:
            if(remote_ctrl_data.throttle < 100)
            {
                throttle_state = MIN;
                // 记录进入时间
                min_enter_time = xTaskGetTickCount();
            }
            break;
        case MIN:
            // 判断是否超过1秒
            if(xTaskGetTickCount() - min_enter_time < 1000)
            {
                // 还未到1秒，动了油门
                if(remote_ctrl_data.throttle > 100)
                {
                    throttle_state = FREE;
                }
            }
            else
            {
                throttle_state = UNLOCK;
            }
            break;
        case UNLOCK:
            break;
        default:
            break;
    }
    if(throttle_state == UNLOCK)
    {
        return 0;
    }
    return 1;
}

/**
 * @brief 检查飞行状态
 */
extern Flight_State flight_state;
extern uint16_t fixed_height_target;
void App_Check_Flight_State(void)
{
    switch (flight_state)
    {
        case FLIGHT_IDLE:
            if(App_Flight_Unlock() == 0)
            {
                flight_state = FLIGHT_TAKEOFF;
                throttle_state = FREE;
            }
            break;
        case FLIGHT_TAKEOFF:
            // 进入悬停状态
            if(remote_ctrl_data.fixed_position == 1)
            {
                // 记录目标高度
                fixed_height_target = Int_VL53L1X_Get_Distance();
                flight_state = FLIGHT_FIX_POSITION;
            }
            // 进入定高状态
            else if(remote_ctrl_data.fixed_height == 1)
            {
                // 记录目标高度
                fixed_height_target = Int_VL53L1X_Get_Distance();
                flight_state = FLIGHT_FIX_HEIGHT;
            }
            // 故障状态
            if(remote_state == REMOTE_DISCONNECTED)
            {
                flight_state = FLIGHT_FAULT;
            }
            break;
        case FLIGHT_FIX_HEIGHT:
            // 取消定高
            if(remote_ctrl_data.fixed_height == 0)
            {
                flight_state = FLIGHT_TAKEOFF;
            }
            // 故障状态
            if(remote_state == REMOTE_DISCONNECTED)
            {
                flight_state = FLIGHT_FAULT;
            }
            break;
        case FLIGHT_FIX_POSITION:
            // 取消悬停
            if(remote_ctrl_data.fixed_position == 0)
            {
                flight_state = FLIGHT_TAKEOFF;
            }
            // 故障状态
            if(remote_state == REMOTE_DISCONNECTED)
            {
                flight_state = FLIGHT_FAULT;
            }
            break;
        case FLIGHT_FAULT:
            // 处理故障失联
            vTaskDelay(10000);
            flight_state = FLIGHT_IDLE;
            break;
        default:
            break;
    }
}
