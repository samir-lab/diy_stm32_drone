#include "App_Receive_Data.h"

/**
 * @brief 接收遥控器数据
 * @return 0:接收成功 1:校验/接收失败
 */
// 遥控数据结构体
extern Mixed_Ctrl_Data_Type mixed_ctrl_data;
// 通讯任务接收数据缓冲区
uint8_t comm_rx_buffer[SI24R1_TX_PLOAD_WIDTH] = {0};
uint8_t App_Receive_Data(void)
{
    // 1. 清空接收缓冲区
    memset(comm_rx_buffer, 0, SI24R1_TX_PLOAD_WIDTH);
    // 2. 接收数据到缓冲区
    uint8_t res = Int_SI24R1_RxPacket(comm_rx_buffer);
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
    uint32_t recived_checksum = (comm_rx_buffer[14] << 24) 
                                | (comm_rx_buffer[15] << 16) 
                                | (comm_rx_buffer[16] << 8) 
                                | comm_rx_buffer[17];
    for(uint8_t i = 0; i < 14; i++)
    {
        checksum += comm_rx_buffer[i];
    }
    if(checksum != recived_checksum)
    {
        return 1; // 校验失败
    }

    // 3. 解析数据到结构体
    mixed_ctrl_data.throttle = (comm_rx_buffer[3] << 8) | comm_rx_buffer[4];
    mixed_ctrl_data.yaw = (comm_rx_buffer[5] << 8) | comm_rx_buffer[6];
    mixed_ctrl_data.pitch = (comm_rx_buffer[7] << 8) | comm_rx_buffer[8];
    mixed_ctrl_data.roll = (comm_rx_buffer[9] << 8) | comm_rx_buffer[10];
    mixed_ctrl_data.shutdown = comm_rx_buffer[11];
    mixed_ctrl_data.fixed_height = comm_rx_buffer[12];
    mixed_ctrl_data.fixed_position = comm_rx_buffer[13];

    // debug_printf(": %d, %d, %d, %d, %d, %d, %d\n", 
    //     mixed_ctrl_data.throttle, 
    //     mixed_ctrl_data.yaw, 
    //     mixed_ctrl_data.pitch, 
    //     mixed_ctrl_data.roll, 
    //     mixed_ctrl_data.shutdown, 
    //     mixed_ctrl_data.fixed_height, 
    //     mixed_ctrl_data.fixed_position);
    return 0; // 接收成功
}

/**
 * @brief 检查遥控器连接状态
 * @param conn_result 连接结果，0:接收成功 1:接收失败
 */
extern Remote_State remote_state;
uint8_t retry_count = 0; // 重试计数器
void App_Check_Remote_State(uint8_t conn_result)
{
    if(conn_result == 0)
    {
        remote_state = REMOTE_CONNECTED;
        retry_count = 0; // 重置计数器
    }
    else if(conn_result == 1)
    {
        retry_count++;
        if(retry_count >= MAX_RETRY_COUNT)
        {
            remote_state = REMOTE_DISCONNECTED;
            retry_count = 0; // 重置计数器
        }
    }
}
